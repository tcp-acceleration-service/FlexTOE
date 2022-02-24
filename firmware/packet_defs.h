/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef PACKET_DEFS_H_
#define PACKET_DEFS_H_

#include <stdint.h>

#include <pkt/pkt.h>
#include <net/eth.h>
#include <net/ip.h>
#include <net/tcp.h>

/**
 * TCP Options definitions
 *
 * NOTE: Only EOL/NOOP/TS supported in fastpath
 *
 * Format:
 *  Kind:     1 byte
 *  Length:   1 byte [Not present for EOL/NOOP]
 *  Data:     Variable
 */
#define NET_TCP_OPT_KIND_EOL        0   /*> End of List indicator */
#define NET_TCP_OPT_KIND_NOOP       1   /*> No-Option: used for padding */
#define NET_TCP_OPT_KIND_TIMESTAMP  6   /*> Timestamp */

#define NET_TCP_OPT_LEN_NOOP          1   /*> Length of No-Op */
#define NET_TCP_OPT_LEN_TIMESTAMP     10  /*> Length of Timestamp option including kind + length */
#define NET_TCP_OPT_LEN_PADTIMESTAMP    (2 * NET_TCP_OPT_LEN_NOOP + NET_TCP_OPT_LEN_TIMESTAMP)
#define NET_TCP_OPT_LEN32_PADTIMESTAMP  (NET_TCP_OPT_LEN_PADTIMESTAMP/4)

/* Type definition for Timestamp option */
__packed struct tcp_timestamp_opt {
  uint8_t   opt_kind;
  uint8_t   opt_length;
  uint32_t  ts_val;
  uint32_t  ts_ecr;
};

/* TS option is often padded to ensure alignment */
struct tcp_timestamp_padded_opt {
  union {
    __packed struct {
      uint8_t __noop1;      /*> No-option for padding */
      uint8_t __noop2;      /*> No-option for padding */
      uint8_t   opt_kind;   /*> Option Kind */
      uint8_t   opt_length; /*> Option Header Length */
      uint32_t  ts_val;     /*> Timestamp */
      uint32_t  ts_ecr;     /*> Timestamp Echo Reply */
    };
    uint32_t __raw[NET_TCP_OPT_LEN32_PADTIMESTAMP];
  };
};

/**
 * Packet structure (RAW: before preprocessing!)
 *
 * Packet Metadata and MAC Prepend with Catamaran Pico Engine load (see Catamaran IDD):
 * Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Word  +-----------+-------------------+---+---------------------------+
 *    0  | CTM Number| Packet Number     |BLS|     Packet Length         |
 *       +-+---+-----+-------------------+---+---------------------------+
 *    1  |S|Rsv|                   MU Pointer                            |
 *       +-+---+-------------------------+---+-----+-----+-+-----+-+-----+
 *    2  |      Sequence Number          |NFP|  R  | Seq |P|MType|V| BP  |
 *       +-------------------------+-+---+---+---+-+-----+-+-----+-+-----+
 *    3  |        Reserved         |E|TLD|  OL4  |  OL3  |   OL2   |  R  |
 *       +---------------+---------+-+---+---+---+-------+---------+-----+
 *    4  |     Port      |    HP-Off1    |    HP-Off0    |F|   Misc      |
 *       +-+-+-+-+-+-+-+-+---------------+---------------+---------------+
 *    5  |P|I|P|S|Tag|I|T|     O-Off     |       LIF+Mode / Match        |
 *       |E|E|W|p|Cnt|T|S|               |                               |
 *       +-+-+-+-+-+-+-+-+---------------+-------------------------------+
 *    6  |                           Timestamp                           |
 *       +-----+-+-+-+-+-+-+-+---+---+---+-------------------------------+
 *    7  |P_STS|M|E|A|F|D|R|H|L3 |MPL|VLN|           Checksum            |
 *       +-----+-+-+-+-+-+-+-+---+---+---+-------------------------------+
 *      S -> 1 if packet is split between CTM and MU data
 *      BLS -> Buffer List
 *
 * P_STS - Parse Status
 *         (0 = no data,
 *          1 = ESP,
 *          2 = TCP CSUM OK,
 *          3 = TCP CSUM FAIL,
 *          4 = UDP CSUM OK,
 *          5 = UDP CSUM FAIL,
 *          6 = AH,
 *          7 = FRAG)
 *
 * L3    - MAC L3 Information (0 = unkown, 1 = IPv6, 2 = IPv4 CSUM FAIL, 3 = IPv4 OK)
 * MPL   - MPLS Tag Depth (3 = 3 or more)
 * VLN   - VLAN Tag Depth (3 = 3 or more)
 */

struct pkt_raw_t {
  struct nbi_meta_catamaran meta; /*> NBI characterization metadata */
  uint32_t mac_timestamp;         /*> Ingress timestamp added by MAC */
  uint32_t mac_prepend;           /*> Ingress MAC prepend data */
};

/*
 * This routine takes about 7 cycles to execute in the worst case for small
 * packets.  As it turns out it takes 7-9 cycles plus a serial I/O to read in
 * the packet status and get the CTM split from the packet engine.  So it's a
 * win just to compute the CTM size manually.
 */
__intrinsic static uint32_t
compute_ctm_size(__xread struct nbi_meta_pkt_info* pkt_info)
{
#if SPLIT_LENGTH == 0
  /* if SPLIT_LEN == 0, assume initialized to 0 already */
  return 0;
#endif

#if SPLIT_LENGTH > 0
  uint32_t len = (pkt_info->__raw[0] + (PKT_NBI_OFFSET - 1));
  uint32_t x;

  /*
    * Computing: len + offset - 1
    *   00 0000 xxxx xxxx .. 11 1111 xxxx xxxx
    */

#if SPLIT_LENGTH == 1
  /*
    * 00 0000 xxxx xxxx -> 00
    * 00 0001 xxxx xxxx .. 11 1111 xxxx xxxx -> 01
    */

  len = ((len >> 8) & 0x3f) != 0;

#elif SPLIT_LENGTH == 2

  /*
    * 00 0000 xxxx xxxx -> 00
    * 00 0001 xxxx xxxx -> 01
    * 00 0010 xxxx xxxx .. 11 1111 xxxx xxxx -> 10
    */

  len = ((len >> 9) & 0x1f) ? 2 : (len >> 8) & 1;

#else /* SPLIT_LENGTH == 3 */

  /*
    * 00 0000 xxxx xxxx -> 00
    * 00 0001 xxxx xxxx -> 01
    * 00 0010 xxxx xxxx .. 00 0011 xxxx xxxx -> 10
    * 00 0100 xxxx xxxx .. 11 1111 xxxx xxxx -> 11
    * N.B. that for sizes 1025 .. 16384:
    *   00 1000 xxxx xxxx .. 11 1111 xxxx xxxx
    * the rxd->pktio_meta.split bit will be set.
    */

  if ((len >> 10) & 0x3F) {
      len = 3;
  } else {
      __critical_path();
      len = (len >> 8) & 3;
      x = (len >> 1) & 1;
      len = x ? 2 : (len & 1);
  }

#endif /* SPLIT_LENGTH == 3 */

  return len;

#endif /* SPLIT_LENGTH > 0 */
}

/**
 * Packet header definition
 *
 * NOTE: 2-byte padding to ensure aligned access to TCP/IP headers
 */
#define NET_HDR_LEN   (NET_ETH_LEN + NET_IP4_LEN + NET_TCP_LEN + NET_TCP_OPT_LEN_PADTIMESTAMP)
#define NET_HDR_LEN32 ((NET_HDR_LEN/4) + 1)
struct pkt_hdr_t {
  union {
    __packed struct {
      uint16_t __pad;    /*> Padding to align TCP/IP hdrs */

      /* Protocol headers */
      struct eth_hdr  eth;
      struct ip4_hdr  ip;
      struct tcp_hdr  tcp;

      /* TCP options */
      struct tcp_timestamp_padded_opt tcpopts;  /*> NOTE: Hope to receive padded TS opt */
    };
    uint32_t __raw[NET_HDR_LEN32];
  };
};

/**
 * Packet summary: Condensed protocol headers after processing
 */
struct pkt_summary_t {
  union {
    __packed struct {
      uint32_t seq;
      uint32_t ack;

      uint32_t ecn:1;
      uint32_t rsvd:7;
      uint32_t flags:8;
      uint32_t win:16;

      uint32_t ts_val;
      uint32_t ts_ecr;
    };

    uint32_t __raw[5];
  };
};

#define TCPH_HDRLEN_FLAGS_WIN_SET(_HDR, _HLEN, _FLAGS, _WIN)  \
          (_HDR)->__raw[3] = (_HLEN) | ((_FLAGS) << 8) | ((_WIN) << 16)
#define TCPH_CHKSUM_URP_SET(_HDR, _VAL)  \
          (_HDR)->__raw[4] = _VAL
#define TCPH_TSOPT_INIT(_HDR)            \
          (_HDR)->__raw[5] =  (NET_TCP_OPT_KIND_NOOP | (NET_TCP_OPT_KIND_NOOP << 8) | (NET_TCP_OPT_KIND_TIMESTAMP << 16) | (NET_TCP_OPT_LEN_TIMESTAMP << 24))
/**
 * TCP header with padded TS option
 */
#define NET_TCPPADTS_LEN        (NET_TCP_LEN + NET_TCP_OPT_LEN_PADTIMESTAMP)
#define NET_TCPPADTS_LEN32      (NET_TCPPADTS_LEN/4)
struct hdr_tcp_t {
  union {
    __packed struct {
      struct tcp_hdr tcp;
      struct tcp_timestamp_padded_opt tcpopts;
    };

    uint32_t __raw[NET_TCPPADTS_LEN32];
  };
};
#endif /* PACKET_DEFS_H_ */
