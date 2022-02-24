/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp/me.h>
#include <nfp/cls.h>
#include <nfp/mem_ring.h>
#include <nfp/mem_atomic.h>

#include "qman.h"
#include "fp_mem.h"
#include "pipeline.h"

#include "shared.h"
#include "debug.h"
#include "global.h"

#define QM_LOCAL_FIFO   256
__shared __lmem uint32_t queue_status[QM_NUM_SLOTS];
__shared __lmem uint32_t queue_local_fifo[QM_LOCAL_FIFO][2];

__shared __gpr unsigned int queue_status_fifo;
__shared __gpr unsigned int qraddr;
__shared __gpr unsigned int credits;

__shared __gpr unsigned int current_slot, skip_slot;
__shared __gpr unsigned int q_head, q_tail;

/* Does not swap out! */
__forceinline void flow_add_to_queue(unsigned int flow_id, uint32_t avail, uint32_t rate)
{
  unsigned int slot, queue, secondary, qword, qbit;
  unsigned int new_qtail;
  unsigned int ts, chunk;

  /* Add to correct queue */
  if (rate == 0) {  /* Add to fifo queue */
    new_qtail = (q_tail + 1) & (QM_LOCAL_FIFO - 1);
    if (avail < TCP_MSS || (q_head == new_qtail)) { /* Add to FIFO queue */
      queue_status_fifo += 1;
      mem_ring_journal_fast(QM_FIFO_RNUM, qraddr, flow_id);
    }
    else {  /* Add to local queue */
      queue_local_fifo[q_tail][0] = flow_id;
      queue_local_fifo[q_tail][1] = MIN(avail, 0x4000);
      q_tail = new_qtail;
    }
  }
  else {  /* Add to rate limit queue */
    /* rate is stored as clk cnts for transmitting 1 byte */
    chunk = MIN(TCP_MSS, avail);
    ts = (rate * chunk) >> 10; // Measured in 1024 sub-cycles!
    slot = (ts >> QM_SLOT_RESOLUTION_TS_CYC_SHIFT) + skip_slot;
    if (slot >= QM_NUM_SLOTS) {
      slot = QM_NUM_SLOTS - 1;
    }

    /* Add to secondary queue */
    slot = (current_slot + slot) & (QM_NUM_SLOTS - 1);
    queue = QM_SLOT_RNUM_BASE + slot;
    queue_status[slot] += 1;

    mem_ring_journal_fast(queue, qraddr, flow_id);
  }
}

__forceinline void queue_postproc_poll()
{
  unsigned int rnum, raddr_hi, flow_id, flow_id_grp;
  unsigned int slot, queue, secondary, qword, qbit;
  __xread unsigned int flow_id_xfer;
  __xread uint32_t cc[2];
  SIGNAL sig;

  __mem40 uint32_t* addr;

  rnum = MEM_RING_GET_NUM(qm_bump_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(qm_bump_ring);

  __mem_workq_add_thread(rnum, raddr_hi, &flow_id_xfer,
                        sizeof(unsigned int), sizeof(unsigned int),
                        sig_done, &sig);
  for (;;)
  {
    /* Read flow_id */
    __wait_for_all(&sig);
    flow_id_grp = flow_id_xfer;
    flow_id = (flow_id_grp & 0xFFFF);

    addr = (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_avail;

    /* Issue next read */
    __mem_workq_add_thread(rnum, raddr_hi, &flow_id_xfer,
                          sizeof(unsigned int), sizeof(unsigned int),
                          sig_done, &sig);

    /* Read t_avail & tx_rate */
    mem_read_atomic(cc, addr, 2 * sizeof(uint32_t));

    /* Add to correct queue */
    flow_add_to_queue(flow_id_grp, cc[0], cc[1]);
  }

  return;
}

__forceinline void refresh_credits()
{
  __xrw uint32_t update_credits;

  update_credits = MIN(FLOW_QM_CREDITS/8, 128);
  mem_test_subsat(&update_credits, (__mem40 void*) &qm_credits, sizeof(update_credits));

  credits += update_credits;
  return;
}

#define MIN_SLOT_CYCLES 8
__forceinline void select_slot()
{
  unsigned int slot_ts, next_ts, cyc, deficit;

  slot_ts = local_csr_read(local_csr_timestamp_low);
  cyc     = QM_SLOT_RESOLUTION_TS_CLK_CNT;
  next_ts = slot_ts + cyc;
  deficit = 0;
  skip_slot = 0;

  for (;;) {
    /* 16 cycles per cnt */
    sleep(cyc << 4);

    /* Wait until all flows from the current slot are dequeued! */
    skip_slot = 1;  /*> Prevent adding more flows to the current slot */
    while (queue_status[current_slot] != 0) {
      sleep(50);
    }

    /* Goto next slot */
    current_slot = (current_slot + 1) & (QM_NUM_SLOTS - 1);
    slot_ts = local_csr_read(local_csr_timestamp_low);
    deficit = slot_ts - next_ts;
    if (deficit >= QM_SLOT_RESOLUTION_TS_CLK_CNT) {
      deficit = QM_SLOT_RESOLUTION_TS_CLK_CNT - MIN_SLOT_CYCLES;
    }
    cyc = QM_SLOT_RESOLUTION_TS_CLK_CNT - deficit;
    next_ts = slot_ts + cyc;
    skip_slot = 0;
  }
}

__forceinline void queue_localfifo_poll()
{
  unsigned int flow_id, local_avail, flow_id_grp, flow_grp;
  __mem40 void* avail_addr;

  for (;;)
  {
    if (q_head == q_tail) {
      sleep(100);
      continue;
    }

    /* TX Queue is full! */
    while (credits == 0) {
      sleep(100);
      refresh_credits();
    }

    flow_id_grp = queue_local_fifo[q_head][0];
    local_avail = queue_local_fifo[q_head][1];
    q_head = (q_head + 1) & (QM_LOCAL_FIFO - 1);

    flow_id = flow_id_grp & 0xFFFF;
    flow_grp = (flow_id_grp >> 16) & (NUM_FLOW_GROUPS - 1);

    /* Schedule flow */
    credits -= 1;
    mem_workq_add_work_imm(QM_SCHED_RNUM_BASE + flow_grp, qraddr, flow_id);

    avail_addr = (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_avail;
    mem_subsat32_imm(TCP_MSS, avail_addr);
    STATS_INC(QM_SCHEDULE);

    local_avail -= TCP_MSS; /* Guaranteed to be > TCP_MSS */
    flow_add_to_queue(flow_id, local_avail, 0);

    if (credits < (FLOW_QM_CREDITS/4)) {
      refresh_credits();
    }
  }
}


__forceinline void queue_fifo_poll()
{
  __xread uint32_t queue_flows[4];
  __xread uint32_t rate;
  __xrw uint32_t avail;
  SIGNAL rate_sig;
  SIGNAL_PAIR avail_sig;

  __mem40 uint32_t* avail_addr;
  __mem40 uint32_t* rate_addr;

  unsigned int idx, flow_id, flow_id_grp, flow_grp;

  for (;;)
  {
    if (queue_status_fifo == 0) {
      sleep(100);
      continue;
    }

    /* Read upto 4 flows */
    mem_ring_get_freely(QM_FIFO_RNUM, qraddr, queue_flows, sizeof(queue_flows));

    for (idx = 0; idx < 4; idx++)
    {
      if (queue_flows[idx] == 0)
        break;

      /* TX Queue is full! */
      while (credits == 0) {
        sleep(100);
        refresh_credits();
      }

      queue_status_fifo -= 1;
      flow_id_grp = queue_flows[idx];
      flow_id = flow_id_grp & 0xFFFF;
      flow_grp = (flow_id_grp >> 16) & (NUM_FLOW_GROUPS - 1);

      /* Schedule flow */
      credits -= 1;
      mem_workq_add_work_imm(QM_SCHED_RNUM_BASE + flow_grp, qraddr, flow_id);
      STATS_INC(QM_SCHEDULE);

      /* rate, avail in consecutive locations */
      avail_addr = (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_avail;
      rate_addr  = avail_addr + 1;

      /* Subtract TCP_MSS from queue occupancy */
      avail = TCP_MSS;
      __mem_test_subsat(&avail, avail_addr, sizeof(avail), sizeof(avail), sig_done, &avail_sig);
      __mem_read_atomic(&rate, rate_addr, sizeof(rate), sizeof(rate), sig_done, &rate_sig);
      __wait_for_all(&avail_sig, &rate_sig);

      /* Data available! */
      if (avail > TCP_MSS) {
        /* Add to correct queue */
        flow_add_to_queue(flow_id_grp, avail - TCP_MSS, rate);
      }

      if (credits < (FLOW_QM_CREDITS/4)) {
        refresh_credits();
      }
    }
  }

  return;
}

__forceinline void queue_slot_poll()
{
  __xread uint32_t queue_flows;
  __xread uint32_t rate;
  __xrw uint32_t avail;
  SIGNAL rate_sig;
  SIGNAL_PAIR avail_sig;

  __mem40 uint32_t* avail_addr;
  __mem40 uint32_t* rate_addr;

  unsigned int idx, queue, flow_id, flow_id_grp, flow_grp;

  for (;;) {
    if (queue_status[current_slot] == 0) {
      sleep(100);
      continue;
    }

    /* Dequeue one element */
    queue_status[current_slot] -= 1;
    queue = QM_SLOT_RNUM_BASE + current_slot;
    mem_ring_get_freely(queue, qraddr, &queue_flows, sizeof(queue_flows));

    if (queue_flows == 0)
      continue;

    /* TX Queue is full! */
    while (credits == 0) {
      sleep(100);
      refresh_credits();
    }

    flow_id_grp = queue_flows;
    flow_id = flow_id_grp & 0xFFFF;
    flow_grp = (flow_id_grp >> 16) & (NUM_FLOW_GROUPS - 1);

    /* Schedule flow */
    credits -= 1;
    mem_workq_add_work_imm(QM_SCHED_RNUM_BASE + flow_grp, qraddr, flow_id);
    STATS_INC(QM_SCHEDULE);

    /* rate, avail in consecutive locations */
    avail_addr = (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_avail;
    rate_addr  = avail_addr + 1;

    /* Subtract TCP_MSS from queue occupancy */
    avail = TCP_MSS;
    __mem_test_subsat(&avail, avail_addr, sizeof(avail), sizeof(avail), sig_done, &avail_sig);
    __mem_read_atomic(&rate, rate_addr, sizeof(rate), sizeof(rate), sig_done, &rate_sig);
    __wait_for_all(&avail_sig, &rate_sig);

    /* Data available! */
    if (avail > TCP_MSS) {
      /* Add to correct queue */
      flow_add_to_queue(flow_id_grp, avail - TCP_MSS, rate);
    }

    if (credits < (FLOW_QM_CREDITS/4)) {
      refresh_credits();
    }
  }
  return;
}

int main()
{
  SIGNAL start_sig;

  if (ctx() == 0) {
    enable_global_timestamp();

    queue_status_fifo = 0;
    q_head = q_tail = 0;
    qraddr = MEM_RING_GET_MEMADDR(qm_slot_fifo);

    credits = 3*FLOW_QM_CREDITS/4 - 1;

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

  switch (ctx()) {
  case 0:
    queue_postproc_poll();
    break;

  case 1:
    select_slot();
    break;

  case 2:
    queue_localfifo_poll();
    break;

  case 3:
    queue_fifo_poll();
    break;

  default:
    queue_slot_poll();
    break;
  }

  return 0;
}
