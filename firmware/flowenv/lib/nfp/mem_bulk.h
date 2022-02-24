/*
 * Copyright (C) 2012-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/mem_bulk.h
 * @brief         NFP memory bulk interface
 */
#ifndef _NFP__MEM_BULK_H_
#define _NFP__MEM_BULK_H_

#include <nfp.h>
#include <types.h>

/**
 * Read a multiple of 8B (read64), 4B (read32), 1B (read8) from a memory unit.
 * @param data      Pointer to sufficient number of read transfer registers
 * @param addr      40-bit pointer to the memory start address
 * @param size      Size of the read, must be a multiple of the base size
 * @param max_size  Used to determine largest read, if size is not a constant
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note write64 size max = 128
 * @note write32 size max = 128
 * @note write8  size max = 32
 * @note read8 returns random data in the upper bytes of a read; this must be
 * masked out if not required. For this reason, there is no read8_le version.
 *
 * These functions provide basic bulk reads from NFP memory units
 * (EMEM, IMEM, or CTM).  The functions prefixed with '__' may take
 * run-time determined sizes while the simplified versions must use
 * compile time constants as the size argument.
 *
 * Addresses may be arbitrarily aligned.  For non-32bit-aligned
 * addresses big endian byte addressing is assumed, except for the
 * 'le' versions, which use little endian byte addressing.
 *
 * The 'swap' versions perform byte-swapping on the data read.
 * The 'swap_le' versions use little endian byte addressing and perform
 * byte-swapping on the data read.
 */
__intrinsic void __mem_read64(__xread void *data, __mem40 void *addr,
                              size_t size, const size_t max_size,
                              sync_t sync, SIGNAL *sig);

__intrinsic void mem_read64(__xread void *data, __mem40 void *addr,
                            const size_t size);

__intrinsic void __mem_read64_le(__xread void *data, __mem40 void *addr,
                                 size_t size, const size_t max_size,
                                 sync_t sync, SIGNAL *sig);

__intrinsic void mem_read64_le(__xread void *data, __mem40 void *addr,
                               const size_t size);

__intrinsic void __mem_read64_swap(__xread void *data, __mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_read64_swap(__xread void *data, __mem40 void *addr,
                                 const size_t size);

__intrinsic void __mem_read64_swap_le(__xread void *data, __mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_read64_swap_le(__xread void *data, __mem40 void *addr,
                                 const size_t size);


__intrinsic void __mem_read32(__xread void *data, __mem40 void *addr,
                              size_t size, const size_t max_size,
                              sync_t sync, SIGNAL *sig);

__intrinsic void mem_read32(__xread void *data, __mem40 void *addr,
                            const size_t size);

__intrinsic void __mem_read32_le(__xread void *data, __mem40 void *addr,
                                 size_t size, const size_t max_size,
                                 sync_t sync, SIGNAL *sig);

__intrinsic void mem_read32_le(__xread void *data, __mem40 void *addr,
                               const size_t size);

__intrinsic void __mem_read32_swap(__xread void *data, __mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_read32_swap(__xread void *data, __mem40 void *addr,
                                 const size_t size);

__intrinsic void __mem_read32_swap_le(__xread void *data, __mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_read32_swap_le(__xread void *data, __mem40 void *addr,
                                 const size_t size);


__intrinsic void __mem_read8(__xread void *data, __mem40 void *addr,
                             size_t size, const size_t max_size,
                             sync_t sync, SIGNAL *sig);

__intrinsic void mem_read8(__xread void *data, __mem40 void *addr,
                           const size_t size);


/**
 * Write a multiple of 8B (write64), 4B (write32), 1B (write8) to a memory unit.
 * @param data      Pointer to sufficient number of write transfer registers
 * @param addr      40-bit pointer to the memory start address
 * @param size      Size of the write, must be a multiple of the base unit
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note write64 size max = 128
 * @note write32 size max = 128
 * @note write8  size max = 32
 *
 * These functions provide basic bulk writes to the NFP memory units
 * (EMEM, IMEM, or CTM).  The functions prefixed with '__' may take
 * run-time determined sizes while the simplified versions must use
 * compile time constants as the size argument.
 *
 * Addresses may be arbitrarily aligned.  For non-32bit-aligned
 * addresses big endian byte addressing is assumed, except for the
 * 'le' versions, which use little endian byte addressing.
 *
 * The 'swap' version perform byte swapping on the data written.
 * The 'swap_le' versions use little endian byte addressing and perform
 * byte-swapping on the data written.
 */
__intrinsic void __mem_write64(__xwrite void *data, __mem40 void *addr,
                               size_t size, const size_t max_size,
                               sync_t sync, SIGNAL *sig);

__intrinsic void mem_write64(__xwrite void *data, __mem40 void *addr,
                             const size_t size);

__intrinsic void __mem_write64_le(__xwrite void *data, __mem40 void *addr,
                                  size_t size, const size_t max_size,
                                  sync_t sync, SIGNAL *sig);

__intrinsic void mem_write64_le(__xwrite void *data, __mem40 void *addr,
                                const size_t size);

__intrinsic void __mem_write64_swap(__xwrite void *data, __mem40 void *addr,
                                    size_t size, const size_t max_size,
                                    sync_t sync, SIGNAL *sig);

__intrinsic void mem_write64_swap(__xwrite void *data, __mem40 void *addr,
                                  const size_t size);

__intrinsic void __mem_write64_swap_le(__xwrite void *data, __mem40 void *addr,
                                    size_t size, const size_t max_size,
                                    sync_t sync, SIGNAL *sig);

__intrinsic void mem_write64_swap_le(__xwrite void *data, __mem40 void *addr,
                                  const size_t size);


__intrinsic void __mem_write32(__xwrite void *data, __mem40 void *addr,
                               size_t size, const size_t max_size,
                               sync_t sync, SIGNAL *sig);

__intrinsic void mem_write32(__xwrite void *data, __mem40 void *addr,
                             const size_t size);

__intrinsic void __mem_write32_le(__xwrite void *data, __mem40 void *addr,
                                  size_t size, const size_t max_size,
                                  sync_t sync, SIGNAL *sig);

__intrinsic void mem_write32_le(__xwrite void *data, __mem40 void *addr,
                                const size_t size);

__intrinsic void __mem_write32_swap(__xwrite void *data, __mem40 void *addr,
                                    size_t size, const size_t max_size,
                                    sync_t sync, SIGNAL *sig);

__intrinsic void mem_write32_swap(__xwrite void *data, __mem40 void *addr,
                                  const size_t size);

__intrinsic void __mem_write32_swap_le(__xwrite void *data, __mem40 void *addr,
                                    size_t size, const size_t max_size,
                                    sync_t sync, SIGNAL *sig);

__intrinsic void mem_write32_swap_le(__xwrite void *data, __mem40 void *addr,
                                  const size_t size);


__intrinsic void __mem_write8(__xwrite void *data, __mem40 void *addr,
                              size_t size, const size_t max_size,
                              sync_t sync, SIGNAL *sig);

__intrinsic void mem_write8(__xwrite void *data, __mem40 void *addr,
                            const size_t size);

__intrinsic void __mem_write8_le(__xwrite void *data, __mem40 void *addr,
                                 size_t size, const size_t max_size,
                                 sync_t sync, SIGNAL *sig);

__intrinsic void mem_write8_le(__xwrite void *data, __mem40 void *addr,
                               const size_t size);

__intrinsic void __mem_write8_swap(__xwrite void *data, __mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_write8_swap(__xwrite void *data, __mem40 void *addr,
                                 const size_t size);

__intrinsic void __mem_write8_swap_le(__xwrite void *data, __mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_write8_swap_le(__xwrite void *data, __mem40 void *addr,
                                 const size_t size);

#endif /* !_NFP__MEM_BULK_H_ */
