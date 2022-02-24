/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef DESC_POOL_H_
#define DESC_POOL_H_

#include <nfp.h>
#include <nfp/me.h>
#include <nfp/ctm.h>
#include <nfp/mem_ring.h>

#include "fp_mem.h"

struct appctx_desc_t {
  union {
    struct flextcp_pl_atx_t atx;
    struct flextcp_pl_arx_t arx;
    uint32_t __raw[8];
  };
};

__export __shared __emem struct appctx_desc_t atx_desc_pool[DMA_ATX_DESC_POOL_NUM];
__export __shared __emem struct appctx_desc_t arx_desc_pool[DMA_ARX_DESC_POOL_NUM];

/**
 * CTM ring configurations
 */
#define ATX_DESC_POOL_ISLAND      36
#define ATX_DESC_POOL_RNUM        1

MEM_RING_INIT(atx_desc_ring, DMA_ATX_DESC_POOL_SIZE);
MEM_RING_INIT(arx_desc_ring, DMA_ARX_DESC_POOL_SIZE);

#endif /* DESC_POOL_H_ */
