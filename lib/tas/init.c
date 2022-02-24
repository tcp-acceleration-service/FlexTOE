/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>
#include <endian.h>

#include <rte/io.h>
#include "util/common.h"

#include "tas_ll_connect.h"
#include "sp_app_if.h"
#include "tas_ll.h"
#include "internal.h"
#include "fp_mem.h"

static inline int event_kappin_conn_opened(
    struct sp_appin_conn_opened *inev, struct flextcp_event *outev,
    unsigned avail);
static inline void event_kappin_listen_newconn(
    struct sp_appin_listen_newconn *inev, struct flextcp_event *outev);
static inline int event_kappin_accept_conn(
    struct sp_appin_accept_conn *inev, struct flextcp_event *outev,
    unsigned avail);
static inline void event_kappin_st_conn_move(
    struct sp_appin_status *inev, struct flextcp_event *outev);
static inline void event_kappin_st_listen_open(
    struct sp_appin_status *inev, struct flextcp_event *outev);
static inline void event_kappin_st_conn_closed(
    struct sp_appin_status *inev, struct flextcp_event *outev);

static inline int event_arx_connupdate(struct flextcp_context *ctx,
    volatile struct flextcp_pl_arx_t *inev,
    struct flextcp_event *outevs, int outn);

static int sp_poll(struct flextcp_context *ctx, int num,
    struct flextcp_event *events, int *used);
static int fastpath_poll(struct flextcp_context *ctx, int num,
    struct flextcp_event *events, int *used);
static void conns_bump(struct flextcp_context *ctx) __attribute__((noinline));
static void txq_probe(struct flextcp_context *ctx, unsigned n) __attribute__((noinline));

void *flexnic_mem = NULL;
struct flexnic_info_t *flexnic_info = NULL;
volatile struct flextcp_pl_mem *fp_internal = NULL;

int flextcp_init(void)
{
  if (flextcp_sp_connect() != 0) {
    fprintf(stderr, "flextcp_init: connecting to sp failed\n");
    return -1;
  }

  if (flexnic_driver_connect(&flexnic_info, &flexnic_mem) != 0) {
    fprintf(stderr, "flextcp_init: connecting to flexnic failed\n");
    return -1;
  }

  if (flexnic_driver_internal((void**) &fp_internal) != 0) {
    fprintf(stderr, "flexnic_driver_internal: connecting to flexnic failed\n");
    return -1;
  }

  return 0;
}

int flextcp_context_create(struct flextcp_context *ctx)
{
  static uint16_t ctx_id = 0;

  memset(ctx, 0, sizeof(struct flextcp_context));

  ctx->ctx_id = __sync_fetch_and_add(&ctx_id, 1);
  if (ctx->ctx_id >= FLEXNIC_PL_APPCTX_NUM) {
    fprintf(stderr, "flextcp_context_create: maximum number of contexts "
        "exeeded\n");
    return -1;
  }

  ctx->evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (ctx->evfd < 0) {
    perror("flextcp_context_create: eventfd for waiting fd failed");
    return -1;
  }

  return flextcp_sp_newctx(ctx);
}

static int sp_poll(struct flextcp_context *ctx, int num,
    struct flextcp_event *events, int *used)
{
  int i, j = 0;
  uint32_t pos;
  struct sp_appin *spout;
  uint8_t type;

  /* poll sp queues */
  pos = ctx->spout_head;
  for (i = 0; i < num;) {
    spout = (struct sp_appin *) ctx->spout_base + pos;
    j = 1;

    type = spout->type;
    MEM_BARRIER();

    if (type == SP_APPIN_INVALID) {
      break;
    } else if (type == SP_APPIN_CONN_OPENED) {
      j = event_kappin_conn_opened(&spout->data.conn_opened, &events[i],
          num - i);
    } else if (type == SP_APPIN_LISTEN_NEWCONN) {
      event_kappin_listen_newconn(&spout->data.listen_newconn, &events[i]);
    } else if (type == SP_APPIN_ACCEPTED_CONN) {
      j = event_kappin_accept_conn(&spout->data.accept_connection, &events[i],
          num - i);
    } else if (type == SP_APPIN_STATUS_LISTEN_OPEN) {
      event_kappin_st_listen_open(&spout->data.status, &events[i]);
    } else if (type == SP_APPIN_STATUS_CONN_MOVE) {
      event_kappin_st_conn_move(&spout->data.status, &events[i]);
    } else if (type == SP_APPIN_STATUS_CONN_CLOSE) {
      event_kappin_st_conn_closed(&spout->data.status, &events[i]);
    } else {
      fprintf(stderr, "flextcp_context_poll: unexpected spout type=%u pos=%u len=%u\n",
          type, pos, ctx->spout_len);
      abort();
    }
    ctx->flags |= CTX_FLAG_POLL_EVENTS;

    if (j == -1) {
      break;
    }

    i += j;

    MEM_BARRIER();
    spout->type = SP_APPIN_INVALID;

    pos = pos + 1;
    if (pos >= ctx->spout_len) {
      pos = 0;
    }
  }
  ctx->spout_head = pos;

  *used = i;
  return (j == -1 ? -1 : 0);
}

static int fastpath_poll(struct flextcp_context *ctx, int num,
    struct flextcp_event *events, int *used)
{
  int i, j, ran_out;
  struct flextcp_pl_arx_t *arx_q;
  volatile struct flextcp_pl_arx_t *arx;
  uint32_t head;
  uint32_t type;

  ran_out = 0;

  arx_q = (struct flextcp_pl_arx_t *) ctx->rxq_base;
  head = ctx->rxq_head;

  for (i = 0; i < num;) {
    j = 0;

    arx = &arx_q[head];
    type = be32toh(arx->type);

    /* handle based on queue entry type */
    if (type == FLEXTCP_PL_ARX_INVALID)
      break;

    /* prevent reordering type */
    rte_rmb();

    if (type == FLEXTCP_PL_ARX_CONNUPDATE) {
      j = event_arx_connupdate(ctx, arx, events + i, num - i);
    }
    else {
      fprintf(stderr, "flextcp_context_poll: spout arx=%p type=%u head=%x\n", arx, type, head);
    }
    ctx->flags |= CTX_FLAG_POLL_EVENTS;

    if (j == -1) {
      ran_out = 1;
      break;
    }
    i += j;

    arx->type = htobe32(FLEXTCP_PL_ARX_INVALID);

    /* next entry */
    head += 1;
    if (head >= ctx->rxq_len)
      head -= ctx->rxq_len;
  }

  /* prevent reordering */
  rte_wmb();

  if (head != ctx->rxq_head) {
    nn_writel(head, &fp_internal->appctx[ctx->db_id].rx.c_idx);
    ctx->rxq_head = head;
  }

  *used = i;
  if (ran_out)
    return -1;

  return 0;
}

int flextcp_context_poll(struct flextcp_context *ctx, int num,
    struct flextcp_event *events)
{
  int i, j;

  ctx->flags |= CTX_FLAG_POLL_CALLED;

  /* poll sp */
  if (sp_poll(ctx, num, events, &i) == -1) {
    /* not enough event space, abort */
    return i;
  }

  /* poll NIC queus */
  fastpath_poll(ctx, num - i, events + i, &j);

  txq_probe(ctx, num);
  conns_bump(ctx);

  return i + j;
}

int flextcp_context_tx_alloc(struct flextcp_context *ctx,
    struct flextcp_pl_atx_t **patx)
{
  struct flextcp_pl_atx_t *atx_q;
  uint32_t tail, updated_tail;

  /* if queue is full, abort */
  if (ctx->txq_avail == 0)
    return -1;

  tail = ctx->txq_tail;
  updated_tail = tail + 1;
  if (updated_tail >= ctx->txq_len)
    updated_tail -= ctx->txq_len;

  atx_q = (struct flextcp_pl_atx_t *) ctx->txq_base;
  *patx = &atx_q[tail];

  ctx->txq_tail = updated_tail;
  ctx->txq_avail -= 1;

  return 0;
}

void flextcp_context_tx_done(struct flextcp_context *ctx)
{
  uint16_t tail = (uint16_t) ctx->txq_tail;

  /* Prevent reordering */
  rte_wmb();

  nn_writel(tail, &fp_internal->appctx[ctx->db_id].tx.p_idx);
}

static inline int event_kappin_conn_opened(
    struct sp_appin_conn_opened *inev, struct flextcp_event *outev,
    unsigned avail)
{
  struct flextcp_connection *conn;
  int j = 1;

  conn = OPAQUE_PTR(inev->opaque);

  outev->event_type = FLEXTCP_EV_CONN_OPEN;
  outev->ev.conn_open.status = inev->status;
  outev->ev.conn_open.conn = conn;

  if (inev->status != 0) {
    conn->status = CONN_CLOSED;
    return 1;
  } else if (conn->rxb_used > 0 && conn->rx_closed && avail < 3) {
    /* if we've already received updates, we'll need to inject them */
    return -1;
  } else if ((conn->rxb_used > 0 || conn->rx_closed) && avail < 2) {
    /* if we've already received updates, we'll need to inject them */
    return -1;
  }

  conn->status = CONN_OPEN;
  conn->local_ip = inev->local_ip;
  conn->local_port = inev->local_port;
  conn->seq_rx = inev->seq_rx;
  conn->seq_tx = inev->seq_tx;
  conn->flow_id = inev->flow_id;
  conn->flow_group = inev->flow_group;

  conn->rxb_base = (uint8_t *) flexnic_mem + inev->rx_off;
  conn->rxb_len = inev->rx_len;

  conn->txb_base = (uint8_t *) flexnic_mem + inev->tx_off;
  conn->txb_len = inev->tx_len;

  /* inject bump if necessary */
  if (conn->rxb_used > 0) {
    conn->seq_rx += conn->rxb_used;

    outev[j].event_type = FLEXTCP_EV_CONN_RECEIVED;
    outev[j].ev.conn_received.conn = conn;
    outev[j].ev.conn_received.buf = conn->rxb_base;
    outev[j].ev.conn_received.len = conn->rxb_used;
    j++;
  }

  /* add end of stream notification if necessary */
  if (conn->rx_closed) {
    outev[j].event_type = FLEXTCP_EV_CONN_RXCLOSED;
    outev[j].ev.conn_rxclosed.conn = conn;
    j++;
  }

  return j;
}

static inline void event_kappin_listen_newconn(
    struct sp_appin_listen_newconn *inev, struct flextcp_event *outev)
{
  struct flextcp_listener *listener;

  listener = OPAQUE_PTR(inev->opaque);

  outev->event_type = FLEXTCP_EV_LISTEN_NEWCONN;
  outev->ev.listen_newconn.remote_ip = inev->remote_ip;
  outev->ev.listen_newconn.remote_port = inev->remote_port;
  outev->ev.listen_newconn.listener = listener;
}

static inline int event_kappin_accept_conn(
    struct sp_appin_accept_conn *inev, struct flextcp_event *outev,
    unsigned avail)
{
  struct flextcp_connection *conn;
  int j = 1;

  conn = OPAQUE_PTR(inev->opaque);

  outev->event_type = FLEXTCP_EV_LISTEN_ACCEPT;
  outev->ev.listen_accept.status = inev->status;
  outev->ev.listen_accept.conn = conn;

  if (inev->status != 0) {
    conn->status = CONN_CLOSED;
    return 1;
  } else if (conn->rxb_used > 0 && conn->rx_closed && avail < 3) {
    /* if we've already received updates, we'll need to inject them */
    return -1;
  } else if ((conn->rxb_used > 0 || conn->rx_closed) && avail < 2) {
    /* if we've already received updates, we'll need to inject them */
    return -1;
  }

  conn->status = CONN_OPEN;
  conn->local_ip = inev->local_ip;
  conn->remote_ip = inev->remote_ip;
  conn->remote_port = inev->remote_port;
  conn->seq_rx = inev->seq_rx;
  conn->seq_tx = inev->seq_tx;
  conn->flow_id = inev->flow_id;
  conn->flow_group = inev->flow_group;

  conn->rxb_base = (uint8_t *) flexnic_mem + inev->rx_off;
  conn->rxb_len = inev->rx_len;

  conn->txb_base = (uint8_t *) flexnic_mem + inev->tx_off;
  conn->txb_len = inev->tx_len;

  /* inject bump if necessary */
  if (conn->rxb_used > 0) {
    conn->seq_rx += conn->rxb_used;

    outev[j].event_type = FLEXTCP_EV_CONN_RECEIVED;
    outev[j].ev.conn_received.conn = conn;
    outev[j].ev.conn_received.buf = conn->rxb_base;
    outev[j].ev.conn_received.len = conn->rxb_used;
    j++;
  }

  /* add end of stream notification if necessary */
  if (conn->rx_closed) {
    outev[j].event_type = FLEXTCP_EV_CONN_RXCLOSED;
    outev[j].ev.conn_rxclosed.conn = conn;
    j++;
  }

  return j;
}

static inline void event_kappin_st_conn_move(
    struct sp_appin_status *inev, struct flextcp_event *outev)
{
  struct flextcp_connection *conn;

  conn = OPAQUE_PTR(inev->opaque);

  outev->event_type = FLEXTCP_EV_CONN_MOVED;
  outev->ev.conn_moved.status = inev->status;
  outev->ev.conn_moved.conn = conn;
}

static inline void event_kappin_st_listen_open(
    struct sp_appin_status *inev, struct flextcp_event *outev)
{
  struct flextcp_listener *listener;

  listener = OPAQUE_PTR(inev->opaque);

  outev->event_type = FLEXTCP_EV_LISTEN_OPEN;
  outev->ev.listen_open.status = inev->status;
  outev->ev.listen_open.listener = listener;
}

static inline void event_kappin_st_conn_closed(
    struct sp_appin_status *inev, struct flextcp_event *outev)
{
  struct flextcp_connection *conn;

  conn = OPAQUE_PTR(inev->opaque);

  outev->event_type = FLEXTCP_EV_CONN_CLOSED;
  outev->ev.conn_closed.status = inev->status;
  outev->ev.conn_closed.conn = conn;

  conn->status = CONN_CLOSED;
}

static inline int event_arx_connupdate(struct flextcp_context *ctx,
    volatile struct flextcp_pl_arx_t *inev,
    struct flextcp_event *outevs, int outn)
{
  struct flextcp_connection *conn;
  uint32_t rx_bump, rx_len, tx_bump, tx_sent, flags;
  int i = 0, evs_needed, tx_avail_ev, eos;
  uint64_t opaque;

  opaque = be64toh(inev->msg.connupdate.opaque);
  conn = OPAQUE_PTR(opaque);

  rx_bump = be32toh(inev->msg.connupdate.rx_bump);
  tx_bump = be32toh(inev->msg.connupdate.tx_bump);
  flags = be32toh(inev->msg.connupdate.flags);
  eos = ((flags & FLEXTCP_PL_ARX_FLRXDONE) == FLEXTCP_PL_ARX_FLRXDONE);

  if (conn->status == CONN_OPEN_REQUESTED ||
      conn->status == CONN_ACCEPT_REQUESTED) {
    /* due to a race we might see connection updates before we see the
     * connection confirmation from the sp */
    assert(tx_bump == 0);
    conn->rx_closed = !!eos;
    conn->rxb_head += rx_bump;
    conn->rxb_used += rx_bump;
    /* TODO: should probably handle eos here as well */
    return 0;
  } else if (conn->status == CONN_CLOSED ||
      conn->status == CONN_CLOSE_REQUESTED) {
    /* just drop bumps for closed connections */
    return 0;
  }

  assert(conn->status == CONN_OPEN);

  /* figure out how many events for rx */
  evs_needed = 0;
  if (rx_bump > 0) {
    evs_needed++;
    if (conn->rxb_head + rx_bump > conn->rxb_len) {
      evs_needed++;
    }
  }

  /* if tx buffer was depleted, we'll generate a tx avail event */
  tx_avail_ev = (tx_bump > 0 && flextcp_conn_txbuf_available(conn) == 0);
  if (tx_avail_ev) {
    evs_needed++;
  }

  tx_sent = conn->txb_sent - tx_bump;

  /* if tx close was acked, also add that event */
  if ((conn->flags & CONN_FLAG_TXEOS_ALLOC) == CONN_FLAG_TXEOS_ALLOC &&
      !tx_sent)
  {
    evs_needed++;
  }

  /* if receive stream closed need additional event */
  if (eos) {
    evs_needed++;
  }

  /* if we can't fit all events, try again later */
  if (evs_needed > outn) {
    return -1;
  }

  /* generate rx events */
  if (rx_bump > 0) {
    outevs[i].event_type = FLEXTCP_EV_CONN_RECEIVED;
    outevs[i].ev.conn_received.conn = conn;
    outevs[i].ev.conn_received.buf = conn->rxb_base + conn->rxb_head;
    // util_prefetch0(conn->rxb_base + conn->rxb_head);
    if (conn->rxb_head + rx_bump > conn->rxb_len) {
      /* wrap around in rx buffer */
      rx_len = conn->rxb_len - conn->rxb_head;
      outevs[i].ev.conn_received.len = rx_len;

      i++;
      outevs[i].event_type = FLEXTCP_EV_CONN_RECEIVED;
      outevs[i].ev.conn_received.conn = conn;
      outevs[i].ev.conn_received.buf = conn->rxb_base;
      outevs[i].ev.conn_received.len = rx_bump - rx_len;
    } else {
      outevs[i].ev.conn_received.len = rx_bump;
    }
    i++;

    /* update rx buffer */
    conn->seq_rx += rx_bump;
    conn->rxb_head += rx_bump;
    if (conn->rxb_head >= conn->rxb_len) {
      conn->rxb_head -= conn->rxb_len;
    }
    conn->rxb_used += rx_bump;
  }

  /* bump tx */
  if (tx_bump > 0) {
    conn->txb_sent -= tx_bump;

    if (tx_avail_ev) {
      outevs[i].event_type = FLEXTCP_EV_CONN_SENDBUF;
      outevs[i].ev.conn_sendbuf.conn = conn;
      i++;
    }

    /* if we were previously unable to push out TX EOS, do so now. */
    if ((conn->flags & CONN_FLAG_TXEOS) == CONN_FLAG_TXEOS &&
        !(conn->flags & CONN_FLAG_TXEOS_ALLOC))
    {
      if (flextcp_conn_pushtxeos(ctx, conn) != 0) {
        /* should never happen */
        fprintf(stderr, "event_arx_connupdate: flextcp_conn_pushtxeos "
            "failed\n");
        abort();
      }
    } else if ((conn->flags & CONN_FLAG_TXEOS_ALLOC) == CONN_FLAG_TXEOS_ALLOC) {
      /* There should be no data after we push out the EOS */
      assert(!(conn->flags & CONN_FLAG_TXEOS_ACK));

      /* if this was the last bump, mark TX EOS as acked */
      if (conn->txb_sent == 0) {
        conn->flags |= CONN_FLAG_TXEOS_ACK;

        outevs[i].event_type = FLEXTCP_EV_CONN_TXCLOSED;
        outevs[i].ev.conn_txclosed.conn = conn;
        i++;
      }
    }
  }

  /* add end of stream notification */
  if (eos) {
    outevs[i].event_type = FLEXTCP_EV_CONN_RXCLOSED;
    outevs[i].ev.conn_rxclosed.conn = conn;
    conn->rx_closed = 1;
    i++;
  }

  return i;
}

static void txq_probe(struct flextcp_context *ctx, unsigned n)
{
  struct flextcp_pl_atx_t *atx_q;
  volatile struct flextcp_pl_atx_t *atx;
  uint32_t pos, i, tail, avail, len, type;

  len = ctx->txq_len;
  avail = ctx->txq_avail;

  if (avail > len / 2)
    return;

  tail = ctx->txq_tail;

  pos = tail + avail + 1;
  if (pos >= len)
    pos -= len;

  i = 0;
  while (avail < len && i < 2 * n) {
    atx_q = (struct flextcp_pl_atx_t *) (ctx->txq_base);
    atx = &atx_q[pos];

    type = be32toh(atx->type);
    if (type != FLEXTCP_PL_ATX_INVALID)
      break;

    rte_rmb();

    avail += 1;
    pos += 1;
    if (pos >= len)
      pos -= len;
    i++;
  }

  ctx->txq_avail = avail;
}

static void conns_bump(struct flextcp_context *ctx)
{
  struct flextcp_connection *c;
  struct flextcp_pl_atx_t *atx;
  uint8_t flags;
  unsigned cnt = 0;

  while ((c = ctx->bump_pending_first) != NULL) {
    assert(c->status == CONN_OPEN);

    if (flextcp_context_tx_alloc(ctx, &atx) != 0) {
      break;
    }

    flags = 0;

    atx->msg.connupdate.rx_bump = htobe32(c->rxb_bump);
    atx->msg.connupdate.tx_bump = htobe32(c->txb_bump);
    atx->msg.connupdate.flow_id = htobe32(c->flow_id);
    atx->msg.connupdate.flow_grp = htobe32(c->flow_group);
    atx->msg.connupdate.bump_seq = htobe16(c->bump_seq++);
    atx->msg.connupdate.flags = flags;
    atx->type = htobe32(FLEXTCP_PL_ATX_CONNUPDATE);

    c->rxb_bump = c->txb_bump = 0;
    c->bump_pending = 0;

    if (c->bump_next == NULL) {
      ctx->bump_pending_last = NULL;
    }
    ctx->bump_pending_first = c->bump_next;

    cnt++;
  }

  if (cnt > 0) {
    flextcp_context_tx_done(ctx);
  }
}

int flextcp_context_waitfd(struct flextcp_context *ctx)
{
  return ctx->evfd;
}

int flextcp_context_canwait(struct flextcp_context *ctx)
{
  /* At a high level this code implements a state machine that ensures that at
   * least POLL_CYCLE time has elapsed between two unsuccessfull poll calls.
   * This is a bit messier because we don't want to move any of the timestamp
   * code into the poll call and make it more expensive for apps that don't
   * block. Instead we use the timing of calls to canwait along with flags that
   * the poll call sets when it's called and when it finds events.
   */

  /* if blocking is disabled, we can never wait */
  if (flexnic_info->poll_cycle_app == UINT64_MAX) {
    return -1;
  }

  /* if there were events found in the last poll, it's back to square one. */
  if ((ctx->flags & CTX_FLAG_POLL_EVENTS) != 0) {
    ctx->flags &= ~(CTX_FLAG_POLL_EVENTS | CTX_FLAG_WANTWAIT |
        CTX_FLAG_LASTWAIT);

    return -1;
  }

  /* from here on we know that there are no events */

  if ((ctx->flags & CTX_FLAG_WANTWAIT) != 0) {
    /* in want wait state: just wait for grace period to be over */
    if ((util_rdtsc() - ctx->last_inev_ts) > flexnic_info->poll_cycle_app) {
      /* past grace period, move on to lastwait. clear polled flag, to make sure
       * it gets polled again before we clear lastwait. */
      ctx->flags &= ~(CTX_FLAG_POLL_CALLED | CTX_FLAG_WANTWAIT);
      ctx->flags |= CTX_FLAG_LASTWAIT;
    }
  } else if ((ctx->flags & CTX_FLAG_LASTWAIT) != 0) {
    /* in last wait state */
    if ((ctx->flags & CTX_FLAG_POLL_CALLED) != 0) {
      /* if we have polled once more after the grace period, we're good to go to
       * sleep */
      return 0;
    }
  } else if ((ctx->flags & CTX_FLAG_POLL_CALLED) != 0) {
    /* not currently getting ready to wait, so start */
    ctx->last_inev_ts = util_rdtsc();
    ctx->flags |= CTX_FLAG_WANTWAIT;
  }

  return -1;
}

void flextcp_context_waitclear(struct flextcp_context *ctx)
{
  ssize_t ret;
  uint64_t val;

  ret = read(ctx->evfd, &val, sizeof(uint64_t));
  if ((ret >= 0 && ret != sizeof(uint64_t)) ||
      (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK))
  {
    perror("flextcp_context_waitclear: read failed");
    abort();
  }

  ctx->flags &= ~(CTX_FLAG_WANTWAIT | CTX_FLAG_LASTWAIT | CTX_FLAG_POLL_CALLED);
}

int flextcp_context_wait(struct flextcp_context *ctx, int timeout_ms)
{
  struct pollfd pfd;
  int ret;

  if (flextcp_context_canwait(ctx) != 0) {
    return -1;
  }

  pfd.fd = ctx->evfd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  ret = poll(&pfd, 1, timeout_ms);
  if (ret < 0) {
    perror("flextcp_context_wait: poll returned error");
    return -1;
  }

  flextcp_context_waitclear(ctx);
  return 0;
}
