/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <stdlib.h>

#include "util/shm.h"

#include "connect.h"
#include "flextoe.h"
#include "internal.h"

#define PACKETMEM_MAX_ZONES   16    /* Restrict to 16 hugepages */

struct packetmem_handle {
  uintptr_t base;
  size_t len;
  uint32_t zone;

  struct packetmem_handle *next;
};

static int packetmem_zone_alloc(uint32_t zone, size_t length, uintptr_t *off,
    struct packetmem_handle **handle);
static inline struct packetmem_handle *ph_alloc(void);
static inline void ph_free(struct packetmem_handle *ph);
static inline void merge_items(uint32_t zone, struct packetmem_handle *ph_prev);

static struct packetmem_handle *freelist[PACKETMEM_MAX_ZONES];
static uint32_t total_zones;

int packetmem_init(void)
{
  uint32_t zone;

  if (flextoe_info->dma_mem_size % HUGE_PAGE_SIZE != 0) {
    fprintf(stderr, "%s(): invalid flextoe_dma_mem memory\n", __func__);
    return -1;
  }

  total_zones = (uint32_t) (flextoe_info->dma_mem_size / HUGE_PAGE_SIZE);
  if (total_zones == 0 || total_zones > PACKETMEM_MAX_ZONES) {
    fprintf(stderr, "%s(): invalid flextoe_dma_mem memory\n", __func__);
    return -1;
  }

  for (zone = 0; zone < total_zones; zone++) {
    struct packetmem_handle *ph;

    if ((ph = ph_alloc()) == NULL) {
      fprintf(stderr, "packetmem_init: ph_alloc failed\n");
      return -1;
    }

    ph->base = 0;
    ph->len  = HUGE_PAGE_SIZE;
    ph->zone = zone;
    ph->next = NULL;
    freelist[zone] = ph;
  }

  return 0;
}

static int packetmem_zone_alloc(uint32_t zone, size_t length, uintptr_t *off,
    struct packetmem_handle **handle)
{
  struct packetmem_handle *ph, *ph_prev, *ph_new;

  /* look for first fit */
  ph_prev = NULL;
  ph = freelist[zone];
  while (ph != NULL && ph->len < length) {
    ph_prev = ph;
    ph = ph->next;
  }

  /* didn't find a fit */
  if (ph == NULL) {
    return -1;
  }

  if (ph->len == length) {
    /* simple case, don't need to split this handle */

    /* pointer to previous next pointer for removal */
    if (ph_prev == NULL) {
      freelist[zone] = ph->next;
    } else {
      ph_prev->next = ph->next;
    }

    ph_new = ph;
  } else {
    /* need to split */

    /* new packetmem handle for splitting */
    if ((ph_new = ph_alloc()) == NULL) {
      fprintf(stderr, "packetmem_alloc: ph_alloc failed\n");
      return -1;
    }

    ph_new->base = ph->base;
    ph_new->len = length;
    ph_new->zone = ph->zone;
    ph_new->next = NULL;

    ph->base += length;
    ph->len -= length;
  }

  *handle = ph_new;
  *off = ph_new->base;

  return 0;
}

int packetmem_alloc(size_t length, uintptr_t *off,
    struct packetmem_handle **handle)
{
  uint32_t zone;
  int ret;

  for (zone = 0; zone < total_zones; zone ++) {
    ret = packetmem_zone_alloc(zone, length, off, handle);

    if (ret < 0)
      continue;

    return 0;
  }

  return -1;
}

void packetmem_free(struct packetmem_handle *handle)
{
  struct packetmem_handle *ph, *ph_prev;
  uint32_t zone = handle->zone;

  /* look for first successor */
  ph_prev = NULL;
  ph = freelist[zone];
  while (ph != NULL && ph->next != NULL && ph->next->base < handle->base) {
    ph_prev = ph;
    ph = ph->next;
  }

  /* add to list */
  if (ph_prev == NULL) {
    handle->next = freelist[zone];
    freelist[zone] = handle;
  } else {
    handle->next = ph_prev->next;
    ph_prev->next = handle;
  }

  /* merge items if necessary */
  merge_items(zone, ph_prev);
}

/** Merge handles around newly inserted item (pointer to predecessor or NULL
 * passed).
 */
static inline void merge_items(uint32_t zone, struct packetmem_handle *ph_prev)
{
  struct packetmem_handle *ph, *ph_next;

  /* try to merge with predecessor if there is one */
  if (ph_prev != NULL) {
    ph = ph_prev->next;
    if (ph_prev->base + ph_prev->len == ph->base) {
      /* merge with predecessor */
      ph_prev->next = ph->next;
      ph_prev->len += ph->len;
      ph_free(ph);
      ph = ph_prev;
    }
  } else {
    ph = freelist[zone];
  }

  /* try to merge with successor if there is one */
  ph_next = ph->next;
  if (ph_next != NULL && ph->base + ph->len == ph_next->base) {
    ph->len += ph_next->len;
    ph->next = ph_next->next;
    ph_free(ph_next);
  }
}

static inline struct packetmem_handle *ph_alloc(void)
{
  return malloc(sizeof(struct packetmem_handle));
}

static inline void ph_free(struct packetmem_handle *ph)
{
  free(ph);
}
