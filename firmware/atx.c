/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdint.h>
#include <stdlib.h>

#include <nfp.h>
#include <nfp_intrinsic.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_ring.h>
#include <nfp/cls.h>

#include "dma.h"
#include "debug.h"
#include "appctx.h"
#include "global.h"
#include "desc_pool.h"
#include "fp_mem.h"

#define ATX_WQ_SIZE     512
__export __shared __cls __align4K struct dma_appctx_cmd_t dma_atx_wq[ATX_WQ_SIZE];

__shared __lmem struct flextcp_pl_appctx_queue_t ctx_queues[FLEXNIC_PL_APPCTX_NUM];
__shared __gpr uint32_t seq;
__shared __gpr uint64_t fp_queue_base;
__shared __gpr uint32_t first_inactive_ctx;

__shared __gpr uint32_t on_queue;
__shared __gpr uint32_t queue_head, queue_tail;
__shared __lmem uint32_t sched_queue[FLEXNIC_PL_APPCTX_NUM];

#define FP_QUEUE_TX_ADDR(IDX)     ((__mem40 void*) (fp_queue_base + (IDX << 6) + offsetof(struct flextcp_pl_appctx_t, tx)))
#define FP_QUEUE_TAIL_ADDR(IDX)   ((__mem40 void*) (fp_queue_base + (IDX << 6) + offsetof(struct flextcp_pl_appctx_t, tx) + offsetof(struct flextcp_pl_appctx_queue_t, p_idx)))

__asm {
  .init_csr mecsr:NNPut 0;
  .init_csr mecsr:NNGet 0;
  .init_csr mecsr:CtxEnables.NextNeighborEmptyAssert 0
}

__forceinline void init_desc_pool()
{
  unsigned int idx;
  unsigned int rnum, raddr_hi;

  rnum = MEM_RING_GET_NUM(atx_desc_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(atx_desc_ring);

  /* DMA_ATX_DESC_POOL_NUM - 1 entries for allocation */
  for (idx = 1; idx < DMA_ATX_DESC_POOL_NUM; idx++) {
    mem_ring_journal_fast(rnum, raddr_hi, idx);
  }
}

__forceinline void read_ctxqueue(int idx)
{
  __xread struct flextcp_pl_appctx_queue_t queue_xfer;
  __mem40 void* fp_queue_ptr;

  fp_queue_ptr = FP_QUEUE_TX_ADDR(idx);
  mem_read32(&queue_xfer, fp_queue_ptr, sizeof(struct flextcp_pl_appctx_queue_t));
  ctx_queues[idx] = queue_xfer;

  return;
}

/** Poll updates to APPCTX on main thread */
__forceinline void poll_inactive_queues()
{
  __xread struct flextcp_pl_appctx_queue_t queue;
  __mem40 void* addr;

  while (first_inactive_ctx < FLEXNIC_PL_APPCTX_NUM) {
    sleep((1 << 20) - 1);

    /* Poll idle context */
    addr = FP_QUEUE_TX_ADDR(first_inactive_ctx);
    mem_read32(&queue, addr, sizeof(struct flextcp_pl_appctx_queue_t));
    ctx_queues[first_inactive_ctx] = queue;

    if (ctx_queues[first_inactive_ctx].len != 0) {
      first_inactive_ctx += 1;
    }
  }

  return;
}

__shared __gpr unsigned int nn_desc_pool;
__forceinline void refresh_local_desc_pool()
{
  unsigned int rnum, raddr_hi;
  __xread unsigned int desc[8];

  rnum = MEM_RING_GET_NUM(atx_desc_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(atx_desc_ring);

  for (;;) {
    while (nn_desc_pool > 64) {
      ctx_swap();
    }

    /* Get 8 descriptors in a batch */
    if (mem_ring_get(rnum, raddr_hi, desc, 8 * sizeof(unsigned int)) != 0)
      continue;

    nn_ring_enqueue_incr(desc[0]);
    nn_ring_enqueue_incr(desc[1]);
    nn_ring_enqueue_incr(desc[2]);
    nn_ring_enqueue_incr(desc[3]);
    nn_ring_enqueue_incr(desc[4]);
    nn_ring_enqueue_incr(desc[5]);
    nn_ring_enqueue_incr(desc[6]);
    nn_ring_enqueue_incr(desc[7]);
    nn_desc_pool += 8;
  }

  return;
}

__forceinline void poll_bump()
{
  __xwrite struct dma_appctx_cmd_t cmd_xfer;
  __gpr struct dma_appctx_cmd_t cmd;

  unsigned int len_mask, head, idx;

  unsigned int desc;

  for (;;) {
    while (nn_desc_pool == 0) {
      STATS_INC(ATX_NODESC);
      ctx_swap();
    }

    desc = nn_ring_dequeue_incr();
    nn_desc_pool -= 1;

    while (queue_head == queue_tail) {
      sleep(50);
    }

    __no_swap_begin();
    idx = sched_queue[queue_head];
    queue_head = (queue_head + 1) & (FLEXNIC_PL_APPCTX_NUM - 1);
    on_queue &= ~(1 << idx);

    head = ctx_queues[idx].c_idx;
    len_mask = ctx_queues[idx].len - 1;
    cmd.desc_idx = desc;
    cmd.seq = seq++;
    cmd.buf_pos = ctx_queues[idx].base_lo + (head << 5);
    ctx_queues[idx].c_idx = (head + 1) & len_mask;
    cmd_xfer = cmd;

    if (ctx_queues[idx].p_idx != ctx_queues[idx].c_idx) {
      sched_queue[queue_tail] = idx;
      queue_tail = (queue_tail + 1) & (FLEXNIC_PL_APPCTX_NUM - 1);
      on_queue |= (1 << idx);
    }
    __no_swap_end();

    cls_workq_add_work(ATX_DMA_RNUM, &cmd_xfer, sizeof(cmd_xfer));
    STATS_INC(ATX);
  }

  return;
}

__forceinline void poll_active_queues()
{
  int idx;
  __xread uint32_t tail;
  __mem40 void* addr;
  int x, y;

  for (;;) {
    for (idx = 1; idx < first_inactive_ctx; idx++) {
      addr = FP_QUEUE_TAIL_ADDR(idx);
      mem_read32(&tail, addr, sizeof(tail));

      __no_swap_begin();
      ctx_queues[idx].p_idx = tail;
      if (((on_queue & (1 << idx)) == 0) && ctx_queues[idx].p_idx != ctx_queues[idx].c_idx) {
        sched_queue[queue_tail] = idx;
        queue_tail = (queue_tail + 1) & (FLEXNIC_PL_APPCTX_NUM - 1);
        on_queue |= (1 << idx);
      }
      __no_swap_end();
    }

    sleep(50);
  }

  return;
}

int main()
{
  SIGNAL start_sig;

  if (ctx() == 0) {
    ctassert(sizeof(struct flextcp_pl_appctx_t) == 64);
    ctassert(sizeof(struct flextcp_pl_appctx_queue_t) == 16);
    ctassert(FLEXNIC_PL_APPCTX_NUM == 32);

    fp_queue_base = (uint64_t) ((__mem40 void*) &fp_state.appctx[0]);
    nn_desc_pool = 0;
    seq = 0;
    first_inactive_ctx = 1;
    queue_head = queue_tail = 0;
    on_queue = 0;

    init_desc_pool();
    cls_workq_setup(ATX_DMA_RNUM, (__cls void*) dma_atx_wq, ATX_WQ_SIZE * sizeof(struct dma_appctx_cmd_t));

    /* Wait for start signal */
    while (1) {
      if (fp_state.cfg.sig != 0)
        break;

      sleep(1 << 10);
    }

    signal_ctx(0, __signal_number(&start_sig));
    signal_ctx(1, __signal_number(&start_sig));
    signal_ctx(2, __signal_number(&start_sig));
    signal_ctx(3, __signal_number(&start_sig));
    signal_ctx(4, __signal_number(&start_sig));
    signal_ctx(5, __signal_number(&start_sig));
    signal_ctx(6, __signal_number(&start_sig));
    signal_ctx(7, __signal_number(&start_sig));
  }

  __implicit_write(&start_sig);
  __wait_for_all(&start_sig);

  if (ctx() == 0) {
    poll_inactive_queues();
  }
  else if (ctx() == 1) {
    refresh_local_desc_pool();
  }
  else if (ctx() == 2) {
    poll_active_queues();
  }
  else {
    poll_bump();
  }

  return 0;
}
