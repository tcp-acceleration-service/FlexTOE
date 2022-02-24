/*
 * Copyright (C) 2017-2018,  Netronome Systems, Inc.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          lib/nfp/_c/mem_lkup.c
 * @brief         NFP Memory Unit Lookup Engine API
 */

/*
 * This library provides an API to utilize features in the NFP Memory Unit
 * Lookup Engine.  This library encompasses three main functionalities provided
 * by the Lookup Engine (denoted by the prefix "mem_lkup_"):
 *   1. Direct Table lookups ("mem_lkup_direct_")
 *   2. Algorithmic lookups  ("mem_lkup_algo_")
 *   3. Hash lookups         ("mem_lkup_hash_")
 *
 * DISCLAIMER: This library uses the top locality bit to select whether to use
 * the internal or external lookup engine. Each MU has both engines but with
 * IMEM only the internal engine is accessible from the ME's.
 * This code was tested with high locality and direct access pointers only.
 * It should not be used with low locality or discard after read (DAR) without
 * further testing.
 */

#include <assert.h>

#include <nfp/mem_lkup.h>

/* *** Lookup Engine Hash Lookup Data-Structures and Macros. *** */

/** CAM/linear hash base address field size (in bits). */
#define HASH_BASE_ADDR_FIELD_SZ     17

/** Mask for CAM/linear hash base address field. */
#define HASH_BASE_ADDR_FIELD_MASK  ((1 << HASH_BASE_ADDR_FIELD_SZ) - 1)

/** Minimum alignment boundary for CAM/linear hash tables. */
#define HASH_BASE_ADDR_MIN_ALIGN   0x10000

/** Maximum number of buckets for CAM/linear hash tables. */
#define HASH_MAX_NUM_BUCKETS       (1 << 17)

/** Minimum number of buckets for CAM/linear hash tables. */
#define HASH_MIN_NUM_BUCKETS       (1 << 10)

/** Maximum table size (in bytes) for CAM/linear hash tables w/ 16B buckets. */
#define HASH_16B_TABLE_SIZE_MAX    (HASH_MAX_NUM_BUCKETS << 4)

/** Minimum table size (in bytes) for CAM/linear hash tables w/ 16B buckets. */
#define HASH_16B_TABLE_SIZE_MIN    (HASH_MIN_NUM_BUCKETS << 4)

/** Maximum table size (in bytes) for CAM/linear hash tables w/ 64B buckets. */
#define HASH_64B_TABLE_SIZE_MAX    (HASH_MAX_NUM_BUCKETS << 6)

/** Minimum table size (in bytes) for CAM/linear hash tables w/ 64B buckets. */
#define HASH_64B_TABLE_SIZE_MIN    (HASH_MIN_NUM_BUCKETS << 6)

/** Enumeration of Lookup Engine hash/CAM opcodes. */
enum hash_lkup_opcode {
    HASH_OP_CAMR32_16B  = 0x0,  /**< CAM w/ result, 32b key, 16B bucket. */
    HASH_OP_CAMR32_64B  = 0x1,  /**< CAM w/ result, 32b key, 64B bucket. */
    HASH_OP_CAMR48_64B  = 0x5,  /**< CAM w/ result, 48b key, 64B bucket. */
    HASH_OP_CAMR64_16B  = 0x8,  /**< CAM w/ result, 64b key, 16B bucket. */
    HASH_OP_CAMR64_64B  = 0x9,  /**< CAM w/ result, 64b key, 64B bucket. */
    HASH_OP_CAM32_16B   = 0x10, /**< CAM, 32b key, 16B bucket. */
    HASH_OP_CAM32_64B   = 0x11, /**< CAM, 32b key, 64B bucket. */
    HASH_OP_CAM48_16B   = 0x14, /**< CAM, 48b key, 16B bucket. */
    HASH_OP_CAM48_64B   = 0x15, /**< CAM, 48b key, 64B bucket. */
    HASH_OP_CAM64_16B   = 0x18, /**< CAM, 64b key, 16B bucket. */
    HASH_OP_CAM64_64B   = 0x19, /**< CAM, 64b key, 64B bucket. */
    HASH_OP_CAM128_16B  = 0x1c, /**< CAM, 128b key, 16B bucket. */
    HASH_OP_CAM128_64B  = 0x1d, /**< CAM, 128b key, 64B bucket. */
};

/** Macro for determining whether the table is using 64B buckets. */
#define HASH_IS_64B_TABLE(_opcode) ((_opcode) & 0x1)

/** Structure for the input CPP address for the Lookup Engine CAM operation. */
struct hash_lkup_addr {
    union {
        struct {
            unsigned int access_mode     : 2;  /**< Direct access field. */
            unsigned int __rsvd          : 30; /**< Reserved. */
            /**< 1 = Int LU Engine, 0 = Ext LU Engine*/
            unsigned int use_internal_le : 1;
            unsigned int direct_lkup     : 1;  /**< Direct lookup (set to 0). */
            unsigned int hash_lkup       : 1;  /**< Hash lookup (set to 1). */
            unsigned int base_addr       : 17; /**< Table base address. */
            unsigned int __unused1       : 1;  /**< Unused: Don't care. */
            unsigned int table_size      : 3;  /**< Table size. */
            unsigned int hash_opcode     : 6;  /**< Hash command opcode. */
            unsigned int start_pos       : 2;  /**< Starting bit position. */
        };
        struct {
            uint32_t addr_hi; /**< Upper 8 bits of the input CPP address. */
            uint32_t addr_lo; /**< Lower 32 bits of the input CPP address. */
        };
        uint32_t __raw[2];    /**< Raw value. */
    };
};


/* ------------------------------------------------------------------------- */


/* *** Hash Lookup Helper Methods. *** */

/** Helper method to encode the address for the hash lookup operation. */
static __intrinsic void
hash_init_lkup_addr(__gpr struct hash_lkup_addr *lkup_addr, __mem40 void *addr,
                    enum hash_lkup_opcode opcode, unsigned int data_offset,
                    size_t table_size)
{
    unsigned int table_sz_enc;
    unsigned int start_pos = (data_offset >> 5) & 0x3;

    /* Check the parameters. */
    try_ctassert(__is_aligned(addr, HASH_BASE_ADDR_MIN_ALIGN));
    try_ctassert(__is_aligned(data_offset, 32) && (data_offset < 128));
    try_ctassert(__is_log2(table_size));
    try_ctassert(__is_aligned(addr, table_size));
    try_ctassert(HASH_IS_64B_TABLE(opcode) ?
                 ((table_size >= HASH_64B_TABLE_SIZE_MIN) &&
                  (table_size <= HASH_64B_TABLE_SIZE_MAX)) :
                 ((table_size >= HASH_16B_TABLE_SIZE_MIN) &&
                  (table_size <= HASH_16B_TABLE_SIZE_MAX)));

    table_sz_enc = (__log2(table_size) -
                    (HASH_IS_64B_TABLE(opcode) ? 16 : 14)) & 0x7;

    /* Encode the CPP address for the Lookup Engine operation. */
    lkup_addr->addr_hi          = ((uint64_t)addr >> 8) & 0xff000000;
    lkup_addr->addr_lo          = 0;
    lkup_addr->base_addr        = (((uint64_t)addr >> 16) &
                              HASH_BASE_ADDR_FIELD_MASK);
    /* Select internal lookup engine for imem and emem_cache_upper. Based on top
    * locality bit*/
    lkup_addr->use_internal_le  = ((uint64_t)addr >> 39) & 0x1;
    lkup_addr->direct_lkup      = 0;
    lkup_addr->hash_lkup        = 1;
    lkup_addr->hash_opcode      = opcode;
    lkup_addr->start_pos        = start_pos;
    lkup_addr->table_size       = table_sz_enc;
}

/** Generic helper method to issue a hash lookup operation. */
static __intrinsic void
hash_generic_lkup(__xrw void *data, __mem40 void *addr,
                  enum hash_lkup_opcode opcode, unsigned int data_offset,
                  size_t data_size, size_t table_size, sync_t sync,
                  SIGNAL_PAIR *sig_pair)
{
    __gpr struct hash_lkup_addr lkup_addr;
    unsigned int cnt = (data_size <= 8) ? 1 : 2;

    try_ctassert((data_size == 8) || (data_size == 16));
    ctassert(sync == sig_done);

    hash_init_lkup_addr(&lkup_addr, addr, opcode, data_offset, table_size);
    __asm mem[lookup, *data, lkup_addr.addr_hi, <<8, lkup_addr.addr_lo, cnt], \
              sig_done[*sig_pair];
}


/* ------------------------------------------------------------------------- */


/* *** Lookup Engine Hash Lookup API Methods. *** */

__intrinsic void
__mem_lkup_cam32_16B(__xrw void *data, __mem40 void *addr,
                     unsigned int data_offset, size_t data_size,
                     size_t table_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    hash_generic_lkup(data, addr, HASH_OP_CAM32_16B, data_offset, data_size,
                      table_size, sync, sig_pair);
}

__intrinsic void
mem_lkup_cam32_16B(__xrw void *data, __mem40 void *addr,
                   unsigned int data_offset, size_t data_size,
                   size_t table_size)
{
    SIGNAL_PAIR sig_pair;

    __mem_lkup_cam32_16B(data, addr, data_offset, data_size, table_size,
                         sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_lkup_cam128_64B(__xrw void *data, __mem40 void *addr,
                      unsigned int data_offset, size_t data_size,
                      size_t table_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    hash_generic_lkup(data, addr, HASH_OP_CAM128_64B, data_offset, data_size,
                      table_size, sync, sig_pair);
}

__intrinsic void
mem_lkup_cam128_64B(__xrw void *data, __mem40 void *addr,
                    unsigned int data_offset, size_t data_size,
                    size_t table_size)
{
    SIGNAL_PAIR sig_pair;

    __mem_lkup_cam128_64B(data, addr, data_offset, data_size, table_size,
                          sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_lkup_cam48_64B(__xrw void *data, __mem40 void *addr,
                      unsigned int data_offset, size_t data_size,
                      size_t table_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    hash_generic_lkup(data, addr, HASH_OP_CAM48_64B, data_offset, data_size,
                      table_size, sync, sig_pair);
}

__intrinsic void
mem_lkup_cam48_64B(__xrw void *data, __mem40 void *addr,
                    unsigned int data_offset, size_t data_size,
                    size_t table_size)
{
    SIGNAL_PAIR sig_pair;

    __mem_lkup_cam48_64B(data, addr, data_offset, data_size, table_size,
                          sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_lkup_cam_r_48_64B(__xrw void *data, __mem40 void *addr,
                      unsigned int data_offset, size_t data_size,
                      size_t table_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    hash_generic_lkup(data, addr, HASH_OP_CAMR48_64B, data_offset, data_size,
                      table_size, sync, sig_pair);
}

__intrinsic void
mem_lkup_cam_r_48_64B(__xrw void *data, __mem40 void *addr,
                    unsigned int data_offset, size_t data_size,
                    size_t table_size)
{
    SIGNAL_PAIR sig_pair;

    __mem_lkup_cam_r_48_64B(data, addr, data_offset, data_size, table_size,
                          sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

