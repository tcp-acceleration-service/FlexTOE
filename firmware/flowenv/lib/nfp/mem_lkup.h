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
 * @file          lib/nfp/mem_lkup.h
 * @brief         NFP Memory Unit Lookup Engine API
 */

#ifndef _NFP__MEM_LKUP_H_
#define _NFP__MEM_LKUP_H_

#include <nfp.h>


/*
 * Example 1: Lookup Engine, CAM 128-bit lookup using a 64B bucket.
 *
 *     1.) Creating the table:
 *
 *         #define NUM_BUCKETS 0x20000
 *         #define TABLE_SZ    (NUM_BUCKETS * 64)
 *
 *         __export __emem __align(TABLE_SZ)                 \
 *             struct mem_lkup_cam128_64B_table_bucket_entry \
 *             lkup_table[NUM_BUCKETS];
 *
 *     2.) Adding an entry to the table:
 *
 *         #define DATA_OFFSET 0
 *
 *         __xwrite uint32_t entry_data_xw[4];
 *         __lmem uint32_t lkup_data[4] =
 *             { 0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff };
 *         unsigned int table_idx =
 *             MEM_LKUP_CAM_64B_BUCKET_IDX(lkup_data, DATA_OFFSET,
 *                                         sizeof(lkup_table));
 *         unsigned int key_shf =
 *             MEM_LKUP_CAM_64B_KEY_OFFSET(DATA_OFFSET, sizeof(lkup_table));
 *
 *         entry_data_xw[0] = ((lkup_data[1] << (32 - key_shf)) |
 *                             (lkup_data[0] >> key_shf));
 *         entry_data_xw[1] = ((lkup_data[2] << (32 - key_shf)) |
 *                            (lkup_data[1] >> key_shf));
 *         entry_data_xw[2] = ((lkup_data[3] << (32 - key_shf)) |
 *                             (lkup_data[2] >> key_shf));
 *         entry_data_xw[3] = lkup_data[3] >> key_shf;
 *
 *         mem_write32(entry_data_xw,
 *                     (__mem40 void *) &(lkup_table[table_idx].lookup_key0),
 *                     sizeof(lkup_table));
 *
 *     3.) Looking up an entry:
 *
 *         __xrw lkup_data_xrw[4];
 *         int entry_found = 0;
 *
 *         reg_cp(lkup_data_xrw, lkup_data, sizeof(lkup_data));
 *         mem_lkup_cam128_64B(lkup_data_xrw, (__mem40 void *) lkup_table,
 *                             DATA_OFFSET, sizeof(lkup_data_xrw),
 *                             sizeof(lkup_table));
 *         if (lkup_data_xrw[0])
 *             entry_found = 1;
 */

/*
 * Example 2: Lookup Engine, CAMR (w/ result) 48-bit lookup using a 64B bucket.
 * This example does a lookup of a 48 bit MAC address and returns a stored
 * (R)esult in read transfer registers.
 *
 *     1.) Creating the table:
 *
 *      #define NUM_BUCKETS  (1 << 10)
 *      #define TABLE_SZ_64    (NUM_BUCKETS * 64) // bucket size is 64 bytes
 *      #define TBL_MEM __imem
 *
 *      __export TBL_MEM __align(TABLE_SZ_64)                 \
 *                struct mem_lkup_cam_r_48_64B_table_bucket_entry \
 *                lkup_table_r_48[NUM_BUCKETS];
 *
 *      struct mac_lkup_key {
 *          union {
 *              struct {
 *                  uint8_t __unused[2];
 *                  uint8_t mac_byte[6];
 *              };
 *              struct {
 *                  uint32_t mac_word[2];
 *              };
 *              uint64_t mac_dword;
 *          };
 *      };
 *
 *      #define MY_MAC 0x112233445566ull
 *      __shared __lmem struct mac_lkup_key lkup_data_48;
 *      lkup_data_48.mac_dword = MY_MAC;
 *
 *     2.) Adding an entry to the table:
 *
 *      #define DATA_OFFSET 0
 *
 *      __gpr uint64_t lkup_data_48_shf;
 *
 *      unsigned int table_idx = lkup_data_48.mac_word[1] \
 *                    & (MEM_LKUP_CAM_64B_NUM_ENTRIES(sizeof(lkup_table_r_48)) - 1);
 *      unsigned long key_shf =
 *          MEM_LKUP_CAM_64B_KEY_OFFSET(DATA_OFFSET, sizeof(lkup_table_r_48));
 *
 *      lkup_data_48_shf = lkup_data_48.mac_dword >> (uint64_t)key_shf;
 *
 *      // You can also experiment by using keys and results 0 - 4
 *      lkup_table_r_48[table_idx].lookup_key_lower5 = (lkup_data_48_shf & 0xffff);
 *      lkup_table_r_48[table_idx].lookup_key_middle5 = (lkup_data_48_shf >> 16ull) & 0xffff;
 *      lkup_table_r_48[table_idx].lookup_key_upper5 = (lkup_data_48_shf >> 32ull) & 0xffff;
 *
 *      lkup_table_r_48[table_idx].result5 = 0xabcdef12;
 *
 *     3.) Looking up an entry:
 *
 *     __xrw uint32_t lkup_data_xrw[2];
 *     __gpr int entry_found = 0;
 *
 *     lkup_data_xrw[0] = lkup_data_48.mac_word[1];
 *     lkup_data_xrw[1] = lkup_data_48.mac_word[0];
 *
 *     mem_lkup_cam_r_48_64B(lkup_data_xrw, (__mem40 void *) lkup_table_r_48,
 *                         DATA_OFFSET, sizeof(lkup_data_xrw),
 *                         sizeof(lkup_table_r_48));
 *
 *     if (lkup_data_xrw[0])
 *         entry_found = 1;
 *
 *     local_csr_write(local_csr_mailbox0, entry_found);
 *     local_csr_write(local_csr_mailbox1, lkup_data_xrw[0]);
 *     //Mailbox 1 should now contain the value 0xabcdef12
 */


/** Macro for converting CAM table data offset into a word offset. */
#define MEM_LKUP_CAM_WORD_OFFSET(_data_offset)    ((_data_offset) >> 5)

/** Macro for converting CAM table size into number of 16B bucket entries. */
#define MEM_LKUP_CAM_16B_NUM_ENTRIES(_table_size) ((_table_size) >> 4)

/** Macro for calculating the index of a CAM table 16B bucket. */
#define MEM_LKUP_CAM_16B_BUCKET_IDX(_data_ptr, _data_offset, _table_size)      \
    (((uint32_t *)(_data_ptr))[MEM_LKUP_CAM_WORD_OFFSET(_data_offset) & 0x3] & \
     (MEM_LKUP_CAM_16B_NUM_ENTRIES(_table_size) - 1))

/** Macro for calculating the key offset for a CAM lookup w/ 16B buckets. */
#define MEM_LKUP_CAM_16B_KEY_OFFSET(_data_offset, _table_size)           \
    ((_data_offset) + __log2(MEM_LKUP_CAM_16B_NUM_ENTRIES(_table_size)))


/** Macro for converting CAM table size into number of 64B bucket entries. */
#define MEM_LKUP_CAM_64B_NUM_ENTRIES(_table_size) ((_table_size) >> 6)

/** Macro for calculating the index of a CAM table 64B bucket. */
#define MEM_LKUP_CAM_64B_BUCKET_IDX(_data_ptr, _data_offset, _table_size)      \
    (((uint32_t *)(_data_ptr))[MEM_LKUP_CAM_WORD_OFFSET(_data_offset) & 0x3] & \
     (MEM_LKUP_CAM_64B_NUM_ENTRIES(_table_size) - 1))

/** Macro for calculating the key offset for a CAM lookup w/ 64B buckets. */
#define MEM_LKUP_CAM_64B_KEY_OFFSET(_data_offset, _table_size)           \
    ((_data_offset) + __log2(MEM_LKUP_CAM_64B_NUM_ENTRIES(_table_size)))


/** Data-structure for a 16B CAM table bucket entry w/ a 32-bit lookup key. */
struct mem_lkup_cam32_16B_table_bucket_entry {
    union {
        struct {
            uint32_t lookup_key0; /**< 32-bit CAM lookup key #0. */
            uint32_t lookup_key1; /**< 32-bit CAM lookup key #1. */
            uint32_t lookup_key2; /**< 32-bit CAM lookup key #2. */
            uint32_t lookup_key3; /**< 32-bit CAM lookup key #3. */
        };
        uint8_t __raw[16]; /**< Raw bucket data. */
    };
};

/** Data-structure for a 64B CAM table bucket entry w/ a 128-bit lookup key. */
struct mem_lkup_cam128_64B_table_bucket_entry {
    union {
        struct {
            uint32_t lookup_key0[4]; /**< 128-bit CAM lookup key #0. */
            uint32_t lookup_key1[4]; /**< 128-bit CAM lookup key #1. */
            uint32_t lookup_key2[4]; /**< 128-bit CAM lookup key #2. */
            uint32_t lookup_key3[4]; /**< 128-bit CAM lookup key #3. */
        };
        uint8_t __raw[64]; /**< Raw bucket data. */
    };
};

/** Data-structure for a 64B CAM table bucket entry w/ a 48-bit lookup key.
 *
 * This is format is dictated by the silicon design.
 * Also see the MU lookup engine section in the NFP-6xxx databook.
 *
 *Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 *-----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *Word  +-------------------------------+-------------------------------+
 *   0  |              key 0 (31-16)    |             key 0 (15-0)      | < dataline 1
 *      +-------------------------------+-------------------------------+
 *   1  |              key 1 (15-0)     |             key 0  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   2  |              key 1 (47-32)    |             key 1  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   3  |                           Reserved                            |
 *      +-------------------------------+-------------------------------+
 *   4  |              key 2 (31-16)    |             key 2 (15-0)      | < dataline 2
 *      +-------------------------------+-------------------------------+
 *   5  |              key 3 (15-0)     |             key 2  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   6  |              key 3 (47-32)    |             key 3  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   7  |                           Reserved                            |
 *      +-------------------------------+-------------------------------+
 *   8  |              key 4 (31-16)    |             key 4 (15-0)      | < dataline 3
 *      +-------------------------------+-------------------------------+
 *   9  |              key 5 (15-0)     |             key 4  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   10 |              key 5 (47-32)    |             key 5  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   11 |                           Reserved                            |
 *      +-------------------------------+-------------------------------+
 *   12 |              key 6 (31-16)    |             key 6 (15-0)      | < dataline 4
 *      +-------------------------------+-------------------------------+
 *   13 |              key 7 (15-0)     |             key 6  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   10 |              key 7 (47-32)    |             key 7  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   15 |                           Reserved                            |
 *      +-------------------------------+-------------------------------+
 */
struct mem_lkup_cam48_64B_table_bucket_entry {
    union {
        struct {
            /* Data line 1*/
            uint16_t lookup_key_middle0;
            uint16_t lookup_key_lower0;
            uint16_t lookup_key_lower1;
            uint16_t lookup_key_upper0;
            uint16_t lookup_key_upper1;
            uint16_t lookup_key_middle1;
            uint32_t reserved0;

            /* Data line 2*/
            uint16_t lookup_key_middle2;
            uint16_t lookup_key_lower2;
            uint16_t lookup_key_lower3;
            uint16_t lookup_key_upper2;
            uint16_t lookup_key_upper3;
            uint16_t lookup_key_middle3;
            uint32_t reserved1;

            /* Data line 3*/
            uint16_t lookup_key_middle4;
            uint16_t lookup_key_lower4;
            uint16_t lookup_key_lower5;
            uint16_t lookup_key_upper4;
            uint16_t lookup_key_upper5;
            uint16_t lookup_key_middle5;
            uint32_t reserved2;

            /* Data line 4*/
            uint16_t lookup_key_middle6;
            uint16_t lookup_key_lower6;
            uint16_t lookup_key_lower7;
            uint16_t lookup_key_upper6;
            uint16_t lookup_key_upper7;
            uint16_t lookup_key_middle7;
            uint32_t reserved3;
        };
        uint8_t __raw[64]; /**< Raw bucket data. */
    };
};

/** Data-structure for the first three datalines in a 64B CAMR table bucket
 * entry w/ a 48-bit lookup key.
 */
struct mem_lkup_cam_r_48_64B_table_bucket_dataline1_3 {
    union {
        struct {
            /* Data lines 1-3*/
            uint16_t lookup_key_middle0;
            uint16_t lookup_key_lower0;
            uint16_t lookup_key_lower1;
            uint16_t lookup_key_upper0;
            uint16_t lookup_key_upper1;
            uint16_t lookup_key_middle1;
            uint32_t result0;
        };
        uint32_t raw[4];
    };
};

/** Data-structure for the fourth dataline in a 64B CAMR table bucket entry
 * w/ a 48-bit lookup key.
 */
struct mem_lkup_cam_r_48_64B_table_bucket_dataline4 {
    union {
        struct {
                /* Data line 4*/
                uint32_t result1;
                uint16_t result3_lower;
                uint16_t reserved0;
                uint16_t reserved1;
                uint16_t result3_upper;
                uint32_t result5;
        };
        uint32_t raw[4];
    };
};

/** Data-structure for a 64B CAMR table bucket entry w/ a 48-bit lookup key.
 *
 * This is composed of three mem_lkup_cam_r_48_64B_table_bucket_dataline1_3
 * data structures, followed by a mem_lkup_cam_r_48_64B_table_bucket_dataline4
 * data structure.
 *
 * This format is dictated by the silicon design. Also see the MU lookup
 * engine section in the NFP-6xxx databook.
 *
 *Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 *-----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *Word  +-------------------------------+-------------------------------+
 *   0  |              key 0 (31-16)    |             key 0 (15-0)      | < dataline 1
 *      +-------------------------------+-------------------------------+
 *   1  |              key 1 (15-0)     |             key 0  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   2  |              key 1 (47-32)    |             key 1  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   3  |                           Result 0                            |
 *      +-------------------------------+-------------------------------+
 *   4  |              key 2 (31-16)    |             key 2 (15-0)      | < dataline 2
 *      +-------------------------------+-------------------------------+
 *   5  |              key 3 (15-0)     |             key 2  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   6  |              key 3 (47-32)    |             key 3  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   7  |                           Result 2                            |
 *      +-------------------------------+-------------------------------+
 *   8  |              key 4 (31-16)    |             key 4 (15-0)      | < dataline 3
 *      +-------------------------------+-------------------------------+
 *   9  |              key 5 (15-0)     |             key 4  (47-32)    |
 *      +-------------------------------+-------------------------------+
 *   10 |              key 5 (47-32)    |             key 5  (31-16)    |
 *      +-------------------------------+-------------------------------+
 *   11 |                           Result 4                            |
 *      +-------------------------------+-------------------------------+
 *   12 |                           Result 1                            | < dataline 4
 *      +-------------------------------+-------------------------------+
 *   13 |              Result 3 (15-0)  |             reserved          |
 *      +-------------------------------+-------------------------------+
 *   14 |              reserved         |             Result 3 (31-16)  |
 *      +-------------------------------+-------------------------------+
 *   15 |                           Result 5                            |
 *      +-------------------------------+-------------------------------+
 */
struct mem_lkup_cam_r_48_64B_table_bucket_entry {
    union {
        struct {
            struct mem_lkup_cam_r_48_64B_table_bucket_dataline1_3 dataline1;
            struct mem_lkup_cam_r_48_64B_table_bucket_dataline1_3 dataline2;
            struct mem_lkup_cam_r_48_64B_table_bucket_dataline1_3 dataline3;
            struct mem_lkup_cam_r_48_64B_table_bucket_dataline4 dataline4;
        };
        uint8_t __raw[64]; /**< Raw bucket data. */
    };
};


/**
 * Method for performing a CAM lookup w/ a 32-bit key, in a 16B bucket entry.
 * @param data         Xfer registers for lookup key and results (8B or 16B).
 * @param addr         Base address of the CAM table (at least 64kB-aligned).
 * @param data_offset  Starting offset for the lookup data (0, 32, 64 or 96).
 * @param data_size    Size of the data used for the lookup key (8B or 16B).
 * @param table_size   Size of the CAM table (power of 2, from 16kB to 2MB).
 * @param sync         Type of synchronization (must be sig_done).
 * @param sig_pair     Signal pair to use for the operation.
 *
 * @note data will be set to the address of the CAM entry or all 0's if a miss.
 *
 * @note addr must be aligned to 64 kB or to table_size, whichever is greater.
 *
 * @note The address of the specific CAM bucket is as follows:
 *           bucket_index    = (data >> data_offset) & (table_size - 1)
 *           bucket_address  = (((addr & 0x1ffff000) << 4) | (bucket_idx << 4))
 *
 * @note The CAM lookup key for this operation is as follows:
 *           cam_key_offset  = data_offset + log2(table_size)
 *           cam_lookup_key  = (data >> cam_key_offset) & ((1 << 32) - 1)
 *
 * @note The lookup key passed in through 'data' is padded with extra 0's, in
 *       the event that the operation requires more bits than is provided.
 */
__intrinsic void __mem_lkup_cam32_16B(__xrw void *data, __mem40 void *addr,
                                      unsigned int data_offset,
                                      size_t data_size, size_t table_size,
                                      sync_t sync, SIGNAL_PAIR *sig_pair);

__intrinsic void mem_lkup_cam32_16B(__xrw void *data, __mem40 void *addr,
                                    unsigned int data_offset,
                                    size_t data_size, size_t table_size);

/**
 * Method for performing a CAM lookup w/ a 128-bit key, in a 64B bucket entry.
 * @param data         Xfer registers for lookup key and results (8B or 16B).
 * @param addr         Base address of the CAM table (at least 64kB-aligned).
 * @param data_offset  Starting offset for the lookup data (0, 32, 64 or 96).
 * @param data_size    Size of the data used for the lookup key (8B or 16B).
 * @param table_size   Size of the CAM table (power of 2, from 64kB to 8MB).
 * @param sync         Type of synchronization (must be sig_done).
 * @param sig_pair     Signal pair to use for the operation.
 *
 * @note data will be set to the address of the CAM entry or all 0's if a miss.
 *
 * @note addr must be aligned to 64 kB or to table_size, whichever is greater.
 *
 * @note The address of the specific CAM bucket is as follows:
 *           bucket_index    = (data >> data_offset) & (table_size - 1)
 *           bucket_address  = (((addr & 0x1ffff000) << 4) | (bucket_idx << 6))
 *
 * @note The CAM lookup key for this operation is as follows:
 *           cam_key_offset  = data_offset + log2(table_size)
 *           cam_lookup_key  = (data >> cam_key_offset) & ((1 << 128) - 1)
 *
 * @note The lookup key passed in through 'data' is padded with extra 0's, in
 *       the event that the operation requires more bits than is provided.
 */
__intrinsic void __mem_lkup_cam128_64B(__xrw void *data, __mem40 void *addr,
                                       unsigned int data_offset,
                                       size_t data_size, size_t table_size,
                                       sync_t sync, SIGNAL_PAIR *sig_pair);

__intrinsic void mem_lkup_cam128_64B(__xrw void *data, __mem40 void *addr,
                                     unsigned int data_offset,
                                     size_t data_size, size_t table_size);

/**
 * Method for performing a CAM lookup w/ a 48-bit key, in a 64B bucket entry.
 *
 * @param data         Xfer registers for lookup key and results (8B or 16B).
 * @param addr         Base address of the CAM table (at least 64kB-aligned).
 * @param data_offset  Starting offset for the lookup data (0, 32, 64 or 96).
 * @param data_size    Size of the data used for the lookup key (8B or 16B).
 * @param table_size   Size of the CAM table (power of 2, from 64kB to 8MB).
 * @param sync         Type of synchronization (must be sig_done).
 * @param sig_pair     Signal pair to use for the operation.
 *
 * @note data will be set to the address of the CAM entry or all 0's if a miss.
 *
 * @note addr must be aligned to 64 kB or to table_size, whichever is greater.
 *
 * @note The address of the specific CAM bucket is as follows:
 *           bucket_index    = (data >> data_offset) & (table_size - 1)
 *           bucket_address  = (((addr & 0x1ffff000) << 4) | (bucket_idx << 6))
 *
 * @note The CAM lookup key for this operation is as follows:
 *           cam_key_offset  = data_offset + log2(table_size)
 *           cam_lookup_key  = (data >> cam_key_offset) & ((1 << 48) - 1)
 *
 * @note The lookup key passed in through 'data' is padded with extra 0's, in
 *       the event that the operation requires more bits than is provided.
 */
__intrinsic void __mem_lkup_cam48_64B(__xrw void *data, __mem40 void *addr,
                      unsigned int data_offset, size_t data_size,
                      size_t table_size, sync_t sync, SIGNAL_PAIR *sig_pair);

__intrinsic void mem_lkup_cam48_64B(__xrw void *data, __mem40 void *addr,
                    unsigned int data_offset, size_t data_size,
                    size_t table_size);

/**
 * Method for performing a CAMR lookup w/ a 48-bit key, in a 64B bucket entry.
 *
 * @param data         Xfer registers for lookup key and results (8B or 16B).
 * @param addr         Base address of the CAM table (at least 64kB-aligned).
 * @param data_offset  Starting offset for the lookup data (0, 32, 64 or 96).
 * @param data_size    Size of the data used for the lookup key (8B or 16B).
 * @param table_size   Size of the CAM table (power of 2, from 64kB to 8MB).
 * @param sync         Type of synchronization (must be sig_done).
 * @param sig_pair     Signal pair to use for the operation.
 *
 * @note data will be set to the address of the CAM entry or all 0's if a miss.
 *
 * @note addr must be aligned to 64 kB or to table_size, whichever is greater.
 *
 * @note The address of the specific CAM bucket is as follows:
 *           bucket_index    = (data >> data_offset) & (table_size - 1)
 *           bucket_address  = (((addr & 0x1ffff000) << 4) | (bucket_idx << 6))
 *
 * @note The CAM lookup key for this operation is as follows:
 *           cam_key_offset  = data_offset + log2(table_size)
 *           cam_lookup_key  = (data >> cam_key_offset) & ((1 << 48) - 1)
 *
 * @note The lookup key passed in through 'data' is padded with extra 0's, in
 *       the event that the operation requires more bits than is provided.
 *
 * @note This is similar mem_lkup_cam48_64B functions above. Instead for a CAM
 *       operation, a CAMR operation is performed and the result it returned
 *       in the "data" read transfer registers.
 */
__intrinsic void __mem_lkup_cam_r_48_64B(__xrw void *data, __mem40 void *addr,
                      unsigned int data_offset, size_t data_size,
                      size_t table_size, sync_t sync, SIGNAL_PAIR *sig_pair);

__intrinsic void mem_lkup_cam_r_48_64B(__xrw void *data, __mem40 void *addr,
                    unsigned int data_offset, size_t data_size,
                    size_t table_size);


#endif /* !_NFP__MEM_LKUP_H_ */
