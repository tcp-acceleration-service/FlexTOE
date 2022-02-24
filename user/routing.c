/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdlib.h>
#include <stdio.h>

#include "flextoe.h"
#include "internal.h"

/** Routing table entry */
struct routing_table_entry {
  /** Destination IP address */
  uint32_t dest_ip;
  /** Destination IP address mask */
  uint32_t dest_mask;
  /** Next hop IP address */
  uint32_t next_hop;
};

static inline uint32_t prefix_len_mask(uint8_t len);
static inline struct routing_table_entry *resolve(uint32_t ip);

/** Routing table */
static struct routing_table_entry *routing_table = NULL;
static size_t routing_table_len = 0;

int routing_init(void)
{
  struct config_route *cr;
  size_t i;
  uint32_t mask;

  /* count number of entries to be added */
  routing_table_len = 1;
  for (cr = config.routes; cr != NULL; routing_table_len++, cr = cr->next);

  /* allocate table */
  if ((routing_table = calloc(routing_table_len, sizeof(*routing_table)))
      == NULL)
  {
    fprintf(stderr, "routing_init: allocating routing table failed\n");
    return -1;
  }

  /* first fill in network route based on ip and prefix */
  mask = prefix_len_mask(config.ip_prefix);
  routing_table[0].dest_ip = config.ip & mask;
  routing_table[0].dest_mask = mask;
  routing_table[0].next_hop = 0;

  /* fill in routing table */
  for (i = 1, cr = config.routes; cr != NULL; i++, cr = cr->next) {
    mask = prefix_len_mask(cr->ip_prefix);
    if ((mask & cr->ip) != cr->ip) {
      fprintf(stderr, "routing_init: mask removes non-0 bits "
          "(d=%x m=%x n=%x)\n", cr->ip, mask, cr->next_hop_ip);
      return -1;
    }

    routing_table[i].dest_ip = cr->ip;
    routing_table[i].dest_mask = mask;
    routing_table[i].next_hop = cr->next_hop_ip;
  }

  return 0;
}

int routing_resolve(struct nicif_completion *comp, uint32_t ip, uint64_t *mac)
{
  struct routing_table_entry *rte;

  while (1) {
    rte = resolve(ip);
    if (rte == NULL) {
      fprintf(stderr, "routing_resolve: routing failed\n");
      return -1;
    }

    if (rte->next_hop == 0) {
      break;
    }

    ip = rte->next_hop;
  }

  return arp_request(comp, ip, mac);
}

static inline uint32_t prefix_len_mask(uint8_t len)
{
  return ~((1ULL << (32 - len)) - 1);
}

static inline struct routing_table_entry *resolve(uint32_t ip)
{
  size_t i;

  for (i = 0; i < routing_table_len; i++) {
    if (routing_table[i].dest_ip == (ip & routing_table[i].dest_mask)) {
      return &routing_table[i];
    }
  }

  return NULL;
}
