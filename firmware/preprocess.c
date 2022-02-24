/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdint.h>
#include <memory.h>

#include <nfp.h>
#include <nfp/me.h>
#include <nfp/cls.h>
#include <nfp_intrinsic.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_pe.h>
#include <std/hash.h>
#include <std/reg_utils.h>
#include <pkt/pkt.h>
#include <gro/gro_cli.h>
#include <blm.h>

#include "packet_defs.h"
#include "fp_mem.h"
#include "pipeline.h"
#include "dma.h"
#include "qman.h"
#include "flow_lookup.h"

#include "shared.h"
#include "global.h"
#include "debug.h"

__export __shared __cls struct ctm_pkt_credits ctm_credits;

__forceinline unsigned int get_flowgrp_rnum(uint32_t flow_grp)
{
  unsigned int rnum;

  /* Number of flowgrps is 4 */
  switch (flow_grp) {
  case 0:
    STATS_INC(GRP0);
    rnum = MEM_RING_GET_NUM(flow_ring_0);
    break;

  case 1:
    STATS_INC(GRP1);
    rnum = MEM_RING_GET_NUM(flow_ring_1);
    break;

  case 2:
    STATS_INC(GRP2);
    rnum = MEM_RING_GET_NUM(flow_ring_2);
    break;

  case 3:
    STATS_INC(GRP3);
    rnum = MEM_RING_GET_NUM(flow_ring_3);
    break;

  default:
    STATS_INC(RX_SP);
    rnum = MEM_RING_GET_NUM(flow_ring_sp);
    break;
  }

  return rnum;
}

__forceinline void prepare_gro_meta(__xwrite struct gro_meta_memq* gro_meta,
                                    __gpr    struct work_t* work,
                                    uint32_t flow_grp)
{
  unsigned int rnum;

  rnum = get_flowgrp_rnum(flow_grp);
  gro_cli_build_workq_meta3(gro_meta, EMEM_ISL, rnum,
        work->__raw[0], work->__raw[1], work->__raw[2]);
}

/** Raw metadata fields for validation */
/** NOTE: These are determined from experimentation */
/**
 *  Word 3:
 *    Reserved [31-20]   0
 *    E        [19]      0
 *    TLD      [18-16]   0
 *    OL4      [15-12]   3    -> TCP
 *    OL3      [11-8]    4    -> IP
 *    OL2      [7-3]     2    -> ETH
 *    R        [2-0]     0
 */
#define PKT_META_RAW3           0x00003420
/**
 *  Word 4:
 *    Port     [31-24]   NS_PLATFORM_NBI_TM_QID_UNTAGGED(PORT_IN_USE, 0)
 *    HP-Off1  [23-16]   0x4A -> MAC_PREPEND_BYTES + NET_ETH_LEN + NET_IP4_LEN + NET_TCP_LEN + NET_TCP_OPT_LEN_PADTIMESTAMP
 *    HP-Off0  [15-8]    0x2A -> MAC_PREPEND_BYTES + NET_ETH_LEN + NET_IP4_LEN
 *    F        [7]       0
 *    Misc     [6-0]     0
 */
#define PKT_META_RAW4           ((NS_PLATFORM_MAC_CHANNEL_UNTAGGED(PORT_IN_USE) << 24) | (0x4A << 16) | (0x2A << 8))
/**
 *  Word 5:
 *    Flags    [31-24]    0
 *    O-Offset [23-16]    0x16 -> MAC_PREPEND_BYTES + NET_ETH_LEN
 *    LIF+Mode [15-0]     0
 */
#define PKT_META_RAW5           0x00160000

/**
 *  Word 7: MAC prepend
 *    P_STS    [31-29]    2   -> TCP_SUM_OK
 *    MEAFDRH  [28-23]    0
 *    L3       [22-20]    3   -> IPv4 OK
 *    MPL-VLN  [19-16]    0
 *    Checksum [15-0]     IGNORE
 */
#define PKT_META_RAW7           0x40300000

__intrinsic int validate_pkt_meta(__xread struct pkt_raw_t* pkt)
{
  int cond;

  cond =  (pkt->meta.__raw[3] == PKT_META_RAW3) &&
          (pkt->meta.__raw[4] == PKT_META_RAW4) &&
          (pkt->meta.__raw[5] == PKT_META_RAW5) &&
          ((pkt->mac_prepend & PKT_META_RAW7) == PKT_META_RAW7);

  return cond;
}

/*> TCP flags that are handled within fastpath */
#define TCP_FP_FLAGS      (NET_TCP_FLAG_ACK   | NET_TCP_FLAG_PSH |  \
                           NET_TCP_FLAG_ECE   | NET_TCP_FLAG_CWR |  \
                           NET_TCP_FLAG_FIN)
/*> TCP Padded TS Option (NOP_KIND + NOP_KIND + TS_KIND + TS_LEN) */
#define PKT_TCP_OPT_RAW0       0x0101080A

__intrinsic int validate_pkt_hdr(__xread struct pkt_hdr_t* hdr)
{
  int cond;

  cond =  ((hdr->tcp.flags & (~TCP_FP_FLAGS)) == 0) &&
          (hdr->tcpopts.__raw[0] == PKT_TCP_OPT_RAW0);

  return cond;
}

__intrinsic void generate_pkt_summary(__xread struct pkt_hdr_t* hdr,
                                  __xwrite struct pkt_summary_t* pkt_summary)
{
  uint32_t raw;

  pkt_summary->seq = hdr->tcp.seq;
  pkt_summary->ack = hdr->tcp.ack;
  raw = (hdr->__raw[12] & 0xFFFFFF); // TCP flags;
  if (hdr->ip.tos == 3) /* Congestion indicated */
    raw |= (1 << 31);
  pkt_summary->__raw[2] = raw;
  pkt_summary->ts_val = hdr->tcpopts.ts_val;
  pkt_summary->ts_ecr = hdr->tcpopts.ts_ecr;
}

struct pkt_hdr_preproc_t
{
  union {
    __packed struct {
      uint16_t        __pad;     /*> Padding to align IP/TCP */
      struct eth_hdr  eth;
      struct ip4_hdr  ip;
      uint16_t        sport;     /*> L4: Source port */
      uint16_t        dport;     /*> L4: Destination port */
    };

    uint32_t __raw[10];
  };
};

__intrinsic void alloc_packet_buffer(__gpr struct nbi_meta_pkt_info* pkt)
{
  unsigned int pnum;
  __xread blm_buf_handle_t buf;

  /* Allocate CTM buffer */
  while (1) {
    /* Allocate and Replenish credits */
    pnum = pkt_ctm_alloc(&ctm_credits, __ISLAND, PKTBUF_CTM_SIZE, 1, 1);
    if (pnum != ((unsigned int)-1))
      break;
  }

  /* Allocate MU buffer */
  while (1) {
    if (blm_buf_alloc(&buf, 0) == 0)
      break;
  }

  pkt->isl   = __ISLAND;
  pkt->pnum  = pnum;
  pkt->bls   = 0;
  pkt->muptr = buf;
  pkt->split = 1;
  pkt->len   = 0;
}

__forceinline void prepare_ack_header(__xread  struct pkt_hdr_t* hdr,
                                __xwrite struct pkt_hdr_preproc_t* hdr_mod)
{
  __gpr struct pkt_hdr_preproc_t hdr_temp;

  hdr_temp.eth.src = hdr->eth.dst;
  hdr_temp.eth.dst = hdr->eth.src;
  hdr_temp.eth.type = hdr->eth.type;

  /* Banking on the compiler to eliminate redundant copies */
  hdr_temp.ip = hdr->ip;
  hdr_temp.ip.len = NET_IP4_LEN + NET_TCP_LEN + NET_TCP_OPT_LEN_PADTIMESTAMP; /* Modify length to ACK length */
  hdr_temp.ip.tos = 0;    /* Mark ACKs as ECN incapable */
  hdr_temp.ip.ttl = 0xff;
  hdr_temp.ip.src = hdr->ip.dst;
  hdr_temp.ip.dst = hdr->ip.src;

  hdr_temp.sport = hdr->tcp.dport;
  hdr_temp.dport = hdr->tcp.sport;

  *hdr_mod = hdr_temp;
}

__shared struct eth_addr local_mac;              /*> Local MAC address */

#define IP_RAW_WORD0(tos, len)     (0x45000000 | (tos << 16) | (len))     /*> Ver: 4 | HL: 5 | TOS: y | LEN: x */
#define IP_RAW_WORD1               0x00034000                             /*> ID: 3 | Frag: DF */
#define IP_RAW_WORD2               0xff060000                             /*> TTL: 255 | Proto: TCP | Chksum: 0 */
#define IP_RAW_WORD3(srcip)        (srcip)                                /*> srcip */
#define IP_RAW_WORD4(dstip)        (dstip)                                /*> dstip */

__forceinline void prepare_seg_header(__xwrite struct pkt_hdr_preproc_t* hdr_write,
                                      __lmem   struct flowst_conn_t*     conn_info)
{
  __gpr struct pkt_hdr_preproc_t hdr;

  /*> NOTE: This function depends on struct layout of flowst_conn_t. DO NOT MODIFY! */
  /*> Hopefully, the compiler can eliminate GPR convenience variables */

  /* Remote MAC address */
  hdr.__raw[0] = (conn_info->__raw[1] >> 16);   /*> Upper nibble */
  hdr.__raw[1] = (conn_info->__raw[1] << 16) | (conn_info->__raw[2] >> 16); /*> Bottom word */

  /* Local MAC & Ethertype */
  hdr.eth.src  = local_mac;
  hdr.eth.type = NET_ETH_TYPE_IPV4;

  /* IP WORD 0 */
  if ((conn_info->__raw[2] & FLEXNIC_PL_FLOWST_ECN) != 0) { /*> ECN flag enabled ? */
    hdr.__raw[4] = IP_RAW_WORD0(2, NET_IP4_LEN + NET_TCPPADTS_LEN);
  } else {
    hdr.__raw[4] = IP_RAW_WORD0(0, NET_IP4_LEN + NET_TCPPADTS_LEN);
  }

  /* IP WORD 1-2 */
  hdr.__raw[5] = IP_RAW_WORD1;
  hdr.__raw[6] = IP_RAW_WORD2;

  /* IP WORD 3-4 SRC/DST IP */
  hdr.__raw[7] = IP_RAW_WORD3(conn_info->__raw[3]);
  hdr.__raw[8] = IP_RAW_WORD4(conn_info->__raw[4]);

  /* TCP SRC/DST Port */
  hdr.__raw[9] = conn_info->__raw[5];

  /** Copy GPR -> XW */
  hdr_write->__raw[0] = hdr.__raw[0];
  hdr_write->__raw[1] = hdr.__raw[1];
  hdr_write->__raw[2] = hdr.__raw[2];
  hdr_write->__raw[3] = hdr.__raw[3];
  hdr_write->__raw[4] = hdr.__raw[4];
  hdr_write->__raw[5] = hdr.__raw[5];
  hdr_write->__raw[6] = hdr.__raw[6];
  hdr_write->__raw[7] = hdr.__raw[7];
  hdr_write->__raw[8] = hdr.__raw[8];
  hdr_write->__raw[9] = hdr.__raw[9];
}

#define CACHE_CLEAN_DURATION                (1 << 30)   /* Roughly every 20 seconds with 800Mhz clock */

#define FLOWHASH_CACHE_SIZE                 (1 << 7)                      /* 128 * 16 = 2k */
#define FLOWHASH_CACHE_MASK                 (FLOWHASH_CACHE_SIZE - 1)
__shared __lmem struct flowht_entry_t flowhash_cache[FLOWHASH_CACHE_SIZE];

#define CONN_CACHE_SIZE                     (1 << 4)                      /* 16  * 32 = 512 */
#define CONN_CACHE_MASK                     (CONN_CACHE_SIZE - 1)
__shared __lmem struct flowst_conn_t  conn_cache[CONN_CACHE_SIZE];

__forceinline void reset_cache()
{
  memset_lmem(&flowhash_cache[0], 0, FLOWHASH_CACHE_SIZE * sizeof(struct flowht_entry_t));

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

__forceinline uint32_t lookup_flowhash(__xread struct pkt_hdr_t* hdr,
                                       unsigned int* flow_grp)
{
  int idx;
  uint32_t flow_id;
  __gpr struct flow_key_t key;
  uint64_t hash;
  reg_cp(&key, (void*) &hdr->ip.src, sizeof(struct flow_key_t));

  hash = flow_hash(&key);
  idx  = hash & FLOWHASH_CACHE_MASK;
  *flow_grp = hash & (NUM_FLOW_GROUPS - 1);

  /* Match with Slot IDX */
  if (key.__raw[0] == flowhash_cache[idx].key.__raw[0] &&
      key.__raw[1] == flowhash_cache[idx].key.__raw[1] &&
      key.__raw[2] == flowhash_cache[idx].key.__raw[2])
  {
    return flowhash_cache[idx].flow_id;
  }

  /* Lookup & insert in cache */
  flow_id = flow_lookup(&key, hash);
  if (flow_id != INVALID_FLOWID) {
    flowhash_cache[idx].key     = key;
    flowhash_cache[idx].flow_id = flow_id;
  }

  return flow_id;
}

__forceinline unsigned int handle_conn_cache_miss(uint32_t flow_id)
{
  __xread struct flowst_conn_t conn_info;

  __mem40 void* addr;
  cam_lookup_t conn_lkup;

  addr = (__mem40 void*) &fp_state.flows_conn_info[flow_id];
  mem_read32(&conn_info, addr, sizeof(conn_info));

  conn_lkup = cam_lookup(flow_id);
  if (conn_lkup.hit)
    return conn_lkup.entry_num;

  cam_write(conn_lkup.entry_num, flow_id, 0);
  conn_cache[conn_lkup.entry_num] = conn_info;

  return conn_lkup.entry_num;
}

__forceinline void issue_fetch_rx(__xread struct pkt_raw_t* pkt,
                                  SIGNAL* pkt_sig)
{
  __pkt_nbi_recv_with_hdrs(pkt, sizeof(struct pkt_raw_t), PKT_NBI_OFFSET, sig_done, pkt_sig);
}

__forceinline void preprocess_rx(__xread struct pkt_raw_t*     pkt,
                              SIGNAL* pkt_sig)
{
  __xread  struct pkt_hdr_t         hdr;
  __xwrite struct pkt_hdr_preproc_t hdr_mod;
  __xwrite struct pkt_summary_t     pkt_summary;
  __xwrite struct gro_meta_memq     gro_meta;
  __xwrite uint32_t egress_cmd;
  __xwrite uint32_t pkt_modscript_info[2];
  SIGNAL summary_sig, hdr_mod_sig, egress_cmd_sig, pkt_msi_sig;

  uint32_t seq, seqr;
  uint32_t flow_id, flow_grp;
  __mem40 uint8_t* pbuf;

  __gpr struct work_t work;

  STATS_INC(RX_TOTAL);

  flow_grp     = NUM_FLOW_GROUPS; /* SLOWPATH */
  work.io.type    = WORK_TYPE_RX;
  work.io.cbs     = compute_ctm_size(&pkt->meta.pkt_info);
  work.io.isl     = pkt->meta.pkt_info.isl - 32;
  work.io.pnum    = pkt->meta.pkt_info.pnum;
  work.io.bls     = pkt->meta.pkt_info.bls;
  work.io.muptr   = pkt->meta.pkt_info.muptr;
  work.io.flow_id = 0;
  work.io.plen    = pkt->meta.pkt_info.len;

  if (!validate_pkt_meta(pkt))
    goto FORWARD;

  pbuf = pkt_ctm_ptr40(pkt->meta.pkt_info.isl, pkt->meta.pkt_info.pnum, 0);
  mem_read32(&hdr, pbuf + PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2, sizeof(struct pkt_hdr_t));

  if (!validate_pkt_hdr(&hdr))
    goto FORWARD;

  generate_pkt_summary(&hdr, &pkt_summary);
  prepare_ack_header(&hdr, &hdr_mod);

  /* Issue summary, header writes hoping for lookup to succeed! */
  __mem_write32(&pkt_summary, pbuf + sizeof(struct nbi_meta_catamaran),
                sizeof(pkt_summary), sizeof(pkt_summary),
                sig_done, &summary_sig);

  __mem_write32(&hdr_mod, pbuf + PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2,
                sizeof(hdr_mod), sizeof(hdr_mod),
                sig_done, &hdr_mod_sig);

  /* Write Egress CMD */
  __pkt_mac_egress_cmd_write(pbuf, PKT_NBI_OFFSET + MAC_PREPEND_BYTES, 1, 1, &egress_cmd, sig_done, &egress_cmd_sig);
  __pkt_msd_write(pbuf, PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 4, pkt_modscript_info, sizeof(pkt_modscript_info), sig_done, &pkt_msi_sig);

  flow_id = lookup_flowhash(&hdr, &flow_grp);
  if (flow_id == INVALID_FLOWID) {
    /* This path is unlikely to occur! */
    __wait_for_all(&summary_sig, &hdr_mod_sig, &egress_cmd_sig, &pkt_msi_sig);
    flow_grp = NUM_FLOW_GROUPS;
    goto FORWARD;
  }

  work.io.flow_id = flow_id;
  work.io.plen   -= (MAC_PREPEND_BYTES + NET_HDR_LEN);
  __wait_for_all(&summary_sig, &hdr_mod_sig, &egress_cmd_sig, &pkt_msi_sig);

FORWARD:
  seq  = pkt->meta.seq;
  seqr = RX_SEQR(pkt->meta.seqr);
  work.io.seqr = flow_grp + 1;
  prepare_gro_meta(&gro_meta, &work, flow_grp);

  gro_cli_send(&gro_meta, seqr, seq);

  /* Issue new work fetch */
  issue_fetch_rx(pkt, pkt_sig);
}

__forceinline void issue_fetch_tx(__xread struct schedule_t* sched,
                                  SIGNAL* sched_sig)
{
  unsigned int grp = (__ISLAND - 32);
  unsigned int raddr_hi = MEM_RING_GET_MEMADDR(qm_sched_ring0);

  __mem_workq_add_thread(QM_SCHED_RNUM_BASE + grp, raddr_hi, sched,
      sizeof(struct schedule_t), sizeof(struct schedule_t),
      sig_done, sched_sig);
}

__forceinline void preprocess_tx(__xread struct schedule_t* sched,
                                SIGNAL* sched_sig)
{
  __gpr    struct nbi_meta_pkt_info pkt_info;
  __xwrite struct nbi_meta_pkt_info pkt_info_write;
  __xwrite struct pkt_hdr_preproc_t hdr;
  __xwrite struct gro_meta_memq     gro_meta;
  __xwrite uint32_t egress_cmd;
  __xwrite uint32_t pkt_modscript_info[2];
  SIGNAL pkt_info_sig, hdr_sig, egress_cmd_sig, pkt_msi_sig;

  uint32_t seq, seqr;
  __mem40 uint8_t* pbuf;
  unsigned int conn_info_idx;
  cam_lookup_t conn_lkup;
  uint32_t flow_grp;
  unsigned int rnum, raddr_hi;

  __gpr struct work_t work;
  __xwrite struct work_t work_xfer;

  STATS_INC(TX_TOTAL);

  /* Allocate PKT buffer */
  alloc_packet_buffer(&pkt_info);
  pkt_info.len = MAC_PREPEND_BYTES + NET_HDR_LEN;
  pkt_info_write = pkt_info;

  pbuf = pkt_ctm_ptr40(pkt_info.isl, pkt_info.pnum, 0);
  __mem_write32(&pkt_info_write, pbuf, sizeof(pkt_info_write), sizeof(pkt_info_write), sig_done, &pkt_info_sig);

  /* Fetch conn info */
  conn_lkup     = cam_lookup(sched->flow_id);
  conn_info_idx = conn_lkup.entry_num;
  if (!conn_lkup.hit) {
    conn_info_idx = handle_conn_cache_miss(sched->flow_id);
  }

  /* Prepare segment header */
  flow_grp = conn_cache[conn_info_idx].flow_grp;
  prepare_seg_header(&hdr, &conn_cache[conn_info_idx]);
  __mem_write32(&hdr, pbuf + PKT_NBI_OFFSET + MAC_PREPEND_BYTES, sizeof(hdr), sizeof(hdr), sig_done, &hdr_sig);

  /* Write Egress CMD */
  __pkt_mac_egress_cmd_write(pbuf, PKT_NBI_OFFSET + MAC_PREPEND_BYTES + 2, 1, 1, &egress_cmd, sig_done, &egress_cmd_sig);
  __pkt_msd_write(pbuf, PKT_NBI_OFFSET + MAC_PREPEND_BYTES - 2, pkt_modscript_info, sizeof(pkt_modscript_info), sig_done, &pkt_msi_sig);

  /* Prepare Work */
  work.io.type    = WORK_TYPE_TX;
  work.io.cbs     = PKTBUF_CTM_SIZE;
  work.io.isl     = pkt_info.isl - 32;
  work.io.pnum    = pkt_info.pnum;
  work.io.force   = sched->force;
  work.io.bls     = pkt_info.bls;
  work.io.muptr   = pkt_info.muptr;
  work.io.flow_id = sched->flow_id;
  work.io.seqr    = flow_grp + 1;
  work.io.plen    = 0;

  __wait_for_all(&pkt_info_sig, &hdr_sig, &egress_cmd_sig, &pkt_msi_sig);

  work_xfer = work;
  rnum = get_flowgrp_rnum(flow_grp);
  raddr_hi = (EMEM_ISL | 0x80) << 24;

  mem_workq_add_work(rnum, raddr_hi, &work_xfer, sizeof(work_xfer));

  /* Issue new work fetch */
  issue_fetch_tx(sched, sched_sig);
}

__forceinline void issue_fetch_ac(__xread struct actx_bump_t* bump,
                                  SIGNAL* bump_sig)
{
  unsigned int rnum, raddr_hi;

  rnum     = MEM_RING_GET_NUM(actx_bump_ring);
  raddr_hi = MEM_RING_GET_MEMADDR(actx_bump_ring);

  __mem_workq_add_thread(rnum, raddr_hi, bump, sizeof(struct actx_bump_t), sizeof(struct actx_bump_t), sig_done, bump_sig);
}

__forceinline void preprocess_ac(__xread struct actx_bump_t* bump,
                                SIGNAL* bump_sig)
{
  __xread struct flextcp_pl_atx_t atx;

  __mem40 uint8_t* pbuf;
  uint32_t desc_idx, flow_grp;
  unsigned int rnum, raddr_hi;

  __gpr struct work_t work;
  __xwrite struct work_t work_xfer;

  STATS_INC(AC_TOTAL);

  /* Read ATX descriptor */
  desc_idx = bump->desc_idx;
  pbuf = (__mem40 void*) &atx_desc_pool[bump->desc_idx];
  mem_read32(&atx, pbuf, sizeof(atx));

  flow_grp = atx.msg.connupdate.flow_grp;

  /* Prepare work */
  work.ac.type     = WORK_TYPE_AC;
  work.ac.fin      = ((atx.msg.connupdate.flags & FLEXTCP_PL_ATX_FLTXDONE) != 0);
  work.ac.rx_bump  = atx.msg.connupdate.rx_bump;
  work.ac.tx_bump  = atx.msg.connupdate.tx_bump;
  work.ac.flow_id  = atx.msg.connupdate.flow_id;
  work.ac.desc_idx = desc_idx;

  work_xfer = work;
  rnum = get_flowgrp_rnum(flow_grp);
  raddr_hi = (EMEM_ISL | 0x80) << 24;

  mem_workq_add_work(rnum, raddr_hi, &work_xfer, sizeof(work_xfer));

  /* Issue new work fetch */
  issue_fetch_ac(bump, bump_sig);
}

__forceinline void preprocess_rx_thread()
{
  __xread  struct pkt_raw_t pkt;
  SIGNAL pkt_sig;

  /* Kickstart */
  issue_fetch_rx(&pkt, &pkt_sig);

  for (;;) {
    __wait_for_all(&pkt_sig);

    preprocess_rx(&pkt, &pkt_sig);

    __implicit_read(&pkt);
    __implicit_write(&pkt_sig);
  }

  return;
}

__forceinline void preprocess_tx_thread()
{
  __xread struct schedule_t sched;
  SIGNAL sched_sig;

  /* Kickstart */
  issue_fetch_tx(&sched, &sched_sig);

  for (;;) {
    __wait_for_all(&sched_sig);

    preprocess_tx(&sched, &sched_sig);

    __implicit_read(&sched);
    __implicit_write(&sched_sig);
  }

  return;
}

__forceinline void preprocess_ac_thread()
{
  __xread struct actx_bump_t bump;
  SIGNAL bump_sig;

  /* Kickstart */
  issue_fetch_ac(&bump, &bump_sig);

  for (;;) {
    __wait_for_all(&bump_sig);

    preprocess_ac(&bump, &bump_sig);

    __implicit_read(&bump);
    __implicit_write(&bump_sig);
  }

  return;
}

int main()
{
  SIGNAL start_sig;

  /* Timestamp */
  uint32_t t_prev, t_curr, t_diff;

  if (ctx() == 0) {
    gro_cli_init();
    reset_cache();
    enable_global_timestamp();

    pkt_ctm_init_credits(&ctm_credits, ME_CTM_ALLOC_MAX_PKT_CREDITS - 1, ME_CTM_ALLOC_MAX_BUF_CREDITS - 1);

    /* Wait for start signal */
    while (1) {
      if (fp_state.cfg.sig != 0)
        break;

      sleep(1 << 10);
    }

    local_mac = fp_state.cfg.local_mac;
    t_prev = local_csr_read(local_csr_timestamp_low);

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

  /* TODO: Clear Cache periodically! */

  switch (ctx()) {
  case 0:
  case 1:
    preprocess_ac_thread();
    break;

  case 2:
  case 3:
  case 4:
    preprocess_tx_thread();
    break;

  case 5:
  case 6:
  case 7:
    preprocess_rx_thread();
    break;
  }

  return 0;
}
