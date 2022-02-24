/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLEXTOE_FLOW_STATE_H_
#define FLEXTOE_FLOW_STATE_H_

#include <stdint.h>
#include "common.h"

#if FIRMWARE
  #include <net/eth.h>
#else
  #include <netinet/ether.h>
#endif

/** Connection identifiers */
PACKED_ALIGN_STRUCT(flowst_conn_t, 32)
{
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint32_t              flow_grp;
#if FIRMWARE
      struct eth_addr       remote_mac;
      uint16_t              flags;
#else
      uint32_t              remote_mac_1;
      uint16_t              flags;
      uint16_t              remote_mac_2;
#endif
      uint32_t              local_ip;
      uint32_t              remote_ip;
#if FIRMWARE
      uint16_t              local_port;
      uint16_t              remote_port;
#else
      uint16_t              remote_port;
      uint16_t              local_port;
#endif
      uint32_t              rsvd[2];
    };

    uint32_t    __raw[8];
  };
};

/** Memory buffer addresses */
PACKED_ALIGN_STRUCT(flowst_mem_t, 32)
{
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
#if FIRMWARE
      uint16_t db_id;         /*> Application CTX doorbell ID */
      uint16_t opaque_hi;     /*> Upper 16 bits of opaque flow identifier from application */
#else
      uint16_t opaque_hi;     /*> Upper 16 bits of opaque flow identifier from application */
      uint16_t db_id;         /*> Application CTX doorbell ID */
#endif
      uint32_t opaque_lo;     /*> Lower word of Opaque flow identifier from application */
      uint64_t rx_base;       /*> Base address of connection RX buffer */
      uint64_t tx_base;       /*> Base address of connection TX buffer */
      uint32_t rx_len;        /*> Length of connection RX buffer */
      uint32_t tx_len;        /*> Length of connection TX buffer */
    };

    uint32_t __raw[8];
  };
};

/** TCP state */
PACKED_ALIGN_STRUCT(flowst_tcp_t, 64)
{
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint32_t tx_len;                   /*> Length of transmission buffer */
      uint32_t tx_avail;                 /*> Bytes in TX buffer ready for transmission */
      uint32_t tx_remote_avail;          /*> Available space in remote RX buffer */
      uint32_t tx_sent;                  /*> Unacknowledged bytes in TX buffer */
      uint32_t tx_next_seq;              /*> Sequence number of next byte to be sent */
      uint32_t tx_next_pos;              /*> Position of next byte in TX buffer to be sent */
      uint32_t tx_next_ts;               /*> Timestamp to echo in next packet */
#if FIRMWARE
      uint16_t flags;                    /*> RX/TX Flags */
      uint16_t dupack_cnt;               /*> Duplicate ACK count */
#else
      uint16_t dupack_cnt;
      uint16_t flags;
#endif
      uint32_t rx_len;                   /*> Length of receive buffer */
      uint32_t rx_avail;                 /*> Available RX buffer space */
      uint32_t rx_next_seq;              /*> Next sequence number expected */
      uint32_t rx_next_pos;              /*> Offset of next byte in RX buffer */
      uint32_t rx_ooo_len;               /*> Length of Out-of-Order bytes */
      uint32_t rx_ooo_start;             /*> Start position of Out-of-Order bytes */

      uint32_t rsvd0;
      uint32_t rsvd1;
    };

    uint32_t __raw[16];
  };
};

/** Congestion Control state */
PACKED_ALIGN_STRUCT(flowst_cc_t, 32)
{
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint32_t tx_avail;              /*> Bytes available for transmission */
      uint32_t tx_rate;               /*> Transmission rate */
      uint32_t rtt_est;               /*> Round Trip Time estimate */
      uint32_t txp;                   /*> tx_sent != 0 */
      uint32_t cnt_tx_drops;          /*> Counter drops */
      uint32_t cnt_rx_acks;           /*> Counter ACKs */
      uint32_t cnt_rx_ack_bytes;      /*> Counter acknowledged bytes */
      uint32_t cnt_rx_ecn_bytes;      /*> Counter ECN marked bytes */
    };

    uint32_t __raw[8];
  };
};

/* Flags */
#define FLEXNIC_PL_FLOWST_ECN           (1 << 0)    /*> ECN-enabled flow */
#define FLEXNIC_PL_FLOWST_TXFIN         (1 << 1)    /*> TX FIN requested */
#define FLEXNIC_PL_FLOWST_SLOWPATH      (1 << 2)    /*> Redirect flow to slowpath permanently! */
#define FLEXNIC_PL_FLOWST_RXFIN         (1 << 3)    /*> RX FIN reached */

/* FIXME: Add STATIC asserts on struct sizes */

#endif /* FLEXTOE_FLOW_STATE_H_ */
