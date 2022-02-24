/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLEXTOE_MEMIF_H_
#define FLEXTOE_MEMIF_H_

#include <stdint.h>

#include "common.h"
#include "fp_sp_if.h"
#include "fp_app_if.h"
#include "flow_state.h"

/******************************************************************************/
PACKED_ALIGN_STRUCT(flextcp_pl_mem, 64)
{
  /* registers for application context queues */
  struct flextcp_pl_appctx_t appctx[FLEXNIC_PL_APPCTX_NUM];

  /* registers for flow state */
  struct flowst_tcp_t  flows_tcp_state[FLEXNIC_PL_FLOWST_NUM];
  struct flowst_conn_t flows_conn_info[FLEXNIC_PL_FLOWST_NUM];
  struct flowst_mem_t  flows_mem_info[FLEXNIC_PL_FLOWST_NUM];
  struct flowst_cc_t   flows_cc_info[FLEXNIC_PL_FLOWST_NUM];

  /* sp context queues */
  struct flextcp_pl_spctx_t spctx;

  /* registers for application state */
  struct flextcp_pl_appst_t appst[FLEXNIC_PL_APPST_NUM];

  /* registers for fastpath configuration */
  struct flextcp_pl_config cfg;
};

#endif /* FLEXTOE_MEMIF_H_ */
