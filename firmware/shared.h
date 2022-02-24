/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef SHARED_H_
#define SHARED_H_

#include <nfp.h>
#include <nfp/me.h>

/**
 * Island Local CLS Rings
 */
#define POSTPROC_RING         0

__export __shared __cls __align(FLOW_POSTPROC_RING_SIZE) uint8_t postproc_ring[FLOW_POSTPROC_RING_SIZE];

/* Call from a single thread only! */
__intrinsic void enable_global_timestamp()
{
#define OVL_MISC_XPB_DEVICE_ID 4
#define OVL_TIMER_PA_CFG_OFFSET 0x5000
#define TIMER_ENABLE  ((4<<2) | 1)
#define TIMER_RESTART ((4<<2) | (1<<1) | 1)

  __xwrite unsigned int data;
  unsigned int isl, addr, offset;
  SIGNAL sig;

  __asm {
    local_csr_wr[local_csr_timestamp_low,  0];
    local_csr_wr[local_csr_timestamp_high, 0];
  }

  // 30: Global bit
  // 29-24: Island number
  // 21-16: Target device ID
  addr = (1 << 30) | ((__ISLAND & 0x3F) << 24) | (OVL_MISC_XPB_DEVICE_ID << 16);
  offset = OVL_TIMER_PA_CFG_OFFSET;

  data = TIMER_RESTART;
  __asm {
    ct[xpb_write, data, addr, offset, 1], ctx_swap[sig];
  }

  data = TIMER_ENABLE;
  __asm {
    ct[xpb_write, data, addr, offset, 1], ctx_swap[sig];
  }

#undef OVL_MISC_XPB_DEVICE_ID
#undef OVL_TIMER_PA_CFG_OFFSET
#undef TIMER_ENABLE
#undef TIMER_RESTART
}

#endif /* SHARED_H_ */
