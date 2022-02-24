/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/cls.h>
#include <nfp/pcie.h>
#include <nfp/mem_ring.h>
#include <pkt/pkt.h>
#include <blm.h>

#include "dma.h"
#include "msix.h"
#include "appctx.h"
#include "packet_defs.h"
#include "pipeline.h"

#include "global.h"
#include "debug.h"

__gpr unsigned int dma_cmd_word1;
__shared __gpr unsigned int tx_dma_cmd_word1_nosig, tx_dma_cmd_word3;
__shared __gpr unsigned int rx_dma_cmd_word1_nosig, rx_dma_cmd_word3;

__shared __gpr unsigned int tx_dma_rnum, tx_dma_raddr_hi;
__shared __gpr unsigned int rx_dma_rnum, rx_dma_raddr_hi;

#define TM_Q_DST  NS_PLATFORM_NBI_TM_QID_UNTAGGED(PORT_IN_USE, 0)
__shared __gpr struct pkt_ms_info msi_rx;
__shared __gpr struct pkt_ms_info msi_tx;

#define DMA_TX_INFLIGHT  (2 * 2 * 4)
#define DMA_RX_INFLIGHT  (2 * 2 * 4)
/* Reserve PCIe Resources */
PCIE_DMA_ALLOC(tx, me, 0, frompci_lo, DMA_TX_INFLIGHT);
PCIE_DMA_ALLOC(rx, me, 0, topci_lo, DMA_RX_INFLIGHT);

struct fwd_info_t {
  union {
    __packed struct {
      uint32_t rsvd0:2;
      uint32_t cbs:2;                 /*> CTM buffer size as a multiple of 256 */
      uint32_t isl:2;                 /*> NOTE: Valid island numbers are 32, 33, 34, 35. Add +32 to use in NFP APIs */
      uint32_t pnum:10;
      uint32_t seq:16;

      uint32_t rsvd1:1;
      uint32_t bls:2;                 /*> Buffer list of the MU buffer */
      uint32_t muptr:29;              /*> Pointer to the MU buffer >>11 */

      uint32_t rsvd2:16;
      uint32_t seqr:5;                /*> Packet sequencer                 */
      uint32_t plen:11;               /*> Payload length */

      uint32_t desc_idx:16;           /*> Descriptor buffer index          */
      uint32_t db_id:5;               /*> APPCTX doorbell id               */
      uint32_t flags:11;              /*> Flags                            */
    };

    uint32_t __raw[4];
  };
};

__intrinsic void
backup_fwd_info(__xread struct dma_pkt_cmd_t* cmd,
                __gpr   struct fwd_info_t*    fwd)
{
  fwd->__raw[0] = cmd->__raw[0];
  fwd->__raw[1] = cmd->__raw[1];
  fwd->__raw[2] = cmd->__raw[2];
  fwd->__raw[3] = cmd->__raw[7];
}

__intrinsic void
issue_fwd_tx(__gpr struct fwd_info_t* fwd)
{
  /* Send packet over NBI */
  pkt_nbi_send(fwd->isl + 32, fwd->pnum, &msi_tx,
      fwd->plen + NET_HDR_LEN + 4, 0, TM_Q_DST, fwd->seqr, fwd->seq, PKTBUF_CTM_SIZE);
  STATS_INC(DMA_TX_FWD_SEG);
}

__intrinsic void
issue_fwd_rx(__gpr struct fwd_info_t* fwd,
          __xwrite struct nfp_pcie_dma_cmd* dma_cmd,
          SIGNAL* sig)
{
  /* Send packet over NBI */
  if ((fwd->flags & WORK_RESULT_TX) != 0) {
    pkt_nbi_send(fwd->isl + 32, fwd->pnum, &msi_rx,
        NET_HDR_LEN + 4, 0, TM_Q_DST, fwd->seqr, fwd->seq, fwd->cbs);

    STATS_INC(DMA_RX_FWD_ACK);
  } else {
    pkt_nbi_drop_seq(fwd->isl + 32, fwd->pnum, &msi_rx,
        NET_HDR_LEN + 4, 0, 0, fwd->seqr, fwd->seq, fwd->cbs);

    blm_buf_free(fwd->muptr, fwd->bls);
    pkt_ctm_free(fwd->isl + 32, fwd->pnum);

    STATS_INC(DMA_RX_FWD_DROP);
  }

  /* Enqueue ARX descriptor? */
  if ((fwd->flags & WORK_RESULT_DMA_ACDESC) != 0) {
    dma_cmd->__raw[0] = fwd->__raw[3];
    __cls_workq_add_work(ARX_WQ_RNUM, dma_cmd, sizeof(struct arx_cmd_t), sig_done, sig);
    __wait_for_all(sig);

    STATS_INC(DMA_RX_FWD_ARX);
  }
}

#define FETCH_TX(X)    __mem_workq_add_thread(tx_dma_rnum, tx_dma_raddr_hi, &tx_cmd[X], sizeof(struct dma_pkt_cmd_t), sizeof(struct dma_pkt_cmd_t), sig_done, &tx_sig ## X)
#define BACKUP_TX(X)   backup_fwd_info(&tx_cmd[X], &tx_backup[X])
#define ISSUE_TX(X)    issue_pkt_dma_2(DMA_TX_PCIe_QUEUE, &tx_cmd[X], &dma_cmd[X * 2], dma_cmd_word1, tx_dma_cmd_word1_nosig, tx_dma_cmd_word3, &dma_sig ## X)
#define FORWARD_TX(X)  issue_fwd_tx(&tx_backup[X])

#define WAIT_TX(X, Y)  __wait_for_any(&tx_sig ## X, &dma_sig ## Y)
#define WAIT_CMD_TX(X) __wait_for_any(&tx_sig ## X)
#define WAIT_DMA_TX(Y) __wait_for_any(&dma_sig ## Y)

#define PROC_CMD_TX(X, P, Q) \
                      if (signal_test(&tx_sig ## X)) { \
                        BACKUP_TX(X);  \
                        __implicit_write(dma_cmd);   \
                        __implicit_write(&dma_sig ## X);  \
                        ISSUE_TX(X);   \
                        STATS_INC(DMA_TX_PAYLOAD_ISSUE);  \
                        FETCH_TX(X);   \
                        \
                        goto state_ ## P ##_ ## Q;  \
                      }

#define PROC_DMA_TX(X, P, Q) \
                      if (signal_test(&dma_sig ## X)) { \
                        FORWARD_TX(X); \
                        \
                        goto state_ ## P ##_ ## Q;  \
                      }

#define FETCH_RX(X)    __mem_workq_add_thread(rx_dma_rnum, rx_dma_raddr_hi, &rx_cmd[X], sizeof(struct dma_pkt_cmd_t), sizeof(struct dma_pkt_cmd_t), sig_done, &rx_sig ## X)
#define BACKUP_RX(X)   backup_fwd_info(&rx_cmd[X], &rx_backup[X])
#define ISSUE_RX(X)    issue_pkt_dma_2(DMA_RX_PCIe_QUEUE, &rx_cmd[X], &dma_cmd[X * 2], dma_cmd_word1, rx_dma_cmd_word1_nosig, rx_dma_cmd_word3, &dma_sig ## X)
#define FORWARD_RX(X)  issue_fwd_rx(&rx_backup[X], &dma_cmd[X * 2], &dma_sig ## X)

#define WAIT_RX(X, Y)  __wait_for_any(&rx_sig ## X, &dma_sig ## Y)
#define WAIT_CMD_RX(X) __wait_for_any(&rx_sig ## X)
#define WAIT_DMA_RX(Y) __wait_for_any(&dma_sig ## Y)

#define PROC_CMD_RX(X, P, Q) \
                      if (signal_test(&rx_sig ## X)) { \
                        BACKUP_RX(X);  \
                        __implicit_write(dma_cmd);   \
                        __implicit_write(&dma_sig ## X);  \
                        ISSUE_RX(X);   \
                        STATS_INC(DMA_RX_PAYLOAD_ISSUE);  \
                        FETCH_RX(X);   \
                        \
                        goto state_ ## P ##_ ## Q;  \
                      }

#define PROC_DMA_RX(X, P, Q) \
                      if (signal_test(&dma_sig ## X)) { \
                        FORWARD_RX(X); \
                        \
                        goto state_ ## P ##_ ## Q;  \
                      }

__forceinline void loop_tx_single()
{
  /* Transfer registers */
  __xread struct dma_pkt_cmd_t tx_cmd[1];
  __xwrite struct nfp_pcie_dma_cmd dma_cmd[2];
  SIGNAL tx_sig0;
  SIGNAL dma_sig0;

  /* Backup commands to issue forward */
  __gpr struct fwd_info_t tx_backup[1];

  /**
   * STATE(P, Q):
   * P -> Next DMA cmd xfer reg index
   * Q -> in-flight DMAs
   */

  /* Kickstart */
  FETCH_TX(0);

  while (1) {
    __wait_for_all(&tx_sig0);

    BACKUP_TX(0);
    ISSUE_TX(0);
    FETCH_TX(0);

    __wait_for_all(&dma_sig0);
    FORWARD_TX(0);
  }

  return;
}

__forceinline void loop_tx()
{
  /* Transfer registers */
  __xread struct dma_pkt_cmd_t tx_cmd[2];
  __xwrite struct nfp_pcie_dma_cmd dma_cmd[4];
  SIGNAL tx_sig0, tx_sig1;
  SIGNAL dma_sig0, dma_sig1;

  /* Backup commands to issue forward */
  __gpr struct fwd_info_t tx_backup[2];

  /**
   * STATE(P, Q):
   * P -> Next DMA cmd xfer reg index
   * Q -> in-flight DMAs
   */

  /* Kickstart */
  FETCH_TX(0);
  FETCH_TX(1);

state_0_0:
  WAIT_CMD_TX(0);
  PROC_CMD_TX(0, 1, 1);

  __impossible_path();

state_0_1:
  WAIT_TX(0, 1);
  PROC_CMD_TX(0, 1, 2);
  PROC_DMA_TX(1, 0, 0);

  __impossible_path();

state_0_2:
  WAIT_DMA_TX(0);
  PROC_DMA_TX(0, 0, 1);

  __impossible_path();

state_1_0:
  WAIT_CMD_TX(1);
  PROC_CMD_TX(1, 0, 1);

  __impossible_path();

state_1_1:
  WAIT_TX(1, 0);
  PROC_CMD_TX(1, 0, 2);
  PROC_DMA_TX(0, 1, 0);

  __impossible_path();

state_1_2:
  WAIT_DMA_TX(1);
  PROC_DMA_TX(1, 1, 1);

  __impossible_path();
}

__forceinline void loop_rx_single()
{
  /* Transfer registers */
  __xread struct dma_pkt_cmd_t rx_cmd[1];
  __xwrite struct nfp_pcie_dma_cmd dma_cmd[2];
  SIGNAL rx_sig0;
  SIGNAL dma_sig0;

  /* Backup commands to issue forward */
  __gpr struct fwd_info_t rx_backup[1];

  /**
   * STATE(P, Q):
   * P -> Next DMA cmd xfer reg index
   * Q -> in-flight DMAs
   */

  /* Kickstart */
  FETCH_RX(0);

  while (1) {
    __wait_for_all(&rx_sig0);

    BACKUP_RX(0);
    ISSUE_RX(0);
    FETCH_RX(0);

    __wait_for_all(&dma_sig0);
    FORWARD_RX(0);
  }

  return;
}

__forceinline void loop_rx()
{
  /* Transfer registers */
  __xread struct dma_pkt_cmd_t rx_cmd[2];
  __xwrite struct nfp_pcie_dma_cmd dma_cmd[4];
  SIGNAL rx_sig0, rx_sig1;
  SIGNAL dma_sig0, dma_sig1;

  /* Backup commands to issue forward */
  __gpr struct fwd_info_t rx_backup[2];

  /**
   * STATE(P, Q):
   * P -> Next DMA cmd xfer reg index
   * Q -> in-flight DMAs
   */

  /* Kickstart */
  FETCH_RX(0);
  FETCH_RX(1);

state_0_0:
  WAIT_CMD_RX(0);
  PROC_CMD_RX(0, 1, 1);

  __impossible_path();

state_0_1:
  WAIT_RX(0, 1);
  PROC_CMD_RX(0, 1, 2);
  PROC_DMA_RX(1, 0, 0);

  __impossible_path();

state_0_2:
  WAIT_DMA_RX(0);
  PROC_DMA_RX(0, 0, 1);

  __impossible_path();

state_1_0:
  WAIT_CMD_RX(1);
  PROC_CMD_RX(1, 0, 1);

  __impossible_path();

state_1_1:
  WAIT_RX(1, 0);
  PROC_CMD_RX(1, 0, 2);
  PROC_DMA_RX(0, 1, 0);

  __impossible_path();

state_1_2:
  WAIT_DMA_RX(1);
  PROC_DMA_RX(1, 1, 1);

  __impossible_path();
}

/**
 * MSI-X
 *
 * Track last notification timestamp and notify only if it exceeds a delta
 */
__shared __gpr  uint64_t poll_cycle_app;
__shared __lmem uint64_t notify_last_ts[FLEXNIC_PL_APPCTX_NUM];
__export __shared __cls unsigned int dma_appctx_active[FLEXNIC_PL_APPCTX_NUM];

__forceinline void loop_msix()
{
  int i;
  uint64_t ts, diff;
  __xrw unsigned int dma_cnt;

  while (1) {
    for (i = 0; i < FLEXNIC_PL_APPCTX_NUM; i++) {
      sleep(1000);  /* Run every 1000 cycles */

      ts = me_tsc_read();

      /* Check if any DMAs were issued */
      dma_cnt = 0xFFFFFFFF;
      cls_test_clr(&dma_cnt, (__cls void*) &dma_appctx_active[i], sizeof(dma_cnt));

      /* New DMAs to notify */
      if (dma_cnt != 0) {
        diff = ts - notify_last_ts[i];

        /* Only notify after poll_cycle_app cycles */
        if (diff >= poll_cycle_app) {
          msix_appctx_intr(i);
          notify_last_ts[i] = ts;
        }
      }
    }
  }
}

int main()
{
  SIGNAL start_sig;

  if (ctx() == 0) {
    tx_dma_cmd_word3 = DMA_CMD_WORD3(TX);
    tx_dma_cmd_word1_nosig = DMA_CMD_WORD1_NOSIG(TX);
    rx_dma_cmd_word3 = DMA_CMD_WORD3(RX);
    rx_dma_cmd_word1_nosig = DMA_CMD_WORD1_NOSIG(RX);

    tx_dma_rnum = MEM_RING_GET_NUM(dma_tx_ring_excep);
    tx_dma_raddr_hi = MEM_RING_GET_MEMADDR(dma_tx_ring_excep);
    rx_dma_rnum = MEM_RING_GET_NUM(dma_rx_ring_excep);
    rx_dma_raddr_hi = MEM_RING_GET_MEMADDR(dma_rx_ring_excep);

    msi_rx = pkt_gen_msi_info(PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 4);
    msi_tx = pkt_gen_msi_info(PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2);

    /* Wait for start signal */
    while (1) {
      if (fp_state.cfg.sig != 0)
        break;

      sleep(1 << 10);
    }

    /* Init MSI-X after start signal to make sure MSI-X table is initialized */
    msix_init();
    poll_cycle_app = fp_state.cfg.poll_cycle_app;

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
  case 1:
  case 2:
    dma_cmd_word1 = DMA_CMD_WORD1(TX);
#if DMA_ONE_PKT_AT_A_TIME
    loop_tx_single();
#else
    loop_tx();
#endif
    break;

  case 3:
  case 4:
  case 5:
    dma_cmd_word1 = DMA_CMD_WORD1(RX);
#if DMA_ONE_PKT_AT_A_TIME
    loop_rx_single();
#else
    loop_rx();
#endif
    break;

  case 6:
    loop_msix();
    break;

  case 7:
    /* Nothing to do */
    break;

  default:
    __impossible_path();
    break;
  }

  return 0;
}
