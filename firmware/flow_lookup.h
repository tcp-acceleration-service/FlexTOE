/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLOW_LOOKUP_H_
#define FLOW_LOOKUP_H_

#include <stdint.h>
#include <nfp.h>
#include <lu/cam_hash.h>

#include "fp_mem.h"

struct flow_key_t {
  union {
    __packed struct {
      uint32_t remote_ip;
      uint32_t local_ip;
      uint16_t remote_port;
      uint16_t local_port;
    };
    uint32_t __raw[3];
  };
};

struct flowht_entry_t {
  struct flow_key_t   key;
  uint32_t          flow_id;
};

CAMHT_DECLARE(flowht, __imem, FLEXNIC_PL_FLOWHT_ENTRIES, struct flowht_entry_t);

#define INVALID_FLOWID  ((uint32_t) -1)

__intrinsic int      flow_insert(__gpr struct flowht_entry_t* entry);
__intrinsic int      flow_delete(__gpr struct flow_key_t* key);
__intrinsic uint64_t flow_hash(__gpr struct flow_key_t* key);
__intrinsic int      flow_lookup(__gpr struct flow_key_t* key, uint64_t hash);

#endif /* FLOW_LOOKUP_H_ */
