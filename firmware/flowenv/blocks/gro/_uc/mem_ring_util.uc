/*
 * Copyright (C) 2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          me/blocks/gro/_uc/mem_ring_util.uc
 */

#ifndef __MEM_RING_UTIL_UC
#define __MEM_RING_UTIL_UC

#include <stdmac.uc>


#macro mem_ring_declare(NAME, ISL, LEN, ICOUNT)

    // In case the island is an expression
    #define_eval __RING_DECLARE_ISL (ISL)

    .alloc_mem NAME/**/_mem i/**/__RING_DECLARE_ISL.emem global LEN LEN
    .alloc_mem NAME/**/_desc i/**/__RING_DECLARE_ISL.emem global 16 16
    .init NAME/**/_desc+0 ((log2(LEN >> 9) << 28) | (NAME/**/_mem & 0x3fffffc))
    .init NAME/**/_desc+4 ((NAME/**/_mem & 0xFFFFFFFF) | (ICOUNT * 4) | 2)
    .init NAME/**/_desc+8 (ICOUNT)
    .init NAME/**/_desc+12 0

    #undef __RING_DECLARE_ISL

#endm


#macro mem_ring_declare(NAME, ISL, LEN)
    mem_ring_declare(NAME, ISL, LEN, 0)
#endm


#macro mem_ring_declare_generic(ISL, RINGNUM, LEN, ICOUNT)

    // just in case the RINGNUM or ISL is an expression
    #define_eval __RING_GENERIC_ISL (ISL)
    #define_eval __RING_GENERIC_NUM (RINGNUM)
    #define_eval __RING_GENERIC_NAME 'i/**/__RING_GENERIC_ISL/**/_ring/**/__RING_GENERIC_NUM'

    mem_ring_declare(__RING_GENERIC_NAME, ISL, LEN, ICOUNT)

    #undef __RING_GENERIC_NAME
    #undef __RING_GENERIC_NUM
    #undef __RING_GENERIC_ISL

#endm


#macro mem_ring_declare_generic(NAME, ISL, LEN)
    mem_ring_declare_generic(NAME, ISL, LEN, 0)
#endm


#macro mem_ring_init(NAME, RINGNUM)
.begin

    .reg addr_hi
    .reg addr_lo
    .reg iref

    move(addr_hi, (NAME/**/_desc >> 8))
    move(addr_lo, (NAME/**/_desc & 0xFF))

    // See NFP6xxx PRM Section 2.1.6.4.6.2
    // Overriding data reference with ring number.
    move(iref, (RINGNUM))
    alu[ --, (1 << 3), OR, iref, <<16]
    mem[rd_qdesc, --, addr_hi, <<8, addr_lo], indirect_ref

.end
#endm


#macro mem_ring_init_generic(ISL, RINGNUM)

    // just in case the RINGNUM is an expression
    #define_eval __RING_GENERIC_ISL (ISL)
    #define_eval __RING_GENERIC_NUM (RINGNUM)
    #define_eval __RING_GENERIC_NAME 'i/**/__RING_GENERIC_ISL/**/_ring/**/__RING_GENERIC_NUM'

    mem_ring_init(__RING_GENERIC_NAME, __RING_GENERIC_NUM)

    #undef __RING_GENERIC_NAME
    #undef __RING_GENERIC_NUM
    #undef __RING_GENERIC_ISL

#endm


#macro mem_ring_wait_init(NAME)
.begin

    .reg addr_hi
    .reg addr_lo
    .reg read $readback
    .sig mem_ring_wait_sig

    move(addr_hi, (NAME/**/_desc >> 8))
    move(addr_lo, (NAME/**/_desc & 0xFF))
    mem[read32, $readback, addr_hi, <<8, addr_lo, 1], ctx_swap[mem_ring_wait_sig]

.end
#endm


#macro mem_ring_wait_init_generic(ISL, RINGNUM)

    // just in case the RINGNUM is an expression
    #define_eval __RING_GENERIC_ISL (ISL)
    #define_eval __RING_GENERIC_NUM (RINGNUM)
    #define_eval __RING_GENERIC_NAME 'i/**/__RING_GENERIC_ISL/**/_ring/**/__RING_GENERIC_NUM'

    mem_ring_wait_init(__RING_GENERIC_NAME)

    #undef __RING_GENERIC_NAME
    #undef __RING_GENERIC_NUM
    #undef __RING_GENERIC_ISL

#endm


#macro journal_imm(in_ringnum, in_isl, in_data32)
.begin

    .reg iref
    .reg addr_hi

    #if (is_ct_const(in_isl))
        move(addr_hi, ((0x80 | in_isl) << 24))
    #else
        alu[addr_hi, --, B, in_isl, <<24]
        alu[addr_hi, addr_hi, OR, 1, <<31]
    #endif

    #if (is_ct_const(in_data32) && ((in_data32 & 0xFFFFFF00) != 0))
        .reg fast_journal_data_reg
        move(fast_journal_data_reg, in_data32)
        #define_eval __FJ_ADDR_LO fast_journal_data_reg
    #else
        #define_eval __FJ_ADDR_LO in_data32
    #endif

    // See NFP6xxx PRM Section 2.1.6.4.6.2
    // Overriding data reference with ring number.
    move(iref, (in_ringnum))
    alu[--, (1 << 3), OR, iref, <<16]
    mem[fast_journal, --, addr_hi, <<8, __FJ_ADDR_LO], indirect_ref

    #undef __FJ_ADDR_LO

.end
#endm


#macro fast_journal_init(out_addr_reg, out_iref_reg, in_ringnum, in_isl)
.begin

    #if (is_ct_const(in_isl))
        move(out_addr_reg, ((0x80 | in_isl) << 24))
    #else
        alu[out_addr_reg, --, B, in_isl, <<24]
        alu[out_addr_reg, out_addr_reg, OR, 1, <<31]
    #endif

    // See NFP6xxx PRM Section 2.1.6.4.6.2
    // Overriding data reference with ring number.
    move(out_iref_reg, (in_ringnum))
    alu[out_iref_reg, (1 << 3), OR, out_iref_reg<<16]

.end
#endm


#macro fast_journal(in_data32, in_addr_reg, in_iref_reg)
.begin

    #if (is_ct_const(in_data32) && ((in_data32 & 0xFFFFFF00) != 0))
        .reg fast_journal_data_reg
        move(fast_journal_data_reg, in_data32)
        #define_eval __FJ_ADDR_LO fast_journal_data_reg
    #else
        #define_eval __FJ_ADDR_LO in_data32
    #endif

    alu[--, --, B, in_iref_reg]
    mem[fast_journal, --, in_addr_reg, <<8, __FJ_ADDRLO], indirect_ref

    #undef __FJ_ADDR_LO

.end
#endm


#endif /* __MEM_RING_UTIL_UC */
