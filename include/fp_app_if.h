/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FP_APP_IF_H_
#define FP_APP_IF_H_

#include <stdint.h>
#include "common.h"

/** App RX queue */
enum {
  FLEXTCP_PL_ARX_INVALID = 0,
  FLEXTCP_PL_ARX_CONNUPDATE,
  FLEXTCP_PL_ARX_DUMMY,
};

/** Application RX queue entry */
PACKED_STRUCT(flextcp_pl_arx_t)
{
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint64_t opaque;
      uint32_t rx_bump;
      uint32_t tx_bump;
      uint32_t flags;
    } connupdate;
    uint32_t raw[7];
  } msg;
  uint32_t type;
};

/* Flags */
#define FLEXTCP_PL_ARX_FLRXDONE  0x2


/** App TX queue */
enum {
  FLEXTCP_PL_ATX_INVALID = 0,
  FLEXTCP_PL_ATX_CONNUPDATE,
};

/** Application TX queue entry */
PACKED_STRUCT(flextcp_pl_atx_t)
{
  PACKED_UNION()
  {
    PACKED_STRUCT() {
      uint32_t rx_bump;
      uint32_t tx_bump;
      uint32_t flow_id;
      uint32_t flow_grp;
      uint16_t bump_seq;
      uint8_t flags;
    } connupdate;
    uint32_t raw[7];
  } msg;
  uint32_t type;
};

/* Flags */
#define FLEXTCP_PL_ATX_FLTXDONE       (1 << 0)
#define FLEXTCP_PL_ATX_DUMMY          (1 << 1)

/** Application context queue state */
PACKED_STRUCT(flextcp_pl_appctx_queue_t)
{
  PACKED_UNION()
  {
    PACKED_STRUCT()
    {
      uint32_t base_lo;
      uint32_t len;
      uint32_t p_idx;
      uint32_t c_idx;
    };
    uint32_t __raw[4];
  };
};

/** Application context registers */
PACKED_STRUCT(flextcp_pl_appctx_t)
{
  struct flextcp_pl_appctx_queue_t rx;
  struct flextcp_pl_appctx_queue_t tx;
  uint64_t last_ts;
  uint32_t appst_id;
  uint32_t __pad[5];
};

/** Application state */
PACKED_STRUCT(flextcp_pl_appst_t)
{
  uint16_t ctx_num;                             /*> Number of contexts */
  uint16_t __pad[31];                           /*> Padding */
  uint16_t ctx_ids[FLEXNIC_PL_APPCTX_NUM];      /*> IDs of contexts */
};

/* FIXME: Add STATIC asserts on struct sizes */

#endif /* FP_APPIF_H_ */
