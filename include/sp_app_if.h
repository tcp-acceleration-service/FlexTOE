/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef SP_APPIF_H_
#define SP_APPIF_H_

#include <stdint.h>

#include "common.h"

PACKED_STRUCT(sp_uxsock_request)
{
  uint32_t rxq_len;
  uint32_t txq_len;
};

PACKED_STRUCT(sp_uxsock_response)
{
  uint64_t app_out_off;
  uint64_t app_in_off;

  uint32_t app_out_len;
  uint32_t app_in_len;

  uint32_t status;
  uint16_t flexnic_db_id;

  uint64_t rxq_off;
  uint64_t txq_off;
};

/******************************************************************************/
/* App -> Kernel */

enum {
  SP_APPOUT_INVALID = 0,
  SP_APPOUT_CONN_OPEN,
  SP_APPOUT_CONN_CLOSE,
  SP_APPOUT_CONN_MOVE,
  SP_APPOUT_LISTEN_OPEN,
  SP_APPOUT_LISTEN_CLOSE,
  SP_APPOUT_ACCEPT_CONN,
};

/** Open a new connection */
PACKED_STRUCT(sp_appout_conn_open)
{
  uint64_t opaque;
  uint32_t remote_ip;
  uint32_t flags;
  uint16_t remote_port;
};

#define SP_APPOUT_CLOSE_RESET   (1 << 0)

/** Close connection */
PACKED_STRUCT(sp_appout_conn_close)
{
  uint64_t opaque;
  uint32_t remote_ip;
  uint32_t local_ip;
  uint16_t remote_port;
  uint16_t local_port;
  uint32_t flags;
};

/** Move connection to a new context */
PACKED_STRUCT(sp_appout_conn_move)
{
  uint64_t opaque;
  uint32_t remote_ip;
  uint32_t local_ip;
  uint16_t remote_port;
  uint16_t local_port;
  uint16_t db_id;
};

#define SP_APPOUT_LISTEN_REUSEPORT    (1 << 0)

/** Open listener */
PACKED_STRUCT(sp_appout_listen_open)
{
  uint64_t opaque;
  uint32_t backlog;
  uint16_t local_port;
  uint8_t  flags;
};

/** Close listener */
PACKED_STRUCT(sp_appout_listen_close)
{
  uint64_t opaque;
  uint16_t local_port;
};

/** Accept connection on listening socket */
PACKED_STRUCT(sp_appout_accept_conn)
{
  uint64_t listen_opaque;
  uint64_t conn_opaque;
  uint16_t local_port;
};

/** Common struct for events on sp -> app queue */
PACKED_STRUCT(sp_appout)
{
  PACKED_UNION()
  {
    struct sp_appout_conn_open    conn_open;
    struct sp_appout_conn_close   conn_close;
    struct sp_appout_conn_move    conn_move;

    struct sp_appout_listen_open  listen_open;
    struct sp_appout_listen_close listen_close;
    struct sp_appout_accept_conn  accept_conn;

    uint8_t raw[63];
  } data;
  uint8_t type;
};

/******************************************************************************/
/* Kernel -> App */

enum {
  SP_APPIN_INVALID = 0,
  SP_APPIN_STATUS_CONN_CLOSE,
  SP_APPIN_STATUS_CONN_MOVE,
  SP_APPIN_STATUS_LISTEN_OPEN,
  SP_APPIN_STATUS_LISTEN_CLOSE,
  SP_APPIN_CONN_OPENED,
  SP_APPIN_LISTEN_NEWCONN,
  SP_APPIN_ACCEPTED_CONN,
};

/** Generic operation status */
PACKED_STRUCT(sp_appin_status)
{
  uint64_t opaque;
  int32_t  status;
};

/** New connection opened */
PACKED_STRUCT(sp_appin_conn_opened)
{
  uint64_t opaque;
  uint64_t rx_off;
  uint64_t tx_off;
  uint32_t rx_len;
  uint32_t tx_len;
  int32_t  status;
  uint32_t seq_rx;
  uint32_t seq_tx;
  uint32_t flow_id;
  uint32_t local_ip;
  uint16_t local_port;
  uint16_t flow_group;
};

/** New connection on listener received */
PACKED_STRUCT(sp_appin_listen_newconn)
{
  uint64_t opaque;
  uint32_t remote_ip;
  uint16_t remote_port;
};

/** Accepted connection on listener */
PACKED_STRUCT(sp_appin_accept_conn)
{
  uint64_t opaque;
  uint64_t rx_off;
  uint64_t tx_off;
  uint32_t rx_len;
  uint32_t tx_len;
  int32_t  status;
  uint32_t seq_rx;
  uint32_t seq_tx;
  uint32_t flow_id;
  uint32_t local_ip;
  uint32_t remote_ip;
  uint16_t remote_port;
  uint16_t flow_group;
};

/** Common struct for events on sp -> app queue */
PACKED_STRUCT(sp_appin)
{
  PACKED_UNION()
  {
    struct sp_appin_status          status;
    struct sp_appin_conn_opened     conn_opened;
    struct sp_appin_listen_newconn  listen_newconn;
    struct sp_appin_accept_conn     accept_connection;
    uint8_t raw[63];
  } data;
  uint8_t type;
};

/* FIXME: Add STATIC asserts on struct sizes */

#endif /* SP_APPIF_H_ */
