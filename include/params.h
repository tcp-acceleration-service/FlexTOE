/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLEXTOE_PARAMS_H_
#define FLEXTOE_PARAMS_H_

/** Flow state parameters */
#define FLEXNIC_PL_APPST_NUM        8
#define FLEXNIC_PL_APPST_CTX_NUM    31
#define FLEXNIC_PL_APPCTX_NUM       32
#define FLEXNIC_PL_FLOWST_NUM       16384
#define FLEXNIC_PL_FLOWHT_ENTRIES   (FLEXNIC_PL_FLOWST_NUM * 2)

/**
 * NOTE:
 * Developed for: Beryllium Agilio CX 2x40
 * Untested on other platforms
 */
/** Platform definition */
#define NS_PLATFORM_TYPE                        7   // NS_PLATFORM_BERYLLIUM
#define PORT_IN_USE                             1
#include "platform.h"

/** Debug configuration */
#define PKTBUF_SIZE       2048
#define LOGBUF_SIZE       4096
#define STATBUF_SIZE      128
#define PROFBUF_SIZE      128
#define JOURNAL_SIZE      (4 * 1024 * 1024)  // 4M

#define FP_LOG_ENABLE     0
#define FP_STAT_ENABLE    0
#define FP_PROF_ENABLE    0
#define FP_JRNL_ENABLE    0

#endif /* FLEXTOE_PARAMS_H_ */
