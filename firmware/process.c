/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp_intrinsic.h>

#include <nfp/me.h>
#include <nfp/cls.h>
#include <nfp/ctm.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_ring.h>
#include <std/reg_utils.h>
#include <std/reorder.h>
#include <pkt/pkt.h>

#include "flows.h"
#include "desc_pool.h"

#include "shared.h"
#include "global.h"
#include "debug.h"

/** Flowstate cache **/
/*  Constants */
#define FS_CLS_CACHE_SIZE      512                 /*> NOTE: Should be a power of 2 */
#define FS_CLS_CACHE_MASK      (FS_CLS_CACHE_SIZE - 1)
#define FS_LM_CACHE_SIZE        16      /* 16 entries in CACHE */
#define FS_LM_MISSLOCK_TIMEOUT  100     /* Retry acquiring Misslock every 100 cycles */

/*  Flowst cache definition | size = 64*256 = 16 */
/*  NOTE: CLS size is 64KiB */
__shared __lmem struct flowst_tcp_t fs_lm[FS_LM_CACHE_SIZE];
__shared __lmem uint32_t            fs_cls_status[FS_CLS_CACHE_SIZE];
__shared __cls __align8 uint64_t    fs_miss_lock[FS_LM_CACHE_SIZE]; /* Lock when processing LM cache miss */
__shared __cls __align64 struct flowst_tcp_t flowst_cache[FS_CLS_CACHE_SIZE];

__intrinsic void init_cls_cache()
{
  int idx;

  for (idx = 0; idx < FS_CLS_CACHE_SIZE; idx++)
    fs_cls_status[idx] = 0;   /* FlowID=0 is invalid */
}

__intrinsic void init_lm_cache()
{
/**
 * The following rules must be followed to when using the CAM.
 * 1. CAM is not reset by a FPC reset. Software must either do a CAM_clear prior to using
 * the CAM to initialize the LRU and clear the tags to 0, or explicitly write all entries with
 * CAM_write.
 * 2. No two tags can be written to have same value. If this rule is violated, the result of a
 * lookup that matches that value will be unpredictable, and the LRU state is unpredictable.
 *
 * The value 0x00000000 can be used as a valid lookup value. However, note that CAM_clear instruction puts
 * 0x00000000 into all tags. To avoid violating rule 2 after doing CAM_clear, it is necessary to write all
 * entries to unique values prior to doing a lookup of 0x00000000.
 *
 * NOTE: Disallow flow_id=0 to avoid complications!
 */

  cam_clear();

  cam_write(0, 0xFFFFFFFF - 0, 0);
  cam_write(1, 0xFFFFFFFF - 1, 0);
  cam_write(2, 0xFFFFFFFF - 2, 0);
  cam_write(3, 0xFFFFFFFF - 3, 0);
  cam_write(4, 0xFFFFFFFF - 4, 0);
  cam_write(5, 0xFFFFFFFF - 5, 0);
  cam_write(6, 0xFFFFFFFF - 6, 0);
  cam_write(7, 0xFFFFFFFF - 7, 0);
  cam_write(8, 0xFFFFFFFF - 8, 0);
  cam_write(9, 0xFFFFFFFF - 9, 0);
  cam_write(10, 0xFFFFFFFF - 10, 0);
  cam_write(11, 0xFFFFFFFF - 11, 0);
  cam_write(12, 0xFFFFFFFF - 12, 0);
  cam_write(13, 0xFFFFFFFF - 13, 0);
  cam_write(14, 0xFFFFFFFF - 14, 0);
  cam_write(15, 0xFFFFFFFF - 15, 0);
}

/* Mark entry as locked when handling MISS */
__intrinsic void cam_misslock_acquire(unsigned int entry_idx)
{
  cam_write_state(entry_idx, 1);
}

/* Unlock entry after handling MISS */
__intrinsic void cam_misslock_release(unsigned int entry_idx)
{
  cam_write_state(entry_idx, 0);
}

/* LM Cache miss handler */
__intrinsic void handle_miss(unsigned int entry_idx, uint32_t flow_id)
{
  uint32_t lm_evicted_flowid;       /*> Flow evicted from LM  */
  uint32_t cls_evicted_flowid;      /*> Flow evicted from CLS */
  uint32_t lm_evicted_cls_slot;     /*> CLS slot of the flow evicted from LM */
  uint32_t incoming_cls_slot;       /*> CLS slot of the incoming flow */

  __xread struct flowst_tcp_t incoming_fs;
  __xrw   struct flowst_tcp_t lm_evict_fs;
  __xwrite struct flowst_tcp_t cls_evict_fs;
  __mem40 void* addr;
  SIGNAL fs_sig, evict_sig, swap_sig;

  STATS_INC(PROC_CACHE_MISS);

  incoming_cls_slot   = flow_id & FS_CLS_CACHE_MASK;
  lm_evicted_flowid   = cam_read_tag(entry_idx);
  lm_evicted_cls_slot = lm_evicted_flowid & FS_CLS_CACHE_MASK;
  cls_evicted_flowid  = fs_cls_status[lm_evicted_cls_slot];

  /* Modify the CAM; leave in locked state */
  cam_write(entry_idx, flow_id, 1);

  lm_evict_fs = fs_lm[entry_idx];
  fs_cls_status[lm_evicted_cls_slot] = lm_evicted_flowid;

  if (cls_evicted_flowid == 0) {
    __cls_write(&lm_evict_fs, &flowst_cache[lm_evicted_cls_slot], sizeof(struct flowst_tcp_t), sizeof(struct flowst_tcp_t), sig_done, &evict_sig);
  }
  else {
    __cls_swap(&lm_evict_fs, &flowst_cache[lm_evicted_cls_slot], sizeof(struct flowst_tcp_t), sizeof(struct flowst_tcp_t), sig_done, &swap_sig);

    /* Busy poll */
    while (!signal_test(&swap_sig)) { }

    if (cls_evicted_flowid == flow_id) {
      fs_lm[entry_idx] = lm_evict_fs;
      cam_misslock_release(entry_idx);
      return;
    }

    /* Evict to EMEM */
    cls_evict_fs = lm_evict_fs;
    addr = (__mem40 void*) &fp_state.flows_tcp_state[cls_evicted_flowid];
    __mem_write64(&cls_evict_fs, addr, sizeof(struct flowst_tcp_t), sizeof(struct flowst_tcp_t), sig_done, &evict_sig);
  }

  /* Read from CLS/EMEM */
  if (fs_cls_status[incoming_cls_slot] == flow_id) {
    fs_cls_status[incoming_cls_slot] = 0; /* Clear CLS slot */
    __cls_read(&incoming_fs, &flowst_cache[incoming_cls_slot], sizeof(struct flowst_tcp_t), sizeof(struct flowst_tcp_t), sig_done, &fs_sig);
  } else {
    STATS_INC(PROC_EMEM_CACHE_MISS);
    addr = (__mem40 void*) &fp_state.flows_tcp_state[flow_id];
    __mem_read64(&incoming_fs, addr, sizeof(struct flowst_tcp_t), sizeof(struct flowst_tcp_t), sig_done, &fs_sig);
  }

  /* Now release processor! */
  cls_lockq_lock(&fs_miss_lock[entry_idx]);
  __wait_for_all(&evict_sig, &fs_sig);

  fs_lm[entry_idx] = incoming_fs;

  cls_lockq_unlock(&fs_miss_lock[entry_idx]);
  cam_misslock_release(entry_idx);
}

__shared __gpr unsigned int seq;
__forceinline void process_work(SIGNAL* order_sig)
{
  __xread  struct work_t          work_read;
  __gpr    struct work_t          work;
  __xwrite struct work_result_t   result;
  __xread  struct pkt_summary_t   pkt;
  SIGNAL work_sig, result_sig, input_sig;
  SIGNAL_PAIR fs_miss_lock_sig;

  unsigned int fs_lm_idx;
  unsigned int type, island, pnum;
  unsigned int rnum, raddr_hi;
  uint32_t flow_id;
  __mem40 void* addr;
  cam_lookup_t fs_lock;

  /* Kickstart */
  // Issue work fetch
  __wait_for_all(order_sig);
  /* Select WorkQueue to poll based on FlowGroup */
  island = __ISLAND;
  if (island == 32) {
    rnum = MEM_RING_GET_NUM(flow_ring_0);
    raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_0);
  }
  else if (island == 33) {
    rnum = MEM_RING_GET_NUM(flow_ring_1);
    raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_1);
  }
  else if (island == 34) {
    rnum = MEM_RING_GET_NUM(flow_ring_2);
    raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_2);
  }
  else {
    rnum = MEM_RING_GET_NUM(flow_ring_3);
    raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_3);
  }
  __mem_workq_add_thread(rnum, raddr_hi,
                  &work_read,
                  sizeof(struct work_t), sizeof(struct work_t),
                  sig_done, &work_sig);
  reorder_signal_next(order_sig);

  // Dummy result signal to self
  signal_ctx(ctx(), __signal_number(&result_sig));
  __implicit_write(&result_sig);

  for (;;) {
    // 1. Wait to receive work
    // 2. Issue work fetch
    __wait_for_all(&work_sig, order_sig);
    work = work_read;
    __mem_workq_add_thread(rnum, raddr_hi,
                    &work_read,
                    sizeof(struct work_t), sizeof(struct work_t),
                    sig_done, &work_sig);
    reorder_signal_next(order_sig);

    // 3. Fetch input (pkt)
    type = work.type;
    if (type == WORK_TYPE_RX) {
      island = 32 + work.io.isl;
      pnum   = work.io.pnum;
      addr = pkt_ctm_ptr40(island, pnum, sizeof(struct nbi_meta_catamaran));
      __mem_read32(&pkt, addr, sizeof(pkt), sizeof(pkt), sig_done, &input_sig);
    }

    // 4. Lookup Flow state in LM
    flow_id = work.flow_id;
    fs_lock = cam_lookup(flow_id);
    fs_lm_idx = fs_lock.entry_num;

    // 4a. Handle Miss
    if (!fs_lock.hit) {
      handle_miss(fs_lm_idx, flow_id);
    }

    // 4b. Is Locked? Wait for release
    if (fs_lock.state != 0) { /* Locked */
      cls_lockq_lock_retry(&fs_miss_lock[fs_lm_idx], FS_LM_MISSLOCK_TIMEOUT);  /* Wait until my turn */
      cls_lockq_unlock(&fs_miss_lock[fs_lm_idx]);
    }

    // 5. Process packet
    // Wait for previous result enqueue to complete!
    switch (type) {
    case WORK_TYPE_RX:
      STATS_INC(RX_PROC);
      __wait_for_all(&result_sig, &input_sig);

      work.io.seq = seq++;
      if (work.io.plen == 0) {
        PROF_START(PROC_ACK);
        flows_ack((struct work_t*) &work, &pkt, &fs_lm[fs_lm_idx], &result);
        PROF_END(PROC_ACK);
      }
      else {
        PROF_START(PROC_RX);
        flows_seg((struct work_t*) &work, &pkt, &fs_lm[fs_lm_idx], &result);
        PROF_END(PROC_RX);
      }
      break;

    case WORK_TYPE_TX:
      STATS_INC(TX_PROC);
      __wait_for_all(&result_sig);
      PROF_START(PROC_TX);
      work.io.seq = seq++;
      flows_tx((struct work_t*) &work, &fs_lm[fs_lm_idx], &result);
      PROF_END(PROC_TX);
      break;

    case WORK_TYPE_AC:
      STATS_INC(AC_PROC);
      __wait_for_all(&result_sig);
      PROF_START(PROC_AC);
      flows_ac((struct work_t*) &work, &fs_lm[fs_lm_idx], &result);
      PROF_END(PROC_AC);
      break;

    case WORK_TYPE_RETX:
      STATS_INC(RETX_PROC);
      __wait_for_all(&result_sig);
      PROF_START(PROC_RETX);
      flows_retx((struct work_t*) &work, &fs_lm[fs_lm_idx], &result);
      PROF_END(PROC_RETX);
      break;
    }

    // 6. Ship result!
    __cls_workq_add_work(POSTPROC_RING, &result, sizeof(result), sig_done, &result_sig);

    __implicit_write(&result);
    __implicit_read(&work_read);
  }

  return;
}

int main() {
  SIGNAL order_sig;

  if (ctx() == 0) {
    init_lm_cache();
    init_cls_cache();
    enable_global_timestamp();

    seq = 0;

    /* Wait for start signal */
    while (1) {
      if (fp_state.cfg.sig != 0)
        break;

      sleep(1 << 10);
    }

    sleep(1 << 16);

    reorder_start(0, &order_sig);
  }

  process_work(&order_sig);

  return 0;
}
