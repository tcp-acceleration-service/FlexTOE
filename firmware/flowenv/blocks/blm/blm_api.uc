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
 * @file        blm_api.uc
 * @brief       Microcode API implementation for BLM buffer alloc/free
 */

#ifndef __BLM_BUF_API_UC__
#define __BLM_BUF_API_UC__

#include <ring_utils.uc>
#include "_h/blm_uc.h"

/** @file nfp_buf_api.uc Buffer List Manager Macros
 * @addtogroup blm_api Buffer List Manager
 * @{
 *
 * @name Buffer List Manager Macros
 * @{
 *
 */

/*
 *   Example usage:
 *   NFP_BLM_BUF_POOL_BIND(BLM_NBI8_BLQ0_EMU_QID, BLM_SMALL_PKT,  BLM_NBI8_BLQ0_EMU_Q_LOCALITY, BLM_NBI8_BLQ0_EMU_Q_ISLAND)
 *   NFP_BLM_BUF_POOL_BIND(BLM_NBI8_BLQ1_EMU_QID, BLM_MEDIUM_PKT, BLM_NBI8_BLQ1_EMU_Q_LOCALITY, BLM_NBI8_BLQ1_EMU_Q_ISLAND)
 *   NFP_BLM_BUF_POOL_BIND(BLM_NBI8_BLQ2_EMU_QID, BLM_LARGE_PKT,  BLM_NBI8_BLQ2_EMU_Q_LOCALITY, BLM_NBI8_BLQ2_EMU_Q_ISLAND)
 *   NFP_BLM_BUF_POOL_BIND(BLM_NBI8_BLQ3_EMU_QID, BLM_JUMBO_PKT,  BLM_NBI8_BLQ3_EMU_Q_LOCALITY, BLM_NBI8_BLQ3_EMU_Q_ISLAND)
 */
#macro NFP_BLM_BUF_POOL_BIND(symbol, pool, locality, island)
    .declare_resource blm_qid_/**/pool global 1 symbol
    .alloc_resource pool blm_qid_/**/pool global 1

    #if locality == MU_LOCALITY_HIGH
        #define_eval _BLM_Q_ADDR_HI   ((locality <<30) | (1 <<29) | ((island & 3) <<27))
    #elif locality == MU_LOCALITY_DIRECT_ACCESS
        #define_eval _BLM_Q_ADDR_HI   ((locality <<30) | ((island & 0x3f) <<24))
    #else
    #endif
    .declare_resource blm_q_addr_/**/pool global 1 +_BLM_Q_ADDR_HI
    .alloc_resource pool/**/_q_loc_iid blm_q_addr_/**/pool global 1
    #undef _BLM_Q_ADDR_HI
#endm

/*
 * @param pBuf          Output:Packet buffer Transfer register array. Ex: $x[0]
 * @param pool          Buffer pool to allocate from, synonym for Ring number
 * @param base_pool     This is 'symbol/ring number' for EMU QID corresponding to BLQ-0. This is used
 *                      to deduce QID locality and Island bits.
 * @param len           Number of buffer to allocate
 * @param sig_name      Signal to use
 * @param sig_action    SIG_NONE or SIG_WAIT
 *   Example usage:
 *   In following examples, BLM_SMALL_PKT is the starting QID or base_pool. (It is assumed
 *   that all QID's for BLM are contiguous.
 *   nfp_blm_buf_alloc(a, BLM_SMALL_PKT, BLM_SMALL_PKT, 16, sig_buf, SIG_NONE)
 *   nfp_blm_buf_alloc(b, BLM_SMALL_PKT, BLM_MEDIUM_PKT, 1, sib_buf, SIG_WAIT)
 *   nfp_blm_buf_alloc(c, BLM_SMALL_PKT, BLM_LARGE_PKT, 2, sig_buf, SIG_NONE)
 *   nfp_blm_buf_alloc(d, BLM_SMALL_PKT, BLM_JUMBO_PKT, 8, sig_buf, SIG_NONE)
 */
#macro nfp_blm_buf_alloc(pBuf, base_pool, pool, len, sig_name, sig_action)
.begin
    .reg ir
    .reg q_hi_addr

    move(q_hi_addr, base_pool/**/_q_loc_iid)
    #if (isnum(len))
        #if len < 8
            mem[pop, pBuf, q_hi_addr, <<8, pool, len], sig_done[sig_name]
        #else
            #if len > 15
                #error "Length range error. Valid range = [1-15]"
            #endif
            alu[--, --, b, ((len <<1)|1), <<7] ;override  LEN & override  LEN value
            mem[pop, pBuf, q_hi_addr, <<8, pool, max_/**/len], indirect_ref, sig_done[sig_name]
        #endif
    #else
        alu[ir, 1, OR, len, <<1]
        alu[--, --, b, ir, <<7] ;override  LEN & override  LEN value
        mem[pop, pBuf, q_hi_addr, <<8, pool, max_8], indirect_ref, sig_done[sig_name]
    #endif

    #if (streq('sig_action',SIG_WAIT))
        ctx_arb[sig_name[0]]
    #endif
.end
#endm /* nfp_blm_buf_alloc */

/*
 * @param pBuf          Packet buffer Transfer register array. Ex: $x[0]
 * @param pool          Buffer pool number to return the pBuf to. , synonym for Ring number
 * @param base_pool     This is 'symbol/ring number' for EMU QID corresponding to BLQ-0. This is used
 *                      to deduce QID locality and Island bits.
 * @param len           Number of buffers to free
 * @param sig_name      Signal to use
 * @param sig_action    SIG_NONE or SIG_WAIT
 *
 */
#macro nfp_blm_buf_free(pBuf, base_pool, pool, len, sig_name, sig_action)
.begin
    .reg ir q_hi_addr

    move(q_hi_addr, base_pool/**/_q_loc_iid)
    #if (isnum(len))
        #if len < 8
            mem[put, pBuf, q_hi_addr, <<8, pool, len], sig_done[sig_name]
        #else
            #if len > 15
                #error "Length range error. Valid range = [1-15]"
            #endif
            alu[--, --, b, ((len <<1)|1), <<7] ;override  LEN & override  LEN value
            mem[put, pBuf, q_hi_addr, <<8, pool, max_/**/len], indirect_ref, sig_done[sig_name]
        #endif
    #else
        alu[ir, 1, OR, len, <<1]
        alu[--, --, b, ir, <<7] ;override  LEN & override  LEN value
        mem[put, pBuf, q_hi_addr, <<8, pool, max_8], indirect_ref, sig_done[sig_name]
    #endif

    #if (streq('sig_action',SIG_WAIT))
        ctx_arb[sig_name]
    #endif
.end
#endm /* nfp_blm_buf_free */

/*
 * @param pBuf          Transfer register. Ex: $x[0]
 * @param pool          Buffer pool number to return the pBuf to. , synonym for Ring number
 * @param base_pool     This is 'symbol/ring number' for EMU QID corresponding to BLQ-0. This is used
 *                      to deduce QID locality and Island bits.
 *
 */
#macro nfp_blm_buf_free_j(pBuf, base_pool, pool)
.begin
    .reg ir q_hi_addr
    move(q_hi_addr, base_pool/**/_q_loc_iid)
    #if (isnum(pool) || is_rt_const(pool))
        move(ir, ((pool <<16)|(1 <<3)))
    #else
        alu[--,8, OR, pool, <<16]
    #endif
    mem[fast_journal, --, q_hi_addr, <<8, pBuf], indirect_ref
.end
#endm /* nfp_blm_buf_free_j */

#endif /* __BLM_BUF_API_UC__ */
