/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef _FLEXTOE_CONNECT_H_
#define _FLEXTOE_CONNECT_H_

#include <stdint.h>
#include <sys/types.h>
#include <linux/limits.h>

#include "common.h"

/** Info struct: layout of info shared memory region */
PACKED_STRUCT(flexnic_info_t)
{
  uint64_t flags;                   /*> Flags: see FLEXNIC_FLAG_* */
  uint64_t dma_mem_size;            /*> Size of flexnic dma memory in bytes. */
  uint64_t internal_mem_size;       /*> Size of internal flexnic memory in bytes. */
  uint64_t mac_address;             /*> export mac address */
  uint64_t poll_cycle_app;          /*> Cycles to poll before blocking for application */
  uint32_t cores_num;               /*> Number of cores in flexnic emulator */
  char bar_resource_path[PATH_MAX]; /*> MMIO resource path */
  off_t internal_mem_offset;        /*> Internal memory offset in bar resource */
};

#define FLEXNIC_FLAG_READY            (1 << 0)     /*> Flexnic is done initializing */
#define FLEXNIC_FLAG_HUGEPAGES        (1 << 1)     /*> Huge pages should be used for the internal and dma memory */

/** Connect parameters */
#define FLEXNIC_SHM_PREFIX    "/dev/shm"                    /*> Shared memory mount point */
#define FLEXNIC_HUGE_PREFIX   "/dev/hugepages-1048576kB"    /*> Hugepages mount point */
#define FLEXNIC_NAME_INFO     "flextoe_info"       /*> Name for the info shared memory region */
#define FLEXNIC_NAME_DMA_MEM  "flextoe_memory"     /*> Name for flexnic dma shared memory region */
#define FLEXNIC_INFO_BYTES    0x4000               /*> Size of the info shared memory region */

/** Unix socket for initialization with application */
#define FLEXNIC_SOCKET_PATH   "\0flexnic_os"
#define FLEXNIC_UXSOCK_MAXQ   8

#endif /* _FLEXTOE_CONNECT_H_ */
