/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

/**
 * @brief Handling application context sp queues.
 * @file appif_ctx.c
 * @addtogroup tas-sp-appif
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util/common.h"

#include "flextoe.h"
#include "internal.h"
#include "appif.h"

static int spin_conn_open(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout);
static int spin_conn_move(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout);
static int spin_conn_close(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout);
static int spin_listen_open(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout);
static int spin_accept_conn(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout);

static void appif_ctx_kick(struct app_context *ctx)
{
  assert(ctx->evfd != 0);

  /* FIXME: Skip kick if time_delta < poll_cycle */
  uint64_t val = 1;
  if (write(ctx->evfd, &val, sizeof(uint64_t)) != sizeof(uint64_t)) {
    perror("appif_ctx_kick: write failed");
    abort();
  }
  ctx->last_ts = util_rdtsc();
}

void appif_conn_opened(struct connection *c, int status)
{
  struct app_context *ctx = c->ctx;
  volatile struct sp_appin *spout = ctx->spout_base;
  uint32_t spout_pos = ctx->spout_pos;

  spout += spout_pos;

  /* make sure we have room for a response */
  if (spout->type != SP_APPIN_INVALID) {
    fprintf(stderr, "appif_conn_opened: No space in spout queue (TODO)\n");
    return;
  }

  spout->data.conn_opened.opaque = c->opaque;
  spout->data.conn_opened.status = status;
  if (status == 0) {
    spout->data.conn_opened.rx_off = c->rx_buf - (uint8_t *) flextoe_dma_mem;
    spout->data.conn_opened.tx_off = c->tx_buf - (uint8_t *) flextoe_dma_mem;
    spout->data.conn_opened.rx_len = c->rx_len;
    spout->data.conn_opened.tx_len = c->tx_len;

    spout->data.conn_opened.seq_rx = c->remote_seq;
    spout->data.conn_opened.seq_tx = c->local_seq;
    spout->data.conn_opened.local_ip = config.ip;
    spout->data.conn_opened.local_port = c->local_port;
    spout->data.conn_opened.flow_id = c->flow_id;
    spout->data.conn_opened.flow_group = c->flow_group;
  } else {
    tcp_destroy(c);
  }

  MEM_BARRIER();
  spout->type = SP_APPIN_CONN_OPENED;
  appif_ctx_kick(ctx);

  spout_pos++;
  if (spout_pos >= ctx->spout_len) {
    spout_pos = 0;
  }
  ctx->spout_pos = spout_pos;
}

void appif_conn_closed(struct connection *c, int status)
{
  struct app_context *ctx = c->ctx;
  struct application *app = ctx->app;
  struct connection *c_i;
  volatile struct sp_appin *spout = ctx->spout_base;
  uint32_t spout_pos = ctx->spout_pos;

  spout += spout_pos;

  /* make sure we have room for a response */
  if (spout->type != SP_APPIN_INVALID) {
    fprintf(stderr, "appif_conn_closed: No space in spout queue (TODO)\n");
    return;
  }

  spout->data.status.opaque = c->opaque;
  spout->data.status.status = status;

  MEM_BARRIER();
  spout->type = SP_APPIN_STATUS_CONN_CLOSE;
  appif_ctx_kick(ctx);

  spout_pos++;
  if (spout_pos >= ctx->spout_len) {
    spout_pos = 0;
  }
  ctx->spout_pos = spout_pos;

  /* remove from app connection list */
  if (app->conns == c) {
    app->conns = c->app_next;
  } else {
    for (c_i = app->conns; c_i != NULL && c_i->app_next != c;
        c_i = c_i->app_next);
    if (c_i == NULL) {
      fprintf(stderr, "appif_conn_closed: connection not found\n");
      abort();
    }
    c_i->app_next = c->app_next;
  }
}

void appif_listen_newconn(struct listener *l, uint32_t remote_ip,
    uint16_t remote_port)
{
  struct app_context *ctx = l->ctx;
  volatile struct sp_appin *spout = ctx->spout_base;
  uint32_t spout_pos = ctx->spout_pos;

  spout += spout_pos;

  /* make sure we have room for a response */
  if (spout->type != SP_APPIN_INVALID) {
    fprintf(stderr, "appif_listen_newconn: No space in spout queue (TODO)\n");
    return;
  }

  spout->data.listen_newconn.opaque = l->opaque;
  spout->data.listen_newconn.remote_ip = remote_ip;
  spout->data.listen_newconn.remote_port = remote_port;
  MEM_BARRIER();
  spout->type = SP_APPIN_LISTEN_NEWCONN;
  appif_ctx_kick(ctx);

  spout_pos++;
  if (spout_pos >= ctx->spout_len) {
    spout_pos = 0;
  }
  ctx->spout_pos = spout_pos;

}

void appif_accept_conn(struct connection *c, int status)
{
  struct app_context *ctx = c->ctx;
  struct application *app = ctx->app;
  volatile struct sp_appin *spout = ctx->spout_base;
  uint32_t spout_pos = ctx->spout_pos;

  spout += spout_pos;

  /* make sure we have room for a response */
  if (spout->type != SP_APPIN_INVALID) {
    fprintf(stderr, "appif_accept_conn: No space in spout queue (TODO)\n");
    return;
  }

  spout->data.accept_connection.opaque = c->opaque;
  spout->data.accept_connection.status = status;
  if (status == 0) {
    spout->data.accept_connection.rx_off = c->rx_buf - (uint8_t *) flextoe_dma_mem;
    spout->data.accept_connection.tx_off = c->tx_buf - (uint8_t *) flextoe_dma_mem;
    spout->data.accept_connection.rx_len = c->rx_len;
    spout->data.accept_connection.tx_len = c->tx_len;

    spout->data.accept_connection.seq_rx = c->remote_seq;
    spout->data.accept_connection.seq_tx = c->local_seq;
    spout->data.accept_connection.local_ip = config.ip;
    spout->data.accept_connection.remote_ip = c->remote_ip;
    spout->data.accept_connection.remote_port = c->remote_port;
    spout->data.accept_connection.flow_id = c->flow_id;
    spout->data.accept_connection.flow_group = c->flow_group;

    c->app_next = app->conns;
    app->conns = c;
  } else {
    tcp_destroy(c);
  }

  MEM_BARRIER();
  spout->type = SP_APPIN_ACCEPTED_CONN;
  appif_ctx_kick(ctx);

  spout_pos++;
  if (spout_pos >= ctx->spout_len) {
    spout_pos = 0;
  }
  ctx->spout_pos = spout_pos;
}

unsigned appif_ctx_poll(struct application *app, struct app_context *ctx)
{
  volatile struct sp_appout *spin = ctx->spin_base;
  volatile struct sp_appin *spout = ctx->spout_base;
  uint32_t spin_pos = ctx->spin_pos;
  uint32_t spout_pos = ctx->spout_pos;
  uint8_t type;
  int spout_inc = 0;

  spin += spin_pos;
  spout += spout_pos;

  /* make sure we have room for a response */
  if (spout->type != SP_APPIN_INVALID) {
    return 0;
  }

  type = spin->type;
  MEM_BARRIER();

  switch (type) {
    case SP_APPOUT_INVALID:
      /* nothing yet */
      return 0;

    case SP_APPOUT_CONN_OPEN:
      /* connection request */
      spout_inc += spin_conn_open(app, ctx, spin, spout);
      break;

    case SP_APPOUT_CONN_MOVE:
      /* connection move request */
      spout_inc += spin_conn_move(app, ctx, spin, spout);
      break;

    case SP_APPOUT_CONN_CLOSE:
      /* connection close request */
      spout_inc += spin_conn_close(app, ctx, spin, spout);
      break;

    case SP_APPOUT_LISTEN_OPEN:
      /* listen request */
      spout_inc += spin_listen_open(app, ctx, spin, spout);
      break;

    case SP_APPOUT_ACCEPT_CONN:
      /* accept request */
      spout_inc += spin_accept_conn(app, ctx, spin, spout);
      break;

    case SP_APPOUT_LISTEN_CLOSE:
    default:
      fprintf(stderr, "spin_poll: unsupported request type%u\n", spin->type);
      break;
  }

  MEM_BARRIER();
  spin->type = 0;

  /* update spin queue position */
  spin_pos++;
  if (spin_pos >= ctx->spin_len) {
    spin_pos = 0;
  }
  ctx->spin_pos = spin_pos;

  /* update spout queue position if the entry was used */
  if (spout_inc > 0) {
    spout_pos += spout_inc;
    if (spout_pos >= ctx->spout_len) {
      spout_pos = 0;
    }
    ctx->spout_pos = spout_pos;
  }

  return 1;
}

static int spin_conn_open(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout)
{
  struct connection *conn;

  if (tcp_open(ctx, spin->data.conn_open.opaque, spin->data.conn_open.remote_ip,
      spin->data.conn_open.remote_port, ctx->doorbell->id, &conn) != 0)
  {
    fprintf(stderr, "%s(): tcp_open failed\n", __func__);
    goto error;
  }

  conn->app_next = app->conns;
  app->conns = conn;

  return 0;

error:
  spout->data.conn_opened.opaque = spin->data.conn_open.opaque;
  spout->data.conn_opened.status = -1;
  MEM_BARRIER();
  spout->type = SP_APPIN_CONN_OPENED;
  appif_ctx_kick(ctx);

  return 1;
}

static int spin_conn_move(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout)
{
  struct connection *conn;
  struct app_context *new_ctx;

  for (conn = app->conns; conn != NULL; conn = conn->app_next) {
    if (conn->local_ip == spin->data.conn_move.local_ip &&
        conn->remote_ip == spin->data.conn_move.remote_ip &&
        conn->local_port == spin->data.conn_move.local_port &&
        conn->remote_port == spin->data.conn_move.remote_port &&
        conn->opaque == spin->data.conn_move.opaque)
    {
      break;
    }
  }
  if (conn == NULL) {
    fprintf(stderr, "spin_conn_move: connection not found\n");
    goto error;
  }

  for (new_ctx = app->contexts; new_ctx != NULL; new_ctx = new_ctx->next) {
    if (new_ctx->doorbell->id == spin->data.conn_move.db_id) {
      break;
    }
  }
  if (new_ctx == NULL) {
    fprintf(stderr, "spin_conn_move: destination context not found\n");
    goto error;
  }

  if (conn->status != CONN_OPEN) {
    fprintf(stderr, "spin_conn_move: connection not open\n");
    goto error;
  }

  if (nicif_connection_move(new_ctx->doorbell->id, conn->flow_id) != 0) {
    fprintf(stderr, "spin_conn_move: nicif_connection_move failed\n");
    goto error;
  }

  spout->data.status.opaque = spin->data.conn_move.opaque;
  spout->data.status.status = 0;
  MEM_BARRIER();
  spout->type = SP_APPIN_STATUS_CONN_MOVE;
  appif_ctx_kick(ctx);
  return 1;

error:
  spout->data.status.opaque = spin->data.conn_move.opaque;
  spout->data.status.status = -1;
  MEM_BARRIER();
  spout->type = SP_APPIN_STATUS_CONN_MOVE;
  appif_ctx_kick(ctx);
  return 1;
}

static int spin_conn_close(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout)
{
  struct connection *conn;

  for (conn = app->conns; conn != NULL; conn = conn->app_next) {
    if (conn->local_ip == spin->data.conn_close.local_ip &&
        conn->remote_ip == spin->data.conn_close.remote_ip &&
        conn->local_port == spin->data.conn_close.local_port &&
        conn->remote_port == spin->data.conn_close.remote_port &&
        conn->opaque == spin->data.conn_close.opaque)
    {
      break;
    }
  }
  if (conn == NULL) {
    fprintf(stderr, "spin_conn_close: connection not found\n");
    goto error;
  }

  if (tcp_close(conn) != 0) {
    fprintf(stderr, "spin_conn_close: tcp_close failed\n");
    goto error;
  }

  return 0;

error:
  spout->data.status.opaque = spin->data.conn_close.opaque;
  spout->data.status.status = -1;
  MEM_BARRIER();
  spout->type = SP_APPIN_STATUS_CONN_CLOSE;
  appif_ctx_kick(ctx);

  return 1;
}

static int spin_listen_open(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout)
{
  struct listener *listen;

  if (tcp_listen(ctx, spin->data.listen_open.opaque,
      spin->data.listen_open.local_port, spin->data.listen_open.backlog,
      !!(spin->data.listen_open.flags & SP_APPOUT_LISTEN_REUSEPORT),
      &listen) != 0) {
    fprintf(stderr, "spin_listen_open: tcp_listen failed\n");
    goto error;
  }

  listen->app_next = app->listeners;
  app->listeners = listen;

  spout->data.status.opaque = spin->data.listen_open.opaque;
  spout->data.status.status = 0;
  MEM_BARRIER();
  spout->type = SP_APPIN_STATUS_LISTEN_OPEN;
  appif_ctx_kick(ctx);

  return 1;

error:
  spout->data.status.opaque = spin->data.listen_open.opaque;
  spout->data.status.status = -1;
  MEM_BARRIER();
  spout->type = SP_APPIN_STATUS_LISTEN_OPEN;
  appif_ctx_kick(ctx);

  return 1;
}

static int spin_accept_conn(struct application *app, struct app_context *ctx,
    volatile struct sp_appout *spin, volatile struct sp_appin *spout)
{
  struct listener *listen;

  /* look for listen struct */
  for (listen = app->listeners; listen != NULL; listen = listen->app_next) {
    if (listen->port == spin->data.accept_conn.local_port &&
        listen->opaque == spin->data.accept_conn.listen_opaque)
    {
      break;
    }
  }

  if (tcp_accept(ctx, spin->data.accept_conn.conn_opaque, listen,
        ctx->doorbell->id) != 0)
  {
    fprintf(stderr, "spin_accept_conn\n");
    goto error;
  }

  return 0;

error:
  spout->data.accept_connection.opaque = spin->data.accept_conn.conn_opaque;
  spout->data.accept_connection.status = -1;
  MEM_BARRIER();
  spout->type = SP_APPIN_ACCEPTED_CONN;
  appif_ctx_kick(ctx);

  return 1;
}
