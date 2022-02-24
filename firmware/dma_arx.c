/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/cls.h>
#include <nfp/pcie.h>
#include <nfp/mem_ring.h>

#include "dma.h"
#include "desc_pool.h"
#include "pipeline.h"

#include "global.h"
#include "debug.h"

__shared __gpr uint64_t phyaddr_base;
__shared __gpr uint64_t descpool_base;

__gpr unsigned int dma_cmd_word1;
__shared __gpr unsigned int dma_cmd_word3;

__shared __gpr unsigned int arx_pool_rnum, arx_pool_raddr_hi;

/* Reserve PCIe Resources */
#define DMA_ARX_INFLIGHT_PER_ME (4 * 8)
PCIE_DMA_ALLOC(arx, me, 0, topci_med, DMA_ARX_INFLIGHT_PER_ME);

__intrinsic void
issue_desc_dma(__xread struct dma_appctx_cmd_t* cmd,
              __xwrite struct nfp_pcie_dma_cmd* dma_cmd,
              SIGNAL* sig)
{
  uint64_t nic_addr, host_addr;
  unsigned int desc_idx;

  ctassert(sizeof(struct flextcp_pl_arx_t) == 32);

  desc_idx = cmd->desc_idx;
  nic_addr  = descpool_base + (desc_idx << 5);
  host_addr = phyaddr_base  + cmd->buf_pos;

  issue_dma(DMA_ARX_PCIe_QUEUE, dma_cmd_word1, dma_cmd_word3,
    dma_cmd, __signal_number(sig),
    sizeof(struct flextcp_pl_arx_t),
    (uint32_t) (nic_addr >> 32),  (uint32_t) (nic_addr),
    (uint32_t) (host_addr >> 32), (uint32_t) (host_addr));

  STATS_INC(DMA_ARX_DESC_ISSUE);
}

__intrinsic void
issue_fwd(__gpr struct dma_appctx_cmd_t* cmd)
{
  unsigned int desc_idx;

  /* Enqueue for pre-processing */
  desc_idx = cmd->desc_idx;
  if (desc_idx != 0) {
    mem_ring_journal_fast(arx_pool_rnum, arx_pool_raddr_hi, desc_idx);
    STATS_INC(DMA_ARX_DESC_FWD_FREE);
  }
}

/* Transfer registers */
__xread struct dma_appctx_cmd_t arx_cmd[4];
__xwrite struct nfp_pcie_dma_cmd dma_cmd[4];
SIGNAL arx_sig0, arx_sig1, arx_sig2, arx_sig3;
SIGNAL dma_sig0, dma_sig1, dma_sig2, dma_sig3;

/* Backup commands to issue forward */
__gpr struct dma_appctx_cmd_t arx_backup[4];

#define FETCH(X)    __cls_workq_add_thread(ARX_DMA_RNUM, &arx_cmd[X], sizeof(struct dma_appctx_cmd_t), sig_done, &arx_sig ## X)
#define BACKUP(X)   arx_backup[X] = arx_cmd[X]
#define ISSUE(X)    issue_desc_dma(&arx_cmd[X], &dma_cmd[X], &dma_sig ## X)
#define FORWARD(X)  issue_fwd(&arx_backup[X])

#define WAIT(X, Y)  __wait_for_any(&arx_sig ## X, &dma_sig ## Y)
#define WAIT_CMD(X) __wait_for_any(&arx_sig ## X)
#define WAIT_DMA(Y) __wait_for_any(&dma_sig ## Y)

#define PROC_CMD(X, P, Q) \
                      if (signal_test(&arx_sig ## X)) { \
                        BACKUP(X);  \
                        __implicit_write(dma_cmd);       \
                        __implicit_write(&dma_sig ## X); \
                        ISSUE(X);   \
                        FETCH(X);   \
                        \
                        goto state_ ## P ##_ ## Q;  \
                      }

#define PROC_DMA(X, P, Q) \
                      if (signal_test(&dma_sig ## X)) { \
                        FORWARD(X); \
                        \
                        goto state_ ## P ##_ ## Q;  \
                      }

__forceinline void loop()
{
  /**
   * STATE(P, Q):
   * P -> Next DMA cmd xfer reg index
   * Q -> in-flight DMAs
   */

  /* Kickstart */
  FETCH(0);
  FETCH(1);
  FETCH(2);
  FETCH(3);

state_0_0:
  WAIT_CMD(0);
  PROC_CMD(0, 1, 1);

  __impossible_path();

state_0_1:
  WAIT(0, 3);
  PROC_CMD(0, 1, 2);
  PROC_DMA(3, 0, 0);

  __impossible_path();

state_0_2:
  WAIT(0, 2);
  PROC_CMD(0, 1, 3);
  PROC_DMA(2, 0, 1);

  __impossible_path();

state_0_3:
  WAIT(0, 1);
  PROC_CMD(0, 1, 4);
  PROC_DMA(1, 0, 2);

  __impossible_path();

state_0_4:
  WAIT_DMA(0);
  PROC_DMA(0, 0, 3);

  __impossible_path();

state_1_0:
  WAIT_CMD(1);
  PROC_CMD(1, 2, 1);

  __impossible_path();

state_1_1:
  WAIT(1, 0);
  PROC_CMD(1, 2, 2);
  PROC_DMA(0, 1, 0);

  __impossible_path();

state_1_2:
  WAIT(1, 3);
  PROC_CMD(1, 2, 3);
  PROC_DMA(3, 1, 1);

  __impossible_path();

state_1_3:
  WAIT(1, 2);
  PROC_CMD(1, 2, 4);
  PROC_DMA(2, 1, 2);

  __impossible_path();

state_1_4:
  WAIT_DMA(1);
  PROC_DMA(1, 1, 3);

  __impossible_path();

state_2_0:
  WAIT_CMD(2);
  PROC_CMD(2, 3, 1);

  __impossible_path();

state_2_1:
  WAIT(2, 1);
  PROC_CMD(2, 3, 2);
  PROC_DMA(1, 2, 0);

  __impossible_path();

state_2_2:
  WAIT(2, 0);
  PROC_CMD(2, 3, 3);
  PROC_DMA(0, 2, 1);

  __impossible_path();

state_2_3:
  WAIT(2, 3);
  PROC_CMD(2, 3, 4);
  PROC_DMA(3, 2, 2);

  __impossible_path();

state_2_4:
  WAIT_DMA(2);
  PROC_DMA(2, 2, 3);

  __impossible_path();

state_3_0:
  WAIT_CMD(3);
  PROC_CMD(3, 0, 1);

  __impossible_path();

state_3_1:
  WAIT(3, 2);
  PROC_CMD(3, 0, 2);
  PROC_DMA(2, 3, 0);

  __impossible_path();

state_3_2:
  WAIT(3, 1);
  PROC_CMD(3, 0, 3);
  PROC_DMA(1, 3, 1);

  __impossible_path();

state_3_3:
  WAIT(3, 0);
  PROC_CMD(3, 0, 4);
  PROC_DMA(0, 3, 2);

  __impossible_path();

state_3_4:
  WAIT_DMA(3);
  PROC_DMA(3, 3, 3);

  __impossible_path();
}

int main()
{
  SIGNAL start_sig;

  if (ctx() == 0) {
    dma_cfg_setup(DMA_ARX_CFG_IDX);
    dma_cmd_word3 = DMA_CMD_WORD3(ARX);

    arx_pool_rnum = MEM_RING_GET_NUM(arx_desc_ring);
    arx_pool_raddr_hi = MEM_RING_GET_MEMADDR(arx_desc_ring);

    /* Wait for start signal */
    while (1) {
      if (fp_state.cfg.sig != 0)
        break;

      sleep(1 << 10);
    }

    phyaddr_base  = fp_state.cfg.phyaddr;
    descpool_base = (uint64_t) ((__mem40 void*) &arx_desc_pool[0]);

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

  dma_cmd_word1 = DMA_CMD_WORD1(ARX);

  loop();

  return 0;
}

