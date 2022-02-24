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
 * @file        libblm.c
 * @brief       Micro-C implementation for BLM buffer alloc/free
 */

#include <assert.h>
#include <nfp/mem_ring.h>
#include <nfp6000/nfp_me.h>

#include "blm.h"

__intrinsic int
__blm_buf_alloc(__xread blm_buf_handle_t *buf, unsigned int blq,
                SIGNAL_PAIR *sigpair, sync_t sync)
{
    return __blm_buf_alloc_bulk(buf, 1, blq, sigpair, sync);
}

__intrinsic int
blm_buf_alloc(__xread blm_buf_handle_t *buf, unsigned int blq)
{
    return blm_buf_alloc_bulk(buf, 1, blq);
}

__intrinsic int
__blm_buf_alloc_bulk(__xread blm_buf_handle_t *bufs, unsigned int count,
                     unsigned int blq, SIGNAL_PAIR *sigpair, sync_t sync)
{
    mem_ring_addr_t raddr_hi;
    unsigned int rnum;

    try_ctassert(blq <= 3);
    try_ctassert(count <= 16);

    /* BLM ring IDs are always allocated in an ordered sequance */
    rnum = blq + BLM_EMU_RING_ID(8,0);
    /* All BLM rings are allocated in the same island */
    raddr_hi = mem_ring_get_addr((__dram void *)BLM_NBI8_BLQ0_EMU_Q_BASE);

    if (sync == sig_done) {
        __mem_ring_pop(rnum, raddr_hi, bufs, (count << 2), 64,
                       sync, sigpair);
    } else {
        __mem_ring_pop(rnum, raddr_hi, bufs, (count << 2), 64,
                       sig_done, sigpair);
        /* Wait for the completion signal */
        wait_for_all_single(&sigpair->even);
        /* Check for an error signal error signal */
        if (signal_test(&sigpair->odd))
            return -1;
    }
    return 0;
}

__intrinsic int
blm_buf_alloc_bulk(__xread blm_buf_handle_t *bufs, unsigned int count,
                   unsigned int blq)
{
    SIGNAL_PAIR sigpair;
    return __blm_buf_alloc_bulk(bufs, count, blq, &sigpair, ctx_swap);
}

__intrinsic void
blm_buf_free(blm_buf_handle_t buf, unsigned int blq)
{
    mem_ring_addr_t raddr_hi;
    unsigned int rnum;

    try_ctassert(blq <= 3);

    /* BLM ring IDs are always allocated in an ordered sequance */
    rnum = blq + BLM_EMU_RING_ID(8,0);
    /* All BLM rings are allocated in the same island */
    raddr_hi = mem_ring_get_addr((__dram void *)BLM_NBI8_BLQ0_EMU_Q_BASE);

    mem_ring_journal_fast(rnum, raddr_hi, buf);
}

__intrinsic void
__blm_buf_free_bulk(__xwrite blm_buf_handle_t *bufs, unsigned int count,
                    unsigned int blq, SIGNAL *sig, sync_t sync)
{
    mem_ring_addr_t raddr_hi;
    unsigned int rnum;

    try_ctassert(blq <= 3);
    try_ctassert(count <= 16);

    /* BLM ring IDs are always allocated in an ordered sequance */
    rnum = blq + BLM_EMU_RING_ID(8,0);
    /* All BLM rings are allocated in the same island */
    raddr_hi = mem_ring_get_addr((__dram void *)BLM_NBI8_BLQ0_EMU_Q_BASE);

    __mem_ring_journal(rnum, raddr_hi, bufs, (count << 2), 64, sync, sig);
}

__intrinsic void
blm_buf_free_bulk(__xwrite blm_buf_handle_t *bufs, unsigned int count,
                  unsigned int blq)
{
    SIGNAL sig;
    __blm_buf_free_bulk(bufs, count, blq, &sig, ctx_swap);
}
