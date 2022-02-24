/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLEXTOE_FP_DEBUG_H_
#define FLEXTOE_FP_DEBUG_H_

#include <stdint.h>
#include "common.h"

/* Log Modules */
enum fp_modules_e {
  FP_MOD_RX,
  FP_MOD_SP,
  FP_MOD_AC,
  FP_MOD_QM,
  FP_MOD_TX,
  FP_MOD_ATX,
  FP_MOD_ARX,
};

/* Statistics */
enum fp_stats_e {
  FP_STAT_RX_TOTAL,
  FP_STAT_TX_TOTAL,
  FP_STAT_AC_TOTAL,
  FP_STAT_RETX_TOTAL,
  FP_STAT_GRP0,
  FP_STAT_GRP1,
  FP_STAT_GRP2,
  FP_STAT_GRP3,
  FP_STAT_RX_SP,
  FP_STAT_RX_PROC,
  FP_STAT_TX_PROC,
  FP_STAT_AC_PROC,
  FP_STAT_RETX_PROC,
  FP_STAT_RX_POSTPROC_IN,
  FP_STAT_TX_POSTPROC_IN,
  FP_STAT_AC_POSTPROC_IN,
  FP_STAT_RETX_POSTPROC_IN,
  FP_STAT_DMA_RX_PAYLOAD_ISSUE,
  FP_STAT_DMA_RX_FWD_ACK,
  FP_STAT_DMA_RX_FWD_DROP,
  FP_STAT_DMA_RX_FWD_ARX,
  FP_STAT_DMA_TX_PAYLOAD_ISSUE,
  FP_STAT_DMA_TX_FWD_SEG,
  FP_STAT_DMA_TX_FWD_DROP,
  FP_STAT_DMA_ARX_DESC_ISSUE,
  FP_STAT_DMA_ARX_DESC_FWD_FREE,
  FP_STAT_DMA_ATX_DESC_ISSUE,
  FP_STAT_DMA_ATX_DESC_FWD_AC,
  FP_STAT_ARX,
  FP_STAT_ATX,
  FP_STAT_ATX_NODESC,
  FP_STAT_PROC_CACHE_MISS,
  FP_STAT_PROC_EMEM_CACHE_MISS,
  FP_STAT_QM_SCHEDULE,
  FP_STAT_SP_FWD,
};

/* Profiling sections */
enum fp_prof_e {
  FP_PROF_PROC_RX,
  FP_PROF_PROC_TX,
  FP_PROF_PROC_AC,
  FP_PROF_PROC_RETX,
  FP_PROF_PROC_ACK,
};


PACKED_STRUCT(flextcp_pl_debug)
{
  /* Configuration space */
  uint32_t log_tail_idx;  /*> Incremented by the device */
  uint32_t __pad1[63];    /*> Ensure 256 byte alignment */

  /* Log buffer */
  uint32_t log_buffer[LOGBUF_SIZE];

  /* Stats buffer */
  /* 256 byte aligned */
  uint64_t stats_buffer[STATBUF_SIZE];

  /* Profiling buffer */
  /* 256 byte aligned */
  uint64_t prof_cycles[PROFBUF_SIZE]; /*> Number of cycles per section */
  uint64_t prof_count[PROFBUF_SIZE];  /*> Count of hits on section */

  /* Padding */
  uint64_t __pad2[PROFBUF_SIZE * 4];
};

#endif /* FLEXTOE_FP_DEBUG_H_ */
