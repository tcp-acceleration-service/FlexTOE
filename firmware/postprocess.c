/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdint.h>
#include <memory.h>

#include <nfp.h>
#include <nfp_intrinsic.h>

#include <nfp/me.h>
#include <nfp/cls.h>
#include <nfp/ctm.h>
#include <nfp/mem_ring.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_atomic.h>
#include <std/reg_utils.h>
#include <pkt/pkt.h>
#include <blm.h>

#include "dma.h"
#include "qman.h"
#include "desc_pool.h"
#include "packet_defs.h"
#include "pipeline.h"

#include "shared.h"
#include "global.h"
#include "debug.h"

#define FLOW_MEM_CACHE_SIZE          (1 << 4)                      /* 16  * 32 = 512 */
#define FLOW_MEM_CACHE_MASK          (FLOW_MEM_CACHE_SIZE - 1)
__shared __lmem struct flowst_mem_t  mem_cache[FLOW_MEM_CACHE_SIZE];
__shared __gpr unsigned int flow_grp_mask;

#define CACHE_CLEAN_DURATION                (1 << 30)   /* Roughly every 20 seconds with 800Mhz clock */
__forceinline void reset_cache()
{
  /* Clear CAM */
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

__forceinline unsigned int handle_mem_cache_miss(uint32_t flow_id)
{
  __xread struct flowst_mem_t mem_info;

  __mem40 void* addr;
  cam_lookup_t conn_lkup;

  addr = (__mem40 void*) &fp_state.flows_mem_info[flow_id];
  mem_read32(&mem_info, addr, sizeof(mem_info));

  conn_lkup = cam_lookup(flow_id);
  if (conn_lkup.hit)
    return conn_lkup.entry_num;

  cam_write(conn_lkup.entry_num, flow_id, 0);
  mem_cache[conn_lkup.entry_num] = mem_info;

  return conn_lkup.entry_num;
}

/*> TCP Padded TS Option (NOP_KIND + NOP_KIND + TS_KIND + TS_LEN) */
#define PKT_TCP_OPT_RAW0       0x0101080A
__intrinsic void prepare_ack_header(__xread struct work_result_t* result)
{
  __xwrite struct hdr_tcp_t hdr;
  __mem40 uint8_t* pbuf;
  uint32_t tcp_flags, win;

  /* Skip header prep if no TX */
  if ((result->flags & WORK_RESULT_TX) == 0)
    return;

  /* TCP seq-ack */
  hdr.tcp.seq  = result->seq;
  hdr.tcp.ack  = result->ack;

  /* TCP flags */
  tcp_flags = NET_TCP_FLAG_ACK;
  if (result->flags & WORK_RESULT_ECE) {
    tcp_flags |= NET_TCP_FLAG_ECE;
  }
  win = MIN(result->win, 0xFFFF);
  hdr.__raw[3] = (NET_TCPPADTS_LEN32 <<  28) | (tcp_flags << 16) | (win);   // TCP offset, flags & window
  hdr.__raw[4] = 0;                  // sum, urp = 0

#if TCP_TIMESTAMP_ENABLE
  hdr.tcpopts.__raw[0] = PKT_TCP_OPT_RAW0;
  hdr.tcpopts.ts_val = local_csr_read(local_csr_timestamp_low);
  hdr.tcpopts.ts_ecr = result->ts_ecr;
#else
  hdr.tcpopts.__raw[0] = 0;
  hdr.tcpopts.ts_val = 0;
  hdr.tcpopts.ts_ecr = 0;
#endif

  /* Skip the first word in TCP header */
  pbuf = pkt_ctm_ptr40(result->work.io.isl + 32, result->work.io.pnum,
                  PKT_NBI_OFFSET + MAC_PREPEND_BYTES + NET_ETH_LEN + NET_IP4_LEN + 4);
  mem_write32(&hdr.__raw[1], pbuf, sizeof(struct hdr_tcp_t) - 4);
}

#define IP_RAW_WORD0(tos, len) (0x45000000 | (tos << 16) | (len)) /*> Ver: 4 | HL: 5 | TOS: y | LEN: x */
__intrinsic void prepare_seg_header(__xread struct work_result_t* result)
{
  __xwrite struct hdr_tcp_t hdr;
  __xwrite unsigned int ip_raw0;
  __mem40 uint8_t* pbuf;
  uint32_t tcp_flags, win;
  SIGNAL sig0, sig1;

  /* Skip header prep if no TX */
  if ((result->flags & WORK_RESULT_TX) == 0)
    return;

  /* TCP seq-ack */
  hdr.tcp.seq  = result->seq;
  hdr.tcp.ack  = result->ack;

  /* TCP flags */
  tcp_flags = NET_TCP_FLAG_ACK | NET_TCP_FLAG_PSH;
  if (result->flags & WORK_RESULT_FIN) {
    tcp_flags |= NET_TCP_FLAG_FIN;
  }
  win = MIN(result->win, 0xFFFF);
  hdr.__raw[3] = (NET_TCPPADTS_LEN32 <<  28) | (tcp_flags << 16) | (win);   // TCP offset, flags & window
  hdr.__raw[4] = 0;                  // sum, urp = 0

#if TCP_TIMESTAMP_ENABLE
  hdr.tcpopts.__raw[0] = PKT_TCP_OPT_RAW0;
  hdr.tcpopts.ts_val = local_csr_read(local_csr_timestamp_low);
  hdr.tcpopts.ts_ecr = result->ts_ecr;
#else
  hdr.tcpopts.__raw[0] = 0;
  hdr.tcpopts.ts_val = 0;
  hdr.tcpopts.ts_ecr = 0;
#endif

  /* Modify IP len */
  pbuf = pkt_ctm_ptr40(result->work.io.isl + 32, result->work.io.pnum,
                  PKT_NBI_OFFSET + MAC_PREPEND_BYTES + 2 + NET_ETH_LEN);
  ip_raw0 = IP_RAW_WORD0(2, NET_IP4_LEN + NET_TCPPADTS_LEN + result->dma_len);
  __mem_write32(&ip_raw0, pbuf, sizeof(ip_raw0), sizeof(ip_raw0), sig_done, &sig0);

  /* Skip the first word in TCP header */
  pbuf += NET_IP4_LEN + 4;
  __mem_write32(&hdr.__raw[1], pbuf,
            sizeof(struct hdr_tcp_t) - 4, sizeof(struct hdr_tcp_t) - 4,
            sig_done, &sig1);
  __wait_for_all(&sig0, &sig1);
}

__intrinsic unsigned int allocate_arx_desc(__xread struct work_result_t* result)
{
  __xread unsigned int desc_idx;
  unsigned int rnum, raddr_hi;

  if ((result->flags & WORK_RESULT_DMA_ACDESC) == 0)
    return 0;

  rnum = MEM_RING_GET_NUM(arx_desc_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(arx_desc_ring);
  while (mem_ring_get(rnum, raddr_hi, &desc_idx, sizeof(desc_idx)) != 0) {}

  return desc_idx;
}

__intrinsic void prepare_arx_desc(__xread struct work_result_t*  result,
                                  __lmem  struct flowst_mem_t*  fs,
                                 __xwrite struct flextcp_pl_arx_t* arx)
{
  if ((result->flags & WORK_RESULT_DMA_ACDESC) == 0)
    return;

  arx->msg.connupdate.opaque  = ((((uint64_t) fs->opaque_hi) << 32) | fs->opaque_lo);
  arx->msg.connupdate.rx_bump = result->ac_rx_bump;
  arx->msg.connupdate.tx_bump = result->ac_tx_bump;
  arx->msg.connupdate.flags   = (((result->flags & WORK_RESULT_FIN) != 0) ? FLEXTCP_PL_ARX_FLRXDONE : 0);
  arx->type = FLEXTCP_PL_ARX_CONNUPDATE;
}

#define TCP_MAX_RTT 100000
__intrinsic void collect_cc_stats(__xread struct work_result_t* result)
{
  uint32_t flow_id;
  __mem40 struct flowst_cc_t* cc_info;
  __xrw uint32_t rtt, tx_rate;

  flow_id = result->work.flow_id;
  cc_info = (__mem40 struct flowst_cc_t*) &fp_state.flows_cc_info[flow_id];

  /* Count ACK */
  mem_incr32((__mem40 uint32_t*) &cc_info->cnt_rx_acks);

  /* Count ACK bytes */
  if ((result->flags & WORK_RESULT_DMA_ACDESC) != 0) {
    mem_add32_imm(result->ac_tx_bump, (__mem40 uint64_t*) &cc_info->cnt_rx_ack_bytes);

    /* Count ECN bytes */
    if ((result->flags & WORK_RESULT_ECNB) != 0) {
      mem_add32_imm(result->ac_tx_bump, (__mem40 uint64_t*) &cc_info->cnt_rx_ecn_bytes);
    }
  }

  /* Count tx_drops */
  if ((result->flags & WORK_RESULT_RETX) != 0) {
    mem_incr32((__mem40 uint32_t*) &cc_info->cnt_tx_drops);
    mem_write32_atomic_imm(0, &cc_info->txp);

    /* Cut the rate by half => double the cycles */
    mem_read_atomic(&tx_rate, (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_rate, sizeof(tx_rate));
    mem_add32(&tx_rate, (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_rate, sizeof(tx_rate));
  }

  if ((result->flags & WORK_RESULT_TXP_ZERO) != 0) {
    mem_write32_atomic_imm(0, &cc_info->txp);
  }

  /* Update RTT */
  if (result->ts_val != 0) {
    rtt = MIN(TCP_MAX_RTT >> 3, (local_csr_read(local_csr_timestamp_low) - result->ts_val) >> 9);
    mem_test_add(&rtt, (__mem40 uint32_t*) &cc_info->rtt_est, sizeof(rtt));
    mem_subsat32_imm(rtt >> 3, (__mem40 uint32_t*) &cc_info->rtt_est);
  }
}

__intrinsic void push_qm_bump(__xread struct work_result_t* result)
{
  unsigned int flow_id, rnum, raddr_hi;

  __xrw unsigned int qm_bump;
  __mem40 uint32_t* qm_avail_addr;

  if ((result->flags & WORK_RESULT_QM) == 0)
    return;

  flow_id = result->work.flow_id;
  qm_avail_addr = (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_avail;

#if QM_BYPASS
  if (result->qm_bump < TCP_MSS) {
    raddr_hi = MEM_RING_GET_MEMADDR(qm_sched_ring0);
    mem_workq_add_work_imm(QM_SCHED_RNUM_BASE + (flow_grp_mask >> 16), raddr_hi, flow_id);

    /* Push QM credit */
    mem_subsat32_imm(1, (__mem40 void*) &qm_credits);

    STATS_INC(QM_SCHEDULE);

    return;
  }
#endif

  qm_bump = result->qm_bump;
  mem_test_add(&qm_bump, qm_avail_addr, sizeof(unsigned int));

  /* If avail was 0 previously, bump the QM */
  if (qm_bump == 0) {
    rnum = MEM_RING_GET_NUM(qm_bump_ring);
    raddr_hi = MEM_RING_GET_MEMADDR(qm_bump_ring);
    mem_workq_add_work_imm(rnum, raddr_hi, flow_grp_mask | flow_id);
  }
}

__forceinline
int prepare_dma_cmd(__xread struct work_result_t* result,
                    __lmem   struct flowst_mem_t*  fs,
                    unsigned int desc_idx,
                    __xwrite struct dma_pkt_cmd_t* dma_xfer)
{
  __gpr struct dma_pkt_cmd_t dma;
  unsigned int dma_len, len_mask;
  unsigned int wraparound, buf_len, pos, off;
  unsigned int isl, pnum;
  uint64_t buf_base, pkt_base;
  unsigned int ret, flags;

  dma.__raw[0] = result->work.__raw[0];
  dma.__raw[1] = result->work.__raw[1];
  dma.__raw[2] = result->work.__raw[2];

  if (result->work.type == WORK_TYPE_RX) {
    buf_len  = fs->rx_len;
    buf_base = fs->rx_base;
    off = result->dma_offset + PKT_NBI_OFFSET + MAC_PREPEND_BYTES + NET_HDR_LEN;
  }
  else {
    buf_len  = fs->tx_len;
    buf_base = fs->tx_base;

    dma.plen = result->dma_len;
    off = PKT_NBI_OFFSET + MAC_PREPEND_BYTES + 2 + NET_HDR_LEN;
  }

  if (result->flags & WORK_RESULT_DMA_PAYLOAD) {
    dma_len  = result->dma_len;
    len_mask = buf_len - 1;

    pos = result->dma_pos & len_mask;
    wraparound = buf_len - pos;

    isl = result->work.io.isl + 32;
    pnum = result->work.io.pnum;
    pkt_base = (uint64_t) pkt_ctm_ptr40(isl, pnum, off);
    if (dma_len <= wraparound) {
      /* No wraparound -> Single DMA */
      __critical_path();
      ret = 0;

      dma.param1 = (uint32_t) pkt_base;
      dma.param2 = (uint32_t) (pkt_base >> 32);

      buf_base += pos;
      dma.param3 = (uint32_t) buf_base;
      dma.param4 = ((uint32_t) (buf_base >> 32)) | ((dma_len - 1) << 20);
    }
    else {
      /* Wraparound -> Two DMAs */
      ret = 1;

      dma_len -= wraparound;

      dma.param1 = (uint32_t) pkt_base;
      dma.param2 = pos;
      dma.param3 = (uint32_t) buf_base;
      dma.param4 = ((uint32_t) (buf_base >> 32)) | (dma_len << 8) | ((wraparound - 1) << 20);
    }
  }

  dma.desc_idx = desc_idx;
  dma.db_id    = fs->db_id;
  dma.flags    = result->flags;

  reg_cp(dma_xfer, &dma, sizeof(dma));

  return ret;
}

__forceinline void postprocess_ac(__xread struct work_result_t* result)
{
  unsigned int rnum, raddr_hi;
  unsigned int desc_idx;
  __gpr struct schedule_t sched;

  /* Update QM */
  push_qm_bump(result);

  /* Push QM force */
  if (result->flags & WORK_RESULT_QM_FORCE) {
    rnum = QM_SCHED_RNUM_BASE + (__ISLAND - 32);
    raddr_hi = MEM_RING_GET_MEMADDR(qm_sched_ring0);
    sched.force = 1;
    sched.flow_id = result->work.flow_id;
    mem_workq_add_work_imm(rnum, raddr_hi, sched.__raw);
    STATS_INC(QM_SCHEDULE);
  }

  /* Free descriptor */
  rnum = MEM_RING_GET_NUM(atx_desc_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(atx_desc_ring);
  desc_idx = result->work.ac.desc_idx;
  mem_ring_journal_fast(rnum, raddr_hi, desc_idx);

  return;
}

__forceinline void postprocess_retx(__xread struct work_result_t* result)
{
  uint32_t flow_id;
  __xrw uint32_t tx_rate;

  /* Update QM */
  push_qm_bump(result);

  /* Half the rate => double the cycles */
  flow_id = result->work.flow_id;
  mem_read_atomic(&tx_rate, (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_rate, sizeof(tx_rate));
  mem_add32(&tx_rate, (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].tx_rate, sizeof(tx_rate));

  /* Set CC-TXP to 1 */
  mem_write32_atomic_imm(0, (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].txp);

  mem_incr32((__mem40 uint32_t*) (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].cnt_tx_drops);
}

#define TM_Q_DST  NS_PLATFORM_NBI_TM_QID_UNTAGGED(PORT_IN_USE, 0)
__forceinline void postprocess_tx(__xread struct work_result_t* result)
{
  __xwrite struct dma_pkt_cmd_t dma_cmd;
  unsigned int ret, flow_id;

  __gpr struct pkt_ms_info msi;

  unsigned int mem_info_idx;
  cam_lookup_t conn_lkup;

  unsigned int rnum, raddr_hi;

  /* Push QM credit */
  mem_incr32((__mem40 void*) &qm_credits);

  if ((result->flags & WORK_RESULT_DMA_PAYLOAD) == 0) {
    /* Drop packet */
    msi = pkt_gen_msi_info(PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2);
    pkt_nbi_drop_seq(result->work.io.isl + 32, result->work.io.pnum, &msi,
        NET_HDR_LEN + 4, 0, 0, result->work.io.seqr, result->work.io.seq, PKTBUF_CTM_SIZE);

    blm_buf_free(result->work.io.muptr, result->work.io.bls);
    pkt_ctm_free(result->work.io.isl + 32, result->work.io.pnum);
    STATS_INC(DMA_TX_FWD_DROP);
    return;
  }

  /* Write TCP header */
  prepare_seg_header(result);

  /* Fetch mem info */
  flow_id      = result->work.flow_id;
  conn_lkup    = cam_lookup(flow_id);
  mem_info_idx = conn_lkup.entry_num;
  if (!conn_lkup.hit) {
    mem_info_idx = handle_mem_cache_miss(flow_id);
  }

  /* Prepare DMA descriptor */
  ret = 0;
  if (result->dma_len > 0) {
    ret = prepare_dma_cmd(result, &mem_cache[mem_info_idx], 0, &dma_cmd);
  }
  else if (result->work.io.force != 0) {  /* Force send */
    /* Send packet over NBI */
    msi = pkt_gen_msi_info(PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2);
    pkt_nbi_send(result->work.io.isl + 32, result->work.io.pnum, &msi,
        NET_HDR_LEN + 4, 0, TM_Q_DST, result->work.io.seqr, result->work.io.seq, PKTBUF_CTM_SIZE);
    STATS_INC(DMA_TX_FWD_SEG);
    return;
  }

  /* Set CC-TXP to 1 */
  mem_write32_atomic_imm(1, (__mem40 uint32_t*) &fp_state.flows_cc_info[flow_id].txp);

  if (ret) {
    rnum = MEM_RING_GET_NUM(dma_tx_ring_excep);
    raddr_hi = MEM_RING_GET_MEMADDR(dma_tx_ring_excep);
  }
  else {
    rnum = MEM_RING_GET_NUM(dma_tx_ring);
    raddr_hi = MEM_RING_GET_MEMADDR(dma_tx_ring);
  }
  mem_workq_add_work(rnum, raddr_hi, &dma_cmd, sizeof(dma_cmd));

  return;
}

__forceinline void postprocess_rx(__xread struct work_result_t* result)
{
  __xwrite struct flextcp_pl_arx_t arx;
  __xwrite struct dma_pkt_cmd_t dma_cmd;
  unsigned int ret;

  unsigned int rnum, raddr_hi;
  unsigned int desc_idx;
  unsigned int db_id, opaque_hi, opaque_lo;

  __mem40 void* desc_buf;

  unsigned int mem_info_idx;
  cam_lookup_t conn_lkup;

  /* Write TCP header */
  prepare_ack_header(result);

  /* Update QM */
  push_qm_bump(result);

  /* Update CC stats */
  collect_cc_stats(result);

  /* Allocate APPCTX desc */
  desc_idx = allocate_arx_desc(result);

  /* Fetch mem info */
  conn_lkup    = cam_lookup(result->work.flow_id);
  mem_info_idx = conn_lkup.entry_num;
  if (!conn_lkup.hit) {
    mem_info_idx = handle_mem_cache_miss(result->work.flow_id);
  }

  /* Prepare ARX command */
  prepare_arx_desc(result, &mem_cache[mem_info_idx], &arx);

  /* Prepare DMA descriptor */
  ret = prepare_dma_cmd(result, &mem_cache[mem_info_idx], desc_idx, &dma_cmd);

  if (result->flags & WORK_RESULT_DMA_ACDESC) {
    desc_buf = (__mem40 void*) &arx_desc_pool[desc_idx];
    mem_write32(&arx, desc_buf, sizeof(struct flextcp_pl_arx_t));
  }

  if (ret) {
    rnum = MEM_RING_GET_NUM(dma_rx_ring_excep);
    raddr_hi = MEM_RING_GET_MEMADDR(dma_rx_ring_excep);
  }
  else {
    rnum = MEM_RING_GET_NUM(dma_rx_ring);
    raddr_hi = MEM_RING_GET_MEMADDR(dma_rx_ring);
  }
  mem_workq_add_work(rnum, raddr_hi, &dma_cmd, sizeof(dma_cmd));

  return;
}

int main()
{
  SIGNAL start_sig;

  __xread struct work_result_t result;

  /* Timestamp */
  uint32_t t_prev, t_curr, t_diff;

  if (ctx() == 0) {
    reset_cache();
    enable_global_timestamp();

    cls_workq_setup(POSTPROC_RING, (__cls void*) postproc_ring, FLOW_POSTPROC_RING_SIZE);

    flow_grp_mask = (__ISLAND - 32) << 16;

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

  for (;;) {
    cls_workq_add_thread(POSTPROC_RING, &result, sizeof(result));

    switch (result.work.type) {
    case WORK_TYPE_AC:
      STATS_INC(AC_POSTPROC_IN);
      postprocess_ac(&result);
      break;

    case WORK_TYPE_RETX:
      STATS_INC(RETX_POSTPROC_IN);
      postprocess_retx(&result);
      break;

    case WORK_TYPE_TX:
      STATS_INC(TX_POSTPROC_IN);
      postprocess_tx(&result);
      break;

    case WORK_TYPE_RX:
      STATS_INC(RX_POSTPROC_IN);
      postprocess_rx(&result);
      break;
    }

    if (ctx() == 0) {
      t_curr = local_csr_read(local_csr_timestamp_low);

      /* Clean cache */
      if (t_curr > t_prev) {
        t_diff = t_curr - t_prev;
      } else {
        t_diff = (0xFFFFFFFF - t_prev) + t_curr;
      }
      if (t_diff >= CACHE_CLEAN_DURATION) {
        t_prev = t_curr;
        reset_cache();
      }
    }
  }

  return 0;
}
