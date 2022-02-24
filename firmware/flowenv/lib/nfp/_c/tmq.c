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
 * @file          lib/nfp/tmq.c
 * @brief         TM queue utilities.
 *
 */

#include <nfp6000/nfp_me.h>
#include <nfp6000/nfp_nbi_tm.h>
#include <assert.h>
#include <nfp/xpb.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_atomic.h>
#include <nfp/tmq.h>

#define MAX_NBI_NUMBER      1
#define MAX_TM_QUEUE_NUM    1023
#define MAX_CMD_BURST_SZ    16

/** Base addresses for the NBI TM queue registers. */
#define TMQ_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + NFP_NBI_TM_QUEUE_REG)

/** Address of the TM queue drop count register. */
#define TMQ_DROP_READ_ADDR(_nbi, _qnum)                       \
    (TMQ_XPB_BASE(_nbi) | NFP_NBI_TM_QUEUE_DROP_COUNT(_qnum))

/** Address of the clear-on-read TM queue drop count register. */
#define TMQ_DROP_READ_CLEAR_ADDR(_nbi, _qnum)                       \
    (TMQ_XPB_BASE(_nbi) | NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR(_qnum))

/** Address of the TM queue status register. */
#define TMQ_STATUS_ADDR(_nbi, _qnum)                     \
    (TMQ_XPB_BASE(_nbi) | NFP_NBI_TM_QUEUE_STATUS(qnum))


/* Reads, and optionally clears, a single queue drop counter. */
__intrinsic int
tmq_cnt_read(uint32_t nbi, __gpr uint32_t *counter, uint32_t qnum, int clear)
{
    __gpr uint32_t addr;
    int ret = 0;

    if (nbi > MAX_NBI_NUMBER) {
        ret = -1;
        goto out;
    }

    if (qnum > MAX_TM_QUEUE_NUM) {
        ret = -1;
        goto out;
    }

    if (clear)
        addr = TMQ_DROP_READ_CLEAR_ADDR(nbi, qnum);
    else
        addr = TMQ_DROP_READ_ADDR(nbi, qnum);

    *counter = xpb_read(addr);

out:
    return ret;
}


__intrinsic void
__tmq_status_read(__xread void *status, uint32_t nbi, uint32_t qnum,
                  unsigned int num_qs, sync_t sync, SIGNAL *sig)
{
    uint32_t addr = TMQ_STATUS_ADDR(nbi, qnum);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(qnum < MAX_TM_QUEUE_NUM);

    __xpb_read(status, addr, num_qs << 2, MAX_CMD_BURST_SZ, sync, sig);
}


__intrinsic void
tmq_status_read(__xread void *status, uint32_t nbi, uint32_t qnum,
                unsigned int num_qs)
{
    SIGNAL sig;

    __tmq_status_read(status, nbi, qnum, num_qs, ctx_swap, &sig);
}
