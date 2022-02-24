/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "util/shm.h"
#include "util/timeout.h"

#include "connect.h"

#include "flextoe.h"

void *flextoe_dma_mem = NULL;
struct flexnic_info_t *flextoe_info = NULL;

int shm_init(void)
{
  flextoe_dma_mem = util_create_shm_huge(FLEXNIC_NAME_DMA_MEM, config.shm_len, NULL);
  if (flextoe_dma_mem == NULL) {
    fprintf(stderr, "dma memory allocation failed\n");
    return -1;
  }

  if (!config.quiet) {
    printf("flextoe_dma_mem virt:%p phy:%p len:%lx\n",
      (uint8_t*) flextoe_dma_mem, (uint8_t*) util_virt2phy(flextoe_dma_mem), config.shm_len);
  }

  flextoe_info = util_create_shm(FLEXNIC_NAME_INFO, FLEXNIC_INFO_BYTES, NULL);
  if (flextoe_info == NULL) {
    fprintf(stderr, "flextoe_info allocation failed\n");
    shm_cleanup();
    return -1;
  }

  /* FIXME */
  flextoe_info->dma_mem_size = config.shm_len;
  flextoe_info->cores_num = 1;
  flextoe_info->mac_address = 0;
  flextoe_info->poll_cycle_app = util_us_to_cyc(config.fp_poll_interval_app);

  flextoe_info->flags |= FLEXNIC_FLAG_HUGEPAGES;

  return 0;
}

void shm_cleanup(void)
{
  /* cleanup dma memory region */
  if (flextoe_dma_mem != NULL) {
    util_destroy_shm_huge(FLEXNIC_NAME_DMA_MEM, config.shm_len, flextoe_dma_mem);
  }

  /* cleanup flextoe_info memory region */
  if (flextoe_info != NULL) {
    util_destroy_shm(FLEXNIC_NAME_INFO, FLEXNIC_INFO_BYTES, flextoe_info);
  }
}

void shm_set_ready(void)
{
  flextoe_info->flags |= FLEXNIC_FLAG_READY;
}
