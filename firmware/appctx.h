/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef __ARX_INTERNAL
#define __ARX_INTERNAL

#include <stdint.h>
#include <nfp.h>

#include "fp_mem.h"

/* -> ARX command */
struct arx_cmd_t {
  union {
    __packed struct {
      uint32_t desc_idx:16;     /*> Descriptor buffer index          */
      uint32_t db_id:5;         /*> APPCTX doorbell id               */
      uint32_t rsvd:11;
    };

    uint32_t __raw;
  };
};

/**
 * CLS ring configurations
 * (a)  DMA (arx_cmd_t)            --> ARX
 */
#define ARX_WQ_RNUM     0       /*> CLS Ring number for DMA -> ARX WQ */

#endif /* __ARX_INTERNAL */
