/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef DMA_H_
#define DMA_H_

#include <nfp.h>
#include <nfp/pcie.h>
#include <nfp/mem_ring.h>
#include <pkt/pkt.h>

#include "pipeline.h"
#include "desc_pool.h"

/** DMA reservations */
#define DMA_ATX_CFG_IDX         0
#define DMA_ATX_CPP_TOKEN       2       /*> To prevent byteswap */
#define DMA_ATX_RID             0
#define DMA_ATX_TRAN_CLASS      0
#define DMA_ATX_PCIe_QUEUE      NFP_PCIE_DMA_FROMPCI_MED
#define DMA_ATX_PCIe_INFLIGHT   32      /*> Must be less than 64 */
#define DMA_ARX_CFG_IDX         1
#define DMA_ARX_CPP_TOKEN       2       /*> To prevent byteswap */
#define DMA_ARX_RID             0
#define DMA_ARX_TRAN_CLASS      0
#define DMA_ARX_PCIe_QUEUE      NFP_PCIE_DMA_TOPCI_MED
#define DMA_ARX_PCIe_INFLIGHT   64      /*> Must be less than 64 */
#define DMA_TX_CFG_IDX          2
#define DMA_TX_CPP_TOKEN        2       /*> To prevent byteswap */
#define DMA_TX_RID              0
#define DMA_TX_TRAN_CLASS       0
#define DMA_TX_PCIe_QUEUE       NFP_PCIE_DMA_FROMPCI_LO
#define DMA_TX_PCIe_INFLIGHT    120     /*> Must be less than 128 */
#define DMA_RX_CFG_IDX          3
#define DMA_RX_CPP_TOKEN        2       /*> To prevent byteswap */
#define DMA_RX_RID              0
#define DMA_RX_TRAN_CLASS       0
#define DMA_RX_PCIe_QUEUE       NFP_PCIE_DMA_TOPCI_LO
#define DMA_RX_PCIe_INFLIGHT    120     /*> Must be less than 128 */
#define DMA_SPTX_CFG_IDX        4
#define DMA_SPTX_CPP_TOKEN      2       /*> To prevent byteswap */
#define DMA_SPTX_RID            0
#define DMA_SPTX_TRAN_CLASS     0
#define DMA_SPTX_PCIe_QUEUE     NFP_PCIE_DMA_FROMPCI_HI
#define DMA_SPTX_PCIe_INFLIGHT  8       /*> Must be less than 64 */
#define DMA_SPRX_CFG_IDX        5
#define DMA_SPRX_CPP_TOKEN      2       /*> To prevent byteswap */
#define DMA_SPRX_RID            0
#define DMA_SPRX_TRAN_CLASS     0
#define DMA_SPRX_PCIe_QUEUE     NFP_PCIE_DMA_TOPCI_HI
#define DMA_SPRX_PCIe_INFLIGHT  8       /*> Must be less than 64 */

/*
 * DMA Descriptor: signal only
 *
 * Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Word  +---------------------------------------------------------------+
 *    0  |                        CPP Address Low                        |
 *       +-+-------+-----------+-----+-------+---+-------+---------------+
 *    1  |0| MstID |Master Isl | Ctx |Signum |Tok|DMACFGI|  CPP Addr Hi  |
 *       +-+-------+-----------+-----+-------+---+-------+---------------+
 *    2  |                       PCIE Address Low                        |
 *       +-----------------------+---------------+-+-----+---------------+
 *    3  |   Transfer Length     | Requester ID  |O| XC  | PCIe Addr Hi  |
 *       +-----------------------+---------------+-+-----+---------------+
 */
#define DMA_CMD_WORD1(_MOD) \
          ((NFP_PCIE_DMA_CMD_DMA_CFG_INDEX(DMA_ ## _MOD ## _CFG_IDX)) | \
           (NFP_PCIE_DMA_CMD_CPP_TOKEN(DMA_ ## _MOD ## _CPP_TOKEN))   | \
           (((((__ctx() & 0x7) << 4) | (((__MEID >> 4) & 0x3F) << 7) | ((__MEID & 0xF) << 13)) << NFP_PCIE_DMA_CMD_DMA_MODE_shf)))

#define DMA_CMD_WORD1_NOSIG(_MOD) \
          ((NFP_PCIE_DMA_CMD_DMA_CFG_INDEX(DMA_ ## _MOD ## _CFG_IDX)) | \
           (NFP_PCIE_DMA_CMD_CPP_TOKEN(DMA_ ## _MOD ## _CPP_TOKEN)))

#define DMA_CMD_WORD3(_MOD) \
            ((NFP_PCIE_DMA_CMD_RID(DMA_ ## _MOD ## _RID)) | \
             (NFP_PCIE_DMA_CMD_TRANS_CLASS(DMA_ ## _MOD ## _TRAN_CLASS)))

MEM_RING_INIT(dma_tx_ring, DMA_TX_WQ_SIZE);
MEM_RING_INIT(dma_tx_ring_excep, DMA_TX_WQ_SIZE);
MEM_RING_INIT(dma_rx_ring, DMA_RX_WQ_SIZE);
MEM_RING_INIT(dma_rx_ring_excep, DMA_RX_WQ_SIZE);

struct dma_pkt_cmd_t {
  union {
    __packed struct  {
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

      uint32_t param1;
      uint32_t param2;
      uint32_t param3;
      uint32_t param4;

      uint32_t desc_idx:16;           /*> Descriptor buffer index          */
      uint32_t db_id:5;               /*> APPCTX doorbell id               */
      uint32_t flags:11;              /*> Flags                            */
    };

    uint32_t __raw[8];
  };
};

/* ARX/ATX -> DMA command */
struct dma_appctx_cmd_t {
  union {
    __packed struct {
      uint32_t desc_idx:16;     /*> Descriptor buffer index          */
      uint32_t seq:16;          /*> Sequence number                  */

      uint32_t buf_pos;         /*> Offset in the Host buffer to DMA */
    };

    uint32_t __raw[2];
  };
};

/**
 * CLS ring configurations
 * (a)  ARX -> DMA
 * (b)  ATX -> DMA
 * (c)  DMA -> FWD
 */
#define ARX_DMA_RNUM    1       /*> CLS Ring number for ARX -> DMA WQ */
#define ATX_DMA_RNUM    2       /*> CLS Ring number for ATX -> DMA WQ */
#define DMA_FWD_RNUM    3       /*> CLS Ring number for DMA -> FWD    */

/** DMA utilities */

/** Configure the PCIe DMA configuration register */
/* NOTE: Using the same configuration for all traffic classes */
__intrinsic void dma_cfg_setup(int idx)
{
  struct pcie_dma_cfg_one cfg;

  ctassert(__is_ct_const(idx));

  cfg.__raw = 0;
  cfg.target_64 = 1;
  cfg.cpp_target = 7;

  pcie_dma_cfg_set_one(0, idx, cfg);
}

__intrinsic void
prepare_dma_desc(unsigned int word1, unsigned int word3,
          __xwrite struct nfp_pcie_dma_cmd *cmd, int signo,
          uint32_t len,
          uint32_t dev_addr_hi,  uint32_t dev_addr_lo,
          uint32_t pcie_addr_hi, uint32_t pcie_addr_lo)
{
  /* Word0 is CPP_ADDR_LO */
  cmd->__raw[0] = NFP_PCIE_DMA_CMD_CPP_ADDR_LO(dev_addr_lo);

  /* Word1 -- only modify SIGNAL & CPP_ADDR_HI */
  cmd->__raw[1] = word1 | NFP_PCIE_DMA_CMD_DMA_MODE(signo) | NFP_PCIE_DMA_CMD_CPP_ADDR_HI(dev_addr_hi);

  /* Word2 is PCIE_ADDR_LO */
  cmd->__raw[2] = NFP_PCIE_DMA_CMD_PCIE_ADDR_LO(pcie_addr_lo);

  /* Word4 -- only modify LENGTH & PCIE_ADDR_HI */
  cmd->__raw[3] = word3 | NFP_PCIE_DMA_CMD_LENGTH(len - 1) | NFP_PCIE_DMA_CMD_PCIE_ADDR_HI(pcie_addr_hi);
}

__intrinsic void
issue_dma(unsigned int queue, unsigned int word1, unsigned int word3,
          __xwrite struct nfp_pcie_dma_cmd *cmd, int signo,
          uint32_t len,
          uint32_t dev_addr_hi,  uint32_t dev_addr_lo,
          uint32_t pcie_addr_hi, uint32_t pcie_addr_lo)
{
  prepare_dma_desc(word1, word3, cmd, signo, len, dev_addr_hi, dev_addr_lo, pcie_addr_hi, pcie_addr_lo);
  pcie_dma_enq_no_sig(0, cmd, queue);
}

__intrinsic void
issue_dma_nosig(unsigned int queue, unsigned int word1, unsigned int word3,
                __xwrite struct nfp_pcie_dma_cmd *cmd,
                uint32_t len,
                uint32_t dev_addr_hi,  uint32_t dev_addr_lo,
                uint32_t pcie_addr_hi, uint32_t pcie_addr_lo)
{
  prepare_dma_desc(word1, word3, cmd, 0, len, dev_addr_hi, dev_addr_lo, pcie_addr_hi, pcie_addr_lo);
  pcie_dma_enq_no_sig(0, cmd, queue);
}

__intrinsic void
issue_pkt_dma(unsigned int queue,
              __xread struct dma_pkt_cmd_t* cmd,
              __xwrite struct nfp_pcie_dma_cmd* dma_cmd,
              unsigned int word1, unsigned int word3,
              SIGNAL* sig)
{
  unsigned int signo = __signal_number(sig);

  if ((cmd->flags & WORK_RESULT_DMA_PAYLOAD) == 0) {
    signal_ctx(ctx(), signo);
    return;
  }

  dma_cmd->__raw[0] = cmd->param1;
  dma_cmd->__raw[1] = word1 | NFP_PCIE_DMA_CMD_DMA_MODE(signo) | cmd->param2;
  dma_cmd->__raw[2] = cmd->param3;
  dma_cmd->__raw[3] = word3 | cmd->param4;

  pcie_dma_enq_no_sig(0, dma_cmd, queue);
}

__intrinsic void
issue_pkt_dma_2(unsigned int queue,
                __xread struct dma_pkt_cmd_t* cmd,
                __xwrite struct nfp_pcie_dma_cmd dma_cmd[2],
                unsigned int word1, unsigned int word1_nosig, unsigned int word3,
                SIGNAL* sig)
{
  unsigned int isl, pkt_addr_hi, len1, len2;
  unsigned int signo = __signal_number(sig);

  if ((cmd->flags & WORK_RESULT_DMA_PAYLOAD) == 0) {
    signal_ctx(ctx(), signo);
    return;
  }

  // refer to pkt_ctm_ptr40
  isl = cmd->isl + 32;
  pkt_addr_hi = 0x80 | isl;
  len1 = (cmd->param4 >> 20) + 1;
  len2 = (cmd->param4 >> 8) & 0xFFF;

  dma_cmd[0].__raw[0] = cmd->param1;
  dma_cmd[0].__raw[1] = word1_nosig | pkt_addr_hi;
  dma_cmd[0].__raw[2] = cmd->param3 + cmd->param2;
  dma_cmd[0].__raw[3] = (cmd->param4 & 0xFFF000FF) | word3;

  dma_cmd[1].__raw[0] = cmd->param1 + len1;
  dma_cmd[1].__raw[1] = word1 | NFP_PCIE_DMA_CMD_DMA_MODE(signo) | pkt_addr_hi;
  dma_cmd[1].__raw[2] = cmd->param3;
  dma_cmd[1].__raw[3] = word3 | NFP_PCIE_DMA_CMD_LENGTH(len2 - 1) | NFP_PCIE_DMA_CMD_PCIE_ADDR_HI(cmd->param4);

  pcie_dma_enq_no_sig(0, &dma_cmd[0], queue);
  pcie_dma_enq_no_sig(0, &dma_cmd[1], queue);
}

#endif /* DMA_H_ */
