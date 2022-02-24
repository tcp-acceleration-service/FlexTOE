/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef __FIRMWARE_GLOBAL_STATE__
#define __FIRMWARE_GLOBAL_STATE__

#include "fp_mem.h"
#include "fp_debug.h"

__export __shared __emem __align8M struct flextcp_pl_mem fp_state;

#if FP_LOG_ENABLE || FP_STAT_ENABLE || FP_PROF_ENABLE
__export __shared __emem struct flextcp_pl_debug fp_debug;
#endif

#if FP_JRNL_ENABLE
MEM_RING_INIT(fp_dbg_journal, JOURNAL_SIZE);
#endif

#endif /* __FIRMWARE_GLOBAL_STATE__ */
