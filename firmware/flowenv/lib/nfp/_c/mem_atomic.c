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
 * @file          lib/nfp/_c/mem_atomic.c
 * @brief         NFP memory atomics interface
 */

#include <assert.h>
#include <nfp.h>
#include <stdint.h>
#include <types.h>

#include <nfp/mem_atomic.h>

#define _MEM_ATOMIC_32_CMD 0
#define _MEM_ATOMIC_64_CMD 1

#define _MEM_ATOMIC_CMD(cmdname, data, addr, size, max_size, sync, sig, shift, \
                        cmdlen)                                                \
do {                                                                           \
    struct nfp_mecsr_prev_alu ind;                                             \
    unsigned int count = (size >> shift);                                      \
    unsigned int max_count = (max_size >> shift);                              \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    ctassert(__is_ct_const(sync));                                             \
    ctassert(__is_ct_const(max_size));                                         \
    try_ctassert(size != 0);                                                   \
    ctassert(sync == sig_done || sync == ctx_swap);                            \
    try_ctassert(count <= 8);                                                  \
    ctassert(__is_ct_const(cmdlen));                                           \
    ctassert( (cmdlen == _MEM_ATOMIC_32_CMD) ||                                \
              (cmdlen == _MEM_ATOMIC_64_CMD) );                                \
                                                                               \
    /* This code is inefficient if addr is >256B aligned, */                   \
    /* but will work for 40bit or 32bit pointers. */                           \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    if (__is_ct_const(size) ) {                                                \
        if (sync == sig_done) {                                                \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                 \
                        __ct_const_val(count)], sig_done[*sig] }               \
        } else {                                                               \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                 \
                        __ct_const_val(count)], ctx_swap[*sig] }               \
        }                                                                      \
    } else {                                                                   \
        /* Setup length in PrevAlu for the indirect */                         \
        ind.__raw = 0;                                                         \
        ind.ov_len = 1;                                                        \
        /* length[2:2] must be set for 64-bit non-imm arithmetic operations */ \
        ind.length = (count-1) | (cmdlen<<2);                                  \
                                                                               \
        if (sync == sig_done) {                                                \
            __asm { alu[--, --, B, ind.__raw] }                                \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                 \
                        __ct_const_val(max_count)], sig_done[*sig],            \
                        indirect_ref }                                         \
        } else {                                                               \
            __asm { alu[--, --, B, ind.__raw] }                                \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                 \
                        __ct_const_val(max_count)], ctx_swap[*sig],            \
                        indirect_ref }                                         \
        }                                                                      \
    }                                                                          \
} while (0)

#define _MEM_ATOMIC_CMD_TEST(cmdname, data, addr, size, max_size, sync,        \
                             sig_pair, shift, cmdlen)                          \
do {                                                                           \
    struct nfp_mecsr_prev_alu ind;                                             \
    unsigned int count = (size >> shift);                                      \
    unsigned int max_count = (max_size >> shift);                              \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    ctassert(__is_ct_const(sync));                                             \
    ctassert(__is_ct_const(max_size));                                         \
    try_ctassert(size != 0);                                                   \
    ctassert(sync == sig_done);                                                \
    try_ctassert(count <= 8);                                                  \
    ctassert(__is_ct_const(cmdlen));                                           \
    ctassert((cmdlen == _MEM_ATOMIC_32_CMD) ||                                 \
             (cmdlen == _MEM_ATOMIC_64_CMD));                                  \
                                                                               \
    /* This code is inefficient if addr is >256B aligned, */                   \
    /* but will work for 40bit or 32bit pointers. */                           \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    if (__is_ct_const(size) ) {                                                \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                 \
                        __ct_const_val(count)], sig_done[*sig_pair] }          \
    } else {                                                                   \
        /* Setup length in PrevAlu for the indirect */                         \
        ind.__raw = 0;                                                         \
        ind.ov_len = 1;                                                        \
        /* length[2:2] must be set for 64-bit non-imm arithmetic operations */ \
        ind.length = (count-1) | (cmdlen<<2);                                  \
                                                                               \
        __asm { alu[--, --, B, ind.__raw] }                                    \
        __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                     \
                    __ct_const_val(max_count)], sig_done[*sig_pair],           \
                    indirect_ref }                                             \
    }                                                                          \
} while (0)

#define _MEM_ATOMIC_CMD_NO_DATA(cmdname, addr)                                 \
do {                                                                           \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    __asm { mem[cmdname, --, addr_hi, <<8, addr_lo] }                          \
} while(0)

/* length[3:3] must be set for atomic 16-bit imm op
   length[2:2] must be set for 64-bit immediate arithmetic operations
   without read back
 */
#define _MEM_ATOMIC_CMD_IMM(cmdname, val, addr, cmdlen)                        \
do {                                                                           \
    struct nfp_mecsr_prev_alu ind;                                             \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    ctassert(__is_ct_const(cmdlen));                                           \
    ctassert( (cmdlen == _MEM_ATOMIC_32_CMD) ||                                \
              (cmdlen == _MEM_ATOMIC_64_CMD) );                                \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    ind.__raw = 0;                                                             \
    ind.ove_data = 2;                                                          \
    ind.data16 = val;                                                          \
    ind.ov_len = 1;                                                            \
    ind.length = (1<<3) | (cmdlen<<2);                                         \
                                                                               \
    __asm { alu[--, --, B, ind.__raw] }                                        \
    __asm { mem[cmdname, --, addr_hi, <<8, addr_lo, 1], indirect_ref }         \
} while(0)

#define _MEM_ATOMIC_CMD_BMASK(cmdname, data, addr, bmask, size, max_size,      \
                              sync, sig)                                       \
do {                                                                           \
    struct nfp_mecsr_prev_alu ind;                                             \
    unsigned int count = size >> 2;                                            \
    unsigned int max_count = max_size >> 2;                                    \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    try_ctassert(size != 0);                                                   \
    ctassert(__is_ct_const(max_size));                                         \
    ctassert(__is_ct_const(sync));                                             \
    ctassert(sync == sig_done || sync == ctx_swap);                            \
    try_ctassert(count <= 8);                                                  \
                                                                               \
    /* This code is inefficient if addr is >256B aligned, */                   \
    /* but will work for 40bit or 32bit pointers. */                           \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    /* Setup byte mask and length in PrevAlu for the indirect */               \
    ind.__raw = 0;                                                             \
    ind.ove_data = 6;                                                          \
    ind.ov_len = 1;                                                            \
    ind.length = (1 << 3) | (count - 1);                                       \
    ind.data16 = bmask & 0xff;                                                 \
                                                                               \
    if (sync == sig_done) {                                                    \
        __asm { alu[--, --, B, ind.__raw] }                                    \
        __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                     \
                    __ct_const_val(max_count)], sig_done[*sig], indirect_ref } \
    } else {                                                                   \
        __asm { alu[--, --, B, ind.__raw] }                                    \
        __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                     \
                    __ct_const_val(max_count)], ctx_swap[*sig], indirect_ref } \
    }                                                                          \
} while (0)

#define _MEM_ATOMIC_CMD_BMASK_TEST(cmdname, data, addr, bmask, size, max_size, \
                                   sync, sig_pair)                             \
do {                                                                           \
    struct nfp_mecsr_prev_alu ind;                                             \
    unsigned int count = size >> 2;                                            \
    unsigned int max_count = max_size >> 2;                                    \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    try_ctassert(size != 0);                                                   \
    ctassert(__is_ct_const(max_size));                                         \
    ctassert(__is_ct_const(sync));                                             \
    ctassert(sync == sig_done);                                                \
    try_ctassert(count <= 8);                                                  \
                                                                               \
    /* This code is inefficient if addr is >256B aligned, */                   \
    /* but will work for 40bit or 32bit pointers. */                           \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    /* Setup byte mask and length in PrevAlu for the indirect */               \
    ind.__raw = 0;                                                             \
    ind.ove_data = 6;                                                          \
    ind.ov_len = 1;                                                            \
    ind.length = (1 << 3) | (count - 1);                                       \
    ind.data16 = bmask & 0xff;                                                 \
                                                                               \
    __asm { alu[--, --, B, ind.__raw] }                                        \
    __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,                         \
                __ct_const_val(max_count)], sig_done[*sig_pair],               \
                indirect_ref }                                                 \
} while (0)

__intrinsic void
__mem_read_atomic(__xread void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD(atomic_read, data, addr, size, max_size, sync, sig, 2,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_read_atomic(__xread void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_read_atomic(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write_atomic(__xwrite void *data, __mem40 void *addr,
                   size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD(write_atomic, data, addr, size, max_size, sync, sig, 2,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_write_atomic(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_write_atomic(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
mem_incr32(__mem40 void *addr)
{
    _MEM_ATOMIC_CMD_NO_DATA(incr, addr);
}

__intrinsic void
mem_decr32(__mem40 void *addr)
{
    _MEM_ATOMIC_CMD_NO_DATA(decr, addr);
}

__intrinsic void
mem_incr64(__mem40 void *addr)
{
    _MEM_ATOMIC_CMD_NO_DATA(incr64, addr);
}

__intrinsic void
mem_decr64(__mem40 void *addr)
{
    _MEM_ATOMIC_CMD_NO_DATA(decr64, addr);
}

__intrinsic void
mem_add32_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val <= SZ_64K);

    _MEM_ATOMIC_CMD_IMM(add_imm, val, addr, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_add64_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val <= SZ_64K);

    _MEM_ATOMIC_CMD_IMM(add64_imm, val, addr, _MEM_ATOMIC_64_CMD);
}

__intrinsic void
mem_sub32_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val <= SZ_64K);

    _MEM_ATOMIC_CMD_IMM(sub_imm, val, addr, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_sub64_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val <= SZ_64K);

    _MEM_ATOMIC_CMD_IMM(sub64_imm, val, addr, _MEM_ATOMIC_64_CMD);
}

__intrinsic void
mem_subsat32_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val <= SZ_64K);

    _MEM_ATOMIC_CMD_IMM(subsat_imm, val, addr, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_subsat64_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val <= SZ_64K);

    _MEM_ATOMIC_CMD_IMM(subsat64_imm, val, addr, _MEM_ATOMIC_64_CMD);
}

__intrinsic void
mem_bitset_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val < SZ_64K);

    _MEM_ATOMIC_CMD_IMM(set_imm, val, addr, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_bitclr_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val < SZ_64K);

    _MEM_ATOMIC_CMD_IMM(clr_imm, val, addr, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_write32_atomic_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val < SZ_64K);

    _MEM_ATOMIC_CMD_IMM(atomic_write_imm, val, addr, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_write64_atomic_imm(unsigned int val, __mem40 void *addr)
{
    try_ctassert(val < SZ_64K);

    _MEM_ATOMIC_CMD_IMM(atomic_write_imm, val, addr, _MEM_ATOMIC_64_CMD);
}

__intrinsic void
__mem_add32(__xwrite void *data, __mem40 void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 16);

    _MEM_ATOMIC_CMD(add, data, addr, size, max_size, sync, sig, 2,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_add32(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_add32(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_add64(__xwrite void *data, __mem40 void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD(add64, data, addr, size, max_size, sync, sig, 3,
                    _MEM_ATOMIC_64_CMD);
}

__intrinsic void
mem_add64(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_add64(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_sub32(__xwrite void *data, __mem40 void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 16);

    _MEM_ATOMIC_CMD(sub, data, addr, size, max_size, sync, sig, 2,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_sub32(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_sub32(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_sub64(__xwrite void *data, __mem40 void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD(sub64, data, addr, size, max_size, sync, sig, 3,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_sub64(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_sub64(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_bitset(__xwrite void *data, __mem40 void *addr,
             size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    /* Set bits specified in write transfers registers to a number of
     * 32-bit memory words. A byte mask specifies which memory bytes
     * are affected for all memory words. This implementation does not
     * allow the byte mask to be configured. It defaults to all bits
     * specfied in the transfer register will be set. */
    _MEM_ATOMIC_CMD(set, data, addr, size, max_size, sync, sig, 2,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_bitset(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_bitset(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_bitclr(__xwrite void *data, __mem40 void *addr,
             size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    /* Clear bits specified in write transfers registers to a number
     * of 32-bit memory words. A byte mask specifies which memory
     * bytes are affected for all memory words. This implementation
     * does not allow the byte mask to be configured. It defaults to
     * all bits specified in the transfer register will be cleared. */
    _MEM_ATOMIC_CMD(clr, data, addr, size, max_size, sync, sig, 2,
                    _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_bitclr(__xwrite void *data, __mem40 void *addr, size_t size)
{
    SIGNAL sig;

    __mem_bitclr(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_cmp_write(__xwrite void *data, __mem40 void *addr,
                unsigned int byte_mask, size_t size, const size_t max_size,
                sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD_BMASK(mask_compare_write, data, addr, byte_mask, size,
                          max_size, sync, sig);
}

__intrinsic void
mem_cmp_write(__xwrite void *data, __mem40 void *addr, unsigned int byte_mask,
              size_t size)
{
    SIGNAL sig;

    __mem_cmp_write(data, addr, byte_mask, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_test_set(__xrw void *data, __mem40 void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD_TEST(test_set, data, addr, size, max_size, sync, sig_pair,
                         2, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_test_set(__xrw void *data, __mem40 void *addr, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_test_set(data, addr, size, size, sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_test_clr(__xrw void *data, __mem40 void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD_TEST(test_clr, data, addr, size, max_size, sync, sig_pair,
                         2, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_test_clr(__xrw void *data, __mem40 void *addr, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_test_clr(data, addr, size, size, sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

 __intrinsic void
__mem_swap(__xrw void *data, __mem40 void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD_TEST(swap, data, addr, size, max_size, sync, sig_pair,
                         2, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_swap(__xrw void *data, __mem40 void *addr, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_swap(data, addr, size, size, sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_test_add(__xrw void *data, __mem40 void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 16);

    _MEM_ATOMIC_CMD_TEST(test_add, data, addr, size, max_size, sync, sig_pair,
                         2, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_test_add(__xrw void *data, __mem40 void *addr, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_test_add(data, addr, size, size, sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_test_sub(__xrw void *data, __mem40 void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 16);

    _MEM_ATOMIC_CMD_TEST(test_sub, data, addr, size, max_size, sync, sig_pair,
                         2, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_test_sub(__xrw void *data, __mem40 void *addr, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_test_sub(data, addr, size, size, sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_test_subsat(__xrw void *data, __mem40 void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 16);

    _MEM_ATOMIC_CMD_TEST(test_subsat, data, addr, size, max_size, sync, sig_pair,
                         2, _MEM_ATOMIC_32_CMD);
}

__intrinsic void
mem_test_subsat(__xrw void *data, __mem40 void *addr, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_test_subsat(data, addr, size, size, sig_done, &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_test_cmp_write(__xrw void *data, __mem40 void *addr,
                     unsigned int byte_mask, size_t size,
                     const size_t max_size, sync_t sync, SIGNAL_PAIR *sig_pair)
{
    try_ctassert(size <= 32);

    _MEM_ATOMIC_CMD_BMASK_TEST(test_mask_compare_write, data, addr, byte_mask,
                               size, max_size, sync, sig_pair);
}

__intrinsic void
mem_test_cmp_write(__xrw void *data, __mem40 void *addr,
                   unsigned int byte_mask, size_t size)
{
    SIGNAL_PAIR sig_pair;

    __mem_test_cmp_write(data, addr, byte_mask, size, size, sig_done,
                         &sig_pair);
    __wait_for_all(&sig_pair);
}

__intrinsic void
__mem_meter(__xrw uint32_t *meter_data, __mem40 void *addr,
            enum meter_mode mode, enum meter_color color,
            SIGNAL_PAIR *sig_pair)
{
    uint32_t addr_hi;
    uint32_t addr_lo;

    try_ctassert(__is_aligned(addr, 8));
    try_ctassert(color <= METER_COLOR_RED);
    try_ctassert((mode == METER_MODE_PEAK) || (mode == METER_MODE_EXCESS));

    addr_hi  = ((uint64_t)addr >> 8) & 0xFF000000;
    addr_lo  =  (uint64_t)addr       & 0xFFFFFFF8;
    addr_lo |= ((color & 0x3) << 1) | (mode & 0x1);

    __asm mem[meter, *meter_data, addr_hi, <<8, addr_lo], sig_done[*sig_pair];
}

__intrinsic enum meter_color
mem_meter(uint32_t meter_amt, __mem40 void *addr, enum meter_mode mode,
          enum meter_color color)
{
    SIGNAL_PAIR sig_pair;
    __xrw uint32_t meter_data = meter_amt;

    __mem_meter(&meter_data, addr, mode, color, &sig_pair);
    __wait_for_all(&sig_pair);

    return meter_data;
}
