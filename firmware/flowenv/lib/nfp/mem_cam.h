/*
 * Copyright 2014-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/mem_cam.h
 * @brief         NFP memory CAM interface
 */
#ifndef _NFP__MEM_CAM_H_
#define _NFP__MEM_CAM_H_

#include <nfp.h>

/*
 * NOTE: The 'search.value' fields in the below structures are all
 * declared as full word values rather than bitfields even if the
 * whole word is not use to hold the value.  This is because the
 * 'search' fields are used for input data to the cam functions and
 * typically reside in write transfer registers.  By not using
 * bitfields in the structure definition we can ensure that assignment
 * to the 'search.value' field is not treated as a partial register
 * write (which is not possible or legal for write transfer
 * registers).
 */

/**
 * CAM search and result data for 8bit CAM lookups.
 */
struct mem_cam_8bit {
    union {
        struct {
            unsigned int value; /* Lower 8 bits valid */
            unsigned int __pad;
        } search;

        union {
            struct {
                unsigned int mask_lo;
                unsigned int mask_hi;
            };
            unsigned long long mask;
        } result;
    };
};

/**
 * CAM search and result data for 16bit CAM lookups.
 */
struct mem_cam_16bit {
    union {
        struct {
            unsigned int value; /* Lower 16 bits valid */
            unsigned int __pad;
        } search;

        struct {
            unsigned int __pad:24;
            unsigned int match:8;
            unsigned int mask;
        } result;
    };
};

/**
 * CAM search and result data for 24bit CAM lookups.
 */
struct mem_cam_24bit {
    union {
        struct {
            unsigned int value; /* Lower 24 bits valid */
        } search;

        struct {
            unsigned int mask:16;
            unsigned int data:8;
            unsigned int match:8;
        } result;
    };
};

/**
 * CAM search and result data for 32bit CAM lookups.
 */
struct mem_cam_32bit {
    union {
        struct {
            unsigned int value;
        } search;

        struct {
            unsigned int mask:16;
            unsigned int data:8;
            unsigned int match:8;
        } result;
    };
};

/**
 * Check if CAM lookup resulted in a hit.
 * @param data          CAM result data
 *
 * Return true if there was a match.  Note that function is only valid
 * for 16bit, 24bit, and 32bit lookups.  For 8bit lookups the match
 * bitmask in the result must be consulted.
 */
#define mem_cam_lookup_hit(data) ((data).result.match != 0xff)

#define mem_cam_lookup8_hit(data) \
    (!(((data).result.mask_lo == 0) && ((data).result.mask_hi == 0)))

/**
 * Check if CAM lookup add resulted in a new entry add.
 * @param data          CAM result data
 *
 * On a cam lookup and add command if no match was found and there is space
 * for the new entry, the new entry will be inserted.
 * Bit 7 of the "match" field indicates an add(1) or hit(0) - assuming the
 * entire match field is not 0xff (which indicates both a miss and failure
 * to add due to a full CAM).
 * For more details see DB section "9.2.2.1.2.7 CAM and TCAM Lookup Commands"
 */
#define mem_cam_lookup_add_added(data) (((data).result.match & 0x80) != 0)
#define mem_cam_lookup_add_fail(data) ((data).result.match == 0xff)

/**
 * Perform CAM lookup.
 * @param data          Search/result data
 * @param addr          CAM location
 * @param camsize       Table size of CAM (128, 256, 384, or 512)
 * @param cambits       Entry size of CAM (8, 16, 24, or 32)
 * @param sync          Type of synchronization (must be sig_done)
 * @param sigpair       Signal pair to report completion on
 *
 * Perform indicated CAM lookup.  The @camsize and @cambits parameters
 * must be constant literals.
 */
__intrinsic void __mem_cam_lookup(__xrw void *data, __mem40 void *addr,
                                  int camsize, int cambits,
                                  sync_t sync, SIGNAL_PAIR *sigpair);

/**
 * Perform synchronous CAM lookup.
 * @param data          Search/result data
 * @param addr          CAM location
 * @param camsize       Table size of CAM (128, 256, 384, or 512)
 * @param cambits       Entry size of CAM (8, 16, 24, or 32)
 *
 * Perform indicated CAM lookup.  Return when operation completes.
 */
__intrinsic void mem_cam_lookup(__xrw void *data, __mem40 void *addr,
                                int camsize, int cambits);

/**
 * Perform CAM lookup add.
 * @param data          Search/result data
 * @param addr          CAM location
 * @param camsize       Table size of CAM (128, 256, 384, or 512)
 * @param cambits       Entry size of CAM (8, 16, 24, or 32)
 * @param sync          Type of synchronization (must be sig_done)
 * @param sigpair       Signal pair to report completion on
 *
 * Perform indicated CAM lookup add.  The @camsize and @cambits parameters
 * must be constant literals.
 */
__intrinsic void __mem_cam_lookup_add(__xrw void *data, __mem40 void *addr,
                                      int camsize, int cambits,
                                      sync_t sync, SIGNAL_PAIR *sigpair);

/**
 * Perform synchronous CAM lookup add.
 * @param data          Search/result data
 * @param addr          CAM location
 * @param camsize       Table size of CAM (128, 256, 384, or 512)
 * @param cambits       Entry size of CAM (8, 16, 24, or 32)
 *
 * Perform indicated CAM lookup add.  Return when operation completes.
 */
__intrinsic void mem_cam_lookup_add(__xrw void *data, __mem40 void *addr,
                                    int camsize, int cambits);

/*
 * Convenience wrappers around the above generic CAM functions.  These
 * wrappers also ensure that the appropriate search and result data
 * parameters are used.
 */

__intrinsic void __mem_cam128_lookup8(__xrw struct mem_cam_8bit *data,
                                      __mem40 __align16 void *addr,
                                      sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup8(__xrw struct mem_cam_8bit *data,
                                    __mem40 __align32 void *addr);

__intrinsic void __mem_cam128_lookup16(__xrw struct mem_cam_16bit *data,
                                       __mem40 __align16 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup16(__xrw struct mem_cam_16bit *data,
                                     __mem40 __align16 void *addr);

__intrinsic void __mem_cam128_lookup24(__xrw struct mem_cam_24bit *data,
                                       __mem40 __align16 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup24(__xrw struct mem_cam_24bit *data,
                                     __mem40 __align16 void *addr);

__intrinsic void __mem_cam128_lookup32(__xrw struct mem_cam_32bit *data,
                                       __mem40 __align16 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup32(__xrw struct mem_cam_32bit *data,
                                     __mem40 __align16 void *addr);

__intrinsic void __mem_cam256_lookup8(__xrw struct mem_cam_8bit *data,
                                      __mem40 __align32 void *addr,
                                      sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup8(__xrw struct mem_cam_8bit *data,
                                    __mem40 __align32 void *addr);

__intrinsic void __mem_cam256_lookup16(__xrw struct mem_cam_16bit *data,
                                       __mem40 __align32 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup16(__xrw struct mem_cam_16bit *data,
                                     __mem40 __align32 void *addr);

__intrinsic void __mem_cam256_lookup24(__xrw struct mem_cam_24bit *data,
                                       __mem40 __align32 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup24(__xrw struct mem_cam_24bit *data,
                                     __mem40 __align32 void *addr);

__intrinsic void __mem_cam256_lookup32(__xrw struct mem_cam_32bit *data,
                                       __mem40 __align32 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup32(__xrw struct mem_cam_32bit *data,
                                     __mem40 __align32 void *addr);

__intrinsic void __mem_cam384_lookup8(__xrw struct mem_cam_8bit *data,
                                      __mem40 __align64 void *addr,
                                      sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup8(__xrw struct mem_cam_8bit *data,
                                    __mem40 __align64 void *addr);

__intrinsic void __mem_cam384_lookup16(__xrw struct mem_cam_16bit *data,
                                       __mem40 __align64 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup16(__xrw struct mem_cam_16bit *data,
                                     __mem40 __align64 void *addr);

__intrinsic void __mem_cam384_lookup24(__xrw struct mem_cam_24bit *data,
                                       __mem40 __align64 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup24(__xrw struct mem_cam_24bit *data,
                                     __mem40 __align64 void *addr);

__intrinsic void __mem_cam384_lookup32(__xrw struct mem_cam_32bit *data,
                                       __mem40 __align64 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup32(__xrw struct mem_cam_32bit *data,
                                     __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup8(__xrw struct mem_cam_8bit *data,
                                      __mem40 __align64 void *addr,
                                      sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup8(__xrw struct mem_cam_8bit *data,
                                    __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup16(__xrw struct mem_cam_16bit *data,
                                       __mem40 __align64 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup16(__xrw struct mem_cam_16bit *data,
                                     __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup24(__xrw struct mem_cam_24bit *data,
                                       __mem40 __align64 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup24(__xrw struct mem_cam_24bit *data,
                                     __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup32(__xrw struct mem_cam_32bit *data,
                                       __mem40 __align64 void *addr,
                                       sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup32(__xrw struct mem_cam_32bit *data,
                                     __mem40 __align64 void *addr);

/* Lookup and add */
__intrinsic void __mem_cam128_lookup8_add(__xrw struct mem_cam_8bit *data,
                                          __mem40 __align16 void *addr,
                                          sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup8_add(__xrw struct mem_cam_8bit *data,
                                        __mem40 __align32 void *addr);

__intrinsic void __mem_cam128_lookup16_add(__xrw struct mem_cam_16bit *data,
                                           __mem40 __align16 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup16_add(__xrw struct mem_cam_16bit *data,
                                         __mem40 __align16 void *addr);

__intrinsic void __mem_cam128_lookup24_add(__xrw struct mem_cam_24bit *data,
                                           __mem40 __align16 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup24_add(__xrw struct mem_cam_24bit *data,
                                         __mem40 __align16 void *addr);

__intrinsic void __mem_cam128_lookup32_add(__xrw struct mem_cam_32bit *data,
                                           __mem40 __align16 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam128_lookup32_add(__xrw struct mem_cam_32bit *data,
                                         __mem40 __align16 void *addr);

__intrinsic void __mem_cam256_lookup8_add(__xrw struct mem_cam_8bit *data,
                                          __mem40 __align32 void *addr,
                                          sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup8_add(__xrw struct mem_cam_8bit *data,
                                        __mem40 __align32 void *addr);

__intrinsic void __mem_cam256_lookup16_add(__xrw struct mem_cam_16bit *data,
                                           __mem40 __align32 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup16_add(__xrw struct mem_cam_16bit *data,
                                         __mem40 __align32 void *addr);

__intrinsic void __mem_cam256_lookup24_add(__xrw struct mem_cam_24bit *data,
                                           __mem40 __align32 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup24_add(__xrw struct mem_cam_24bit *data,
                                         __mem40 __align32 void *addr);

__intrinsic void __mem_cam256_lookup32_add(__xrw struct mem_cam_32bit *data,
                                           __mem40 __align32 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam256_lookup32_add(__xrw struct mem_cam_32bit *data,
                                         __mem40 __align32 void *addr);

__intrinsic void __mem_cam384_lookup8_add(__xrw struct mem_cam_8bit *data,
                                          __mem40 __align64 void *addr,
                                          sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup8_add(__xrw struct mem_cam_8bit *data,
                                        __mem40 __align64 void *addr);

__intrinsic void __mem_cam384_lookup16_add(__xrw struct mem_cam_16bit *data,
                                           __mem40 __align64 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup16_add(__xrw struct mem_cam_16bit *data,
                                         __mem40 __align64 void *addr);

__intrinsic void __mem_cam384_lookup24_add(__xrw struct mem_cam_24bit *data,
                                           __mem40 __align64 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup24_add(__xrw struct mem_cam_24bit *data,
                                         __mem40 __align64 void *addr);

__intrinsic void __mem_cam384_lookup32_add(__xrw struct mem_cam_32bit *data,
                                           __mem40 __align64 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam384_lookup32_add(__xrw struct mem_cam_32bit *data,
                                         __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup8_add(__xrw struct mem_cam_8bit *data,
                                          __mem40 __align64 void *addr,
                                          sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup8_add(__xrw struct mem_cam_8bit *data,
                                        __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup16_add(__xrw struct mem_cam_16bit *data,
                                           __mem40 __align64 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup16_add(__xrw struct mem_cam_16bit *data,
                                         __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup24_add(__xrw struct mem_cam_24bit *data,
                                           __mem40 __align64 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup24_add(__xrw struct mem_cam_24bit *data,
                                         __mem40 __align64 void *addr);

__intrinsic void __mem_cam512_lookup32_add(__xrw struct mem_cam_32bit *data,
                                           __mem40 __align64 void *addr,
                                           sync_t sync, SIGNAL_PAIR *sigpair);

__intrinsic void mem_cam512_lookup32_add(__xrw struct mem_cam_32bit *data,
                                         __mem40 __align64 void *addr);

#endif /* !_NFP__MEM_CAM_H_ */
