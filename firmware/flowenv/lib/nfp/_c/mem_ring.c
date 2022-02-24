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
 * @file          lib/nfp/_c/mem_ring.c
 * @brief         NFP memory ring interface
 */

#include <assert.h>
#include <nfp.h>
#include <types.h>

#include <nfp6000/nfp_me.h>

#include <nfp/mem_bulk.h>
#include <nfp/mem_ring.h>

struct nfp_memring_t2 {
    unsigned int ring_size:4;
    unsigned int reserved1:2;
    unsigned int head_ptr:24;
    unsigned int eop:1;
    unsigned int zero:1;
    unsigned int tail_ptr:30;
    unsigned int ring_type:2;
    unsigned int q_loc:2;
    unsigned int reserved2:4;
    unsigned int q_page:2;
    unsigned int q_count:24;
    unsigned int reserved3:28;
    unsigned int reserved4:4;
};

__intrinsic mem_ring_addr_t
mem_ring_get_addr(__dram void *base)
{
    unsigned int addr_hi;

    addr_hi = ((unsigned long long int)base >> 8) & 0xff000000;
    return addr_hi;
}

__intrinsic mem_ring_addr_t
mem_ring_setup(unsigned int rnum, __dram void *base, size_t size)
{
    struct nfp_memring_t2 desc_tmp;
    __xrw struct nfp_memring_t2 desc;
    unsigned int entries = size / 4;
    struct nfp_mecsr_prev_alu ind;
    unsigned int addr_hi;

    try_ctassert(rnum < 1024);

    /* Note, size is in bytes and an entry is 32bits */
    ctassert(__is_ct_const(size));
    ctassert(__is_log2(entries));
    ctassert((entries >= (SZ_512)) && (entries <= SZ_16M));

    /* Prepare the ring descriptor information.
     * Mostly can be performed compile time.
     * "base" is probably linker allocated, so unknown at compile time.*/
    desc_tmp.ring_size = __log2(entries) - 9;
    desc_tmp.head_ptr = (((unsigned int) base) >> 2) & 0xffffff;
    desc_tmp.eop = 0;
    desc_tmp.tail_ptr = (((unsigned int) base) >> 2) & 0x3fffffff;
    desc_tmp.ring_type = 2;
    desc_tmp.q_loc = 0; /* Use high locality of reference */
    desc_tmp.q_page = (((unsigned long long) base) >> 32) & 0x3;
    desc_tmp.q_count = 0;
    desc_tmp.reserved1 = 0;
    desc_tmp.reserved2 = 0;
    desc_tmp.reserved3 = 0;
    desc_tmp.reserved4 = 0;
    desc_tmp.zero = 0;
    desc = desc_tmp;

    /* Write descriptor to EMEM (the ring itself is a convenient place),
     * then read back to confirm write completes before continuing. */
    mem_write64(&desc, base, sizeof desc);
    mem_read64(&desc, base, sizeof desc);

    __implicit_read(&desc, sizeof desc);

    /* Read the descriptor into queue array */
    /* "base" must be at least 2k aligned, so low bits are zero. */
    addr_hi = ((unsigned long long int)base >> 8) & 0xffffffff;
    ind.__raw = 0;
    ind.data16 = rnum;
    ind.ove_data = 1;
    __asm {
        alu[--, --, B, ind.__raw];
        mem[rd_qdesc, --, addr_hi, <<8, 0], indirect_ref;
    }

    /* Mask low 24 bits so that addr_hi can be used with rnum
     * in future ring operations. */
    addr_hi &= 0xff000000;
    return addr_hi;
}

__intrinsic mem_ring_addr_t
mem_journal_setup(unsigned int rnum, __dram void *base, size_t size)
{
    return mem_ring_setup(rnum, base, size);
}


__intrinsic mem_ring_addr_t
mem_workq_setup(unsigned int rnum, __dram void *base, size_t size)
{
    return mem_ring_setup(rnum, base, size);
}


#define _MEM_RING_CMD_SIGPAIR(cmdname, data, rnum, raddr, size, max_sz, \
                              sync, sigpair)                            \
    do {                                                                \
        struct nfp_mecsr_prev_alu ind;                                  \
        unsigned int count = (size >> 2);                               \
                                                                        \
        try_ctassert(rnum < 1024);                                      \
        try_ctassert(size != 0);                                        \
        try_ctassert(__is_aligned(size, 4));                            \
        try_ctassert(size <= (16 * 4));                                 \
        ctassert(__is_ct_const(sync));                                  \
        ctassert(sync == sig_done);                                     \
                                                                        \
        if (__is_ct_const(size)) {                                      \
            if (size <= (8*4)) {                                        \
                __asm { mem[cmdname, *data, raddr, <<8, rnum,           \
                            __ct_const_val(count)],                     \
                            sig_done[*__ct_sig_pair(sigpair)] }         \
            } else {                                                    \
                /* Setup length in PrevAlu for the indirect */          \
                ind.__raw = 0;                                          \
                ind.ov_len = 1;                                         \
                ind.length = count - 1;                                 \
                                                                        \
                __asm { alu[--, --, B, ind] }                           \
                __asm { mem[cmdname, *data, raddr, <<8, rnum,           \
                            __ct_const_val(count)], indirect_ref,       \
                            sig_done[*__ct_sig_pair(sigpair)] }         \
            }                                                           \
        } else {                                                        \
            unsigned int max_count = (max_sz >> 2);                     \
                                                                        \
            /* Setup length in PrevAlu for the indirect */              \
            ind.__raw = 0;                                              \
            ind.ov_len = 1;                                             \
            ind.length = count - 1;                                     \
                                                                        \
            __asm { alu[--, --, B, ind] }                               \
            __asm { mem[cmdname, *data, raddr, <<8, rnum,               \
                        __ct_const_val(max_count)], indirect_ref,       \
                        sig_done[*__ct_sig_pair(sigpair)] }             \
        }                                                               \
    } while (0)


#define _MEM_RING_CMD_SIG(cmdname, data, rnum, raddr, size, max_sz, sync, sig) \
    do {                                                                \
        struct nfp_mecsr_prev_alu ind;                                  \
        unsigned int count = (size >> 2);                               \
                                                                        \
        try_ctassert(rnum < 1024);                                      \
        try_ctassert(size != 0);                                        \
        try_ctassert(__is_aligned(size, 4));                            \
        try_ctassert(size <= (16 * 4));                                 \
        ctassert(max_sz <= (16 * 4));                                   \
        ctassert(__is_ct_const(sync));                                  \
        ctassert(sync == sig_done || sync == ctx_swap);                 \
                                                                        \
        if (__is_ct_const(size)) {                                      \
            if (size <= (8*4)) {                                        \
                if (sync == sig_done) {                                 \
                    __asm { mem[cmdname, *data, raddr, <<8, rnum,       \
                                __ct_const_val(count)],                 \
                                sig_done[*sig] }                        \
                } else {                                                \
                    __asm { mem[cmdname, *data, raddr, <<8, rnum,       \
                                __ct_const_val(count)],                 \
                                ctx_swap[*sig] }                        \
                }                                                       \
            } else {                                                    \
                /* Setup length in PrevAlu for the indirect */          \
                ind.__raw = 0;                                          \
                ind.ov_len = 1;                                         \
                ind.length = count - 1;                                 \
                                                                        \
                if (sync == sig_done) {                                 \
                    __asm { alu[--, --, B, ind] }                       \
                    __asm { mem[cmdname, *data, raddr, <<8, rnum,       \
                                __ct_const_val(count)], indirect_ref,   \
                                sig_done[*sig] }                        \
                } else {                                                \
                    __asm { alu[--, --, B, ind] }                       \
                    __asm { mem[cmdname, *data, raddr, <<8, rnum,       \
                                __ct_const_val(count)], indirect_ref,   \
                                ctx_swap[*sig] }                        \
                }                                                       \
            }                                                           \
        } else {                                                        \
            unsigned int max_count = (max_sz >> 2);                     \
                                                                        \
            /* Setup length in PrevAlu for the indirect */              \
            ind.__raw = 0;                                              \
            ind.ov_len = 1;                                             \
            ind.length = count - 1;                                     \
                                                                        \
            if (sync == sig_done) {                                     \
                __asm { alu[--, --, B, ind] }                           \
                __asm { mem[cmdname, *data, raddr, <<8, rnum,           \
                            __ct_const_val(max_count)], indirect_ref,   \
                            sig_done[*sig] }                            \
            } else {                                                    \
                __asm { alu[--, --, B, ind] }                           \
                __asm { mem[cmdname, *data, raddr, <<8, rnum,           \
                            __ct_const_val(max_count)], indirect_ref,   \
                            ctx_swap[*sig] }                            \
            }                                                           \
        }                                                               \
    } while (0)


__intrinsic void
__mem_ring_get(unsigned int rnum, mem_ring_addr_t raddr, __xread void *data,
               size_t size, const size_t max_size, sync_t sync,
               SIGNAL_PAIR *sigpair)
{
    ctassert(__is_read_reg(data));

    _MEM_RING_CMD_SIGPAIR(get, data, rnum, raddr, size, max_size,
                          sync, sigpair);
}

__intrinsic int
mem_ring_get(unsigned int rnum, mem_ring_addr_t raddr, __xread void *data,
             const size_t size)
{
    SIGNAL_PAIR sigpair;

    __mem_ring_get(rnum, raddr, data, size, size, sig_done, &sigpair);

    /* Wait for the completion signal */
    wait_for_all_single(&sigpair.even);

    /* Check for an error signal error signal */
    if (signal_test(&sigpair.odd))
        return -1;

    return 0;
}


__intrinsic void
__mem_ring_get_freely(unsigned int rnum, mem_ring_addr_t raddr,
                      __xread void *data, size_t size, const size_t max_size,
                      sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));

    _MEM_RING_CMD_SIG(get_freely, data, rnum, raddr, size, max_size, sync, sig);
}

__intrinsic void
mem_ring_get_freely(unsigned int rnum, mem_ring_addr_t raddr,
                    __xread void *data, const size_t size)
{
    SIGNAL sig;

    __mem_ring_get_freely(rnum, raddr, data, size, size, ctx_swap, &sig);
}


__intrinsic void
__mem_ring_pop(unsigned int rnum, mem_ring_addr_t raddr,  __xread void *data,
               size_t size, const size_t max_size, sync_t sync,
               SIGNAL_PAIR *sigpair)
{
    ctassert(__is_read_reg(data));

    _MEM_RING_CMD_SIGPAIR(pop, data, rnum, raddr, size, max_size,
                          sync, sigpair);
}

__intrinsic int
mem_ring_pop(unsigned int rnum, mem_ring_addr_t raddr, __xread void *data,
             const size_t size)
{
    SIGNAL_PAIR sigpair;

    __mem_ring_pop(rnum, raddr, data, size, size, sig_done, &sigpair);

    /* Wait for the completion signal */
    wait_for_all_single(&sigpair.even);

    /* Check for an error signal error signal */
    if (signal_test(&sigpair.odd))
        return -1;

    return 0;
}

__intrinsic void
__mem_ring_put(unsigned int rnum, mem_ring_addr_t raddr,  __xrw void *data,
               size_t size, const size_t max_size, sync_t sync,
               SIGNAL_PAIR *sigpair)
{
    ctassert(__is_read_reg(data));
    ctassert(__is_write_reg(data));

    _MEM_RING_CMD_SIGPAIR(put, data, rnum, raddr, size, max_size,
                          sync, sigpair);

    /* mem[put] returns a status word in data[0]. It is the user's
     * responsibility to check this status word when calling this method.
     * mem_ring_put waits on sigpair, and performs the check for the user.*/
}

__intrinsic int
mem_ring_put(unsigned int rnum, mem_ring_addr_t raddr, __xrw void *data,
             const size_t size)
{
    SIGNAL_PAIR sigpair;
    int result;

    __mem_ring_put(rnum, raddr, data, size, size, sig_done, &sigpair);
    wait_for_all(&sigpair);

    result = ((__xread int *) data)[0];
    return (result & (1 << 31)) ? (result << 2) : -1;
}


__intrinsic void
__mem_ring_journal(unsigned int rnum, mem_ring_addr_t raddr,
                   __xwrite void *data, size_t size, const size_t max_size,
                   sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));

    _MEM_RING_CMD_SIG(journal, data, rnum, raddr, size, max_size, sync, sig);
}

__intrinsic void
mem_ring_journal(unsigned int rnum, mem_ring_addr_t raddr, __xwrite void *data,
                 const size_t size)
{
    SIGNAL sig;

    __mem_ring_journal(rnum, raddr, data, size, size, ctx_swap, &sig);
}

__intrinsic void
mem_ring_journal_fast(unsigned int rnum, mem_ring_addr_t raddr,
                      unsigned int value)
{
    struct nfp_mecsr_prev_alu ind;

    try_ctassert(rnum < 1024);

    ind.__raw = 0;
    ind.data16 = rnum;
    ind.ove_data = 1;
    __asm {
        alu[--, --, B, ind.__raw];
        mem[fast_journal,--, raddr, <<8, value], indirect_ref
    }
}


__intrinsic void
__mem_workq_add_work(unsigned int rnum, mem_ring_addr_t raddr,
                     __xwrite void *data, size_t size, const size_t max_size,
                     sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));

    _MEM_RING_CMD_SIG(qadd_work, data, rnum, raddr, size, max_size,
                      sync, sig);
}

__intrinsic void
mem_workq_add_work(unsigned int rnum, mem_ring_addr_t raddr,
                   __xwrite void *data, const size_t size)
{
    SIGNAL sig;

    __mem_workq_add_work(rnum, raddr, data, size, size, ctx_swap, &sig);
}

__intrinsic void mem_workq_add_work_imm(unsigned int rnum, mem_ring_addr_t raddr, unsigned int data)
{
    struct nfp_mecsr_prev_alu ind;

    try_ctassert(rnum < 1024);

    ind.__raw = 0;
    ind.data16 = rnum;
    ind.ove_data = 1;

    __asm {
        alu[--, --, B, ind.__raw];
        mem[qadd_work_imm, --, raddr, <<8, data, 1], indirect_ref
    }
}

__intrinsic void
__mem_workq_add_thread(unsigned int rnum, mem_ring_addr_t raddr,
                       __xread void *data, size_t size, const size_t max_size,
                       sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));

    _MEM_RING_CMD_SIG(qadd_thread, data, rnum, raddr, size, max_size,
                      sync, sig);
}

__intrinsic void
mem_workq_add_thread(unsigned int rnum, mem_ring_addr_t raddr,
                     __xread void *data, const size_t size)
{
    SIGNAL sig;

    __mem_workq_add_thread(rnum, raddr, data, size, size, ctx_swap, &sig);
}


__intrinsic size_t
mem_ring_current_size(unsigned int rnum, mem_ring_addr_t raddr)
{
    __xread struct nfp_memring_t2 desc;
    SIGNAL sig;

    try_ctassert(rnum < 1024);

    __asm mem[push_qdesc, desc, raddr, <<8, rnum], ctx_swap[sig];

    return (size_t) (desc.q_count << 2);
}
