/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FP_SP_IF_H_
#define FP_SP_IF_H_

#include <stdint.h>
#include "common.h"

#if FIRMWARE
  #include <net/eth.h>
#else
  #include <netinet/ether.h>
#endif

/** Kernel RX queue */
enum {
  FLEXTCP_PL_SPRX_INVALID = 0,
  FLEXTCP_PL_SPRX_PACKET,
};

/** Kernel RX queue entry */
PACKED_STRUCT(flextcp_pl_sprx_t)
{
  uint32_t type;
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint32_t len;
      uint32_t flow_group;
      uint64_t flow_hash;
    } packet;
    uint32_t raw[15];
  } msg;
};

/** Kernel TX queue */
enum {
  FLEXTCP_PL_SPTX_INVALID = 0,
  FLEXTCP_PL_SPTX_PACKET,
  FLEXTCP_PL_SPTX_PACKET_NOTS,
  FLEXTCP_PL_SPTX_CONN_RETX,
  FLEXTCP_PL_SPTX_CONN_SETRATE,
  FLEXTCP_PL_SPTX_FLOWHT_ADD,
  FLEXTCP_PL_SPTX_FLOWHT_DEL,
  FLEXTCP_PL_SPTX_CONN_CLOSE,
  FLEXTCP_PL_SPTX_DEBUG_RESET,
};

/** Kernel TX queue entry */
PACKED_STRUCT(flextcp_pl_sptx_t)
{
  uint32_t type;
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint32_t len;
      uint32_t ts_offset;
    } packet;
    PACKED_STRUCT()
    {
      uint32_t flow_id;
      uint32_t flow_grp;
    } connretran;
    PACKED_STRUCT()
    {
      uint32_t flow_id;
      uint32_t tx_rate;
    } connsetrate;
    PACKED_STRUCT()
    {
      uint32_t remote_ip;
      uint32_t local_ip;
      uint16_t remote_port;
      uint16_t local_port;
      uint32_t flow_id;
    } flowht;
    PACKED_STRUCT()
    {
      uint32_t flow_id;
    } connclose;
    uint32_t raw[15];
  } msg;
};

/** Kernel context registers */
PACKED_STRUCT(flextcp_pl_spctx_t)
{
  /* read-only fields */
  uint64_t rx_base;
  uint64_t rx_desc_base;
  uint64_t tx_base;
  uint64_t tx_desc_base;
  uint32_t rx_len;
  uint32_t tx_len;

  /* read-write fields */
  uint64_t last_ts;
  uint32_t rx_head;
  uint32_t rx_tail;
  uint32_t tx_head;
  uint32_t tx_tail;
};

/** FP configuration */
PACKED_STRUCT(flextcp_pl_config)
{
  volatile uint64_t sig;

#if FIRMWARE
  struct eth_addr local_mac;
  uint16_t __pad1;
#else
  uint32_t local_mac_1;
  uint16_t __pad1;
  uint16_t local_mac_2;
#endif

  uint64_t phyaddr;
  uint64_t memsize;

  uint64_t poll_cycle_app;

  uint8_t __pad2[16];
};

/* FIXME: Add STATIC asserts on struct sizes */

#endif /* FP_SP_IF_H_ */
