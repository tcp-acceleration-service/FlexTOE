/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2018 Netronome Systems, Inc.
 * All rights reserved.
 */

#ifndef __NFP_NFP6000_H__
#define __NFP_NFP6000_H__

/* CPP Target IDs */
#define NFP_CPP_TARGET_INVALID          0
#define NFP_CPP_TARGET_NBI              1
#define NFP_CPP_TARGET_QDR              2
#define NFP_CPP_TARGET_ILA              6
#define NFP_CPP_TARGET_MU               7
#define NFP_CPP_TARGET_PCIE             9
#define NFP_CPP_TARGET_ARM              10
#define NFP_CPP_TARGET_CRYPTO           12
#define NFP_CPP_TARGET_ISLAND_XPB       14	/* Shared with CAP */
#define NFP_CPP_TARGET_ISLAND_CAP       14	/* Shared with XPB */
#define NFP_CPP_TARGET_CT_XPB           14
#define NFP_CPP_TARGET_LOCAL_SCRATCH    15
#define NFP_CPP_TARGET_CLS              NFP_CPP_TARGET_LOCAL_SCRATCH

#define NFP_ISL_EMEM0                   24

#define NFP_MU_ADDR_ACCESS_TYPE_MASK    3ULL
#define NFP_MU_ADDR_ACCESS_TYPE_DIRECT  2ULL

#include "nfp6000/nfp_cls.h"
#include "nfp6000/nfp_ctm.h"
#include "nfp6000/nfp_event.h"
#include "nfp6000/nfp_mac_csr_synch.h"
#include "nfp6000/nfp_mac.h"
#include "nfp6000/nfp_me.h"
#include "nfp6000/nfp_nbi_pc.h"
#include "nfp6000/nfp_nbi_tm.h"
#include "nfp6000/nfp_pcie.h"
#include "nfp6000/nfp_qc.h"

#endif /* NFP_NFP6000_H */
