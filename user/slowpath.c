/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <errno.h>

#include "util/common.h"
#include "util/timeout.h"
#include "util/shm.h"

#include "connect.h"
#include "flextoe.h"
#include "internal.h"

static void timeout_trigger(struct timeout *to, uint8_t type, void *opaque);
static void signal_flextoe_ready(void);

struct timeout_manager timeout_mgr;
struct sp_statistics spstats;
uint32_t cur_ts;
int sp_notifyfd = 0;
static int epfd;
int debug_reset = 0;

extern void *flextoe_dma_mem;
int slowpath_main(void)
{
  uint32_t last_print = 0;
  int local_debug_reset = 0;

  sp_notifyfd = eventfd(0, EFD_NONBLOCK);
  assert(sp_notifyfd != -1);

  struct epoll_event ev = {
    .events = EPOLLIN,
    .data.fd = sp_notifyfd,
  };

  epfd = epoll_create1(0);
  assert(epfd != -1);

  int r = epoll_ctl(epfd, EPOLL_CTL_ADD, sp_notifyfd, &ev);
  assert(r == 0);

  /* initialize timers for timeouts */
  if (util_timeout_init(&timeout_mgr, timeout_trigger, NULL)) {
    fprintf(stderr, "timeout_init failed\n");
    return EXIT_FAILURE;
  }

  /* initialize routing subsystem */
  if (routing_init()) {
    fprintf(stderr, "routing_init failed\n");
    return EXIT_FAILURE;
  }

  /* connect to NIC */
  if (nicif_init()) {
    fprintf(stderr, "nicif_init failed\n");
    return EXIT_FAILURE;
  }

  /* initialize congestion control */
  if (cc_init()) {
    fprintf(stderr, "cc_init failed\n");
    return EXIT_FAILURE;
  }

  /* prepare application interface */
  if (appif_init()) {
    fprintf(stderr, "appif_init failed\n");
    return EXIT_FAILURE;
  }

  if (arp_init()) {
    fprintf(stderr, "arp_init failed\n");
    return EXIT_FAILURE;
  }

  if (tcp_init()) {
    fprintf(stderr, "tcp_init failed\n");
    return EXIT_FAILURE;
  }

  signal_flextoe_ready();

  while (exited == 0) {
    unsigned n = 0;

    cur_ts = util_timeout_time_us();
    n += nicif_poll();
    n += cc_poll(cur_ts);
    n += appif_poll();
    tcp_poll();
    util_timeout_poll_ts(&timeout_mgr, cur_ts);

    /* Reset stats if indicated */
    /* NOTE: wraparound not handled because 2^31 resets not possible */
    if (local_debug_reset < debug_reset) {
      nicif_debug_reset();
      local_debug_reset++;
    }

    if (cur_ts - last_print >= 1000000) {
      if (!config.quiet) {
        printf(
          "stats: drops=%"PRIu64" k_rexmit=%"PRIu64" ecn=%"PRIu64
          " acks=%"PRIu64"\n",
            spstats.drops, spstats.sp_rexmit, spstats.ecn_marked,
            spstats.acks);
        fflush(stdout);
      }

      last_print = cur_ts;
    }
  }

  /* TODO: Gracefully close nicif */
  /* Close appif */
  appif_close();

  return EXIT_SUCCESS;
}

static void timeout_trigger(struct timeout *to, uint8_t type, void *opaque)
{
  switch (type) {
    case TO_ARP_REQ:
      arp_timeout(to, type);
      break;

    case TO_ARP_GRAT:
      gratuitous_arp_timeout(to, type);
      break;

    case TO_TCP_HANDSHAKE:
    case TO_TCP_RETRANSMIT:
    case TO_TCP_CLOSED:
      tcp_timeout(to, type);
      break;

    default:
      fprintf(stderr, "Unknown timeout type: %u\n", type);
      abort();
  }
}

static void signal_flextoe_ready(void)
{
  flextoe_info->flags |= FLEXNIC_FLAG_READY;
  nn_writeq(util_virt2phy(flextoe_dma_mem), &fp_state->cfg.phyaddr);
  nn_writeq(config.shm_len, &fp_state->cfg.memsize);
  nn_writeq(1, &fp_state->cfg.sig);
}
