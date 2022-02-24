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
 * @file          lib/nfp/mem_atomic.h
 * @brief         NFP memory atomics interface
 */
#ifndef _NFP__MEM_ATOMIC_H_
#define _NFP__MEM_ATOMIC_H_

#include <nfp.h>
#include <stdint.h>
#include <types.h>

/**
 * Atomic read/write multiples of 4B in memory (EMEM, IMEM, or CTM).
 * @param data      Pointer to sufficient transfer registers for the op
 * @param addr      40-bit pointer to the memory start address
 * @param size      Size of the op, must be a multiple of 4
 * @param max_size  Used to determine largest op, if size is not a constant
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * These functions provide basic atomic read/write in NFP memory.
 * Atomic read/write functions are used rather than the bulk read/write
 * functions if an address is used by other atomic functions.
 * No special alignment is assumed about address when converting from a 40bit
 * pointer to the "src_op" fields in the __asm command. This makes the function
 * general, but suboptimal if it is guaranteed to be 256B aligned.
 * mem_*_atomic() functions provide a simplified interface where size is
 * assumed to be compile time constant, and the context swaps on an internal
 * signal while waiting for the operation to complete.
 */

__intrinsic void __mem_read_atomic(__xread void *data,__mem40 void *addr,
                                   size_t size, const size_t max_size,
                                   sync_t sync, SIGNAL *sig);

__intrinsic void mem_read_atomic(__xread void *data, __mem40 void *addr,
                                 size_t size);

__intrinsic void __mem_write_atomic(__xwrite void *data, __mem40 void *addr,
                                    size_t size, const size_t max_size,
                                    sync_t sync, SIGNAL *sig);

__intrinsic void mem_write_atomic(__xwrite void *data, __mem40 void *addr,
                                  size_t size);

/**
 * Atomically write a 16bit value to a counter (EMEM, IMEM, or CTM).
 * @param val       16-bit integer
 * @param addr      40-bit pointer to the value in memory
 *
 * These functions provide the ability to write a value to a 32 bit or 64 bit
 * value in memory without the need for transfer registers.
 */
__intrinsic void mem_write32_atomic_imm(unsigned int val, __mem40 void *addr);
__intrinsic void mem_write64_atomic_imm(unsigned int val, __mem40 void *addr);


/**
 * Atomic increment/decrement of a value in memory (EMEM, IMEM, or CTM).
 * @param addr      40-bit pointer to the value in memory start address
 *
 * These functions increment or decrement a single 32 bit or 64 bit
 * word in NFP memory.
 */
__intrinsic void mem_incr32(__mem40 void *addr);

__intrinsic void mem_decr32(__mem40 void *addr);

__intrinsic void mem_incr64(__mem40 void *addr);

__intrinsic void mem_decr64(__mem40 void *addr);

/**
 * Atomic add immediate to a value in memory (EMEM, IMEM, or CTM).
 * @param val       16-bit integer
 * @param addr      40-bit pointer to the value in memory start address
 *
 * These functions provide the ability to increment a single 32 bit or 64 bit
 * word in memory by a 16 bit number without the need for transfer registers.
 */
__intrinsic void mem_add32_imm(unsigned int val, __mem40 void *addr);

__intrinsic void mem_add64_imm(unsigned int val, __mem40 void *addr);

/**
 * Atomic sub immediate to a value in memory (EMEM, IMEM, or CTM).
 * @param val       16-bit integer
 * @param addr      40-bit pointer to the value in memory start address
 *
 * These functions provide the ability to increment a single 32 bit or 64 bit
 * word in memory by a 16 bit number without the need for transfer registers.
 */
__intrinsic void mem_sub32_imm(unsigned int val, __mem40 void *addr);

__intrinsic void mem_sub64_imm(unsigned int val, __mem40 void *addr);

/**
 * Atomic subsat immediate to a value in memory (EMEM, IMEM, or CTM).
 * @param val       16-bit integer
 * @param addr      40-bit pointer to the value in memory start address
 *
 * These functions provide the ability to increment a single 32 bit or 64 bit
 * word in memory by a 16 bit number without the need for transfer registers.
 */
__intrinsic void mem_subsat32_imm(unsigned int val, __mem40 void *addr);

__intrinsic void mem_subsat64_imm(unsigned int val, __mem40 void *addr);

/**
 * Atomic bit set/clr immediate (EMEM, IMEM, or CTM).
 * @param val       16-bit integer
 * @param addr      40-bit pointer to the value in memory
 *
 * These functions provide the ability to bit set/clr a value in memory
 * without the need for transfer registers.
 */
__intrinsic void mem_bitset_imm(unsigned int val, __mem40 void *addr);

__intrinsic void mem_bitclr_imm(unsigned int val, __mem40 void *addr);

/**
 * Atomic add/sub to/from multiple 4B or 8B values (EMEM, IMEM, or CTM).
 * @param data      Pointer to sufficient transfer registers for the op
 * @param addr      40-bit pointer to first value in memory
 * @param size      Size of the op, must be a multiple of 4 or 8
 * @param max_size  Used to determine largest op, if size is not a constant
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note add/sub32 size max = 16
 * @note add/sub64 size max = 32
 * These functions provide basic atomic add/sub in NFP memory. No special
 * alignment is assumed about address when converting from a 40bit pointer to
 * the "src_op" fields in the __asm command. This makes the function general,
 * but suboptimal if it is guaranteed to be 256B aligned.
 * mem_*() functions provide a simplified interface where size is
 * assumed to be compile time constant, and the context swaps on an internal
 * signal while waiting for the operation to complete.
 */
__intrinsic void __mem_add32(__xwrite void *data, __mem40 void *addr,
                             size_t size, const size_t max_size,
                             sync_t sync, SIGNAL *sig);

__intrinsic void mem_add32(__xwrite void *data, __mem40 void *addr,
                           size_t size);

__intrinsic void __mem_add64(__xwrite void *data, __mem40 void *addr,
                             size_t size, const size_t max_size,
                             sync_t sync, SIGNAL *sig);

__intrinsic void mem_add64(__xwrite void *data, __mem40 void *addr,
                           size_t size);

__intrinsic void __mem_sub32(__xwrite void *data, __mem40 void *addr,
                             size_t size, const size_t max_size,
                             sync_t sync, SIGNAL *sig);

__intrinsic void mem_sub32(__xwrite void *data, __mem40 void *addr,
                           size_t size);

__intrinsic void __mem_sub64(__xwrite void *data, __mem40 void *addr,
                             size_t size, const size_t max_size,
                             sync_t sync, SIGNAL *sig);

__intrinsic void mem_sub64(__xwrite void *data, __mem40 void *addr,
                           size_t size);

/**
 * Atomic set/clear bits in multiple of 4B values (EMEM, IMEM, or CTM).
 * @param data      Pointer to sufficient transfer registers for the op
 * @param addr      40-bit pointer to the first value in memory
 * @param size      Size of the op, must be a multiple of 4
 * @param max_size  Used to determine largest op, if size is not a constant
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note size max = 32
 * These functions provide basic atomic bit set/clr in NFP memory.  No special
 * alignment is assumed about address when converting from a 40bit pointer to
 * the "src_op" fields in the __asm command. This makes the function general,
 * but suboptimal if it is guaranteed to be 256B aligned.
 * mem_*() functions provide a simplified interface where size is
 * assumed to be compile time constant, and the context swaps on an internal
 * signal while waiting for the operation to complete.
 */
__intrinsic void __mem_bitset(__xwrite void *data, __mem40 void *addr,
                              size_t size, const size_t max_size,
                              sync_t sync, SIGNAL *sig);

__intrinsic void mem_bitset(__xwrite void *data,
                            __mem40 void *addr, size_t size);

__intrinsic void __mem_bitclr(__xwrite void *data, __mem40 void *addr,
                              size_t size, const size_t max_size,
                              sync_t sync, SIGNAL *sig);

__intrinsic void mem_bitclr(__xwrite void *data,
                            __mem40 void *addr, size_t size);

/**
 * Atomic compare and conditional write of one or more 32-bit values.
 * @param data       Pointer to sufficient transfer registers for the op.
 * @param addr       40-bit pointer to the first value in memory.
 * @param byte_mask  Byte mask to use for the comparison (from 0x1 to 0xF).
 * @param size       Size of the op, must be a multiple of 4.
 * @param max_size   Used to determine largest op, if size is not a constant.
 * @param sync       Type of synchronisation (sig_done or ctx_swap).
 * @param sig        Signal to use.
 *
 * @note Setting bit X in the byte_mask means that byte X should be compared.
 * @note Largest supported size is 32 bytes.
 * @note Largest supported max_size is 32 bytes; must be a multiple of 4.
 *
 * These functions provide an atomic comparison of the original masked memory
 * contents with the masked transfer register value, along with a conditional
 * write of the unmasked transfer register value to the memory location if the
 * comparison succeeds.  This atomic operation can be represented as follows:
 *     mask = (((byte_mask & 0x1) ? 0x000000ff : 0) |
 *             ((byte_mask & 0x2) ? 0x0000ff00 : 0) |
 *             ((byte_mask & 0x4) ? 0x00ff0000 : 0) |
 *             ((byte_mask & 0x8) ? 0xff000000 : 0));
 *     for (n = 0; n < size; ++n)
 *         if ((memory[addr + n] & mask) == (data[n] & mask))
 *             memory[addr + n] = data[n];
 */
__intrinsic void __mem_cmp_write(__xwrite void *data, __mem40 void *addr,
                                 unsigned int byte_mask, size_t size,
                                 const size_t max_size, sync_t sync,
                                 SIGNAL *sig);

__intrinsic void mem_cmp_write(__xwrite void *data, __mem40 void *addr,
                               unsigned int byte_mask, size_t size);

/**
 * Atomic test and set/clr bits in multiple of 4B values (EMEM, IMEM, or CTM).
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      40-bit pointer to the first value in memory
 * @param size      Size of the op, must be a multiple of 4
 * @param max_size  Used to determine largest op, if size is not a constant
 * @param sync      Type of synchronization (must be sig_done)
 * @param sig_pair  Signal pair to use
 *
 * Note: Maximum for @max_size is 32.
 *
 * These functions provide basic atomic bit test and set/clr in NFP EMEM,
 * IMEM or CTM memory types.
 */
__intrinsic void __mem_test_set(__xrw void *data, __mem40 void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL_PAIR *sig_pair);

__intrinsic void mem_test_set(__xrw void *data, __mem40 void *addr,
                              size_t size);

__intrinsic void __mem_test_clr(__xrw void *data, __mem40 void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL_PAIR *sig);

__intrinsic void mem_test_clr(__xrw void *data, __mem40 void *addr,
                              size_t size);

/**
 * Atomic swap in multiple of 4B values (EMEM, IMEM, or CTM).
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      40-bit pointer to the first value in memory
 * @param size      Size of the op, must be a multiple of 4
 * @param max_size  Used to determine largest op, if size is not a constant
 * @param sync      Type of synchronization (must be sig_done)
 * @param sig_pair  Signal pair to use
 *
 * Note: Maximum for @max_size is 32.
 *
 * These functions provide atomic swap in NFP EMEM, IMEM or CTM
 * memory types.
 */
__intrinsic void __mem_swap(__xrw void *data, __mem40 void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL_PAIR *sig);

__intrinsic void mem_swap(__xrw void *data, __mem40 void *addr,
                              size_t size);

/**
 * Atomic test and add/sub in multiple of 4B values (EMEM, IMEM, or CTM).
 * @param data      Pointer to sufficient RW transfer registers for the op
 * @param addr      40-bit pointer to the first value in memory
 * @param size      Size of the op, must be a multiple of 4
 * @param max_size  Used to determine largest op, if size is not a constant
 * @param sync      Type of synchronization (must be sig_done)
 * @param sig_pair  Signal pair to use
 *
 * Note: Maximum for @max_size is 16.
 *
 * These functions will add or subtract data in NFP EMEM, IMEM or CTM memory
 * types, and return the contents of memory before the operation.
 */
__intrinsic void __mem_test_add(__xrw void *data, __mem40 void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL_PAIR *sig);

__intrinsic void mem_test_add(__xrw void *data, __mem40 void *addr,
                              size_t size);

__intrinsic void __mem_test_sub(__xrw void *data, __mem40 void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL_PAIR *sig);

__intrinsic void mem_test_sub(__xrw void *data, __mem40 void *addr,
                              size_t size);

__intrinsic void __mem_test_subsat(__xrw void *data, __mem40 void *addr,
                                size_t size, const size_t max_size,
                                sync_t sync, SIGNAL_PAIR *sig);

__intrinsic void mem_test_subsat(__xrw void *data, __mem40 void *addr,
                                size_t size);

/**
 * Atomic test, compare and conditional write of one or more 32-bit values.
 * @param data       Pointer to sufficient RW transfer registers for the op.
 * @param addr       40-bit pointer to the first value in memory.
 * @param byte_mask  Byte mask to use for the comparison (from 0x1 to 0xF).
 * @param size       Size of the op, must be a multiple of 4.
 * @param max_size   Used to determine largest op, if size is not a constant.
 * @param sync       Type of synchronization (must be sig_done)
 * @param sig_pair   Signal pair to use
 *
 * @note Setting bit X in the byte_mask means that byte X should be compared.
 * @note Largest supported size is 32 bytes.
 * @note Largest supported max_size is 32 bytes; must be a multiple of 4.
 *
 * These functions will store the original memory content in the RW transfer
 * register, then provide an atomic comparison of the original masked memory
 * contents with the masked transfer register value, along with a conditional
 * write of the unmasked transfer register value to the memory location if the
 * comparison succeeds.  This atomic operation can be represented as follows:
 *     mask = (((byte_mask & 0x1) ? 0x000000ff : 0) |
 *             ((byte_mask & 0x2) ? 0x0000ff00 : 0) |
 *             ((byte_mask & 0x4) ? 0x00ff0000 : 0) |
 *             ((byte_mask & 0x8) ? 0xff000000 : 0));
 *     for (n = 0; n < size; ++n) {
 *         data.read[n] = memory[addr + n];
 *         if ((memory[addr + n] & mask) == (data.write[n] & mask))
 *             memory[addr + n] = data.write[n];
 *     }
 */
__intrinsic void __mem_test_cmp_write(__xrw void *data, __mem40 void *addr,
                                      unsigned int byte_mask, size_t size,
                                      const size_t max_size, sync_t sync,
                                      SIGNAL_PAIR *sig_pair);

__intrinsic void mem_test_cmp_write(__xrw void *data, __mem40 void *addr,
                                    unsigned int byte_mask, size_t size);

/*
 * The NFP MU atomic engine provides support for metering.  This metering can
 * be used for metering packets, either by metering the packet size (that is,
 * metering based on bytes-per-second), or by metering the number of packets
 * (that is, metering based on packets-per-second).
 *
 * The metering operation results in three possible colors:
 *     - Green, Yellow and Red
 *
 * In terms of IP Assured Forwarding drop precedences (see RFC 2597), these
 * colors can map to the following drop drecedences:
 *     - Green  -> Gold   (e.g. IP AF11)
 *     - Yellow -> Silver (e.g. IP AF12)
 *     - Red    -> Bronze (e.g. IP AF13)
 *
 * The metering operations can take into account the incoming packet color.  In
 * the event the incoming packet color is taken into consideration, the general
 * guideline is that the drop precedence can only be raised, not lowered.  That
 * translates to the following rules:
 *     - Incoming green packets can be marked as either green, yellow or red
 *         e.g. An IP AF11 packet can be sent out as AF11, AF12 or AF13
 *     - Incoming yellow packets can be marked as either yellow or red
 *         e.g. An IP AF12 packet can be sent out as AF12 or AF13
 *     - Incoming red packets can only be marked as red
 *         e.g. An IP AF13 packet can only be sent out as AF13
 *
 * Packets that do not support packet coloring (e.g. IP CS7), that is,
 * colorless packets, are metered as colorless packets; the metering behavior
 * is the same as if those packets were colored green-- the metering operation
 * can return either green, yellow or red.  (In contrast to colored packets,
 * the IP DSCP is generally left unmodified for colorless packets.)
 *
 * Two modes of operation are supported by the NFP MU atomic engine:
 *     1. Peak mode (RFC 2698)
 *            This mode of operation utilizes a committed token bucket,and a
 *            peak token bucket.  The committed token bucket is refilled
 *            according to the committed information rate (CIR) and the
 *            committed burst size (CBS) for the meter.  Likewise, the peak
 *            token bucket is refilled according to the peak information rate
 *            (PIR) and the peak burst size (PBS) for the meter.
 *
 *            In general, the following guidelines should be followed:
 *                - PIR >= CIR
 *
 *            If metering based on packet size, the following guidelines should
 *            also be followed:
 *                - CBS >= largest supported packet size
 *                - PBS >= largest supported packet size
 *
 *            If not metering based on packet size, then the following
 *            guidelines should be followed:
 *                - CBS >= 0
 *                - PBS >= 0
 *
 *            Note: In general, if the CIR is set to 0, the committed token
 *                  bucket will never replenish its tokens.
 *            Note: In general, if the PIR is set to 0, the peak token bucket
 *                  will never replenish its tokens.
 *            Note: In general, if the CBS is set to 0, packets will never be
 *                  marked as green.
 *            Note: In general, if the PBS is set to 0, packets will always be
 *                  marked as red.
 *
 *            The rules for how the packets are marked are as follows:
 *                - For a packet to be marked as green, both the committed and
 *                  peak token buckets must have sufficient tokens.
 *                - For a packet to be marked as yellow, the peak token buckets
 *                  must have sufficient tokens.
 *
 *     2. Excess mode (RFC 2697/4115)
 *            This mode of operation utilizes a committed token bucket and an
 *            excess token bucket.  The committed token bucket is refilled
 *            according to the committed information rate (CIR) and the
 *            committed burst size (CBS) for the meter.  For RFC 4115, the
 *            excess token bucket is refilled according to the excess
 *            information rate (EIR) and the excess burst size (EBS) for the
 *            meter.  For RFC 2697, the EIR is equal to the CIR.
 *
 *            If metering based on packet size, the following guidelines should
 *            also be followed:
 *                - CBS >= largest supported packet size
 *                - EBS >= largest supported packet size
 *
 *            If not metering based on packet size, then the following
 *            guidelines should be followed:
 *                - CBS >= 0
 *                - EBS >= 0
 *
 *            Note: In general, if the CIR is set to 0, the committed token
 *                  bucket will never replenish its tokens.
 *            Note: In general, if the EIR is set to 0, the excess token bucket
 *                  will never replenish its tokens.
 *            Note: In general, if the CBS is set to 0, packets will never be
 *                  marked as green.
 *            Note: In general, if the EBS is set to 0, packets will never be
 *                  marked as yellow.
 *
 *            The rules for how the packets are marked are as follows:
 *            - For a packet to be marked as green, the committed token bucket
 *              must have sufficient tokens.
 *            - For a packet to be marked as yellow, the excess token bucket
 *              must have sufficient tokens.
 */

/**
 * Metering token bucket data-structure.
 *
 * This data-structure stores the token buckets for the meter.  The
 * data-structure supports two modes of metering:
 *     1. RFC 2698      - This mode of metering uses a committed token bucket
 *                        and a peak token bucket.
 *     2. RFC 2697/4115 - This mode of metering uses a committed token bucket
 *                        and an excess token bucket.
 *
 * @note The long-term token bucket corresponds to the committed token bucket;
 *       this is the name used in the Databook and EAS.
 * @note The short-term token bucket corresponds to the excess/peak token
 *       bucket; this is the name used in the Databook and EAS.
 */
struct meter_bucket {
    union {
        /* IETF RFC 2697/2698/4115 naming schema. */
        struct {
            union {
                uint32_t excess_tokens; /**< Excess token bucket. */
                uint32_t peak_tokens;   /**< Peak token bucket. */
            };
            uint32_t committed_tokens;  /**< Committed token bucket. */
        };
        /* Databook/EAS naming schema. */
        struct {
            uint32_t short_term_tokens; /**< Short-term token bucket. */
            uint32_t long_term_tokens;  /**< Long-term token bucket. */
        };
    };
};

/** Metering mode specifier. */
enum meter_mode {
    METER_MODE_PEAK   = 0, /**< Peak-rate/-size mode (RFC 2698). */
    METER_MODE_EXCESS = 1  /**< Excess-rate/-size mode (RFC 2697/4115). */
};

/** Packet color specifier. */
enum meter_color {
    METER_COLOR_NO_COLOR = 0,
    METER_COLOR_GREEN    = 0,
    METER_COLOR_YELLOW   = 1,
    METER_COLOR_RED      = 2,
};

/**
 * Rate metering command (EMEM, IMEM, or CTM).
 * @param meter_data  Pointer to RW transfer register for the op.
 * @param addr        40-bit pointer to the meter (must be 8-byte aligned).
 * @param mode        Metering mode.
 * @param color       Incoming packet color.
 * @param sig_pair    Signal pair to use.
 *
 * @note meter_data is used to pass in the packet size or packet count.
 * @note meter_data will be set to the packet color by the metering operation.
 */
__intrinsic void __mem_meter(__xrw uint32_t *meter_data, __mem40 void *addr,
                             enum meter_mode mode, enum meter_color color,
                             SIGNAL_PAIR *sig_pair);

/**
 * Rate metering command (EMEM, IMEM, or CTM).
 * @param meter_amt  Number of tokens to remove from the token bucket.
 * @param addr       40-bit pointer to the meter (must be 8-byte aligned).
 * @param mode       Metering mode.
 * @param color      Incoming packet color.
 * @return Returns the resulting packet color (green, yellow or red).
 *
 * @note Set meter_amt to the packet size for byte-per-second metering.
 * @note Set meter_amt to the packet count for packet-per-second metering.
 */
__intrinsic enum meter_color mem_meter(uint32_t meter_amt, __mem40 void *addr,
                                       enum meter_mode mode,
                                       enum meter_color color);

#endif /* !_NFP__MEM_ATOMIC_H_ */
