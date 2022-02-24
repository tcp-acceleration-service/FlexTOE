/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_atomic.h>
#include <std/reg_utils.h>

#include "flow_lookup.h"

__intrinsic int flow_insert(__gpr struct flowht_entry_t* entry)
{
  int added = 0;

  camht_lookup_add(CAMHT_HASH_TBL(flowht), CAMHT_KEY_TBL(flowht),
                  CAMHT_NB_ENTRIES(flowht),
                  (void*) entry, sizeof(struct flow_key_t), sizeof(struct flowht_entry_t),
                  &added);

  if (added)
    return 0;

  return -1;
}

__intrinsic int flow_delete(__gpr struct flow_key_t* key)
{
  __xwrite uint32_t mask;
  int32_t idx;

  idx = camht_lookup(CAMHT_HASH_TBL(flowht), CAMHT_KEY_TBL(flowht),
                CAMHT_NB_ENTRIES(flowht), CAMHT_ENTRY_SZ(flowht),
                (void*) key, sizeof(struct flow_key_t));
  if (idx < 0)
    return -1;

  mask = 0xF;
  mem_bitclr(&mask, (__mem40 void*) &(CAMHT_HASH_TBL(flowht)[idx]),
        (CAMHT_ENTRY_SZ(flowht) >> 2));
  return 0;
}

__intrinsic uint64_t flow_hash(__gpr struct flow_key_t* key)
{
  struct camht_hash_t hash;
  camht_hash((void*) key, sizeof(struct flow_key_t), &hash);

  return hash.__raw;
}

__intrinsic int flow_lookup(__gpr struct flow_key_t* key, uint64_t hash)
{
  __xread struct flowht_entry_t entry;
  __mem40 char* kt;
  struct camht_hash_t cam_hash;
  int32_t idx;

  cam_hash.__raw = hash;
  idx = camht_lookup_idx(CAMHT_HASH_TBL(flowht), CAMHT_NB_ENTRIES(flowht), &cam_hash);
  if (idx < 0)
    goto out;

  kt = (__mem40 char*) CAMHT_KEY_TBL(flowht);
  kt += (idx * sizeof(struct flowht_entry_t));

  mem_read32(&entry, kt, sizeof(struct flowht_entry_t));
  if (!reg_eq(&entry, (void*) key, sizeof(struct flow_key_t)))
    goto out;

  return entry.flow_id;

out:
  return ((uint32_t) -1);
}
