/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/pcie.h>
#include <nfp/mem_ring.h>
#include <pkt/pkt.h>

#include "dma.h"
#include "packet_defs.h"
#include "pipeline.h"

#include "global.h"
#include "debug.h"

__gpr unsigned int dma_cmd_word1;
__shared __gpr unsigned int dma_cmd_word3;

__shared __gpr unsigned int tx_dma_rnum, tx_dma_raddr_hi;

#define TM_Q_DST  NS_PLATFORM_NBI_TM_QID_UNTAGGED(PORT_IN_USE, 0)
__shared __gpr struct pkt_ms_info msi;


#define DMA_TX_INFLIGHT_PER_ME  (4 * 8)
/* Reserve PCIe Resources */
PCIE_DMA_ALLOC(tx, me, 0, frompci_lo, DMA_TX_INFLIGHT_PER_ME);

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
issue_fwd(__gpr struct fwd_info_t* fwd)
{
  /* Send packet over NBI */
  pkt_nbi_send(fwd->isl + 32, fwd->pnum, &msi,
      fwd->plen + NET_HDR_LEN + 4, 0, TM_Q_DST, fwd->seqr, fwd->seq, PKTBUF_CTM_SIZE);
  STATS_INC(DMA_TX_FWD_SEG);
}

__intrinsic void
backup_fwd_info(__xread struct dma_pkt_cmd_t* cmd,
                __gpr   struct fwd_info_t*    fwd)
{
  fwd->__raw[0] = cmd->__raw[0];
  fwd->__raw[1] = cmd->__raw[1];
  fwd->__raw[2] = cmd->__raw[2];
  fwd->__raw[3] = cmd->__raw[7];
}

#define FETCH(X)    __mem_workq_add_thread(tx_dma_rnum, tx_dma_raddr_hi, &tx_cmd[X], sizeof(struct dma_pkt_cmd_t), sizeof(struct dma_pkt_cmd_t), sig_done, &tx_sig ## X)
#define BACKUP(X)   backup_fwd_info(&tx_cmd[X], &tx_backup[X])
#define ISSUE(X)    issue_pkt_dma(DMA_TX_PCIe_QUEUE, &tx_cmd[X], &dma_cmd[X], dma_cmd_word1, dma_cmd_word3, &dma_sig ## X)
#define FORWARD(X)  issue_fwd(&tx_backup[X])

#define WAIT(X, Y)  __wait_for_any(&tx_sig ## X, &dma_sig ## Y)
#define WAIT_CMD(X) __wait_for_any(&tx_sig ## X)
#define WAIT_DMA(Y) __wait_for_any(&dma_sig ## Y)

#define PROC_CMD(X, P, Q) \
                      if (signal_test(&tx_sig ## X)) { \
                        BACKUP(X);  \
                        __implicit_write(dma_cmd);   \
                        __implicit_write(&dma_sig ## X);  \
                        ISSUE(X);   \
                        STATS_INC(DMA_TX_PAYLOAD_ISSUE);  \
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

__forceinline void loop_single()
{
  /* Transfer registers */
  __xread struct dma_pkt_cmd_t tx_cmd[1];
  __xwrite struct nfp_pcie_dma_cmd dma_cmd[1];
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
  FETCH(0);

  while (1) {
    __wait_for_all(&tx_sig0);

    BACKUP(0);
    ISSUE(0);
    FETCH(0);

    __wait_for_all(&dma_sig0);
    FORWARD(0);
  }

  return;
}

__forceinline void loop()
{
  /* Transfer registers */
  __xread struct dma_pkt_cmd_t tx_cmd[4];
  __xwrite struct nfp_pcie_dma_cmd dma_cmd[4];
  SIGNAL tx_sig0, tx_sig1, tx_sig2, tx_sig3;
  SIGNAL dma_sig0, dma_sig1, dma_sig2, dma_sig3;

  /* Backup commands to issue forward */
  __gpr struct fwd_info_t tx_backup[4];

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
    dma_cfg_setup(DMA_TX_CFG_IDX);
    dma_cmd_word3 = DMA_CMD_WORD3(TX);

    tx_dma_rnum = MEM_RING_GET_NUM(dma_tx_ring);
    tx_dma_raddr_hi = MEM_RING_GET_MEMADDR(dma_tx_ring);

    msi = pkt_gen_msi_info(PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2);

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

  dma_cmd_word1 = DMA_CMD_WORD1(TX);

#if DMA_ONE_PKT_AT_A_TIME
  loop_single();
#else
  loop();
#endif

  return 0;
}
