/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/mem_ring.h>
#include <pkt/pkt.h>
#include <blm.h>

#include "dma.h"
#include "fp_mem.h"
#include "packet_defs.h"
#include "flow_lookup.h"
#include "pipeline.h"

#include "global.h"
#include "debug.h"

__export __shared __cls struct ctm_pkt_credits ctm_credits;

/* Cache Kernel CTX queue in LM */
__shared __lmem struct flextcp_pl_spctx_t spctx;

/* Per-thread KCTX entry buffers + shared null buffer*/
__emem struct flextcp_pl_sprx_t sprx;
__emem struct flextcp_pl_sptx_t sptx;
__shared __emem struct flextcp_pl_sptx_t null_sptx;

__shared unsigned int seq;

PCIE_DMA_ALLOC(sprx, me, 0, topci_hi, DMA_SPRX_PCIe_INFLIGHT);
PCIE_DMA_ALLOC(sptx_null, me, 0, topci_hi, DMA_SPTX_PCIe_INFLIGHT);
PCIE_DMA_ALLOC(sptx, me, 0, frompci_hi, DMA_SPTX_PCIe_INFLIGHT);

__intrinsic void dma_transfer_pkt_slowpath(unsigned int queue, unsigned int word1, unsigned int word3,
                                          unsigned int isl,   unsigned int pnum,
                                          unsigned int muptr, unsigned int plen,
                                          uint64_t host_addr)
{
  /* Descriptors for CTM + MU */
  __xwrite struct nfp_pcie_dma_cmd cmd;
  SIGNAL dma_sig;

  uint64_t nic_addr;
  unsigned int len;

  /* CTM only */

  nic_addr = (uint64_t) pkt_ctm_ptr40(isl, pnum, PKT_NBI_OFFSET + MAC_PREPEND_BYTES);
  len  = plen - MAC_PREPEND_BYTES;

  issue_dma(queue, word1, word3,
            &cmd, __signal_number(&dma_sig),
            len,
            (uint32_t) (nic_addr >> 32),  (uint32_t) (nic_addr),
            (uint32_t) (host_addr >> 32), (uint32_t) (host_addr));

  __wait_for_all(&dma_sig);

  return;
}

__forceinline void poll_rx_slowpath()
{
  __xread struct work_t work;
  __xread unsigned int spctx_rx_head;
  __xread struct flow_key_t key_xfer;
  __gpr struct flow_key_t key;
  __xwrite struct nfp_pcie_dma_cmd cmd;
  SIGNAL dma_sig;

  unsigned int rnum, raddr_hi;
  unsigned int tail, updated_tail;
  unsigned int dma_cmd_word1, dma_cmd_word3;

  __mem40 void* addr;
  uint64_t nic_addr, host_addr, hash;

  __gpr struct pkt_ms_info msi = {0, 0};

  dma_cmd_word1 = DMA_CMD_WORD1(SPRX);
  dma_cmd_word3 = DMA_CMD_WORD3(SPRX);

  for (;;) {
    /* Fetch new work! */
    rnum = MEM_RING_GET_NUM(flow_ring_sp);
    raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_sp);
    mem_workq_add_thread(rnum, raddr_hi, &work, sizeof(work));

    /* Acquire IDX in KCTX queue */
    while (1) {
      tail = spctx.rx_tail;
      updated_tail = tail + 1;
      if (updated_tail >= spctx.rx_len)
        updated_tail = 0;

      if (updated_tail != spctx.rx_head)
        break;

      addr = (__mem40 void*) &fp_state.spctx.rx_head;
      mem_read32(&spctx_rx_head, addr, sizeof(unsigned int));

      spctx.rx_head = spctx_rx_head;
    }

    spctx.rx_tail = updated_tail;

    /* Transfer Packet to host */
    ctassert(PKTBUF_SIZE == 2048);
    host_addr = spctx.rx_base + (tail << 11);
    dma_transfer_pkt_slowpath(DMA_SPRX_PCIe_QUEUE, dma_cmd_word1, dma_cmd_word3,
        work.io.isl + 32, work.io.pnum,
        work.io.muptr, work.io.plen - MAC_PREPEND_BYTES,
        host_addr);

    /* Read packet header to determine flow_group */
    addr = pkt_ctm_ptr40(work.io.isl + 32, work.io.pnum, PKT_NBI_OFFSET + MAC_PREPEND_BYTES + NET_ETH_LEN + offsetof(struct ip4_hdr, src));
    mem_read32(&key_xfer, addr, sizeof(key_xfer));
    key = key_xfer;
    hash = flow_hash(&key);

    /* Transfer descriptor to host */
    sprx.type = FLEXTCP_PL_SPRX_PACKET;
    sprx.msg.packet.len  = work.io.plen - MAC_PREPEND_BYTES;
    sprx.msg.packet.flow_group = hash & (NUM_FLOW_GROUPS - 1);
    sprx.msg.packet.flow_hash = hash;

    ctassert(sizeof(struct flextcp_pl_sprx_t) == 64);
    host_addr = spctx.rx_desc_base + (tail << 6);
    nic_addr = (uint64_t) ((__mem40 void*) &sprx);

    issue_dma(DMA_SPRX_PCIe_QUEUE, dma_cmd_word1, dma_cmd_word3,
              &cmd, __signal_number(&dma_sig),
              sizeof(struct flextcp_pl_sprx_t),
              (uint32_t) (nic_addr >> 32), (uint32_t) (nic_addr),
              (uint32_t) (host_addr >> 32), (uint32_t) (host_addr));
    __wait_for_all(&dma_sig);

    STATS_INC(SP_FWD);

    /* Drop packet */
    blm_buf_free(work.io.muptr, work.io.bls);
    pkt_ctm_free(work.io.isl + 32, work.io.pnum);
  }

  return;
}

__forceinline void process_sptx_packet(unsigned int desc_idx,
                                      __xread struct flextcp_pl_sptx_t* sptx_xfer,
                                      uint32_t ts_offset)
{
  __xread  blm_buf_handle_t buf;
  __xwrite struct nbi_meta_pkt_info pkt_xfer;
  __xwrite unsigned int ts;
  struct nbi_meta_pkt_info pkt;

  uint64_t host_addr;
  unsigned int q_dst;
  unsigned int dma_cmd_word1, dma_cmd_word3;

  __gpr struct pkt_ms_info msi;
  __mem40 uint8_t* addr;

  dma_cmd_word1 = DMA_CMD_WORD1(SPTX);
  dma_cmd_word3 = DMA_CMD_WORD3(SPTX);

  /* Allocate packet buffer */
  pkt.isl = __ISLAND;
  while (1) {
    pkt.pnum = pkt_ctm_alloc(&ctm_credits, pkt.isl, PKTBUF_CTM_SIZE, 1, 1);
    if (pkt.pnum != (unsigned int)-1)
      break;
  }

  pkt.bls = 0;
  while (1) {
    if (blm_buf_alloc(&buf, 0) == 0)
      break;
  }
  pkt.muptr = buf;
  pkt.split = 1;
  pkt.len = sptx_xfer->msg.packet.len + MAC_PREPEND_BYTES;

  ctassert(PKTBUF_SIZE == 2048);
  host_addr = spctx.tx_base + (desc_idx << 11);

  /* Transfer packet from host */
  dma_transfer_pkt_slowpath(DMA_SPTX_PCIe_QUEUE, dma_cmd_word1, dma_cmd_word3,
      pkt.isl, pkt.pnum,
      pkt.muptr, pkt.len,
      host_addr);

  /* Write NBI meta */
  addr = pkt_ctm_ptr40(pkt.isl, pkt.pnum, 0);
  pkt_xfer = pkt;
  mem_write32(&pkt_xfer, addr, sizeof(pkt_xfer));

  /* Inject timestamp */
  if (ts_offset != 0) {
    ts = local_csr_read(local_csr_timestamp_low);
    addr += PKT_NBI_OFFSET + MAC_PREPEND_BYTES + sptx_xfer->msg.packet.ts_offset;
    mem_write32(&ts, addr, sizeof(unsigned int));
  }

  /* Send packet */
  addr = pkt_ctm_ptr40(pkt.isl, pkt.pnum, 0);
  pkt_mac_egress_cmd_write(addr, PKT_NBI_OFFSET + MAC_PREPEND_BYTES, 1, 1);
  msi =  pkt_msd_write(addr, PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 4);
  q_dst = NS_PLATFORM_NBI_TM_QID_UNTAGGED(PORT_IN_USE, 0);
  pkt_nbi_send(pkt.isl, pkt.pnum, &msi, pkt.len - MAC_PREPEND_BYTES + 4,
              0, q_dst, SP_SEQR0, seq, PKTBUF_CTM_SIZE);

  /* Increment sequence number! */
  seq += 1;
}

__forceinline void process_sptx_descriptor(unsigned int desc_idx,
                                          __xread struct flextcp_pl_sptx_t* sptx_xfer)
{
  __xwrite struct work_t work_xfer;
  __gpr struct work_t work;
  __gpr struct flowht_entry_t flowht_entry;
  __gpr struct flow_key_t flowht_key;
  unsigned int rnum, raddr_hi;
  unsigned int idx, flow_grp;

  switch (sptx_xfer->type) {
  case FLEXTCP_PL_SPTX_INVALID:
    return;

  case FLEXTCP_PL_SPTX_PACKET:
    process_sptx_packet(desc_idx, sptx_xfer, 1);
    break;

  case FLEXTCP_PL_SPTX_PACKET_NOTS:
    process_sptx_packet(desc_idx, sptx_xfer, 0);
    break;

  case FLEXTCP_PL_SPTX_CONN_RETX:
    work.type = WORK_TYPE_RETX;
    work.flow_id = sptx_xfer->msg.connretran.flow_id;
    flow_grp = sptx_xfer->msg.connretran.flow_grp;

    if (flow_grp == 0) {
      STATS_INC(GRP0);
      rnum = MEM_RING_GET_NUM(flow_ring_0);
      raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_0);
    }
    else if (flow_grp == 1) {
      STATS_INC(GRP1);
      rnum = MEM_RING_GET_NUM(flow_ring_1);
      raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_1);
    }
    else if (flow_grp == 2) {
      STATS_INC(GRP2);
      rnum = MEM_RING_GET_NUM(flow_ring_2);
      raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_2);
    }
    else {
      STATS_INC(GRP3);
      rnum = MEM_RING_GET_NUM(flow_ring_3);
      raddr_hi = MEM_RING_GET_MEMADDR(flow_ring_3);
    }

    STATS_INC(RETX_TOTAL);
    work_xfer = work;
    mem_workq_add_work(rnum, raddr_hi, &work_xfer, sizeof(work_xfer));
    break;

  case FLEXTCP_PL_SPTX_CONN_SETRATE:
    /* TODO: Not yet implemented! */
    break;

  case FLEXTCP_PL_SPTX_FLOWHT_ADD:
    flowht_entry.key.__raw[0] = sptx_xfer->msg.raw[0];
    flowht_entry.key.__raw[1] = sptx_xfer->msg.raw[1];
    flowht_entry.key.__raw[2] = sptx_xfer->msg.raw[2];
    flowht_entry.flow_id = sptx_xfer->msg.raw[3];
    flow_insert(&flowht_entry);
    break;

  case FLEXTCP_PL_SPTX_FLOWHT_DEL:
    flowht_key.__raw[0] = sptx_xfer->msg.raw[0];
    flowht_key.__raw[1] = sptx_xfer->msg.raw[1];
    flowht_key.__raw[2] = sptx_xfer->msg.raw[2];
    flow_delete(&flowht_key);
    break;

  case FLEXTCP_PL_SPTX_CONN_CLOSE:
    /* TODO: Not yet implemented! */
    break;

  case FLEXTCP_PL_SPTX_DEBUG_RESET:
    for (idx = 0; idx < PROFBUF_SIZE; idx++) {
      __PROF_CLR(idx);
    }

#if 0
    for (idx = 0; idx < STATBUF_SIZE; idx++) {
      __STATS_CLR(idx);
    }
#endif
    break;

  default:
    break;
  }

  return;
}

__forceinline void poll_tx_slowpath()
{
  __xread unsigned int spctx_tx_tail;
  __xread struct flextcp_pl_sptx_t sptx_xfer;
  __xwrite struct nfp_pcie_dma_cmd cmd;
  SIGNAL dma_sig;

  uint64_t nic_addr, host_addr;
  unsigned int head, updated_head;
  unsigned int dma_cmd_word1, dma_cmd_word3;

  __mem40 void* addr;

  dma_cmd_word1 = DMA_CMD_WORD1(SPTX);
  dma_cmd_word3 = DMA_CMD_WORD3(SPTX);

  for (;;) {
    /* Acquire KCTX idx */
    head = spctx.tx_head;
    if (head == spctx.tx_tail) {
      addr = (__mem40 void*) &fp_state.spctx.tx_tail;
      mem_read32(&spctx_tx_tail, addr, sizeof(unsigned int));

      spctx.tx_tail = spctx_tx_tail;
      continue;
    }

    updated_head = head + 1;
    if (updated_head >= spctx.tx_len)
      updated_head = 0;
    spctx.tx_head = updated_head;

    /* Read descriptor */
    ctassert(sizeof(struct flextcp_pl_sptx_t) == 64);
    nic_addr  = (uint64_t) ((__mem40 void*) &sptx);
    host_addr = spctx.tx_desc_base + (head << 6);

    issue_dma(DMA_SPTX_PCIe_QUEUE, dma_cmd_word1, dma_cmd_word3,
        &cmd, __signal_number(&dma_sig),
        sizeof(struct flextcp_pl_sptx_t),
        (uint32_t) (nic_addr >> 32), (uint32_t) (nic_addr),
        (uint32_t) (host_addr >> 32), (uint32_t) (host_addr));
    __wait_for_all(&dma_sig);

    mem_read64(&sptx_xfer, (__mem40 void*) nic_addr, sizeof(struct flextcp_pl_sptx_t));

    /* Write null descriptor */
    nic_addr  = (uint64_t) ((__mem40 void*) &null_sptx);
    issue_dma(NFP_PCIE_DMA_TOPCI_HI, dma_cmd_word1, dma_cmd_word3,
        &cmd, __signal_number(&dma_sig),
        sizeof(struct flextcp_pl_sptx_t),
        (uint32_t) (nic_addr >> 32), (uint32_t) (nic_addr),
        (uint32_t) (host_addr >> 32), (uint32_t) (host_addr));
    __wait_for_all(&dma_sig);

    /* Process SPTX descriptor */
    process_sptx_descriptor(head, &sptx_xfer);
  }

  return;
}

void pcie_monitor()
{
  unsigned int count;
  __xread struct nfp_pcie_dma_qsts0 status0;
  __xread struct nfp_pcie_dma_qsts1 status1;

  count = 0;
  for (;;) {
    sleep(1 << 16);
    count += 1;

    /* Print every 2^30 cycles */
    if (count == (1 << 14)) {
      count = 0;

      pcie_dmaq_status(0, 0, &status0, &status1);

      DEBUG(SP, 0, status0.__raw, status1.__raw);

      pcie_dmaq_status(0, 1, &status0, &status1);

      DEBUG(SP, 1, status0.__raw, status1.__raw);
    }
  }

  return;
}

/* Temporary Fix: Reset PCIe queues */
__intrinsic void pcie_queues_reset(unsigned int pcie_isl)
{
  unsigned int count;
  unsigned int addr_hi = pcie_isl << 30;
  unsigned int addr_lo;

  __xread  struct nfp_pcie_dma_qsts0 status_rd;
  __xwrite struct nfp_pcie_dma_qsts0 status_wr;
  unsigned int status;
  SIGNAL sig;

  count = sizeof(struct nfp_pcie_dma_qsts0) >> 2;

  /* Reset FromPCI */
  addr_lo = NFP_PCIE_DMA_QSTS0_FROMPCI;
  __asm pcie[read_pci, status_rd, addr_hi, <<8, addr_lo, __ct_const_val(count)], ctx_swap[sig];
  status = status_rd.__raw;
  status &= ~NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP;
  status &= ~NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP;
  status &= ~NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP;
  status &= ~NFP_PCIE_DMA_QSTS0_DMA_ERROR(NFP_PCIE_DMA_QSTS0_DMA_ERROR_msk);
  status_wr.__raw = status;
  __asm pcie[write_pci, status_wr, addr_hi, <<8, addr_lo, __ct_const_val(count)], ctx_swap[sig];

  /* Reset ToPCI */
  addr_lo = NFP_PCIE_DMA_QSTS0_TOPCI;
  __asm pcie[read_pci, status_rd, addr_hi, <<8, addr_lo, __ct_const_val(count)], ctx_swap[sig];
  status = status_rd.__raw;
  status &= ~NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP;
  status &= ~NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP;
  status &= ~NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP;
  status &= ~NFP_PCIE_DMA_QSTS0_DMA_ERROR(NFP_PCIE_DMA_QSTS0_DMA_ERROR_msk);
  status_wr.__raw = status;
  __asm pcie[write_pci, status_wr, addr_hi, <<8, addr_lo, __ct_const_val(count)], ctx_swap[sig];
}

int main()
{
  SIGNAL start_sig;
  __xread struct flextcp_pl_spctx_t spctx_xfer;
  __mem40 void* addr;

  if (ctx() == 0) {
    dma_cfg_setup(DMA_SPRX_CFG_IDX);
    dma_cfg_setup(DMA_SPTX_CFG_IDX);

    pcie_queues_reset(0);

    pkt_ctm_init_credits(&ctm_credits, ME_CTM_ALLOC_MAX_PKT_CREDITS - 1, ME_CTM_ALLOC_MAX_BUF_CREDITS - 1);
    null_sptx.type = FLEXTCP_PL_SPTX_INVALID;

    seq = 0;

    /* Wait for start signal */
    while (1) {
      if (fp_state.cfg.sig != 0)
        break;

      sleep(1 << 10);
    }

    addr = (__mem40 void*) &fp_state.spctx;
    mem_read64(&spctx_xfer, addr, sizeof(spctx_xfer));
    spctx = spctx_xfer;

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
    poll_rx_slowpath();
  }
  else if (ctx() == 1) {
    poll_tx_slowpath();
  }

  return 0;
}
