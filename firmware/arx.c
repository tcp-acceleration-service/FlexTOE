/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdint.h>

#include <nfp.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp/cls.h>

#include "dma.h"
#include "fp_mem.h"
#include "desc_pool.h"
#include "global.h"
#include "appctx.h"
#include "debug.h"

__export __shared __cls unsigned int dma_appctx_active[FLEXNIC_PL_APPCTX_NUM];

#define ARX_WQ_SIZE     512
__export __shared __cls __align4K struct dma_appctx_cmd_t dma_arx_wq[ARX_WQ_SIZE];
__export __shared __cls __align2K struct arx_cmd_t arx_wq[ARX_WQ_SIZE];

__shared __lmem struct flextcp_pl_appctx_queue_t ctx_queues[FLEXNIC_PL_APPCTX_NUM];

__forceinline void init_desc_pool()
{
  unsigned int idx;
  unsigned int rnum, raddr_hi;

  rnum = MEM_RING_GET_NUM(arx_desc_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(arx_desc_ring);

  /* DMA_ATX_DESC_POOL_NUM - 1 entries for allocation */
  for (idx = 1; idx < DMA_ARX_DESC_POOL_NUM; idx++) {
    mem_ring_journal_fast(rnum, raddr_hi, idx);
  }
}

__forceinline void read_ctxqueue(int idx)
{
  __xread struct flextcp_pl_appctx_queue_t queue_xfer;
  __mem40 void* fp_queue_ptr;
  __gpr unsigned int avail;

  if (ctx_queues[idx].len == 0)
    goto update_local_copy;

  if (ctx_queues[idx].p_idx >= ctx_queues[idx].c_idx)
    avail = (ctx_queues[idx].len - ctx_queues[idx].p_idx) + ctx_queues[idx].c_idx - 1;
  else
    avail = (ctx_queues[idx].c_idx - ctx_queues[idx].p_idx - 1);

  /* > 25% of ctx queue is empty! */
  if (avail > (ctx_queues[idx].len >> 2))
    return;

update_local_copy:

  fp_queue_ptr = (__mem40 void*) &fp_state.appctx[idx].rx;
  mem_read32(&queue_xfer, fp_queue_ptr, sizeof(struct flextcp_pl_appctx_queue_t));
  if (ctx_queues[idx].len > 0) {
    ctx_queues[idx].c_idx = queue_xfer.c_idx;
  }
  else
  {
    ctx_queues[idx] = queue_xfer;
  }
  __implicit_read(&queue_xfer);
}

/** Poll updates to APPCTX on main thread */
static void poll_ctxqueues()
{
  int idx;
  ctassert(FLEXNIC_PL_APPCTX_NUM == 32);

  for (;;) {
    /* This is a low priority task */
    sleep((1 << 16));  /*> Has to be less than (1 << 20) */

    for (idx = 0; idx < FLEXNIC_PL_APPCTX_NUM; idx++)
    {
      read_ctxqueue(idx);
    }
  }

  __impossible_path();
}

__intrinsic void prepare_dma_cmd(__xwrite struct dma_appctx_cmd_t* cmd,
                                unsigned int desc_idx, unsigned int db)
{
  __gpr uint32_t mask, p_idx, c_idx, updated_p_idx;
  __gpr struct dma_appctx_cmd_t cmd_temp;

  ctassert(sizeof(struct flextcp_pl_arx_t) == 32);

  mask = ctx_queues[db].len - 1;
#if 0
  while (1) {
    p_idx = ctx_queues[db].p_idx;
    c_idx = ctx_queues[db].c_idx;

    updated_p_idx = (p_idx + 1) & mask;
    if (updated_p_idx != c_idx)
      break;

    ctx_swap();
  }
#endif
  p_idx = ctx_queues[db].p_idx;
  updated_p_idx = (p_idx + 1) & mask;
  ctx_queues[db].p_idx = updated_p_idx;

  cmd_temp.desc_idx = desc_idx;
  cmd_temp.buf_pos  = ctx_queues[db].base_lo + (p_idx << 5);

  *cmd = cmd_temp;
}

__forceinline void poll_arx()
{
  /* Block 0 */
  __xread  struct arx_cmd_t     rd0;
  __xwrite struct dma_appctx_cmd_t wr0;
  SIGNAL rd_sig0, wr_sig0;

  unsigned int db;

  __cls_workq_add_thread(ARX_WQ_RNUM, &rd0, sizeof(rd0), sig_done, &rd_sig0);
  signal_ctx(ctx(), __signal_number(&wr_sig0));
  __implicit_write(&wr_sig0);

  for (;;)
  {
    __wait_for_all(&rd_sig0, &wr_sig0);

    db = rd0.db_id;
    prepare_dma_cmd(&wr0, rd0.desc_idx, db);
    cls_incr((__cls void*) &dma_appctx_active[db]); /* Mark APPCTX as active for MSI-X */

    __cls_workq_add_thread(ARX_WQ_RNUM, &rd0, sizeof(rd0), sig_done, &rd_sig0);
    __cls_workq_add_work(ARX_DMA_RNUM, &wr0, sizeof(wr0), sig_done, &wr_sig0);

    STATS_INC(ARX);

    __implicit_write(&wr0);
    __implicit_read(&rd0);
  }

  return;
}

int main()
{
  SIGNAL start_sig;

  if (ctx() == 0) {

    cls_workq_setup(ARX_WQ_RNUM, (__cls void*) arx_wq, ARX_WQ_SIZE * sizeof(struct arx_cmd_t));
    cls_workq_setup(ARX_DMA_RNUM, (__cls void*) dma_arx_wq, ARX_WQ_SIZE * sizeof(struct dma_appctx_cmd_t));

    init_desc_pool();

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

  switch (ctx())
  {
  case 0:
    poll_ctxqueues();
    break;

  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    poll_arx();
    break;

  default:
    __impossible_path();
  }

  __impossible_path();
  return 0;
}

