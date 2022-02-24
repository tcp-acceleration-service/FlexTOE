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
 * @file          lib/nfp/_c/mem_bulk.c
 * @brief         NFP memory bulk interface
 */

#include <assert.h>
#include <nfp.h>
#include <types.h>

#include <nfp6000/nfp_me.h>

#include <nfp/mem_bulk.h>


#define _MEM_CMD(cmdname, data, addr, size, max_size, sync, sig, shift) \
do {                                                                    \
    struct nfp_mecsr_prev_alu ind;                                      \
    unsigned int count = (size >> shift);                               \
    unsigned int max_count = (max_size >> shift);                       \
    unsigned int addr_hi, addr_lo;                                      \
                                                                        \
    ctassert(__is_ct_const(sync));                                      \
    try_ctassert(size != 0);                                            \
    ctassert(sync == sig_done || sync == ctx_swap);                     \
                                                                        \
    /* This code is inefficient if addr is >256B aligned, */            \
    /* but will work for 40bit or 32bit pointers. */                    \
                                                                        \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;         \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                \
                                                                        \
    if (__is_ct_const(size)) {                                          \
        if (count <= 8) {                                               \
            if (sync == sig_done) {                                     \
                __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,      \
                            __ct_const_val(count)], sig_done[*sig] }    \
            } else {                                                    \
                __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,      \
                            __ct_const_val(count)], ctx_swap[*sig] }    \
            }                                                           \
        } else {                                                        \
            /* Setup length in PrevAlu for the indirect */              \
            ind.__raw = 0;                                              \
            ind.ov_len = 1;                                             \
            ind.length = count - 1;                                     \
                                                                        \
            if (sync == sig_done) {                                     \
                __asm { alu[--, --, B, ind.__raw] }                     \
                __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,      \
                            __ct_const_val(count)], sig_done[*sig],     \
                            indirect_ref }                              \
            } else {                                                    \
                __asm { alu[--, --, B, ind.__raw] }                     \
                __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,      \
                            __ct_const_val(count)], ctx_swap[*sig],     \
                            indirect_ref }                              \
            }                                                           \
        }                                                               \
    } else {                                                            \
        /* Setup length in PrevAlu for the indirect */                  \
        ind.__raw = 0;                                                  \
        ind.ov_len = 1;                                                 \
        ind.length = count - 1;                                         \
                                                                        \
        if (sync == sig_done) {                                         \
            __asm { alu[--, --, B, ind.__raw] }                         \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,          \
                        __ct_const_val(max_count)], sig_done[*sig],     \
                        indirect_ref }                                  \
        } else {                                                        \
            __asm { alu[--, --, B, ind.__raw] }                         \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,          \
                        __ct_const_val(max_count)], ctx_swap[*sig],     \
                        indirect_ref }                                  \
        }                                                               \
    }                                                                   \
} while (0)

__intrinsic void
__mem_read64(__xread void *data, __mem40 void *addr,
             size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(read, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_read64(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read64(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_read64_le(__xread void *data, __mem40 void *addr,
                size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(read_le, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_read64_le(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read64_le(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_read64_swap(__xread void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(read_swap, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_read64_swap(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read64_swap(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_read64_swap_le(__xread void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(read_swap_le, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_read64_swap_le(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read64_swap_le(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__mem_read32(__xread void *data, __mem40 void *addr,
             size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(read32, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_read32(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read32(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_read32_le(__xread void *data, __mem40 void *addr,
                size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(read32_le, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_read32_le(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read32_le(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_read32_swap(__xread void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(read32_swap, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_read32_swap(__xread void *data, __mem40 void *addr,  const size_t size)
{
    SIGNAL sig;

    __mem_read32_swap(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_read32_swap_le(__xread void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(read32_swap_le, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_read32_swap_le(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read32_swap_le(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__mem_read8(__xread void *data, __mem40 void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    try_ctassert(size <= 32);

    _MEM_CMD(read8, data, addr, size, max_size, sync, sig, 0);
}

__intrinsic void
mem_read8(__xread void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_read8(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__mem_write64(__xwrite void *data, __mem40 void *addr,
              size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(write, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_write64(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write64(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write64_le(__xwrite void *data, __mem40 void *addr,
                 size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(write_le, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_write64_le(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write64_le(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write64_swap(__xwrite void *data, __mem40 void *addr, size_t size,
                   const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(write_swap, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_write64_swap(__xwrite void *data, __mem40 void *addr,  const size_t size)
{
    SIGNAL sig;

    __mem_write64_swap(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write64_swap_le(__xwrite void *data, __mem40 void *addr, size_t size,
                   const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 8));
    try_ctassert(size <= 128);

    _MEM_CMD(write_swap_le, data, addr, size, max_size, sync, sig, 3);
}

__intrinsic void
mem_write64_swap_le(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write64_swap_le(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__mem_write32(__xwrite void *data, __mem40 void *addr,
              size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(write32, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_write32(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write32(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write32_le(__xwrite void *data, __mem40 void *addr,
                 size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(write32_le, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_write32_le(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write32_le(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write32_swap(__xwrite void *data, __mem40 void *addr, size_t size,
                   const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(write32_swap, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_write32_swap(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write32_swap(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write32_swap_le(__xwrite void *data, __mem40 void *addr, size_t size,
                   const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(__is_aligned(size, 4));
    try_ctassert(size <= 128);

    _MEM_CMD(write32_swap_le, data, addr, size, max_size, sync, sig, 2);
}

__intrinsic void
mem_write32_swap_le(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write32_swap_le(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__mem_write8(__xwrite void *data, __mem40 void *addr,
             size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_CMD(write8, data, addr, size, max_size, sync, sig, 0);
}

__intrinsic void
mem_write8(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write8(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write8_le(__xwrite void *data, __mem40 void *addr,
                size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_CMD(write8_le, data, addr, size, max_size, sync, sig, 0);
}

__intrinsic void
mem_write8_le(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write8_le(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write8_swap(__xwrite void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_CMD(write8_swap, data, addr, size, max_size, sync, sig, 0);
}

__intrinsic void
mem_write8_swap(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write8_swap(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__mem_write8_swap_le(__xwrite void *data, __mem40 void *addr,
                  size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    try_ctassert(size <= 32);

    _MEM_CMD(write8_swap_le, data, addr, size, max_size, sync, sig, 0);
}

__intrinsic void
mem_write8_swap_le(__xwrite void *data, __mem40 void *addr, const size_t size)
{
    SIGNAL sig;

    __mem_write8_swap_le(data, addr, size, size, ctx_swap, &sig);
}
