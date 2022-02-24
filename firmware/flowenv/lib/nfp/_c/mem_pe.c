/*
 * Copyright (C) 2015-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/_c/mem_pe.c
 * @brief         NFP memory packet engine interface
 */

#include <assert.h>
#include <nfp.h>
#include <types.h>

#include <nfp6000/nfp_me.h>

#define _MEM_PE_CMD(cmd, mem_addr, ctm_addr, size, sync, sig, shift)        \
do {                                                                        \
    struct nfp_mecsr_prev_alu alu_ind;                                      \
    struct nfp_mecsr_cmd_indirect_ref_0 csr_ind;                            \
    unsigned int count = (size >> shift);                                   \
    unsigned int addr_hi, addr_lo;                                          \
                                                                            \
    ctassert(__is_ct_const(sync));                                          \
    ctassert(sync == sig_done || sync == ctx_swap);                         \
                                                                            \
    addr_hi = (unsigned long long int)mem_addr >> 32;                       \
    addr_lo = (unsigned long long int)mem_addr & 0xffffffff;                \
                                                                            \
    /* Set up PrevAlu for the indirect */                                   \
    alu_ind.__raw = 0;                                                      \
    alu_ind.ov_len = 1;                                                     \
    alu_ind.ove_data = 2;                                                   \
    alu_ind.ov_bm_csr = 1;                                                  \
    alu_ind.length = count - 1;                                             \
    alu_ind.data16 = ((unsigned int)ctm_addr >> 3) & 0x7fff;                \
                                                                            \
    /* Set up CSR cmd for the indirect */                                   \
    csr_ind.__raw = 0;                                                      \
    csr_ind.byte_mask = addr_hi & 0xff;                                     \
                                                                            \
    /* addr_hi is the CTM island ID (local).  addr_lo is the MU lo addr */  \
    addr_hi = (0x80 | __ISLAND) << 24;                                      \
    if (sync == sig_done) {                                                 \
        local_csr_write(local_csr_cmd_indirect_ref_0, csr_ind.__raw);       \
        __asm { alu[--, --, B, alu_ind.__raw] }                             \
        __asm { mem[cmd, --, addr_hi, <<8, addr_lo, --],                    \
                    sig_done[*sig], indirect_ref }                          \
    } else {                                                                \
        local_csr_write(local_csr_cmd_indirect_ref_0, csr_ind.__raw);       \
        __asm { alu[--, --, B, alu_ind.__raw] }                             \
        __asm { mem[cmd, --, addr_hi, <<8, addr_lo, --],                    \
                    ctx_swap[*sig], indirect_ref }                          \
    }                                                                       \
} while (0)


__intrinsic void
__mem_pe_dma_mu_to_ctm(__ctm40 void *ctm_addr, __mem40 void *mem_addr,
                       size_t size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(__is_aligned(size, 64));
    try_ctassert(size <= 2048);

    _MEM_PE_CMD(pe_dma_from_memory_buffer, mem_addr, ctm_addr,
                size, sync, sig, 6);
}

__intrinsic void
mem_pe_dma_mu_to_ctm(__ctm40 void *ctm_addr, __mem40 void *mem_addr,
                     size_t size)
{
    SIGNAL sig;

    __mem_pe_dma_mu_to_ctm(ctm_addr, mem_addr, size, ctx_swap, &sig);
}

__intrinsic void
__mem_pe_dma_ctm_to_mu(__mem40 void *mem_addr, __ctm40 void *ctm_addr,
                       size_t size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(__is_aligned(size, 64));
    try_ctassert(size <= 2048);

    _MEM_PE_CMD(pe_dma_to_memory_buffer, mem_addr, ctm_addr,
                size, sync, sig, 6);
}

__intrinsic void
mem_pe_dma_ctm_to_mu(__mem40 void *mem_addr, __ctm40 void *ctm_addr,
                     size_t size)
{
    SIGNAL sig;

    __mem_pe_dma_ctm_to_mu(mem_addr, ctm_addr, size, ctx_swap, &sig);
}

__intrinsic void
__mem_pe_dma_ctm_to_mu_packet(__mem40 void *mem_addr, unsigned int pnum,
                              unsigned int offset, sync_t sync, SIGNAL *sig)
{
    struct nfp_mecsr_prev_alu alu_ind;
    struct nfp_mecsr_cmd_indirect_ref_0 csr_ind;
    unsigned int addr_hi, addr_lo;
    uint64_t addr;
    unsigned int copy_offset;

    try_ctassert(pnum <= 1024);
    try_ctassert(offset <= 2048);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    copy_offset = (offset >> 3) & 0xff;
    addr = ((uint64_t) mem_addr) >> 11; /* MU buffer is 2K aligned */
    addr_hi = (addr >> 24) & 0x1f;
    addr_lo = (addr << 8) | copy_offset;

    /* Set up PrevAlu for the indirect */
    alu_ind.__raw = 0;
    alu_ind.ov_len = 1;
    alu_ind.ove_data = 2;
    alu_ind.ov_bm_csr = 1;
    alu_ind.length = addr_hi;
    alu_ind.data16 = pnum;

    /* Set up CSR cmd for the indirect */
    csr_ind.__raw = 0;
    csr_ind.byte_mask = copy_offset;

    if (sync == sig_done) {
        __asm {
            local_csr_wr[local_csr_cmd_indirect_ref_0, csr_ind.__raw];
            alu[--, --, B, alu_ind.__raw];
            mem[pe_dma_to_memory_packet, --, addr_lo, 0, <<8, 1], indirect_ref, sig_done[*sig];
        }
    } else {
        __asm {
            local_csr_wr[local_csr_cmd_indirect_ref_0, csr_ind.__raw];
            alu[--, --, B, alu_ind.__raw];
            mem[pe_dma_to_memory_packet, --, addr_lo, 0, <<8, 1], indirect_ref, ctx_swap[*sig];
        }
    }
}

__intrinsic void
mem_pe_dma_ctm_to_mu_packet(__mem40 void *mem_addr, unsigned int pnum,
                              unsigned int offset)
{
    SIGNAL sig;
    __mem_pe_dma_ctm_to_mu_packet(mem_addr, pnum, offset, ctx_swap, &sig);
}
