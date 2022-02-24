/*
 * Copyright (C) 2012-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/cls.h
 * @brief         NFP CLS interface
 */
#ifndef _NFP__CLS_H_
#define _NFP__CLS_H_

#include <nfp.h>
#include <stdint.h>
#include <types.h>
#include <nfp6000/nfp_cls.h>

/**
 * Read a multiple of 4B from CLS.
 * @param data      Pointer to sufficient write transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the read, must be a multiple of 4
 * @param max_size  Used to determine largest read, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides basic bulk reads to the current island's CLS.  There
 * is currently limited support for size to be a runtime value, and for reads
 * >64B. cls_read() provides a simplified interface where size is assumed to
 * be compile time constant, and the context swaps on an internal signal while
 * waiting for the read to complete.
 */
__intrinsic void __cls_read(__xread void *data, __cls void *addr,
                            size_t size, const size_t max_size,
                            sync_t sync, SIGNAL *sig);

__intrinsic void cls_read(__xread void *data, __cls void *addr, size_t size);

/**
 * Write a multiple of 4B to CLS.
 * @param data      Pointer to sufficient write transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides basic bulk writes to the current island's CLS. There
 * is currently limited support for size to be a runtime value, and for read
 * >64B. cls_write() provides a simplified interface where size is assumed to
 * be compile time constant, and the context swaps on an internal signal while
 * waiting for the write to complete.
 */
__intrinsic void __cls_write(__xwrite void *data, __cls void *addr,
                             size_t size, const size_t max_size,
                             sync_t sync, SIGNAL *sig);

__intrinsic void cls_write(__xwrite void *data, __cls void *addr, size_t size);

/*
 * Endian specific functions.  Parameters as per endian neutral functions above.
 */

__intrinsic void __cls_read_be(__xread void *data, __cls void *addr,
                               size_t size, const size_t max_size,
                               sync_t sync, SIGNAL *sig);

__intrinsic void cls_read_be(__xread void *data, __cls void *addr, size_t size);

__intrinsic void __cls_write_be(__xwrite void *data, __cls void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL *sig);

__intrinsic void cls_write_be(__xwrite void *data, __cls void *addr,
                              size_t size);



__intrinsic void __cls_read_le(__xread void *data, __cls void *addr,
                               size_t size, const size_t max_size,
                               sync_t sync, SIGNAL *sig);

__intrinsic void cls_read_le(__xread void *data, __cls void *addr, size_t size);

__intrinsic void __cls_write_le(__xwrite void *data, __cls void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL *sig);

__intrinsic void cls_write_le(__xwrite void *data, __cls void *addr,
                              size_t size);

/**
 * Create a 64-bit hash_index over the transfer registers.
 * @param key       Pointer to sufficient write transfer registers for the hash
 * @param mask      32-bit pointer to the start of the mask in CLS
 * @param size      Size of the key/mask, must be a multiple of 4. Valid [4-128]
 * @param max_size  Used to determine largest hash, if size is not a constant
 * @param idx       CLS hash index. Valid [0-7]
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * These functions perform a hash on up to 128 bytes of data in the transfer
 * registers. The user should configure the CLS hash multiply register and
 * initialize the hash before calling this function. See std/hash.h
 * The clear function is the same as hash_mask, but also clears the initial
 * value of hash_index before calculating the new value. cls_hash_mask() can be
 * used to chain several hashes together.
 */
__intrinsic void __cls_hash_mask_clr(__xwrite void *key, __cls void *mask,
                                     size_t size, const size_t max_size,
                                     uint32_t idx, sync_t sync, SIGNAL *sig);

__intrinsic void cls_hash_mask_clr(__xwrite void *key, __cls void *mask,
                                   size_t size, uint32_t idx);

__intrinsic void __cls_hash_mask(__xwrite void *key, __cls void *mask,
                                 size_t size, const size_t max_size,
                                 uint32_t idx, sync_t sync, SIGNAL *sig);

__intrinsic void cls_hash_mask(__xwrite void *key, __cls void *mask,
                               size_t size, uint32_t idx);

/* Bit atomic functions */
/**
 * Clear the bit(s) in multiples of 4B from CLS.
 * @param data      Pointer to sufficient write transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides bit atomic clear of the bit(s) at the specified
 * address according to a bit mask provided in the xfer register(s).
 * A 1 in the bit position of the bit mask signifies that the bit should be
 * cleared.
 * There is currently limited support for size to be a runtime value, and for
 * clears >64B. cls_clr() provides a simplified interface where size is
 * assumed to be compile time constant, and the context swaps on an internal
 * signal while waiting for the write to complete.
 */
__intrinsic void __cls_clr(__xwrite void *data, __cls void *addr, size_t size,
                           const size_t max_size, sync_t sync, SIGNAL *sig);

__intrinsic void cls_clr(__xwrite void *data, __cls void *addr, size_t size);

/**
 * Set the bit(s) in multiples of 4B from CLS.
 * @param data      Pointer to sufficient write transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides bit atomic set of the bit(s) at the specified
 * address according to a bit mask provided in the transfer register(s). A 1
 * in the bit position of the bit mask signifies that the bit should be set.
 * There is currently limited support for size to be a runtime value, and for
 * sets >64B. cls_set() provides a simplified interface where size is assumed
 * to be compile time constant, and the context swaps on an internal signal
 * while waiting for the write to complete.

 */
__intrinsic void __cls_set(__xwrite void *data, __cls void *addr, size_t size,
                           const size_t max_size, sync_t sync, SIGNAL *sig);

__intrinsic void cls_set(__xwrite void *data, __cls void *addr, size_t size);

/**
 * Swap -- exchange the value in the transfer register with the value in memory.
 * @param data     Pointer to sufficient RW transfer registers for the op
 * @param addr     32-bit pointer to the memory start address in local CLS
 * @param size     Size of the swap, must be a multiple of 4
 * @param max_size Used to determine largest write, if size is not a constant
 * @param sync     Type of synchronization (sig_done or ctx_swap)
 * @param sig      Signal to use
 */
__intrinsic void __cls_swap(__xrw void* data, __cls void* addr,
                            size_t size, const size_t max_size,
                            sync_t sync, SIGNAL *sig);

__intrinsic void cls_swap(__xrw void* data, __cls void* addr,
                          size_t size);
/**
 * Test and Clear the bit(s) in multiples of 4B from CLS.
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides bit atomic clear of the bit(s) at the specified
 * address according to a bit mask provided in the write transfer register(s)
 * and returns the premodified memory contents in the read transfer
 * register(s). A 1 in the bit position of the bit mask signifies that the bit
 * should be cleared.
 * There is currently limited support for size to be a runtime value, and for
 * test and clears >64B. cls_test_clr() provides a simplified interface
 * where size is assumed to be compile time constant, and the context swaps on
 * an internal signal while waiting for the write to complete.
 */

__intrinsic void __cls_test_clr(__xrw void *data, __cls void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL *sig);

__intrinsic void cls_test_clr(__xrw void *data, __cls void *addr,
                              size_t size);

/**
 * Test and Set the bit(s) in multiples of 4B from CLS.
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides bit atomic set of the bit(s) at the specified
 * address according to a bit mask provided in the write transfer register(s)
 * and returns the premodified memory contents in the read transfer
 * register(s). A 1 in the bit position of the bit mask signifies that the bit
 * should be set.
 * There is currently limited support for size to be a runtime value,
 * and for test and sets >64B. cls_test_set() provides a simplified interface
 * where size is assumed to be compile time constant, and the context swaps on
 * an internal signal while waiting for the write to complete.
 */
__intrinsic void __cls_test_set(__xrw void *data, __cls void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL *sig);

__intrinsic void cls_test_set(__xrw void *data, __cls void *addr,
                              size_t size);

/**
 * Test and add in multiples of 4B from CLS.
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides atomic add of words at the specified
 * address according to the data provided in the write transfer register(s)
 * and returns the premodified memory contents in the read transfer
 * register(s).
 * There is currently limited support for size to be a runtime value,
 * and for test and adds >64B. cls_test_add() provides a simplified interface
 * where size is assumed to be compile time constant, and the context swaps on
 * an internal signal while waiting for the write to complete.
 */
__intrinsic void __cls_test_add(__xrw void *data, __cls void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL *sig);

__intrinsic void cls_test_add(__xrw void *data, __cls void *addr,
                              size_t size);

/**
 * Test and sub in multiples of 4B from CLS.
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      32-bit pointer to the memory start address in local CLS
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * This function provides atomic sub of words at the specified
 * address according to the data provided in the write transfer register(s)
 * and returns the premodified memory contents in the read transfer
 * register(s).
 * There is currently limited support for size to be a runtime value,
 * and for test and subs >64B. cls_test_sub() provides a simplified interface
 * where size is assumed to be compile time constant, and the context swaps on
 * an internal signal while waiting for the write to complete.
 */
__intrinsic void __cls_test_sub(__xrw void *data, __cls void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL *sig);

__intrinsic void cls_test_sub(__xrw void *data, __cls void *addr,
                              size_t size);

/**
 * Single instruction to increment by 1 a value in CLS.
 * @param addr     Pointer to the memory address in CLS.
 *
 * These functions provide bit atomic 32 or 64 bit CLS increment by 1.
 */
__intrinsic void cls_incr(__cls void *addr);

__intrinsic void cls_incr64(__cls void *addr);

/**
 * Single instruction to decrement by 1 a value in CLS.
 * @param addr     pointer to the memory address in CLS.
 *
 * These functions provide bit atomic 32 or 64 bit CLS decrement by 1.
 */
__intrinsic void cls_decr(__cls void *addr);

__intrinsic void cls_decr64(__cls void *addr);

/**
 * The VF Translate functions provides an API to use the CLS VF (Virtual
 * Function) Translate feature. The VF translate provides a address space
 * remapping invoked when bit 31 of an CLS address is set. The following items
 * are configurable. The starting offset into CLS, VF page size, CLS bytes per
 * VF, alignment, if any padding between pages and how the write alert, if any
 * is triggered.
 *
 * If using write alert with VF translate you should use the write_alert
 * library to configure aspects related to write alerting.
 *
 * Valid configurations for VF translate are:
 * base_addr = Starting address location in CLS.
 * page_size = 4K, 32K, 256K and 2M.
 * cls_bytes = 64, 1K, 4K and 16K.
 * alert_trig = 0 for bit 11 of address, 1 for bit 20 of address.
 * alignment = None, Resv, 32 bit word and 64 bit word.
 * padding = Provides a protection between VF address spaces.
 */

/**
 * Usage example:
 * VF Translate with 64 CLS bytes per VF, a 4K page size aligned on 32 bit
 * words, no padding and alert being triggered via bit 11 of address starting
 * at address 0x0 of CLS.
 *
 * A CLS address range for VF 0  = 0x80000800 - 0x8000083F
 *                         VF 1  = 0x80001800 - 0x8000183F
 *                         ...
 *                         VF 64 = 0x8003F800 - 0x8003F83F
 *
 * Configures VF translate as described above.
 * cls_vf_translate_setup(0x0, 0, 0, 0, 2, 0);
 *
 * Disables VF translate.
 * cls_vf_translate_disable();
 */

/**
 * Setup a CLS VF Translate config.
 * @param base_addr   Starting address in CLS
 * @param cls_bytes   Number of CLS bytes per VF
 * @param page_size   Page size of VF BAR
 * @param alert_trig  Type of alert trigger configured
 * @param alignment   VF alignment configuration
 * @param padding     Padding between CLS VF address spaces
 */
__intrinsic void cls_vf_translate_setup(unsigned int base_addr,
                                        unsigned int cls_bytes,
                                        unsigned int page_size,
                                        unsigned int alert_trig,
                                        unsigned int alignment,
                                        unsigned int padding);

/**
 * Disables a CLS VF Translate config.
 */
__intrinsic void cls_vf_translate_disable();

/**
 * CLS Ring Operations
 *
 * @param ring_num      CLS ring to configure (0 - 15)
 * @param base_addr     Starting address in CLS -- Must be aligned to ring size
 * @param size          Size of the ring in bytes
 */
__intrinsic void cls_ring_setup(unsigned int ring_num,  __cls void* base_addr, unsigned int size);
__intrinsic void cls_workq_setup(unsigned int ring_num,  __cls void* base_addr, unsigned int size);

__intrinsic void __cls_ring_put(unsigned int rnum, __xwrite void* data,
            size_t size, sync_t sync, SIGNAL* sig);
__intrinsic void cls_ring_put(unsigned int rnum, __xwrite void* data, size_t size);
__intrinsic void __cls_ring_get(unsigned int rnum, __xread void* data,
            size_t size, sync_t sync, SIGNAL* sig);
__intrinsic void cls_ring_get(unsigned int rnum, __xread void* data, size_t size);
__intrinsic void __cls_ring_get_freely(unsigned int rnum, __xread void* data,
            size_t size, sync_t sync, SIGNAL* sig);
__intrinsic void cls_ring_get_freely(unsigned int rnum, __xread void* data, size_t size);
__intrinsic void __cls_workq_add_thread(unsigned int rnum, __xread void* data, size_t size, sync_t sync, SIGNAL* sig);
__intrinsic void cls_workq_add_thread(unsigned int rnum, __xread void* data, size_t size);
__intrinsic void __cls_workq_add_work(unsigned int rnum, __xwrite void* data, size_t size, sync_t sync, SIGNAL* sig);
__intrinsic void cls_workq_add_work(unsigned int rnum, __xwrite void* data, size_t size);

/**
 * CLS Lock Queue operations
 * The lock-queue can hold up to 5 pending signals. Including the initial lock operation on an empty queue,
 * a lockqueue variable can as such support 6 concurrent lock operations without overflowing. All signals
 * are enqueued in FIFO order. When a FPC issues an unlock operation the first signal in the queue is dequeued.
 */
__intrinsic int cls_lockq_lock(__cls void* addr);
__intrinsic void cls_lockq_lock_retry(__cls void* addr, unsigned int retry_timeout);
__intrinsic void cls_lockq_unlock(__cls void* addr);

#endif /* !_NFP__CLS_H_ */
