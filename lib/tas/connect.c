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

#include "util/shm.h"

#include "tas_ll_connect.h"

static struct flexnic_info_t *info = NULL;

int flexnic_driver_connect(struct flexnic_info_t **p_info, void **p_mem_start)
{
  void *m;
  volatile struct flexnic_info_t *fi;
  int err_ret = -1;

  /* return error, if already connected */
  if (info != NULL) {
    fprintf(stderr, "flexnic_driver_connect: already connected\n");
    goto error_exit;
  }

  /* open and map flexnic info shm region */
  if ((m = util_map_region(FLEXNIC_NAME_INFO, FLEXNIC_INFO_BYTES)) == NULL) {
    perror("flexnic_driver_connect: map_region info failed");
    goto error_exit;
  }

  /* abort if not ready yet */
  fi = (volatile struct flexnic_info_t *) m;
  if ((fi->flags & FLEXNIC_FLAG_READY) != FLEXNIC_FLAG_READY) {
    err_ret = 1;
    goto error_unmap_info;
  }

  m = util_map_region_huge(FLEXNIC_NAME_DMA_MEM, fi->dma_mem_size);
  if (m == NULL) {
    perror("flexnic_driver_connect: mapping dma memory failed");
    goto error_unmap_info;
  }

  *p_info = info = (struct flexnic_info_t *) fi;
  *p_mem_start = m;
  return 0;

error_unmap_info:
  util_unmap_region(m, FLEXNIC_INFO_BYTES);
error_exit:
  return err_ret;
}

int flexnic_driver_internal(void **int_mem_start)
{
  int fd;
  char *m;

  if (info == NULL) {
    fprintf(stderr, "flexnic_driver_internal: driver not connected\n");
    return -1;
  }

  fd = open(info->bar_resource_path, O_RDWR);
  if (fd < 0) {
    perror("Failed to map internal memory\n");
    return -1;
  }

  m = mmap(NULL,
          info->internal_mem_size,
          PROT_READ | PROT_WRITE,
          MAP_SHARED,
          fd, 0);
  if (m == MAP_FAILED) {
    perror("flexnic_driver_internal: map_region failed");
    return -1;
  }

  *int_mem_start = (m + info->internal_mem_offset);
  return 0;
}
