/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef APPIF_H_
#define APPIF_H_

#include <stdbool.h>
#include <stdint.h>

#include "util/nbqueue.h"

#include "internal.h"
#include "sp_app_if.h"

struct app_doorbell {
  uint32_t id;
  /* only for freelist */
  struct app_doorbell *next;
};

struct app_context {
  struct application *app;

  void *spin_base;
  uint32_t spin_len;
  uint32_t spin_pos;

  void *spout_base;
  uint32_t spout_len;
  uint32_t spout_pos;

  struct app_doorbell *doorbell;

  int ready, evfd;
  uint64_t last_ts;
  struct app_context *next;

  struct {
    struct packetmem_handle *spinq;
    struct packetmem_handle *spoutq;
    struct packetmem_handle *rxq;
    struct packetmem_handle *txq;
  } handles;
};

struct application {
  int fd;
  struct nbqueue_el nqe;
  size_t req_rx;
  struct sp_uxsock_request req;
  size_t resp_sz;
  struct sp_uxsock_response *resp;

  struct app_context *contexts;
  struct application *next;
  struct app_context *need_reg_ctx;
  struct app_context *need_reg_ctx_done;

  struct connection *conns;
  struct listener   *listeners;

  struct nicif_completion comp;

  uint16_t id;
  volatile bool closed;
};

/**
 * Poll sp->app context queue.
 *
 * @param app Application to poll
 * @param ctx Context to poll
 */
unsigned appif_ctx_poll(struct application *app, struct app_context *ctx);

#endif /* APPIF_H_ */
