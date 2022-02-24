/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>

#include "packet_defs.h"
#include "fp_mem.h"
#include "pipeline.h"

/**
 * Calculate how many bytes can be sent based on unsent bytes in send buffer and
 * flow control window
 *
 * @param fs Pointer to flow state.
 * @param [in] bump Bump to tx_avail.
 *
 * @return Bytes that can be sent.
 */
__intrinsic uint32_t tcp_txavail(__lmem struct flowst_tcp_t* fs,
                                        uint32_t             bump)
{
  /* NOTE: Active LM address already points to FS */
  /* flow control window */
  uint32_t buf_avail, fc_avail;

  __asm {
    alu[buf_avail, bump, +, *l$index1[1]];      // buf_avail = bump + fs->tx_avail
    alu[fc_avail, --, B, *l$index1[2]];         // fc_avail  = fs->tx_remote_avail
    alu[fc_avail, fc_avail, -, *l$index1[3]];   // fc_avail -= fs->tx_sent
  }

  return MIN(buf_avail, fc_avail);
}

/**
 * Check if received ack number is valid, i.e. greater than or equal to the
 * current ack number.
 *
 * @param fs  Pointer to flow state.
 * @param ack Acknowledgement number from package.
 * @param [out] bump num
 *
 * @return 0 if ack is valid, != 0 otherwise
 */
__intrinsic int tcp_valid_rxack(__lmem struct flowst_tcp_t* fs,
              uint32_t ack, uint32_t *bump)
{
  /* NOTE: Active LM address already points to FS */
  uint32_t next_ack, tx_sent;
  uint32_t hole;

  __asm {
    alu[next_ack, --, B, *l$index1[4]];       // next_ack = fs->tx_next_seq
    alu[next_ack, next_ack, -, *l$index1[3]]; // next_ack -= fs->tx_sent
    alu[tx_sent, --, B, *l$index1[3]];        // tx_sent = fs->tx_sent
#if ALLOW_FUTURE_ACKS
    alu[tx_sent, tx_sent, +, *l$index1[1]];   // tx_sent += fs->tx_avail
#endif
  }

  hole = ack - next_ack;
  if (hole > tx_sent)
    return -1;

  *bump = hole;
  return 0;
}

/**
 * Check if seq is in the receive window.
 *
 * @param fs        Pointer to flow state.
 * @param pkt_seq   Sequence number of received packet.
 *
 * @return 0 if packet should be processed, != 0 to drop.
 */
__intrinsic int tcp_rxseq_inwindow(__lmem struct flowst_tcp_t *fs,
                                  uint32_t pkt_seq)
{
  /* NOTE: Active LM address already points to FS */
  uint32_t trim, next_seq, avail;

  __asm {
    alu[next_seq, --, B, *l$index1[10]];      // next_seq = fs->rx_next_seq
    alu[avail, --, B, *l$index1[9]];          // avail = fs->rx_avail
  }

  trim = pkt_seq - next_seq;

  if (trim > avail)
    return -1;

  return 0;
}

/**
 * Check if part of this packet fits into the un-used portion of the receive
 * buffer, even if out of order.
 *
 * @param fs        Pointer to flow state.
 * @param pkt_seq   Sequence number of received packet.
 * @param pkt_bytes Received payload length.
 * @param [out] trim_start  If packet is to be processed, indicates how many
 *                          bytes to trim off from beginning of packet.
 * @param [out] trim_end    If packet is to be processed, indicates how many
 *                          bytes to trim off from end of packet.
 *
 * @return 0 if packet should be processed, != 0 to drop.
 */
__intrinsic int tcp_trim_rxbuf(__lmem struct flowst_tcp_t *fs,
    uint32_t pkt_seq, uint32_t pkt_bytes,
    uint32_t *trim_start, uint32_t *trim_end)
{
  /* NOTE: Active LM address already points to FS */
  uint32_t trim, next_seq, avail;

  __asm {
    alu[next_seq, --, B, *l$index1[10]];      // next_seq = fs->rx_next_seq
    alu[avail, --, B, *l$index1[9]];          // avail = fs->rx_avail
  }

  trim = next_seq - pkt_seq;
  if (trim <= pkt_bytes) {
    *trim_start = trim;
    trim = pkt_bytes - trim;
    *trim_end = (trim <= avail) ? 0 : (trim - avail);

    return 0;
  }

  __critical_path();
  trim = -trim;
  if (trim >= avail)
    return -1;

  __critical_path();
  *trim_start = 0;
  *trim_end = ((avail - trim) >= pkt_bytes) ? 0 : (pkt_bytes - (avail - trim));
  return 0;
}

/**
 * Check if received packet with sequence number #pkt_seq and #pkt_bytes bytes
 * of payload should be processed or dropped.
 *
 * @param fs        Pointer to flow state.
 * @param pkt_seq   Sequence number of received packet.
 * @param pkt_bytes Received payload length.
 * @param [out] trim_start  If packet is to be processed, indicates how many
 *                          bytes to trim off from beginning of packet.
 * @param [out] trim_end    If packet is to be processed, indicates how many
 *                          bytes to trim off from end of packet.
 *
 * @return 0 if packet should be processed, != 0 to drop.
 */
__intrinsic static int tcp_valid_rxseq(__lmem struct flowst_tcp_t *fs,
    uint32_t pkt_seq, uint32_t pkt_bytes,
    uint32_t *trim_start, uint32_t *trim_end)
{
  /* NOTE: Active LM address already points to FS */
  uint32_t trim, next_seq, avail;

  __asm {
    alu[next_seq, --, B, *l$index1[10]];        // sseq_a = fs->rx_next_seq
    alu[avail, --, B, *l$index1[10]];           // avail = fs->rx_avail
  }

  trim = next_seq - pkt_seq;
  if (trim > pkt_bytes)
    return -1;
  *trim_start = trim;

  trim = pkt_bytes - trim;
  *trim_end = (trim <= avail) ? 0 : (trim - avail);

  return 0;
}

__intrinsic void flows_tx(
                          struct work_t*        work,
                   __lmem struct flowst_tcp_t*  fs,
                 __xwrite struct work_result_t* result)
{
  uint32_t payload_len, avail, fin, flags;
  uint32_t fs_flags, rx_avail, tx_avail;
  uint32_t tx_next_seq, rx_next_seq, tx_next_ts, tx_next_pos;

  /* Set Active LM1 address as FS */
  __asm {
    local_csr_wr[local_csr_active_lm_addr_1, fs];
  }

  /* Copy work to result */
  result->work.__raw[0] = work->__raw[0];
  result->work.__raw[1] = work->__raw[1];
  result->work.__raw[2] = work->__raw[2];

  avail = tcp_txavail(fs, 0);
  flags = WORK_RESULT_TX | WORK_RESULT_DMA_PAYLOAD;

  /* exit if there is no data available for TX */
  if (avail == 0 && work->io.force == 0) {
    result->flags = 0;
    return;
  }
  __critical_path();

  payload_len = MIN(avail, TCP_MSS);

  __asm {
    ld_field_w_clr[fs_flags, 12, *l$index1[7], >>16];     // fs_flags = fs->flags
    alu[tx_avail, --, B, *l$index1[1]];                   // tx_avail = fs->tx_avail
  }
  fin = ((fs_flags & FLEXNIC_PL_FLOWST_TXFIN) != 0) && (tx_avail == 0);

  /* Create TCP segment! */
  /* DO NOT MODIFY src/dst port! */
  /* They are filled by the post/pre-processor */
  __asm {
    alu[tx_next_seq, --, B, *l$index1[4]];    // tx_next_seq = fs->tx_next_seq
    alu[rx_next_seq, --, B, *l$index1[10]];   // rx_next_seq = fs->rx_next_seq
    alu[rx_avail, --, B, *l$index1[9]];       // rx_avail = fs->rx_avail
    alu[tx_next_ts, --, B, *l$index1[6]];     // tx_next_ts = fs->tx_next_ts
    alu[tx_next_pos, --, B, *l$index1[5]];    // tx_next_pos = fs->tx_next_pos
  }
  result->seq     = tx_next_seq;
  result->ack     = rx_next_seq;
  result->win     = rx_avail;

  result->ts_ecr = tx_next_ts;

  /* Set DMA parameters */
  result->dma_pos = tx_next_pos;
  WORK_DMA_OFFSET_LEN_SET(result, payload_len, 0);

  __asm {
    alu[*l$index1[4], *l$index1[4], +, payload_len];    // fs->tx_next_seq += payload_len
    alu[*l$index1[5], *l$index1[5], +, payload_len];    // fs->tx_next_pos += payload_len
    alu[*l$index1[3], *l$index1[3], +, payload_len];    // fs->tx_sent     += payload_len
    alu[*l$index1[1], *l$index1[1], -, payload_len];    // fs->tx_avail    -= payload_len
  }

  /* Send FIN flag? */
  /* Make sure we don't send out the dummy byte for FIN */
  if (fin) {
    flags |= WORK_RESULT_FIN;
    WORK_DMA_OFFSET_LEN_SET(result, payload_len - 1, 0);
  }
  result->flags = flags;
}

__intrinsic void flows_ac(
                          struct work_t*         work,
                   __lmem struct flowst_tcp_t*   fs,
                 __xwrite struct work_result_t*  result)
{
  uint32_t rx_avail_prev, old_avail, new_avail, flags, fs_flags, rx_avail;
  uint32_t rx_bump, tx_bump;

  /* Set Active LM1 address as FS */
  __asm {
    local_csr_wr[local_csr_active_lm_addr_1, fs];
  }

  /* Copy work to result */
  result->work.__raw[0] = work->__raw[0];
  result->work.__raw[1] = work->__raw[1];
  result->work.__raw[2] = work->__raw[2];

  rx_bump = work->ac.rx_bump;
  tx_bump = work->ac.tx_bump;

  old_avail = tcp_txavail(fs, 0);
  new_avail = tcp_txavail(fs, tx_bump);

  /* mark connection as closed if requested */
  if (work->ac.fin)
  {
    __asm {
      ld_field_w_clr[fs_flags, 12, *l$index1[7], >>16];    // fs_flags = fs->flags
    }
    fs_flags |= FLEXNIC_PL_FLOWST_TXFIN;
    __asm {
      ld_field[*l$index1[7], 12, fs_flags, <<16];        // fs->flags = fs_flags
    }
  }

  /* Update flow state */
  __asm {
    alu[rx_avail_prev, --, B, *l$index1[9]];      // rx_avail_prev = fs->rx_avail
    alu[*l$index1[9], *l$index1[9], +, rx_bump];  // fs->rx_avail += atx->msg.connupdate.rx_bump;
    alu[*l$index1[1], *l$index1[1], +, tx_bump];  // fs->tx_avail += atx->msg.connupdate.tx_bump;
    alu[rx_avail, --, B, *l$index1[9]];           // rx_avail = fs->rx_avail
  }

  flags = 0;
  if (old_avail < new_avail) {
    result->qm_bump = new_avail - old_avail;
    flags |= WORK_RESULT_QM;
  }

#if FORCE_SEND_WINUPDATE
  /* receive buffer freed up from empty, need to send out a window update, if
   * we're not sending anyways. */
  if (new_avail == 0 && rx_avail_prev == 0 && rx_avail != 0) {
    flags |= WORK_RESULT_QM_FORCE;
  }
#endif

  result->flags = flags;
}

__intrinsic void flows_reset_retransmit(__lmem struct flowst_tcp_t* fs)
{
  /* NOTE: Active LM address already points to FS */
  uint32_t x;

  /* reset flow state as if we never transmitted those segments */
  __asm {
    immed_w0[*l$index1[7], 0];                // fs->dupack_cnt = 0
    alu[x, --, B, *l$index1[3]];              // x = fs->tx_sent
    alu[*l$index1[1], *l$index1[1], +, x]     // fs->tx_avail += x
    alu[*l$index1[2], *l$index1[2], +, x]     // fs->tx_remote_avail += x
    alu[*l$index1[4], *l$index1[4], -, x]     // fs->tx_next_seq -= x
    alu[*l$index1[5], *l$index1[5], -, x]     // fs->tx_next_pos -= x
    alu[*l$index1[3], --, B, 0]               // fs->tx_sent = 0
  }

}

__intrinsic void flows_retx(
                            struct work_t*         work,
                     __lmem struct flowst_tcp_t*   fs,
                   __xwrite struct work_result_t*  result)
{
  uint32_t old_avail, new_avail;
  uint32_t flags;

  /* Set Active LM1 address as FS */
  __asm {
    local_csr_wr[local_csr_active_lm_addr_1, fs];
  }

  /* Copy work to result */
  result->work.__raw[0] = work->__raw[0];
  result->work.__raw[1] = work->__raw[1];
  result->work.__raw[2] = work->__raw[2];
  flags = 0;

  old_avail = tcp_txavail(fs, 0);
  flows_reset_retransmit(fs);
  new_avail = tcp_txavail(fs, 0);

  if (old_avail < new_avail) {
    result->qm_bump = new_avail - old_avail;
    flags = WORK_RESULT_QM;
  }

  result->flags = flags;
}

__intrinsic void flows_ack(
                          struct work_t*         work,
                  __xread struct pkt_summary_t*  pkt,
                   __lmem struct flowst_tcp_t*   fs,
                 __xwrite struct work_result_t*  result)
{
  uint32_t old_avail, new_avail;
  uint32_t tx_sent, tx_pos, dupack_cnt, win, next_ts, rx_next_seq, tx_next_seq, ooo_len, fs_flags;
  uint32_t tx_bump, flags;

  /* Set Active LM1 address as FS */
  __asm {
    local_csr_wr[local_csr_active_lm_addr_1, fs];
  }

  tx_bump = 0;
  flags = 0;

  /* Copy work to result */
  result->work.__raw[0] = work->__raw[0];
  result->work.__raw[1] = work->__raw[1];
  result->work.__raw[2] = work->__raw[2];

  old_avail = tcp_txavail(fs, 0);

  if (tcp_rxseq_inwindow(fs, pkt->seq) != 0) {
    flags |= WORK_RESULT_TX;
    goto finalize;
  }

  if (tcp_valid_rxack(fs, pkt->ack, &tx_bump) == 0) {
    if (tx_bump != 0) {
      __critical_path();

      __asm {
        alu[tx_sent, --, B, *l$index1[3]];              // tx_sent = fs->tx_sent
        alu[--, tx_bump, -, tx_sent];                   // tx_bump - tx_sent
        bhs[flows_rx_1];                                // jump if higher to flows_rx_1
        alu[*l$index1[3], tx_sent, -, tx_bump];         // fs->tx_sent -= tx_bump;
        br[flows_rx_2];                                 // jump to flows_rx_2
        flows_rx_1:
#if ALLOW_FUTURE_ACKS
        alu[*l$index1[4], *l$index1[4], +, tx_bump];    // fs->tx_next_seq += tx_bump;
        alu[*l$index1[4], *l$index1[4], -, tx_sent];    // fs->tx_next_seq -= tx_sent;
        alu[*l$index1[1], *l$index1[1], -, tx_bump];    // fs->tx_avail -= tx_bump;
        alu[*l$index1[1], *l$index1[1], +, tx_sent];    // fs->tx_avail += tx_sent;
        alu[*l$index1[3], --, B, 0];                    // fs->tx_sent = 0;
        alu[tx_pos, *l$index1[5], +, tx_bump];          // tx_pos = fs->tx_next_pos + tx_bump;
        alu[tx_pos, tx_pos, -, tx_sent];                // tx_pos -= tx_sent;
        alu[*l$index1[5], --, B, tx_pos];               // fs->tx_next_pos = tx_pos
#endif
        flows_rx_2:
      }

      __asm {
        immed_w0[*l$index1[7], 0];                      // fs->dupack_cnt = 0;
        alu[tx_sent, --, B, *l$index1[3]];              // tx_sent = fs->tx_sent
      }

      flags |= WORK_RESULT_DMA_ACDESC;
      if (tx_sent == 0) {
        flags |= WORK_RESULT_TXP_ZERO;
      }
    }
    else {
      __asm {
        ld_field_w_clr[dupack_cnt, 3, *l$index1[7], >>0];     // dupack_cnt = fs->dupack_cnt;
        alu[dupack_cnt, dupack_cnt, +, 1];                    // dupack_cnt += 1
        ld_field[*l$index1[7], 3, dupack_cnt, >>0];           // fs->dupack_cnt = dupack_cnt
      }

      if (dupack_cnt >= 3) {
        /* reset to last acknowledged position */
        flows_reset_retransmit(fs);
        flags |= WORK_RESULT_RETX;
        goto finalize;
      }
    }
  }

  __asm {
    alu[rx_next_seq, --, B, *l$index1[10]];   // rx_next_seq = fs->rx_next_seq;
  }
  if (rx_next_seq != pkt->seq) {
    flags |= WORK_RESULT_TX;
    goto finalize;
  }

  /* update remote window */
  win = pkt->win;
  __asm {
    alu[*l$index1[2], --, B, win];       // fs->tx_remote_avail = pkt->win;
  }

  /* update next ts */
  next_ts = pkt->ts_val;
  __asm {
    alu[*l$index1[6], --, B, next_ts];   // fs->tx_next_ts = pkt->ts_val;
  }

  /* process FIN */
  if (pkt->flags & NET_TCP_FLAG_FIN) {
    __asm {
      alu[ooo_len, --, B, *l$index1[12]];       // ooo_len = fs->rx_ooo_len;
      ld_field_w_clr[fs_flags, 12, *l$index1[7], >>16];   // fs_flags = fs->flags
    }

    if (((fs_flags & FLEXNIC_PL_FLOWST_RXFIN) == 0) && (ooo_len == 0))
    {
      fs_flags |= FLEXNIC_PL_FLOWST_RXFIN;

      /* FIN takes up sequence number space */
      __asm {
        ld_field[*l$index1[7], 12, fs_flags, <<16];   // fs->flags = fs_flags;
        alu[*l$index1[10], *l$index1[10], +, 1];      // fs->rx_next_seq += 1;
      }

      flags |= (WORK_RESULT_FIN | WORK_RESULT_TX | WORK_RESULT_DMA_ACDESC);
    }
  }

finalize:
  /* Flow control: More receiver space? -> might need to start sending */
  new_avail = tcp_txavail(fs, 0);
  if (old_avail < new_avail) {
    result->qm_bump = (new_avail - old_avail);
    flags |= WORK_RESULT_QM;
  }

  result->ac_rx_bump = 0;
  result->ac_tx_bump = tx_bump;

  if (pkt->flags & NET_TCP_FLAG_ECE) {
    flags |= WORK_RESULT_ECNB;
  }

  result->ts_val = pkt->ts_ecr;
  if (flags & WORK_RESULT_TX) {
    __asm {
      alu[tx_next_seq, --, B, *l$index1[4]];     // tx_next_seq = fs->tx_next_seq;
      alu[rx_next_seq, --, B, *l$index1[10]];    // rx_next_seq = fs->rx_next_seq;
      alu[win, --, B, *l$index1[9]];             // rx_avail = fs->rx_avail;
      alu[next_ts, --, B, *l$index1[6]];         // tx_next_ts = fs->tx_next_ts;
    }
    result->seq = tx_next_seq;
    result->ack = rx_next_seq;
    result->win = win;
    result->ts_val = 0;                           // Ignore echo TS for OoO or discard data
    result->ts_ecr = next_ts;

    if (pkt->ecn) {
      flags |= WORK_RESULT_ECE;
    }
  }
  result->flags = flags;

  return;
}

__intrinsic void flows_seg(
                          struct work_t*         work,
                  __xread struct pkt_summary_t*  pkt,
                   __lmem struct flowst_tcp_t*   fs,
                 __xwrite struct work_result_t*  result)
{
  uint32_t payload_bytes, orig_payload;
  uint32_t flags;
  uint32_t old_avail, new_avail;
  uint32_t tx_bump, rx_bump;
  uint32_t diff, pos;
  uint32_t seq;
  uint32_t trim_start, trim_end;
  uint32_t tx_sent, tx_pos;
  uint32_t dupack_cnt, fs_flags;
  uint32_t tx_next_seq, rx_next_seq, rx_ooo_len, rx_ooo_start;
  uint32_t rx_avail, win, tx_next_ts;

  /* Set Active LM1 address as FS */
  __asm {
    local_csr_wr[local_csr_active_lm_addr_1, fs];
  }

  tx_bump = rx_bump = 0;
#if SKIP_ACK
  flags = 0;
#else
  flags = WORK_RESULT_TX;
#endif

  /* Copy work to result */
  result->work.__raw[0] = work->__raw[0];
  result->work.__raw[1] = work->__raw[1];
  result->work.__raw[2] = work->__raw[2];

  payload_bytes = orig_payload = work->io.plen;
  old_avail     = tcp_txavail(fs, 0);

#if ALLOW_OOO_RECV
  /* check if we should drop this segment */
  if (tcp_trim_rxbuf(fs, pkt->seq, payload_bytes, &trim_start, &trim_end) != 0) {
    /* packet is completely outside of the unused receive buffer */
#if SKIP_ACK
    flags |= WORK_RESULT_TX;
#endif
    goto finalize;
  }
#else
  /* check if we should drop this segment */
  if (tcp_valid_rxseq(fs, pkt->seq, payload_bytes, &trim_start, &trim_end) != 0) {
#if SKIP_ACK
    flags |= WORK_RESULT_TX;
#endif
    goto finalize;
  }
#endif

  __critical_path();

  /* if there is a valid ack, process it */
  if (tcp_valid_rxack(fs, pkt->ack, &tx_bump) == 0) {
    if (tx_bump != 0) {
      __critical_path();
      __asm {
        alu[tx_sent, --, B, *l$index1[3]];              // tx_sent = fs->tx_sent
        alu[--, tx_bump, -, tx_sent];                   // tx_bump - tx_sent
        bhs[flows_rx_1];                                // jump if higher to flows_rx_1
        alu[*l$index1[3], tx_sent, -, tx_bump];         // fs->tx_sent -= tx_bump;
        br[flows_rx_2];                                 // jump to flows_rx_2
        flows_rx_1:
#if ALLOW_FUTURE_ACKS
        alu[*l$index1[4], *l$index1[4], +, tx_bump];    // fs->tx_next_seq += tx_bump;
        alu[*l$index1[4], *l$index1[4], -, tx_sent];    // fs->tx_next_seq -= tx_sent;
        alu[*l$index1[1], *l$index1[1], -, tx_bump];    // fs->tx_avail -= tx_bump;
        alu[*l$index1[1], *l$index1[1], +, tx_sent];    // fs->tx_avail += tx_sent;
        alu[*l$index1[3], --, B, 0];                    // fs->tx_sent = 0;
        alu[tx_pos, *l$index1[5], +, tx_bump];          // tx_pos = fs->tx_next_pos + tx_bump;
        alu[tx_pos, tx_pos, -, tx_sent];                // tx_pos -= tx_sent;
        alu[*l$index1[5], --, B, tx_pos];               // fs->tx_next_pos = tx_pos
#endif
        flows_rx_2:
      }

      __asm {
        immed_w0[*l$index1[7], 0];                      // fs->dupack_cnt = 0;
        alu[tx_sent, --, B, *l$index1[3]];              // tx_sent = fs->tx_sent
      }

      flags |= WORK_RESULT_DMA_ACDESC;
      if (tx_sent == 0) {
        flags |= WORK_RESULT_TXP_ZERO;
      }
    }
  }

#if ALLOW_OOO_RECV
  /* trim payload to what we can actually use */
  payload_bytes -= trim_start + trim_end;
  seq = pkt->seq + trim_start;

  __asm {
    alu[rx_next_seq, --, B, *l$index1[10]];   // rx_next_seq = fs->rx_next_seq
  }
  /* handle out of order segment */
  if (seq != rx_next_seq) {
#if SKIP_ACK
    flags |= WORK_RESULT_TX;
#endif

    /* if there is no payload abort immediately */
    if (payload_bytes == 0) {
      goto finalize;
    }

    /* otherwise check if we can add it to the out of order interval */
    diff = seq - rx_next_seq;
    __asm {
      alu[pos, *l$index1[11], +, diff]; // pos = rx_next_pos + diff
    }

    __asm {
      alu[rx_ooo_len, --, B, *l$index1[12]];    // rx_ooo_len = fs->rx_ooo_len;
      alu[rx_ooo_start, --, B, *l$index1[13]];  // rx_ooo_start = fs->rx_ooo_start;
    }

    if (rx_ooo_len == 0) {
      __asm {
        alu[*l$index1[12], --, B, payload_bytes];           // fs->rx_ooo_len = payload_bytes
        alu[*l$index1[13], --, B, seq];                     // fs->rx_ooo_start = seq
      }

      result->dma_pos = pos;
      WORK_DMA_OFFSET_LEN_SET(result, payload_bytes, trim_start);
      flags |= WORK_RESULT_DMA_PAYLOAD;
    } else if (seq + payload_bytes == rx_ooo_start) {
      /* TODO: those two overlap checks should be more sophisticated */
      __asm {
        alu[*l$index1[12], *l$index1[12], +, payload_bytes];  // fs->rx_ooo_len += payload_bytes
        alu[*l$index1[13], --, B, seq];                       // fs->rx_ooo_start = seq
      }

      result->dma_pos = pos;
      WORK_DMA_OFFSET_LEN_SET(result, payload_bytes, trim_start);
      flags |= WORK_RESULT_DMA_PAYLOAD;
    } else if (rx_ooo_start + rx_ooo_len == seq) {
      /* TODO: those two overlap checks should be more sophisticated */
      __asm {
        alu[*l$index1[12], *l$index1[12], +, payload_bytes];  // fs->rx_ooo_len += payload_bytes
      }

      result->dma_pos = pos;
      WORK_DMA_OFFSET_LEN_SET(result, payload_bytes, trim_start);
      flags |= WORK_RESULT_DMA_PAYLOAD;
    } else {
      /* FIXME */
    }
    goto finalize;
  }
#else
  /* trim payload to what we can actually use */
  payload_bytes -= trim_start + trim_end;
#endif

  /* update next ts */
  tx_next_ts = pkt->ts_val;
  __asm {
    alu[*l$index1[6], --, B, tx_next_ts];   // fs->tx_next_ts = pkt->ts_val;
  };

  /* update remote window */
  win = pkt->win;
  __asm {
    alu[*l$index1[2], --, B, win];   // fs->tx_remote_avail = pkt->win;
  }

  /* make sure we don't receive anymore payload after FIN */
  __asm {
    ld_field_w_clr[fs_flags, 12, *l$index1[7], >>16];   // fs_flags = fs->flags
  }
  if (((fs_flags & FLEXNIC_PL_FLOWST_RXFIN) != 0) && (payload_bytes != 0)) {
    goto finalize;
  }

  /* if there is payload, dma it to the receive buffer */
  if (payload_bytes != 0) {
    __asm {
      alu[pos, --, B, *l$index1[11]];                       // pos = fs->rx_next_pos
    }
    result->dma_pos = pos;
    WORK_DMA_OFFSET_LEN_SET(result, payload_bytes, trim_start);
    flags |= (WORK_RESULT_DMA_PAYLOAD | WORK_RESULT_DMA_ACDESC);

    rx_bump = payload_bytes;
    __asm {
      alu[*l$index1[9], *l$index1[9], -, payload_bytes];    // fs->rx_avail      -= payload_bytes;
      alu[*l$index1[10], *l$index1[10], +, payload_bytes];  // fs->rx_next_seq   += payload_bytes;
      alu[*l$index1[11], *l$index1[11], +, payload_bytes];  // temp = fs->rx_next_pos + payload_bytes
    }

#if ALLOW_OOO_RECV
    /* if we have out of order segments, check whether buffer is continuous
     * or superfluous */
    __asm {
      alu[rx_ooo_len, --, B, *l$index1[12]];        // rx_ooo_len = fs->rx_ooo_len
      alu[rx_ooo_start, --, B, *l$index1[13]];      // rx_ooo_start = fs->rx_ooo_start
    }
    if (rx_ooo_len != 0) {
      if (tcp_trim_rxbuf(fs, rx_ooo_start, rx_ooo_len, &trim_start, &trim_end) != 0) {
        /* completely superfluous: drop out of order interval */
        __asm {
          alu[*l$index1[12], --, B, 0];           // fs->rx_ooo_len = 0;
        }
      } else {
        __asm {
          alu[*l$index1[13], *l$index1[13], +, trim_start];     // fs->rx_ooo_start += trim_start;
          alu[*l$index1[12], *l$index1[12], -, trim_start];     // fs->rx_ooo_len -= trim_start;
          alu[*l$index1[12], *l$index1[12], -, trim_end];       // fs->rx_ooo_len -= trim_end;
          alu[rx_ooo_len, --, B, *l$index1[12]];                // rx_ooo_len = fs->rx_ooo_len
          alu[rx_ooo_start, --, B, *l$index1[13]];              // rx_ooo_start = fs->rx_ooo_start
          alu[rx_next_seq, --, B, *l$index1[10]];               // rx_next_seq = fs->rx_next_seq
        }

        if (rx_ooo_len > 0 && rx_ooo_start == rx_next_seq) {
          /* yay, we caught up, make continuous and drop OOO interval */

          __asm {
            alu[rx_bump, rx_bump, +, rx_ooo_len];             // rx_bump           += rx_ooo_len;
            alu[*l$index1[9], *l$index1[9], -, rx_ooo_len];   // fs->rx_avail      -= rx_ooo_len;
            alu[*l$index1[10], *l$index1[10], +, rx_ooo_len]; // fs->rx_next_seq   += rx_ooo_len;
            alu[*l$index1[12], --, B, 0];                     // fs->rx_ooo_len     = 0;
            alu[*l$index1[11], *l$index1[11], +, rx_ooo_len]; // temp = fs->rx_next_pos + rx_ooo_len;
          }
        }
      }
    }
    __critical_path();
#endif
  }

  if (((pkt->flags & NET_TCP_FLAG_FIN) != 0) && ((fs_flags & FLEXNIC_PL_FLOWST_RXFIN) == 0)) {
    __asm {
      alu[rx_next_seq, --, B, *l$index1[10]];   // rx_next_seq = fs->rx_next_seq;
      alu[rx_ooo_len, --, B, *l$index1[12]];    // rx_ooo_len = fs->rx_ooo_len;
    }

    if ((rx_next_seq == pkt->seq + orig_payload) && (rx_ooo_len == 0)) {
      flags |= (WORK_RESULT_FIN | WORK_RESULT_DMA_ACDESC | WORK_RESULT_TX);
      fs_flags |= FLEXNIC_PL_FLOWST_RXFIN;

      /* FIN takes up sequence number space */
      __asm {
        ld_field[*l$index1[7], 12, fs_flags, <<16];   // fs->flags = fs_flags;
        alu[*l$index1[10], *l$index1[10], +, 1];      // fs->rx_next_seq += 1;
      }
    }
  }

finalize:
  /* Flow control: More receiver space? -> might need to start sending */
  new_avail = tcp_txavail(fs, 0);
  if (old_avail < new_avail) {
    result->qm_bump = (new_avail - old_avail);
    flags |= WORK_RESULT_QM;
  }

  result->ac_rx_bump = rx_bump;
  result->ac_tx_bump = tx_bump;

  if (pkt->flags & NET_TCP_FLAG_ECE) {
    flags |= WORK_RESULT_ECNB;
  }

  result->ts_val = pkt->ts_ecr;
#if SKIP_ACK
  if (flags & WORK_RESULT_TX) {
#endif
    __asm {
      alu[tx_next_seq, --, B, *l$index1[4]];    // tx_next_seq = fs->tx_next_seq;
      alu[rx_next_seq, --, B, *l$index1[10]];   // rx_next_seq = fs->rx_next_seq;
      alu[rx_avail, --, B, *l$index1[9]];       // rx_avail = fs->rx_avail;
      alu[tx_next_ts, --, B, *l$index1[6]];     // tx_next_ts = fs->tx_next_ts;
    }
    result->seq = tx_next_seq;
    result->ack = rx_next_seq;
    result->win = rx_avail;
    result->ts_ecr = tx_next_ts;

    if (pkt->ecn) {
      flags |= WORK_RESULT_ECE;
    }

#if SKIP_ACK
  }
#endif
  result->flags = flags;
}
