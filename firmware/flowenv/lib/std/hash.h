/*
 * Copyright (C) 2014-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/std/hash.h
 * @brief         Functions to perform a hash over a register region.
 *
 * These functions manually unroll loops depending on the size_t
 * parameter. For good performance and small code size it is important
 * that the size parameter is a compile time constant.
 */

#ifndef _STD__HASH_H_
#define _STD__HASH_H_

#include <nfp.h>
#include <stdint.h>
#include <types.h>

/**
 * Compute the CRC32 over a region located in registers.
 * @param s     Pointer to a region
 * @param n     Size of region (in bytes)
 * @param init  Initial seed value
 * @return      CRC32 checksum
 *
 * @s can be located in GPR or NN register or LM.
 * @n must be a compile time constant and multiple of 4.
 */
__intrinsic uint32_t hash_me_crc32(const void *s, size_t n, uint32_t init);

/**
 * Compute the CRC32-C (iSCSI) over a region located in registers.
 * @param s     Pointer to a region
 * @param n     Size of region (in bytes)
 * @param init  Initial seed value
 * @return      CRC32-C checksum
 *
 * @s can be located in  GPR or NN register or LM.
 * @n must be a compile time constant and multiple of 4.
 */
 __intrinsic uint32_t hash_me_crc32c(const void *s, size_t n, uint32_t init);

/**
 * Initialize the CLS hash mask and configure the CLS hash multiply register.
 * @param mask      Pointer to the mask in CLS memory
 * @param size      Size in bytes of the memory allocated for mask.
 *                  Valid [4-128]
 *
 * This function will initialize the mask to all bits set so that no bits of
 * the data are ignored. It also configures the CLS multiply register to use
 * m63, m53, m36, and m4 in the hash multiplication equation.
 */
void cls_hash_init(__cls void *mask, uint32_t size);

/**
 * Create a 64-bit hash_index over the transfer registers.
 * @param key       Pointer to sufficient write transfer registers for the hash
 * @param mask      Pointer to the start of the mask in CLS
 * @param size      Size of the key/mask, must be a multiple of 4. Valid [4-128]
 * @param idx       CLS hash index. Valid [0-7]
 * @return          64-bit hash of the key
 *
 * This function clears the hash index before creating the hash over the
 * transfer registers. User should call cls_hash_init() prior to this.
 * There are 8 hash indicies, so only 8 contexts can perform a hash at the same
 * time if they use unique indicies. It is the user's responsibility to make
 * sure that only 1 context is hashing each index at a time.
 */
uint64_t cls_hash(__xwrite void *key, __cls void *mask, uint32_t size,
                  uint32_t idx);

/*
 * The Toeplitz hash secret key maximum size is 40 bytes. It is 4
 * bytes longer than the maximum region size to perform the hash over.
 * (e.g. 36 for a IPv6 4-tuple)
 */
#define HASH_TOEPLITZ_SECRET_KEY_SZ    40

/**
 * Compute the Toeplitz hash over a region located in registers.
 * @param s     Pointer to a region
 * @param n     Size of region (in bytes)
 * @param k     Secret hash key (not the input region 's')
 * @param kn    Key size (in bytes)
 * @return      Toeplitz hash
 *
 * @s can be located in GPRs, NN or LMEM.
 * @n must be 8, 12, 32, or 36. This accommodates hash computation
 * over IPv4 only, IPv4+TCP/UDP, IPv6 only and IPv6+TCP/UDP headers
 * respectively.
 */
__intrinsic uint32_t hash_toeplitz(void *s, size_t n, void *k, size_t kn);

#endif /* !_STD__HASH_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
