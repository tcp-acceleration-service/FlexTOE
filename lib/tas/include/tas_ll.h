/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef TAS_LL_H_
#define TAS_LL_H_

/**
 * @file tas_ll.h
 * @brief Public low-level application interface for TAS.
 *
 * @addtogroup libtas-ll
 * @brief Application library for TAS lowlevel interface.
 * @{ */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * A flextcp context is per-thread state for the stack. (opaque)
 * This includes:
 *   - admin queue pair to SP
 *   - notification queue pair to flexnic
 */
struct flextcp_context {
  /* incoming queue from the SP */
  void *spin_base;
  uint32_t spin_len;
  uint32_t spin_head;

  /* outgoing queue to the SP */
  void *spout_base;
  uint32_t spout_len;
  uint32_t spout_head;

  /* queue from NIC */
  uint32_t rxq_len;
  uint32_t txq_len;

  void *txq_base;
  void *rxq_base;
  uint32_t rxq_head;
  uint32_t txq_tail;
  uint32_t txq_avail;
  uint32_t _pad;
  uint64_t last_ts;

  /* list of connections with pending updates for NIC */
  struct flextcp_connection *bump_pending_first;
  struct flextcp_connection *bump_pending_last;

  /* other */
  uint32_t flags;
  uint16_t db_id;
  uint16_t ctx_id;

  /* waiting */
  uint64_t last_inev_ts;
  int evfd;
};

/** TCP listening "socket". (opaque) */
struct flextcp_listener {
  struct flextcp_connection *conns;

  uint16_t local_port;
  uint8_t status;
};

/** TCP connection. (opaque) */
struct flextcp_connection {
  /* rx buffer */
  uint8_t *rxb_base;
  uint32_t rxb_len;
  /** pointer to next new byte to be received */
  uint32_t rxb_head;
  /** number of received but not yet freed bytes (behind head). */
  uint32_t rxb_used;
  /** pending rx bump to fast path */
  uint32_t rxb_bump;

  /* tx buffer */
  uint8_t *txb_base;
  uint32_t txb_len;
  /** pointer to next byte to be sent */
  uint32_t txb_head;
  /** number of sent but not yet acked bytes (behind head) */
  uint32_t txb_sent;
  /** number of allocated but not yet sent bytes (after head) */
  uint32_t txb_allocated;
  /** pending tx bump to fast path */
  uint32_t txb_bump;

  uint32_t local_ip;
  uint32_t remote_ip;
  uint16_t local_port;
  uint16_t remote_port;

  uint32_t seq_rx;
  uint32_t seq_tx;

  uint32_t flow_id;
  uint32_t bump_seq;

  struct flextcp_connection *bump_next;
  struct flextcp_connection *bump_prev;
  uint32_t flow_group;

  uint8_t bump_pending;
  uint8_t status;
  uint8_t flags;
  /* TODO: kill rx closed and merge into flags */
  uint8_t rx_closed;
};

/** Types of events that can occur in flextcp contexts */
enum flextcp_event_type {
  /** flextcp_listen_open() result. */
  FLEXTCP_EV_LISTEN_OPEN,
  /** New connection on listening socket arrived. */
  FLEXTCP_EV_LISTEN_NEWCONN,
  /** Accept operation completed */
  FLEXTCP_EV_LISTEN_ACCEPT,

  /** flextcp_connection_open() result */
  FLEXTCP_EV_CONN_OPEN,
  /** Connection was closed */
  FLEXTCP_EV_CONN_CLOSED,
  /** Data arrived on connection */
  FLEXTCP_EV_CONN_RECEIVED,
  /** More send buffer available */
  FLEXTCP_EV_CONN_SENDBUF,
  /** Receive stream closed */
  FLEXTCP_EV_CONN_RXCLOSED,
  /** transmit stream closed */
  FLEXTCP_EV_CONN_TXCLOSED,
  /** Connection moved to new context */
  FLEXTCP_EV_CONN_MOVED,
};

/** Events that can occur on flextcp contexts. */
struct flextcp_event {
  uint8_t event_type;
  union {
    /** For #FLEXTCP_EV_LISTEN_OPEN */
    struct {
      int16_t status;
      struct flextcp_listener *listener;
    } listen_open;
    /** For #FLEXTCP_EV_LISTEN_NEWCONN */
    struct {
      uint16_t remote_port;
      uint32_t remote_ip;
      struct flextcp_listener *listener;
    } listen_newconn;
    /** For #FLEXTCP_EV_LISTEN_ACCEPT */
    struct {
      int16_t status;
      struct flextcp_connection *conn;
    } listen_accept;

    /** For #FLEXTCP_EV_CONN_OPEN */
    struct {
      int16_t status;
      struct flextcp_connection *conn;
    } conn_open;
    /** For #FLEXTCP_EV_CONN_RECEIVED */
    struct {
      void *buf;
      size_t len;
      struct flextcp_connection *conn;
    } conn_received;
    /** For #FLEXTCP_EV_CONN_SENDBUF */
    struct {
      struct flextcp_connection *conn;
    } conn_sendbuf;
    /** For #FLEXTCP_EV_CONN_RXCLOSED */
    struct {
      struct flextcp_connection *conn;
    } conn_rxclosed;
    /** For #FLEXTCP_EV_CONN_TXCLOSED */
    struct {
      struct flextcp_connection *conn;
    } conn_txclosed;
    /** For #FLEXTCP_EV_CONN_MOVED */
    struct {
      int16_t status;
      struct flextcp_connection *conn;
    } conn_moved;
    /** For #FLEXTCP_EV_CONN_CLOSED */
    struct {
      int16_t status;
      struct flextcp_connection *conn;
    } conn_closed;
  } ev;
};

#define FLEXTCP_LISTEN_REUSEPORT 0x1

/**
 * Initializes global flextcp state, must only be called once.
 * @return 0 on success, < 0 on failure
 */
int flextcp_init(void);

/**
 * Create a flextcp context.
 */
int flextcp_context_create(struct flextcp_context *ctx);

/**
 * Poll events from a flextcp socket.
 */
int flextcp_context_poll(struct flextcp_context *ctx, int num,
    struct flextcp_event *events);

/**
 * Get file descriptor for waiting on events in this context. Note that blocking
 * is only allowed after `flextcp_context_canwait` indicates this. After waking
 * up, canwait has to be called again before blocking again.
 *
 * @return file descriptor for waiting on. Remains constant during lifetime of
 * context. <0 on error.
 */
int flextcp_context_waitfd(struct flextcp_context *ctx);

/**
 * Indicates whether the caller is allowed to block on the wait file descriptor
 * on this context after this call returns.
 *
 * @return 0 if wait is allowed, -1 otherwise.
 */
int flextcp_context_canwait(struct flextcp_context *ctx);

/**
 * Clear events off of waitfd after waking up. Does not process any events any
 * events, just reads the count off the internal eventfd.
 *
 * @return 0 on success, -1 otherwise
 */
void flextcp_context_waitclear(struct flextcp_context *ctx);

/**
 * Waits for events for up to timeout_ms milliseconds. Is not guaranteed to
 * block, but only does so when flextcp_context_canwait returns 0.
 *
 * @return 0 if we waited, -1 otherwise.
 */
int flextcp_context_wait(struct flextcp_context *ctx, int timeout_ms);

/*****************************************************************************/
/* Regular TCP connection management */

/** Open a listening socket (asynchronous). */
int flextcp_listen_open(struct flextcp_context *ctx,
    struct flextcp_listener *lst, uint16_t port, uint32_t backlog,
    uint32_t flags);

/** Accept connections on a listening socket (asynchronous). This can be called
 * more than once to register multiple connection handles. */
int flextcp_listen_accept(struct flextcp_context *ctx,
    struct flextcp_listener *lst, struct flextcp_connection *conn);

/** Open a connection (asynchronous). */
int flextcp_connection_open(struct flextcp_context *ctx,
    struct flextcp_connection *conn, uint32_t dst_ip, uint16_t dst_port);

/** Close a connection (asynchronous). */
int flextcp_connection_close(struct flextcp_context *ctx,
    struct flextcp_connection *conn);

/** Receive processing for `len' bytes done. */
int flextcp_connection_rx_done(struct flextcp_context *ctx, struct flextcp_connection *conn, size_t len);

/** Allocate transmit buffer for `len' bytes, returns number of bytes
 * allocated.
 *
 * NOTE: short allocs can occur if buffer wraps around
 */
ssize_t flextcp_connection_tx_alloc(struct flextcp_connection *conn, size_t len,
    void **buf);

/** Allocate transmit buffer for `len' bytes, returns number of bytes
 * allocated. May be split across two buffers, in case of wrap around. */
ssize_t flextcp_connection_tx_alloc2(struct flextcp_connection *conn, size_t len,
    void **buf_1, size_t *len_1, void **buf_2);

/** Send previously allocated bytes in transmit buffer */
int flextcp_connection_tx_send(struct flextcp_context *ctx,
        struct flextcp_connection *conn, size_t len);

/** Send previously allocated bytes in transmit buffer */
int flextcp_connection_tx_close(struct flextcp_context *ctx,
        struct flextcp_connection *conn);

/** Make sure there is room in the context send queue (not send buffer)
 *
 * Returns 0 if transmit is possible, -1 otherwise.
 */
int flextcp_connection_tx_possible(struct flextcp_context *ctx,
    struct flextcp_connection *conn);

/** Move connection to specfied context */
int flextcp_connection_move(struct flextcp_context *ctx,
        struct flextcp_connection *conn);

#endif /* TAS_LL_H_ */
