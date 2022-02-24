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
 * @file        blm_main.uc
 * @brief
 */

#ifndef __BLM_UC__
#define __BLM_UC__

#include <aggregate.uc>
#include <nfp_chipres.h>

#include <ring_utils.uc>
#include <event_autopush.uc>

#include "_h/blm_uc.h"
#include "_h/blm_internal.h"

#include "_uc/blm_nbi.uc"
#include "blm_api.uc"


#ifndef ALARM_TICKS
#define ALARM_TICKS 32
#endif

/*
 * Pre-Load Error checks
 */
 blm_compatability_check()

.alloc_mem CTM_RESERVED                   ctm+0             island  0x20000     reserved

/* Mem Reserved for BLM use */
.alloc_mem BLM_INFO_SECTION_BASE        i0.ctm                 island   64 8
#ifdef NULL_RECYCLE_FROM_EMU
    .alloc_mem BLM_INGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID    i24.emem             global  128 64
    .alloc_mem BLM_EGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID     i24.emem             global  128 64
#else
    .alloc_mem BLM_INGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID    i0.ctm                 island  128 8
    .alloc_mem BLM_EGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID     i0.ctm                 island  128 8
#endif

/* Allocate BLM Stats memory for 4 BLQ's in each BLM instance Island. */
#for id [CTM_NBI_BLQ0_STATS_BASE, CTM_NBI_BLQ1_STATS_BASE, CTM_NBI_BLQ2_STATS_BASE, CTM_NBI_BLQ3_STATS_BASE]
    .alloc_mem id        i0.ctm                 island  BLQ_STATS_SIZE 8
#endloop

#ifndef SINGLE_NBI
    #define BLM_NBI_MODE        BLM_DUAL_NBI_MODE

    /* mask/match bit pair: 2b00 (ignore), 2b01 (at least one), 2b10 (must be clear), or 2b11 (must be set) */
    /* [23:18]= provided ID(NBI), [17:16] = Sub-Id(DMA/TM), [15:4] = Evnt data, [3:0] = Evnt type(5) */
    /* EvtType:5 from Island:8 and Sub-ID:1 (TM) */
    /* EvtType:5 from Island:8 and Sub-ID:0 (DMA) */

    /* Both NBI's share same EMU rings for packet buffers
     * In this mode one BLM instance handles 2 BLQ's from NBI-8 and 2 BLQ's from NBI-9, thus sharing CTM cache.
     * This mode helps in times where traffic is constantly flowing from NBI-8 to NBI-9 only.
     * In normal mode, above traffic pattern causes:
     * 1) Ingress BLM cache to exhaust as there is no re-fill from NBI-8 egress TM.
     * 2) Egress BLM cache always full as no buffers are re-cycled to NBI-9 ingress
     * For both of the above scenarios, BLM has to constantly fetch buffers from EMU ring to fill the cache and
     * constantly push buffers from TM to EMU ring as local cache is full. This leads to more EMU accessess and
     * also increases BLM latency in recycling buffers and may impacet performance.
     *
     * In Split mode, above case is not an issue as cache is constantly filled from egress side events and mostly available for ingress events.
     * DONT USE THIS NOW - This potentially can cause 200GE worth of traffic to single BLM instance.
     * Since BLM has to serve 2 BLQ's from each NBI, two small packet BLQ's end up to be served by sam BLM Instance.
     * This means, BLM now has to meet 200GE rates for small packets sourcing from NBI. In addition any traffic from NFD etc.
     * Since the traffic in above problem statement is only half the chip bandwidth (uni-directional 100GE from NBI-X to NBI-y)
     * EMU accesses may be fine and justified.
     *
     * To handle above worst case, BLM implements following logic to improve performance.
     * To allieavate EMU accesses and also optimize cache access, BLM does not replenish buffers to ingress side immediately upon receiving an event.
     * Instead it queue's 'N'(configurable) events from ingress(dma) side. Doing so, when a TM BLQ event arrives, BLM checks for pending events from DMA BLQ and
     * if any present, BLM issues direct buffer transfer from NBI to NBI. This increases the chances of pushing buffers directly from
     * NBI-TM to NBI-DMA upon receiving an event from TM and so keeps cache usage to minimum and in trun low EMU access.
     */
    #ifdef BLM_SPLIT_BLQ_MODE
        #define BLM_BLQ_OP_MODE     BLM_BLQ_SPLIT_MODE

        /* Allocate CTM Cache memory for 2 BLQ's in each BLM instance Island. */
		#for id [0,1]
			#define_eval __CACHE_ID     BLM_NBI_BLQ/**/id/**/_CACHE_BASE
			#define_eval __CACHE_SIZE   (BLM_NBI_BLQ/**/id/**/_CACHE_SIZE * 4)
			.alloc_mem __CACHE_ID        i0.ctm                 island  __CACHE_SIZE 8
		#endloop

        #define BLQ_OFFSET          (BLM_INSTANCE_ID * 2)
        // Ingress Filter
        #define CTX0_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x0) <<14 | 8 <<18)
        #define CTX2_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x1) <<14 | 8 <<18)
        #define CTX4_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x0) <<14 | 9 <<18)
        #define CTX6_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x1) <<14 | 9 <<18)

        // Egress Filter
        #define CTX1_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x4) <<14 | 8 <<18)
        #define CTX3_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x5) <<14 | 8 <<18)
        #define CTX5_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x4) <<14 | 9 <<18)
        #define CTX7_FILTER_MATCH   (0x5 | (BLQ_OFFSET + 0x5) <<14 | 9 <<18)
        #define BLM_BLQ_OP_MODE     1
    #else /* BLM_SPLIT_BLQ_MODE */
        #define BLM_BLQ_OP_MODE     BLM_BLQ_NON_SPLIT_MODE
        /* Allocate CTM Cache memory for all BLQ's */
		#for id [0,1,2,3]
			#define_eval __CACHE_ID     BLM_NBI_BLQ/**/id/**/_CACHE_BASE
			#define_eval __CACHE_SIZE   (BLM_NBI_BLQ/**/id/**/_CACHE_SIZE * 4)
			.alloc_mem __CACHE_ID        i0.ctm                 island  __CACHE_SIZE 8
		#endloop

        // Ingress Filter
        #define CTX0_FILTER_MATCH   (0x5 | 0x0 <<14 | NBII <<18)
        #define CTX2_FILTER_MATCH   (0x5 | 0x1 <<14 | NBII <<18)
        #define CTX4_FILTER_MATCH   (0x5 | 0x2 <<14 | NBII <<18)
        #define CTX6_FILTER_MATCH   (0x5 | 0x3 <<14 | NBII <<18)

        // Egress Filter
        #define CTX1_FILTER_MATCH   (0x5 | 0x4 <<14 | NBII <<18)
        #define CTX3_FILTER_MATCH   (0x5 | 0x5 <<14 | NBII <<18)
        #define CTX5_FILTER_MATCH   (0x5 | 0x6 <<14 | NBII <<18)
        #define CTX7_FILTER_MATCH   (0x5 | 0x7 <<14 | NBII <<18)
    #endif /* BLM_SPLIT_BLQ_MODE */
#else /* SINGLE_NBI */
    #define BLM_NBI_MODE        BLM_SINGLE_NBI_MODE

    /* Allocate CTM Cache memory for all BLQ's */
    #for id [0,1,2,3]
        #define_eval __CACHE_ID     BLM_NBI_BLQ/**/id/**/_CACHE_BASE
        #define_eval __CACHE_SIZE   (BLM_NBI_BLQ/**/id/**/_CACHE_SIZE * 4)
        .alloc_mem __CACHE_ID        i0.ctm                 island  __CACHE_SIZE 8
    #endloop

    /* CLS Autopush filter match per context */
    #define CTX0_FILTER_MATCH   (0x5 | 0x0 <<14 | NBII <<18)
    #define CTX2_FILTER_MATCH   (0x5 | 0x1 <<14 | NBII <<18)
    #define CTX4_FILTER_MATCH   (0x5 | 0x2 <<14 | NBII <<18)
    #define CTX6_FILTER_MATCH   (0x5 | 0x3 <<14 | NBII <<18)

    /* bit[17:16] = Sub-Id. For TM sub_id = 0x1 */
    #define CTX1_FILTER_MATCH   (0x5 | 0x4 <<14 | NBII <<18)
    #define CTX3_FILTER_MATCH   (0x5 | 0x5 <<14 | NBII <<18)
    #define CTX5_FILTER_MATCH   (0x5 | 0x6 <<14 | NBII <<18)
    #define CTX7_FILTER_MATCH   (0x5 | 0x7 <<14 | NBII <<18)

#endif /* SINGLE_NBI */
#undef __CACHE_ID
#undef __CACHE_SIZE

#for id [BLQ0_DESC_LMEM_BASE,BLQ1_DESC_LMEM_BASE,BLQ2_DESC_LMEM_BASE,BLQ3_DESC_LMEM_BASE]
        .alloc_mem id lmem me 32 4 addr32
#endloop


/*
 *
 */
#macro alarm(in_ticks, alarm_sig)
.begin

    .reg next

    local_csr_wr[ACTIVE_FUTURE_COUNT_SIGNAL, &alarm_sig]
    local_csr_rd[TIMESTAMP_LOW]
    immed[next, 0]
    alu[next, next, +, in_ticks]
    local_csr_wr[ACTIVE_CTX_FUTURE_COUNT, next]

.end
#endm

/*
 *
 */
#macro blm_init_mailbox()
    #ifdef THSDK_558_FIX
        #for id [0,1,2,3]
            .init_csr mecsr:Mailbox/**/id 0x0 volatile
        #endloop
    #else
        .begin
        .reg zero
        immed[zero, 0]
        local_csr_wr[Mailbox0, zero]
        local_csr_wr[Mailbox1, ALARM_TICKS]
        local_csr_wr[Mailbox2, zero]
        local_csr_wr[Mailbox3, zero]
        .end
    #endif /* THSDK_558_FIX */
#endm /* blm_init_mailbox */

/*
 * Write BLM info to section header
 */
#macro blm_init_info_section()
    /* Write BLM info */
    .init BLM_INFO_SECTION_BASE+0       (BLM_INFO_SECTION_BASE >>8)
    .init BLM_INFO_SECTION_BASE+4       (__MEID)
    .init BLM_INFO_SECTION_BASE+8       (BLM_BLQ_ENABLE_MASK | (BLM_BLQ_NULL_RECYCLE_MASK <<8))
    #if (streq(TH_12713,'ZERO_LEN'))
    #define_eval _BLM_INFO_TH12713  ((1 <<4) | 1)
    #elif (streq(TH_12713,'NBI_READ'))
    #define_eval _BLM_INFO_TH12713  ((1 <<4) | 2)
    #else
    #define_eval _BLM_INFO_TH12713  0
    #endif
    .init BLM_INFO_SECTION_BASE+12      _BLM_INFO_TH12713
    #undef _BLM_INFO_TH12713
    .init BLM_INFO_SECTION_BASE+16      (NBI/**/NBII/**/_BLQ_EMU_0_PKTBUF_SIZE | (NBI/**/NBII/**/_BLQ_EMU_1_PKTBUF_SIZE << 16))
    .init BLM_INFO_SECTION_BASE+20      (NBI/**/NBII/**/_BLQ_EMU_2_PKTBUF_SIZE | (NBI/**/NBII/**/_BLQ_EMU_3_PKTBUF_SIZE << 16))
#endm /* blm_info_section */

/*
 * Intialize LM index register
 */
#macro blm_init_blq_lm_index(lm_base)
    #ifdef THSDK_558_FIX
        /* Intialize per context LM Addresses.
         * Ingress BLQ and egress BLQ contexts(even-odd pair) share same LM Address
         */
        .init_csr mecsr:ctx0.IndLMAddr0 BLQ0_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx1.IndLMAddr0 BLQ0_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx2.IndLMAddr0 BLQ1_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx3.IndLMAddr0 BLQ1_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx4.IndLMAddr0 BLQ2_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx5.IndLMAddr0 BLQ2_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx6.IndLMAddr0 BLQ3_DESC_LMEM_BASE volatile
        .init_csr mecsr:ctx7.IndLMAddr0 BLQ3_DESC_LMEM_BASE volatile

        #for id [0,1,2,3]
            .init BLQ/**/id/**/_DESC_LMEM_BASE+BLM_LM_BLQ_CACHE_ADDR_OFFSET BLM_NBI_BLQ/**/id/**/_CACHE_BASE
            .init BLQ/**/id/**/_DESC_LMEM_BASE+BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET 0
            .init BLQ/**/id/**/_DESC_LMEM_BASE+BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET 0
        #endloop
    #else
        .begin
        .reg tmp
        immed[tmp, lm_base]
        local_csr_wr[BLM_BLQ_LM_CSR, tmp]
        nop
        nop
        nop
        .end
    #endif
#endm /* blm_init_blq_lm_index */

/*
 * Intialize LM
 */
#macro blm_init_lm(lmaddr, blq, blq_cache_base)
    #ifndef THSDK_558_FIX
        move(lmaddr[BLM_LM_BLQ_CACHE_ADDR_OFFSET], (blq_cache_base & 0xFFFFFFFF))
        immed[lmaddr[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], 0]
        immed[lmaddr[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET], 0]
        immed[lmaddr[BLM_LM_BLQ_CACHE_RDWR_BUSY_OFFSET], 0]
    #endif
        #define_eval INGRESS_BLQ_NULL_RECYCLE   ((BLM_BLQ_NULL_RECYCLE_MASK >>blq) & 1)
        #define_eval EGRESS_BLQ_NULL_RECYCLE    ((BLM_BLQ_NULL_RECYCLE_MASK >>(4+blq)) & 1)
        immed[lmaddr[BLM_LM_BLQ_NULL_RECYCLE_OFFSET], (INGRESS_BLQ_NULL_RECYCLE | (EGRESS_BLQ_NULL_RECYCLE <<1))]
        #undef INGRESS_BLQ_NULL_RECYCLE
        #undef EGRESS_BLQ_NULL_RECYCLE
#endm /* blm_init_lm */

/*
 * Check if any BLQ events need to be ignored by BLM.
 */
#macro blm_is_blq_enable(blq, _label)
    #define_eval _bit_MASK       (1 << blq)
    .if ( BLM_BLQ_ENABLE_MASK & _bit_MASK )
        br[_label]
    .else
        ctx_arb[kill]
    .endif
    #undef _bit_MASK
#endm /* blm_is_blq_enable */

/*
 * Macro to pull buffers from Egress(TM) side to EMU ring
 */
#macro blm_egress_pull_buffers_to_emu_ring(NbiNum, blq, addr, ringid)
    #if (__REVISION_MIN < __REVISION_B0)
        #ifndef TH_12713
            #define TH_12713    NBI_READ
        #endif
        #if (streq(TH_12713,'ZERO_LEN'))
            #define_eval _MAX_PULL_DATA_LEN     2
            #define_eval __ITER     (NBI_BLQ_EVENT_THRESHOLD / _MAX_PULL_DATA_LEN)
            #define_eval __LOOP 0
            #while __LOOP < __ITER
                blm_pop2emu_ring(NbiNum, blq, addr, ringid, _MAX_PULL_DATA_LEN)
                #define_eval __LOOP (__LOOP + 1)
            #endloop
            #undef __LOOP
            #undef __ITER
            #undef _MAX_PULL_DATA_LEN
        #elif (streq(TH_12713,'NBI_READ'))
            #define_eval _MAX_PULL_DATA_LEN     16
            .sig sig_pull_buf2emu
            #define_eval _MAX_ITER     (NBI_BLQ_EVENT_THRESHOLD /_MAX_PULL_DATA_LEN)
            #define_eval _LOOP_ITER    0
            #while _LOOP_ITER < _MAX_ITER
                blm_poptome(NbiNum, blq, $nbitmbuf[0], (_MAX_PULL_DATA_LEN >>1), sig_pull_buf2emu, SIG_WAIT)
                aggregate_copy($nbitmbuf, $nbitmbuf, 16)
                alu[--, --, b, ((0xf <<1)|1), <<7]
                mem[journal, $nbitmbuf[0], addr, <<8, ringid, max_15], indirect_ref, sig_done[sig_pull_buf2emu]
                ctx_arb[sig_pull_buf2emu]
                #define_eval _LOOP_ITER     (_LOOP_ITER + 1)
            #endloop
            #undef _LOOP_ITER
            #undef _MAX_PULL_DATA_LEN
            #undef _MAX_ITER
        #else
            #define_eval _MAX_PULL_DATA_LEN     16
            #define_eval __ITER     (NBI_BLQ_EVENT_THRESHOLD / _MAX_PULL_DATA_LEN)
            #define_eval __LOOP 0
            #while __LOOP < __ITER
                blm_pop2emu_ring(NbiNum, blq, addr, ringid, _MAX_PULL_DATA_LEN)
                #define_eval __LOOP (__LOOP + 1)
            #endloop
            #undef __LOOP
            #undef __ITER
            #undef _MAX_PULL_DATA_LEN
        #endif
    #else
        #define_eval _MAX_PULL_DATA_LEN     16
        #define_eval __ITER     (NBI_BLQ_EVENT_THRESHOLD / _MAX_PULL_DATA_LEN)
        #define_eval __LOOP 0
        #while __LOOP < __ITER
            blm_pop2emu_ring(NbiNum, blq, addr, ringid, _MAX_PULL_DATA_LEN)
            #define_eval __LOOP (__LOOP + 1)
        #endloop
        #undef __LOOP
        #undef __ITER
        #undef _MAX_PULL_DATA_LEN
    #endif
#endm /* blm_egress_pull_buffers_to_emu_ring */

/*
 *
 */
#macro blm_cache_release_lock()
    alu[BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_RDWR_BUSY_OFFSET], --, b, 0]
#endm

/*
 *
 */
#macro blm_cache_acquire_lock()
    .while (BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_RDWR_BUSY_OFFSET])
        ctx_arb[voluntary]
    .endw
    alu[BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_RDWR_BUSY_OFFSET], --, b, 1]
#endm /* blm_cache_acquire_lock */

/*
 *
 */
#macro blm_egress_pull_buffers_to_cache(NbiNum, blq, n)
.begin
    .reg _addr
    .reg _offset
    .reg $dummy[2]
    .sig pop_complete_sig
    .xfer_order $dummy
    blm_cache_acquire_lock()
    move(_addr, (__ADDR_I0_CTM >>8) & 0xFF000000)
    #if (__REVISION_MIN < __REVISION_B0)
        #ifndef TH_12713
            #define TH_12713    NBI_READ
        #endif
        #if (streq(TH_12713,'ZERO_LEN'))
            #define_eval _MAX_PULL_DATA_LEN     2
            #define_eval _MAX_ITER     (n/_MAX_PULL_DATA_LEN)
            #define_eval _LOOP_ITER    0
            #while _LOOP_ITER < _MAX_ITER
                alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], <<2]
                alu[_offset, _offset, +, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
                blm_poptomem(NbiNum, blq, _addr, _offset, _MAX_PULL_DATA_LEN)
                blm_incr_cache_cnt(_MAX_PULL_DATA_LEN)
                #define_eval _LOOP_ITER     (_LOOP_ITER + 1)
            #endloop
            #undef _LOOP_ITER
            #undef _MAX_PULL_DATA_LEN
            #undef _MAX_ITER
            alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
            mem[read, $dummy[0], _addr, <<8, _offset, 1],sig_done[pop_complete_sig]
            ctx_arb[pop_complete_sig]
        #elif (streq(TH_12713,'NBI_READ'))
            .sig sig_pull_buf2cache
            #define_eval _MAX_PULL_DATA_LEN     16
            .sig sig_pull_buf2emu
            #define_eval _MAX_ITER     (n/_MAX_PULL_DATA_LEN)
            #define_eval _LOOP_ITER    0
            #while _LOOP_ITER < _MAX_ITER
                blm_poptome(NbiNum, blq, $nbitmbuf[0], (_MAX_PULL_DATA_LEN >>1), sig_pull_buf2emu, SIG_WAIT)
                aggregate_copy($nbitmbuf, $nbitmbuf, 16)
                alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], <<2]
                alu[_offset, _offset, +, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
                mem[write, $nbitmbuf[0], _addr, <<8, _offset, 8], sig_done[sig_pull_buf2cache]
                ctx_arb[sig_pull_buf2cache]
                blm_incr_cache_cnt(_MAX_PULL_DATA_LEN)
                #define_eval _LOOP_ITER     (_LOOP_ITER + 1)
            #endloop
            #undef _LOOP_ITER
            #undef _MAX_PULL_DATA_LEN
            #undef _MAX_ITER
        #else
            #define_eval _MAX_PULL_DATA_LEN     n
            #define_eval _MAX_ITER     (n/_MAX_PULL_DATA_LEN)
            #define_eval _LOOP_ITER    0
            #while _LOOP_ITER < _MAX_ITER
                alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], <<2]
                alu[_offset, _offset, +, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
                blm_poptomem(NbiNum, blq, _addr, _offset, _MAX_PULL_DATA_LEN)
                blm_incr_cache_cnt(_MAX_PULL_DATA_LEN)
                #define_eval _LOOP_ITER     (_LOOP_ITER + 1)
            #endloop
            #undef _LOOP_ITER
            #undef _MAX_PULL_DATA_LEN
            #undef _MAX_ITER
            alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
            mem[read, $dummy[0], _addr, <<8, _offset, 1],sig_done[pop_complete_sig]
            ctx_arb[pop_complete_sig]
        #endif
    #else
        #define_eval _MAX_PULL_DATA_LEN     n
        #define_eval _MAX_ITER     (n/_MAX_PULL_DATA_LEN)
        #define_eval _LOOP_ITER    0
        #while _LOOP_ITER < _MAX_ITER
            alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], <<2]
            alu[_offset, _offset, +, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
            blm_poptomem(NbiNum, blq, _addr, _offset, _MAX_PULL_DATA_LEN)
            blm_incr_cache_cnt(_MAX_PULL_DATA_LEN)
            #define_eval _LOOP_ITER     (_LOOP_ITER + 1)
        #endloop
        #undef _LOOP_ITER
        #undef _MAX_PULL_DATA_LEN
        #undef _MAX_ITER
        alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
        mem[read, $dummy[0], _addr, <<8, _offset, 1],sig_done[pop_complete_sig]
        ctx_arb[pop_complete_sig]
    #endif
    blm_cache_release_lock()
.end
#endm /* blm_egress_pull_buffers_to_cache */

/*
 *
 */
#macro blm_ingress_push_buffers_from_cache(NbiNum, blq, n)
.begin
    .reg _addr
    .reg _offset
    .reg $dummy[2]
    .sig rd_complete_sig
    .xfer_order $dummy
    blm_cache_acquire_lock()
    blm_decr_cache_cnt(n)
    move(_addr, (__ADDR_I0_CTM >>8) & 0xFF000000)
    alu[_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], <<2]
    alu[_offset, _offset, +, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
    blm_push_mem2dma(NbiNum, blq, _addr, _offset, n, 1)
    blm_cache_release_lock()
.end
#endm /* blm_ingress_push_buffers_from_cache */

/*
 *
 */
#macro blm_egress_null_buffer_recycle(NbiNum, blq, n)
.begin
    .reg _addr
    .reg _offset
    move(_addr, ((BLM_EGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID >>8) & 0xFF000000))
    move(_offset, ((BLM_EGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID) & 0xFFFFFFFF))
    #if (__REVISION_MIN < __REVISION_B0)
        #define_eval _MAX_PULL_DATA_LEN     2
    #else
        #define_eval _MAX_PULL_DATA_LEN     n
    #endif
    #define_eval _MAX_ITER     (n/_MAX_PULL_DATA_LEN)
    #define_eval _LOOP_ITER    0
    #while _LOOP_ITER < _MAX_ITER
        blm_poptomem(NbiNum, blq, _addr, _offset, _MAX_PULL_DATA_LEN)
        #define_eval _LOOP_ITER     (_LOOP_ITER + 1)
    #endloop
    #undef _LOOP_ITER
    #undef _MAX_PULL_DATA_LEN
    #undef _MAX_ITER
.end
#endm /* blm_egress_null_buffer_recycle */

/*
 *
 */
#macro blm_ingress_null_buffer_recycle(NbiNum, blq, n)
.begin
    .reg _addr
    .reg _offset
    move(_addr, ((BLM_INGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID >>8) & 0xFF000000))
    move(_offset, ((BLM_INGRESS_NULL_BUF_RECYCLE_BASE_/**/BLM_INSTANCE_ID) & 0xFFFFFFFF))
    blm_push_mem2dma(NbiNum, blq, _addr, _offset, n, 1)
.end
#endm /* blm_ingress_null_buffer_recycle */

/*
 *
 */
#macro blm_incr_dma_evnt_pend_cnt()
    alu[BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET], BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET], +, 1]
#endm /* blm_incr_dma_evnt_pend_cnt */

/*
 *
 */
#macro blm_decr_dma_evnt_pend_cnt()
    alu[BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET], BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET], -, 1]
#endm /* blm_decr_dma_evnt_pend_cnt */

/*
 *
 */
#macro blm_decr_cache_cnt(n)
    alu[BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], -, n]
#endm /* blm_decr_cache_cnt */

/*
 *
 */
#macro blm_incr_cache_cnt(n)
    alu[BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], +, n]
#endm /* blm_incr_cache_cnt */

/*
 *
 */
#macro blm_cache_fill(addr, ringid, sig_memget0, EMU_RING_UFLOW_LABEL)
.begin
    .reg cache_base
    .reg cache_offset
    .sig sig_cache_fill

    alu[--, --, b, ((0xf <<1)|1), <<7]
    mem[pop, $nbidmabuf[0], addr, <<8, ringid, max_16], sig_done[sig_memget0], indirect_ref
    ctx_arb[sig_memget0[0]]
    /* if there are not enough buffers in MU ring, skip cache fill */
    br_signal[sig_memget0[1], emu_ring_underflow#]
    .io_completed sig_memget0

    D(MAILBOX3, 0x3334)

    aggregate_copy($nbidmabuf, $nbidmabuf, 16)
    blm_cache_acquire_lock()
    alu[cache_offset, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET], <<2]
    alu[cache_offset, cache_offset, +, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ADDR_OFFSET]]
    mem[write, $nbidmabuf[0], 0, <<8,  cache_offset, 8], sig_done[sig_cache_fill]
    ctx_arb[sig_cache_fill]
    blm_incr_cache_cnt(16)
    blm_cache_release_lock()
    blm_stats(BLM_STATS_CACHE_REFILLS)
    br[end_cache_fill#]
emu_ring_underflow#:
    blm_stats(BLM_STATS_EMU_RING_UNDERFLOW)
    br[EMU_RING_UFLOW_LABEL]
end_cache_fill#:
.end
#endm /* blm_cache_fill */

/*
 *
 */
#macro blm_init_stats()
#ifdef BLM_STATS_CLEAR
.begin
    .reg $x[16]
    .reg addrhi
    .reg temp
    .xfer_order $x
    .sig ctm_sig
    aggregate_zero($x, 16)
   /* clear counters */
    //immed[addrhi, __ADDR_CTM]
    immed[addrhi, 0]
    mem[write, $x[0], addrhi, <<8, blq_stats_base, 8], sig_done[ctm_sig]
    wait#:
    br_!signal[ctm_sig, wait#]
    nop
    nop
    nop
    alu[temp, blq_stats_base, +, 64]
    mem[write, $x[0], addrhi, <<8, temp, 8], sig_done[ctm_sig]
    wait1#:
    br_!signal[ctm_sig, wait1#]
.end
#endif
#endm /* blm_init_stats */

/*
 *
 */
#macro blm_stats(COUNTER)
.begin
    .reg _addr
    .reg addrhi

    alu[_addr, blq_stats_base, +, COUNTER]
    immed[addrhi, 0]
    #if 0
       .reg _ind_ref len_ov
        move(len_ov, 1)
        // Set OV_LEN and length field
        // Also set OVE_DATA = 0x1 to indicate data reference as immed value
        alu[_ind_ref, 0x88, OR, len_ov, <<8]
        alu[_ind_ref, _ind_ref, OR, 1, <<16]
        alu[--, --, B, _ind_ref]
        mem[add64_imm, --, addrhi, <<8, _addr], indirect_ref
    #else
        mem[incr64, --, addrhi, <<8, _addr]
    #endif
.end
#endm /* blm_stats */

/*
 *
 */
#macro blm_init_filter_match(cls_ap_filter_match, fmatch)
    #ifndef THSDK_558_FIX
        move(cls_ap_filter_match, fmatch)
    #endif
#endm /* blm_init_filter_match */

/*
 *
 */
#macro blm_init_filter_number(cls_ap_filter_number, n)
    #ifndef THSDK_558_FIX
        move(cls_ap_filter_number, n)
    #endif
#endm /* blm_init_filter_number */

/**
 * Initialize an EMU ring queue descriptor at LOAD time
 *
 * @param R_ISLAND      - The EMU ring island number (24-26)
 * @param R_NUM         - The EMU ring number
 * @param R_SIZE        - The EMU ring size (512,1024,2048....16M)
 * @param R_BASE        - The EMU ring memory base address (allocated elsewhere)
 * @param Q_LOC         - The EMU ring locality value (0-3)
 * @param BUF_SIZE      - The packet EMU buffer size to be used
 * @param INIT_COUNT    - The initial number of entries the ring is populated with
 */
#macro blm_emu_ring_init(R_ISLAND, R_NUM, R_SIZE, R_BASE, Q_LOC, BUF_SIZE, INIT_COUNT)
.begin

    /* Check ring size */
    #if ((R_SIZE < 512) || (R_SIZE > 16*1024*1024))
        #error "Ring size must be between 512 and 16M (Given size " R_SIZE")"
    #endif
    /* Size must be a power of 2 */
    #if (R_SIZE & (R_SIZE - 1) != 0)
        #error "Ring size must be a power of 2 (Given size " R_SIZE")"
    #endif
    /* Ring island should be one of 24,25,26 */
    #if ((R_ISLAND < 24) || (R_ISLAND > 26))
        #error "Ring island must be 24-26 (Given island " R_ISLAND")"
    #endif
    /* Init count can not be > ring size */
    #if (R_SIZE < INIT_COUNT)
        #error "Ring init count can not be bigger than ring size (Size "R_SIZE", Init count "INIT_COUNT")"
    #endif
    /* Check Q locality */
    #if (Q_LOC > 3)
        #error "Ring's Q locality shoule be 0-3 (Q_LOC "Q_LOC")"
    #endif

    #define_eval _ISL_ (R_ISLAND - 24)
    #define_eval _SIZE_ log2(R_SIZE >> 9)

    /* Load time initialization of a type 2 ring queue descriptor   */
    /* NOTE :                                                       */
    /* Using "emem" for the island number is covering both emem and */
    /* emem_cache_upper ring location.                              */

    /* For a ring the maximum entries permitted is size-1, so if    */
    /* the initial count equals the ring size we must decrease the  */
    /* initial count by 1.                                          */
    #if (R_SIZE == INIT_COUNT)
        #define_eval BUF_CNT (INIT_COUNT - 1)
    #else
        #define_eval BUF_CNT (INIT_COUNT)
    #endif

    .load_mu_qdesc \
            emem/**/_ISL_ R_NUM \
            ((_SIZE_ <<28) | (R_BASE & 0x03fffffc)) \
            (((R_BASE + BUF_CNT*4) & 0xfffffffc) | 2) \
            ((Q_LOC << 30) | ((R_BASE >> 8) & 0x03000000) | BUF_CNT) \
            0

    #undef BUF_CNT
    #undef _ISL_
    #undef _SIZE_

.end
#endm

/**
 * Initialize a BLQ ring (Writes BLQueCtrl Register) at LOAD time
 *
 * @param NBI_NUM       - The NBI number (8/9)
 * @param BLQ_NUM       - The BLQ ring number (0-3)
 * @param BLQ_LEN       - The BLQ length (512,1024,2048,4096)
 * @param BLQ_HEAD      - The BLQ head pointer (0-4095)
 * @param INIT_COUNT    - The initial number of entries the ring is populated with
 */
#macro blm_blq_ring_init(NBI_NUM, BLQ_NUM, BLQ_LEN, BLQ_HEAD, INIT_COUNT)
.begin
    /* Check nbi number */
    #if ((NBI_NUM != 8) && (NBI_NUM != 9))
        #error "NBI number must be 8 or 9"
    #endif
    /* Check blq number */
    #if ((BLQ_NUM < 0) || (BLQ_NUM > 3))
        #error "BLQ number must be 0-3 (Given number " BLQ_NUM")"
    #endif
    /* Check blq length */
    #if ((BLQ_LEN != 512) && (BLQ_LEN != 1024) && (BLQ_LEN != 2048) && (BLQ_LEN != 4096))
        #error "BLQ length must be 512/1024/2048/4096 (Given length " BLQ_LEN")"
    #endif
    /* Init count can not be > blq length */
    #if (BLQ_LEN < INIT_COUNT)
        #error "BLQ init count can not be bigger than BLQ length (Length "BLQ_LEN", Init count "INIT_COUNT")"
    #endif

    #define_eval _TAIL_PTR_ (BLQ_HEAD + INIT_COUNT)
    #define_eval _HEAD_PTR_ BLQ_HEAD
    #define_eval _SIZE_     (LOG2(BLQ_LEN) - 9)

    #if (_TAIL_PTR_ > _HEAD_PTR_)
        #define_eval _TAIL_PTR_ (_TAIL_PTR_ - 1)
    #endif

    /* Setup the DMA's BLQueCtrl register */
   .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBufferList.BLQueCtrl/**/BLQ_NUM (_TAIL_PTR_ | (_HEAD_PTR_ << 12) | (_SIZE_ << 36)) const

    /* Setup the TM's  BLQueCtrl register */
   .init_csr nbi:i/**/NBI_NUM.NBITMCPP.TMBufferList.BLQueCtrl/**/BLQ_NUM (_HEAD_PTR_ | (_HEAD_PTR_ << 12) | (_SIZE_ << 36)) const

    #undef _TAIL_PTR_
    #undef _HEAD_PTR_
    #undef _SIZE_

.end
#endm

/**
 * Fill up a BLQ or EMU ring
 *
 * @param R_TYPE                        - BLQ or EMU
 * @param NBI_NUM                       - The NBI number (8/9)
 * @param R_SIZE                        - The blq/ring size (entry count)
 * @param R_BASE                        - The BDsram start index OR the EMU ring memory base address
 * @param BUF_SIZE                      - The packet EMU buffer size to be used
 * @param IMEM0/1 or EMEM0/1/2_NUM_BUFS - The number of buffers to allocate in each memory type (could be 0)
 * @param IMEM0/1 or EMEM0/1/2_BASE     - The symbol name to be used for the buffers memory allocation (allocated here)
 * @param IMEM0/1 or EMEM0/1/2_DENSITY  - The density of the buffer pointers to be populated into the ring memory
 *
 */
#macro blm_ring_fill(R_TYPE, NBI_NUM, R_SIZE, R_BASE, BUF_SIZE, \
                     IMEM0_NUM_BUFS, IMEM0_BASE, IMEM0_DENSITY, \
                     IMEM1_NUM_BUFS, IMEM1_BASE, IMEM1_DENSITY, \
                     EMEM0_CACHE_NUM_BUFS, EMEM0_CACHE_BASE, EMEM0_CACHE_DENSITY, \
                     EMEM1_CACHE_NUM_BUFS, EMEM1_CACHE_BASE, EMEM1_CACHE_DENSITY, \
                     EMEM2_CACHE_NUM_BUFS, EMEM2_CACHE_BASE, EMEM2_CACHE_DENSITY, \
                     EMEM0_NUM_BUFS, EMEM0_BASE, EMEM0_DENSITY, \
                     EMEM1_NUM_BUFS, EMEM1_BASE, EMEM1_DENSITY, \
                     EMEM2_NUM_BUFS, EMEM2_BASE, EMEM2_DENSITY)
.begin
    /* Check nbi number */
    #if ((NBI_NUM != 8) && (NBI_NUM != 9))
        #error "NBI number must be 8 or 9"
    #endif

    #if (streq('R_TYPE', 'EMU'))
        /* Check ring size */
        #if ((R_SIZE < 512) || (R_SIZE > 16*1024*1024))
            #error "Ring size must be between 512 and 16M (Given size " R_SIZE")"
        #endif
        /* Size must be a power of 2 */
        #if (R_SIZE & (R_SIZE - 1) != 0)
            #error "Ring size must be a power of 2 (Given size " R_SIZE")"
        #endif
    #elif (streq('R_TYPE', 'BLQ'))
        /* Check blq length */
        #if ((R_SIZE != 512) && (R_SIZE != 1024) && (R_SIZE != 2048) && (R_SIZE != 4096))
            #error "BLQ length must be 512/1024/2048/4096 (Given length " R_SIZE")"
        #endif
    #else
        #error "R_TYPE must be BLQ or EMU only"
    #endif

    /* Init count can not be > (blq length + emu ring size) */
    #if (R_SIZE < (IMEM0_NUM_BUFS+IMEM1_NUM_BUFS+ \
                   EMEM0_CACHE_NUM_BUFS+EMEM1_CACHE_NUM_BUFS+EMEM2_CACHE_NUM_BUFS+ \
                   EMEM0_NUM_BUFS+EMEM1_NUM_BUFS+EMEM2_NUM_BUFS))
        #error "Ring size too small for total number of buffers"
    #endif

    #if (IMEM0_NUM_BUFS > 0)
        #if (IMEM0_NUM_BUFS < IMEM0_DENSITY)
            #error "IMEM0 number of buffers ("IMEM0_NUM_BUFS") < IMEM0 density ("IMEM0_DENSITY")"
        #endif
        #if (IMEM0_DENSITY <= 0)
            #error "IMEM0_DENSITY can not be <= 0"
        #endif
    #endif
    #if (IMEM1_NUM_BUFS > 0)
        #if (IMEM1_NUM_BUFS < IMEM1_DENSITY)
            #error "IMEM1 number of buffers ("IMEM1_NUM_BUFS") < IMEM1 density ("IMEM1_DENSITY")"
        #endif
        #if (IMEM1_DENSITY <= 0)
            #error "IMEM1_DENSITY can not be <= 0"
        #endif
    #endif
    #if (EMEM0_CACHE_NUM_BUFS > 0)
        #if (EMEM0_CACHE_NUM_BUFS < EMEM0_CACHE_DENSITY)
            #error "EMEM0 Cache number of buffers ("EMEM0_CACHE_NUM_BUFS") < EMEM0 CACHE density ("EMEM0_CACHE_DENSITY")"
        #endif
        #if (EMEM0_CACHE_DENSITY <= 0)
            #error "EMEM0_CACHE_DENSITY can not be <= 0"
        #endif
    #endif
    #if (EMEM1_CACHE_NUM_BUFS > 0)
        #if (EMEM1_CACHE_NUM_BUFS < EMEM1_CACHE_DENSITY)
            #error "EMEM1 Cache number of buffers ("EMEM1_CACHE_NUM_BUFS") < EMEM1 CACHE density ("EMEM1_CACHE_DENSITY")"
        #endif
        #if (EMEM1_CACHE_DENSITY <= 0)
            #error "EMEM1_CACHE_DENSITY can not be <= 0"
        #endif
    #endif
    #if (EMEM2_CACHE_NUM_BUFS > 0)
        #if (EMEM2_CACHE_NUM_BUFS < EMEM2_CACHE_DENSITY)
            #error "EMEM2 Cache number of buffers ("EMEM2_CACHE_NUM_BUFS") < EMEM2 CACHE density ("EMEM2_CACHE_DENSITY")"
        #endif
        #if (EMEM2_CACHE_DENSITY <= 0)
            #error "EMEM2_CACHE_DENSITY can not be <= 0"
        #endif
    #endif
    #if (EMEM0_NUM_BUFS > 0)
        #if (EMEM0_NUM_BUFS < EMEM0_DENSITY)
            #error "EMEM0 number of buffers ("EMEM0_NUM_BUFS") < EMEM0 density ("EMEM0_DENSITY")"
        #endif
        #if (EMEM0_DENSITY <= 0)
            #error "EMEM0_DENSITY can not be <= 0"
        #endif
    #endif
    #if (EMEM1_NUM_BUFS > 0)
        #if (EMEM1_NUM_BUFS < EMEM1_DENSITY)
            #error "EMEM1 number of buffers ("EMEM1_NUM_BUFS") < EMEM1 density ("EMEM1_DENSITY")"
        #endif
        #if (EMEM1_DENSITY <= 0)
            #error "EMEM1_DENSITY can not be <= 0"
        #endif
    #endif
    #if (EMEM2_NUM_BUFS > 0)
        #if (EMEM2_NUM_BUFS < EMEM2_DENSITY)
            #error "EMEM2 number of buffers ("EMEM2_NUM_BUFS") < EMEM2 density ("EMEM2_DENSITY")"
        #endif
        #if (EMEM2_DENSITY <= 0)
            #error "EMEM2_DENSITY can not be <= 0"
        #endif
    #endif

    #define_eval TOTAL_BUFS_LEFT (IMEM0_NUM_BUFS+ \
                                  IMEM1_NUM_BUFS+ \
                                  EMEM0_CACHE_NUM_BUFS+ \
                                  EMEM1_CACHE_NUM_BUFS+ \
                                  EMEM2_CACHE_NUM_BUFS+ \
                                  EMEM0_NUM_BUFS+ \
                                  EMEM1_NUM_BUFS+ \
                                  EMEM2_NUM_BUFS)

    #define_eval IMEM0_BUFS_LEFT IMEM0_NUM_BUFS
    #define_eval IMEM0_BUFS_OFF 0
    #define_eval IMEM1_BUFS_LEFT IMEM1_NUM_BUFS
    #define_eval IMEM1_BUFS_OFF 0
    #define_eval EMEM0_CACHE_BUFS_LEFT EMEM0_CACHE_NUM_BUFS
    #define_eval EMEM0_CACHE_BUFS_OFF 0
    #define_eval EMEM1_CACHE_BUFS_LEFT EMEM1_CACHE_NUM_BUFS
    #define_eval EMEM1_CACHE_BUFS_OFF 0
    #define_eval EMEM2_CACHE_BUFS_LEFT EMEM2_CACHE_NUM_BUFS
    #define_eval EMEM2_CACHE_BUFS_OFF 0
    #define_eval EMEM0_BUFS_LEFT EMEM0_NUM_BUFS
    #define_eval EMEM0_BUFS_OFF 0
    #define_eval EMEM1_BUFS_LEFT EMEM1_NUM_BUFS
    #define_eval EMEM1_BUFS_OFF 0
    #define_eval EMEM2_BUFS_LEFT EMEM2_NUM_BUFS
    #define_eval EMEM2_BUFS_OFF 0

    #if (streq('R_TYPE', 'EMU'))
        #define_eval R_OFF 0
    #else
        #define_eval R_OFF R_BASE
    #endif

    #while (TOTAL_BUFS_LEFT > 0)
        #for _MEM_REGION_ [IMEM0, IMEM1, EMEM0_CACHE, EMEM1_CACHE, EMEM2_CACHE, EMEM0, EMEM1, EMEM2]

            #if (_MEM_REGION_/**/_BUFS_LEFT > 0)

                #if (streq('_MEM_REGION_', 'IMEM0'))
                    #define_eval DENSITY IMEM0_DENSITY
                #elif (streq('_MEM_REGION_', 'IMEM1'))
                    #define_eval DENSITY IMEM1_DENSITY
                #elif (streq('_MEM_REGION_', 'EMEM0_CACHE'))
                    #define_eval DENSITY EMEM0_CACHE_DENSITY
                #elif (streq('_MEM_REGION_', 'EMEM1_CACHE'))
                    #define_eval DENSITY EMEM1_CACHE_DENSITY
                #elif (streq('_MEM_REGION_', 'EMEM2_CACHE'))
                    #define_eval DENSITY EMEM2_CACHE_DENSITY
                #elif (streq('_MEM_REGION_', 'EMEM0'))
                    #define_eval DENSITY EMEM0_DENSITY
                #elif (streq('_MEM_REGION_', 'EMEM1'))
                    #define_eval DENSITY EMEM1_DENSITY
                #elif (streq('_MEM_REGION_', 'EMEM2'))
                    #define_eval DENSITY EMEM2_DENSITY
                #endif

                #while ((_MEM_REGION_/**/_BUFS_LEFT > 0) && (DENSITY > 0))


                    #if (streq('R_TYPE', 'EMU'))
                        #if (streq('_MEM_REGION_', 'IMEM0'))
                            .init R_BASE+R_OFF (((IMEM0_BASE + IMEM0_BUFS_OFF) | (1 <<23)) >> 11)
                        #elif (streq('_MEM_REGION_', 'IMEM1'))
                            .init R_BASE+R_OFF (((IMEM1_BASE + IMEM1_BUFS_OFF) | (1 <<23)) >> 11)
                        #elif (streq('_MEM_REGION_', 'EMEM0_CACHE'))
                            .init R_BASE+R_OFF ((EMEM0_CACHE_BASE + EMEM0_CACHE_BUFS_OFF) >> 11)
                        #elif (streq('_MEM_REGION_', 'EMEM1_CACHE'))
                            .init R_BASE+R_OFF ((EMEM1_CACHE_BASE + EMEM1_CACHE_BUFS_OFF) >> 11)
                        #elif (streq('_MEM_REGION_', 'EMEM2_CACHE'))
                            .init R_BASE+R_OFF ((EMEM2_CACHE_BASE + EMEM2_CACHE_BUFS_OFF) >> 11)
                        #elif (streq('_MEM_REGION_', 'EMEM0'))
                            .init R_BASE+R_OFF ((EMEM0_BASE + EMEM0_BUFS_OFF) >> 11)
                        #elif (streq('_MEM_REGION_', 'EMEM1'))
                            .init R_BASE+R_OFF ((EMEM1_BASE + EMEM1_BUFS_OFF) >> 11)
                        #elif (streq('_MEM_REGION_', 'EMEM2'))
                            .init R_BASE+R_OFF ((EMEM2_BASE + EMEM2_BUFS_OFF) >> 11)
                        #endif

                        #define_eval R_OFF (R_OFF + 4)
                    #else
                        #if (streq('_MEM_REGION_', 'IMEM0'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr (((IMEM0_BASE + IMEM0_BUFS_OFF) | (1 <<23)) >> 11) const
                        #elif (streq('_MEM_REGION_', 'IMEM1'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr (((IMEM1_BASE + IMEM1_BUFS_OFF) | (1 <<23)) >> 11) const
                        #elif (streq('_MEM_REGION_', 'EMEM0_CACHE'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr ((EMEM0_CACHE_BASE + EMEM0_CACHE_BUFS_OFF) >> 11) const
                        #elif (streq('_MEM_REGION_', 'EMEM1_CACHE'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr ((EMEM1_CACHE_BASE + EMEM1_CACHE_BUFS_OFF) >> 11) const
                        #elif (streq('_MEM_REGION_', 'EMEM2_CACHE'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr ((EMEM2_CACHE_BASE + EMEM2_CACHE_BUFS_OFF) >> 11) const
                        #elif (streq('_MEM_REGION_', 'EMEM0'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr ((EMEM0_BASE + EMEM0_BUFS_OFF) >> 11) const
                        #elif (streq('_MEM_REGION_', 'EMEM1'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr ((EMEM1_BASE + EMEM1_BUFS_OFF) >> 11) const
                        #elif (streq('_MEM_REGION_', 'EMEM2'))
                            .init_csr nbi:i/**/NBI_NUM.NbiDmaCpp.NbiDmaBDSRAM.NbiDmaBDSramEntry/**/R_OFF.MuAddr ((EMEM2_BASE + EMEM2_BUFS_OFF) >> 11) const
                        #endif

                        #define_eval R_OFF (R_OFF + 1)
                    #endif


                    #define_eval DENSITY (DENSITY - 1)
                    #define_eval TOTAL_BUFS_LEFT (TOTAL_BUFS_LEFT - 1)

                    #if (streq('_MEM_REGION_', 'IMEM0'))
                        #define_eval IMEM0_BUFS_LEFT (IMEM0_BUFS_LEFT - 1)
                        #define_eval IMEM0_BUFS_OFF  (IMEM0_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'IMEM1'))
                        #define_eval IMEM1_BUFS_LEFT (IMEM1_BUFS_LEFT - 1)
                        #define_eval IMEM1_BUFS_OFF  (IMEM1_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'EMEM0_CACHE'))
                        #define_eval EMEM0_CACHE_BUFS_LEFT (EMEM0_CACHE_BUFS_LEFT - 1)
                        #define_eval EMEM0_CACHE_BUFS_OFF  (EMEM0_CACHE_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'EMEM1_CACHE'))
                        #define_eval EMEM1_CACHE_BUFS_LEFT (EMEM1_CACHE_BUFS_LEFT - 1)
                        #define_eval EMEM1_CACHE_BUFS_OFF  (EMEM1_CACHE_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'EMEM2_CACHE'))
                        #define_eval EMEM2_CACHE_BUFS_LEFT (EMEM2_CACHE_BUFS_LEFT - 1)
                        #define_eval EMEM2_CACHE_BUFS_OFF  (EMEM2_CACHE_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'EMEM0'))
                        #define_eval EMEM0_BUFS_LEFT (EMEM0_BUFS_LEFT - 1)
                        #define_eval EMEM0_BUFS_OFF  (EMEM0_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'EMEM1'))
                        #define_eval EMEM1_BUFS_LEFT (EMEM1_BUFS_LEFT - 1)
                        #define_eval EMEM1_BUFS_OFF  (EMEM1_BUFS_OFF  + BUF_SIZE)
                    #elif (streq('_MEM_REGION_', 'EMEM2'))
                        #define_eval EMEM2_BUFS_LEFT (EMEM2_BUFS_LEFT - 1)
                        #define_eval EMEM2_BUFS_OFF  (EMEM2_BUFS_OFF  + BUF_SIZE)
                    #endif

                #endloop

                #undef DENSITY

            #endif

        #endloop
    #endloop

    #undef TOTAL_BUFS_LEFT
    #undef IMEM0_BUFS_LEFT
    #undef IMEM0_BUFS_OFF
    #undef IMEM1_BUFS_LEFT
    #undef IMEM1_BUFS_OFF
    #undef EMEM0_CACHE_BUFS_LEFT
    #undef EMEM0_CACHE_BUFS_OFF
    #undef EMEM1_CACHE_BUFS_LEFT
    #undef EMEM1_CACHE_BUFS_OFF
    #undef EMEM2_CACHE_BUFS_LEFT
    #undef EMEM2_CACHE_BUFS_OFF
    #undef EMEM0_BUFS_LEFT
    #undef EMEM0_BUFS_OFF
    #undef EMEM1_BUFS_LEFT
    #undef EMEM1_BUFS_OFF
    #undef EMEM2_BUFS_LEFT
    #undef EMEM2_BUFS_OFF
    #undef R_OFF

.end
#endm

/**
 * Fill up the DMA's BLQ
 *
 * @param NBI_NUM                       - The NBI number (8/9)
 * @param BLQ_LEN                       - The length of the blq (512/1024/2048/4096)
 * @param BDSRAM_OFF                    - The first index of the BDSram this BLQ is using
 * @param BUF_SIZE                      - The packet EMU buffer size to be used
 * @param IMEM0/1 or EMEM0/1/2_NUM_BUFS - The number of buffers to be populated into the BDSram (BLQ)
 * @param IMEM0/1 or EMEM0/1/2_BASE     - The symbol name to be used for the buffers memory allocation
 * @param IMEM0/1 or EMEM0/1/2_DENSITY  - The density of the buffer pointers to be populated into the ring memory
 *
 */
#macro blm_blq_fill(NBI_NUM, BLQ_LEN, BDSRAM_OFF, BUF_SIZE, \
                    IMEM0_NUM_BUFS, IMEM0_BASE, IMEM0_DENSITY, \
                    IMEM1_NUM_BUFS, IMEM1_BASE, IMEM1_DENSITY, \
                    EMEM0_CACHE_NUM_BUFS, EMEM0_CACHE_BASE, EMEM0_CACHE_DENSITY, \
                    EMEM1_CACHE_NUM_BUFS, EMEM1_CACHE_BASE, EMEM1_CACHE_DENSITY, \
                    EMEM2_CACHE_NUM_BUFS, EMEM2_CACHE_BASE, EMEM2_CACHE_DENSITY, \
                    EMEM0_NUM_BUFS, EMEM0_BASE, EMEM0_DENSITY, \
                    EMEM1_NUM_BUFS, EMEM1_BASE, EMEM1_DENSITY, \
                    EMEM2_NUM_BUFS, EMEM2_BASE, EMEM2_DENSITY)

    blm_ring_fill(BLQ, NBI_NUM, BLQ_LEN, BDSRAM_OFF, BUF_SIZE,
                  IMEM0_NUM_BUFS, IMEM0_BASE, IMEM0_DENSITY,
                  IMEM1_NUM_BUFS, IMEM1_BASE, IMEM1_DENSITY,
                  EMEM0_CACHE_NUM_BUFS, EMEM0_CACHE_BASE, EMEM0_CACHE_DENSITY,
                  EMEM1_CACHE_NUM_BUFS, EMEM1_CACHE_BASE, EMEM1_CACHE_DENSITY,
                  EMEM2_CACHE_NUM_BUFS, EMEM2_CACHE_BASE, EMEM2_CACHE_DENSITY,
                  EMEM0_NUM_BUFS, EMEM0_BASE, EMEM0_DENSITY,
                  EMEM1_NUM_BUFS, EMEM1_BASE, EMEM1_DENSITY,
                  EMEM2_NUM_BUFS, EMEM2_BASE, EMEM2_DENSITY)
#endm

/**
 * Fill up the EMU ring
 *
 * @param NBI_NUM                       - The NBI number (8/9)
 * @param R_SIZE                        - The EMU ring size (512,1024,2048....16M)
 * @param R_BASE                        - The EMU ring memory base address (allocated elsewhere)
 * @param BUF_SIZE                      - The packet EMU buffer size to be used
 * @param IMEM0/1 or EMEM0/1/2_NUM_BUFS - The number of buffers to allocate in each memory type (could be 0)
 * @param IMEM0/1 or EMEM0/1/2_BASE     - The symbol name to be used for the buffers memory allocation (allocated here)
 * @param IMEM0/1 or EMEM0/1/2_DENSITY  - The density of the buffer pointers to be populated into the ring memory
 *
 */
#macro blm_emu_ring_fill(NBI_NUM, R_SIZE, R_BASE, BUF_SIZE, \
                         IMEM0_NUM_BUFS, IMEM0_BASE, IMEM0_DENSITY, \
                         IMEM1_NUM_BUFS, IMEM1_BASE, IMEM1_DENSITY, \
                         EMEM0_CACHE_NUM_BUFS, EMEM0_CACHE_BASE, EMEM0_CACHE_DENSITY, \
                         EMEM1_CACHE_NUM_BUFS, EMEM1_CACHE_BASE, EMEM1_CACHE_DENSITY, \
                         EMEM2_CACHE_NUM_BUFS, EMEM2_CACHE_BASE, EMEM2_CACHE_DENSITY, \
                         EMEM0_NUM_BUFS, EMEM0_BASE, EMEM0_DENSITY, \
                         EMEM1_NUM_BUFS, EMEM1_BASE, EMEM1_DENSITY, \
                         EMEM2_NUM_BUFS, EMEM2_BASE, EMEM2_DENSITY)

    blm_ring_fill(EMU, NBI_NUM, R_SIZE, R_BASE, BUF_SIZE,
                  IMEM0_NUM_BUFS, IMEM0_BASE, IMEM0_DENSITY,
                  IMEM1_NUM_BUFS, IMEM1_BASE, IMEM1_DENSITY,
                  EMEM0_CACHE_NUM_BUFS, EMEM0_CACHE_BASE, EMEM0_CACHE_DENSITY,
                  EMEM1_CACHE_NUM_BUFS, EMEM1_CACHE_BASE, EMEM1_CACHE_DENSITY,
                  EMEM2_CACHE_NUM_BUFS, EMEM2_CACHE_BASE, EMEM2_CACHE_DENSITY,
                  EMEM0_NUM_BUFS, EMEM0_BASE, EMEM0_DENSITY,
                  EMEM1_NUM_BUFS, EMEM1_BASE, EMEM1_DENSITY,
                  EMEM2_NUM_BUFS, EMEM2_BASE, EMEM2_DENSITY)
#endm

/*
 * Context specific initialization
 */
#macro blm_ctx_init()

.begin

    immed[blq_cnt_mask, MASK_10b]
    immed[deficit, 0]
    immed[count, 0]
    immed[blq_cnt_cur, 0]
    immed[blq_cnt_prev, 0]
    immed[blq_evnt_cnt, 0]
    immed[alarm_cnt, 0]
    immed[island, __ISLAND]
    immed[to_ticks, ALARM_TICKS]

    br=ctx[0, ctx0#]
    br=ctx[1, ctx1#]
    br=ctx[2, ctx2#]
    br=ctx[3, ctx3#]
    br=ctx[4, ctx4#]
    br=ctx[5, ctx5#]
    br=ctx[6, ctx6#]
    br=ctx[7, ctx7#]

ctx0#:

    /* Each BLM instance populates the buffers into the BLQ/BDSram */
    #ifndef BLM_SKIP_DMA_INIT
        #for _blq [0,1,2,3]
            #if (_blq == 0)
                #define _BLQ_HEAD   0
            #else
                #define_eval _blq_prev   (_blq - 1)
                #define_eval _BLQ_HEAD  (BLM_NBI/**/NBII/**/_BLQ/**/_blq_prev/**/_LEN + _BLQ_HEAD)
            #endif
			#define_eval _BLQ_INIT_COUNT_ 0
            #for _mem_type [BDSRAM_IMEM0,BDSRAM_IMEM1,BDSRAM_EMEM0,BDSRAM_EMEM1,BDSRAM_EMEM2,BDSRAM_EMEM0_CACHE,BDSRAM_EMEM1_CACHE,BDSRAM_EMEM2_CACHE]
                #define_eval _BLQ_INIT_COUNT_   (_BLQ_INIT_COUNT_ + \
                                            BLM_NBI/**/NBII/**/_BLQ/**/_blq/**/_/**/_mem_type/**/_NUM_BUFS)
            #endloop
            blm_blq_fill(NBI/**/NBII/**/_BLQ/**/_blq/**/_FILL_PARAMS)
            blm_blq_ring_init(NBII, _blq, BLM_NBI/**/NBII/**/_BLQ/**/_blq/**/_LEN, _BLQ_HEAD, _BLQ_INIT_COUNT_)
        #endloop
        #undef _BLQ_INIT_COUNT_
        #undef _BLQ_HEAD
        #undef _blq_prev
        #undef _blq
        #undef _mem_type
    #endif /* BLM_SKIP_DMA_INIT */


#ifdef BLM_INIT_EMU_RINGS
        #ifdef SPLIT_EMU_RINGS
            #define FOR_NBIS    8,9
        #else
            #define FOR_NBIS    8
        #endif /* SPLIT_EMU_RINGS */
        #for _nbi [FOR_NBIS]
            #for _blq [0,1,2,3]
                #define_eval _EMU_INIT_COUNT_   0
                #for _mem_type [EMU_IMEM0,EMU_IMEM1,EMU_EMEM0,EMU_EMEM1,EMU_EMEM2,EMU_EMEM0_CACHE,EMU_EMEM1_CACHE,EMU_EMEM2_CACHE]
                    #define_eval _EMU_INIT_COUNT_   (_EMU_INIT_COUNT_ + \
                                                BLM_NBI/**/NBII/**/_BLQ/**/_blq/**/_/**/_mem_type/**/_NUM_BUFS)
                #endloop
                blm_emu_ring_fill(NBI/**/NBII/**/_BLQ/**/_blq/**/_EMU_RING_FILL_PARAMS)
                blm_emu_ring_init(BLM_NBI/**/_nbi/**/_BLQ/**/_blq/**/_EMU_Q_ISLAND, \
                                  BLM_NBI/**/_nbi/**/_BLQ/**/_blq/**/_EMU_QID, \
                                 (BLM_NBI/**/_nbi/**/_BLQ/**/_blq/**/_Q_SIZE/4), \
                                 _BLM_NBI/**/_nbi/**/_BLQ/**/_blq/**/_EMU_Q_BASE, \
                                  BLM_NBI/**/_nbi/**/_BLQ/**/_blq/**/_EMU_Q_LOCALITY, \
                                  NBI/**/_nbi/**/_BLQ_EMU_/**/_blq/**/_PKTBUF_SIZE, \
                                  _EMU_INIT_COUNT_)
            #endloop
        #endloop
        #undef _EMU_INIT_COUNT_
        #undef _nbi
        #undef _blq
        #undef _mem_type
#endif /* BLM_INIT_EMU_RINGS */

    blm_cfg_blq_evnts(NBII_lsb, 0, ingress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 1, ingress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 2, ingress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 3, ingress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 0, egress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 1, egress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 2, egress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)
    blm_cfg_blq_evnts(NBII_lsb, 3, egress, NBI_BLQ_EVENT_THRESHOLD_ENCODING, 1, 0)

    move(blq_stats_base, (CTM_NBI_BLQ0_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, ((_BLM_NBI/**/_NBIX/**/_BLQ0_EMU_Q_BASE >>8)&0xFF000000))
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ0_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ0_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ0_CACHE_SIZE)
    #undef _NBIX
    blm_init_stats()
    blm_init_filter_match(cls_ap_filter_match, CTX0_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ0_AP_FILTER_NUM)
    /* Clear Mailbox */
    blm_init_mailbox()
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ0_DESC_LMEM_BASE)
    /* Initialize LM */
    blm_init_lm(BLM_BLQ_LM_REF, 0, BLM_NBI_BLQ0_CACHE_BASE)
    blm_init_info_section()
    /* This should be last macro to be called per context */
    blm_is_blq_enable(0, blm_ingress_blq_processing#)
ctx1#:
    move(blq_stats_base, (CTM_NBI_BLQ0_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ0_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ0_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ0_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ0_CACHE_SIZE)
    #undef _NBIX
    blm_init_filter_match(cls_ap_filter_match, CTX1_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ1_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ0_DESC_LMEM_BASE)
    blm_is_blq_enable(4, blm_egress_blq_processing#)
ctx2#:
    move(blq_stats_base, (CTM_NBI_BLQ1_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ1_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ1_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ1_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ1_CACHE_SIZE)
    #undef _NBIX
    blm_init_stats()
    blm_init_filter_match(cls_ap_filter_match, CTX2_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ2_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ1_DESC_LMEM_BASE)
    /* Initialize LM */
    blm_init_lm(BLM_BLQ_LM_REF, 1, BLM_NBI_BLQ1_CACHE_BASE)
    blm_is_blq_enable(1, blm_ingress_blq_processing#)
ctx3#:
    move(blq_stats_base, (CTM_NBI_BLQ1_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ1_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ1_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ1_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ1_CACHE_SIZE)
    #undef _NBIX
    blm_init_filter_match(cls_ap_filter_match, CTX3_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ3_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ1_DESC_LMEM_BASE)
    blm_is_blq_enable(5, blm_egress_blq_processing#)
ctx4#:
    move(blq_stats_base, (CTM_NBI_BLQ2_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ2_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ2_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ2_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ2_CACHE_SIZE)
    #undef _NBIX
    blm_init_stats()
    blm_init_filter_match(cls_ap_filter_match, CTX4_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ4_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ2_DESC_LMEM_BASE)
    /* Initialize LM */
    blm_init_lm(BLM_BLQ_LM_REF, 2, BLM_NBI_BLQ2_CACHE_BASE)
    blm_is_blq_enable(2, blm_ingress_blq_processing#)
ctx5#:
    move(blq_stats_base, (CTM_NBI_BLQ2_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ2_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ2_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ2_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ2_CACHE_SIZE)
    #undef _NBIX
    blm_init_filter_match(cls_ap_filter_match, CTX5_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ5_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ2_DESC_LMEM_BASE)
    blm_is_blq_enable(6, blm_egress_blq_processing#)
ctx6#:
    move(blq_stats_base, (CTM_NBI_BLQ3_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ3_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ3_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ3_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ3_CACHE_SIZE)
    #undef _NBIX
    blm_init_stats()
    blm_init_filter_match(cls_ap_filter_match, CTX6_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ6_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ3_DESC_LMEM_BASE)
    /* Initialize LM */
    blm_init_lm(BLM_BLQ_LM_REF, 3, BLM_NBI_BLQ3_CACHE_BASE)
    blm_is_blq_enable(3, blm_ingress_blq_processing#)
ctx7#:
    move(blq_stats_base, (CTM_NBI_BLQ3_STATS_BASE & 0xffffffff))
    #define_eval _NBIX   (8 + BLM_INSTANCE_ID)
    move(addr, (_BLM_NBI/**/_NBIX/**/_BLQ3_EMU_Q_BASE >>8)&0xFF000000)
    move(ringid, BLM_NBI/**/_NBIX/**/_BLQ3_EMU_QID)
    move(cache_hwm, (BLM_NBI_BLQ3_CACHE_SIZE - BLM_NBI_BLQ_CACHE_DEFICIT))
    move(cache_size, BLM_NBI_BLQ3_CACHE_SIZE)
    #undef _NBIX
    blm_init_filter_match(cls_ap_filter_match, CTX7_FILTER_MATCH)
    blm_init_filter_number(cls_ap_filter_number, BLM_BLQ7_AP_FILTER_NUM)
    /* Setup per BLQ LM index */
    blm_init_blq_lm_index(BLQ3_DESC_LMEM_BASE)
    blm_is_blq_enable(7, blm_egress_blq_processing#)

blm_ctx_init_end#:
.end
#endm /* blm_ctx_init */

/* Global Relative GPR's - .init_ctx only works for global scope GPR's. These should not be in .begin/.end code block */
.reg cls_ap_filter_match
.reg cls_ap_filter_number
.reg blq_stats_base
.reg ringid
.reg cache_hwm
.reg cache_size
.reg cache_cnt
.set ringid

#ifdef THSDK_558_FIX
    /* Initialize filter match */
    .init_ctx 0 cls_ap_filter_match CTX0_FILTER_MATCH
    .init_ctx 1 cls_ap_filter_match CTX1_FILTER_MATCH
    .init_ctx 2 cls_ap_filter_match CTX2_FILTER_MATCH
    .init_ctx 3 cls_ap_filter_match CTX3_FILTER_MATCH
    .init_ctx 4 cls_ap_filter_match CTX4_FILTER_MATCH
    .init_ctx 5 cls_ap_filter_match CTX5_FILTER_MATCH
    .init_ctx 6 cls_ap_filter_match CTX6_FILTER_MATCH
    .init_ctx 7 cls_ap_filter_match CTX7_FILTER_MATCH

    /* Initialize autopush filter number */
    .init_ctx 0 cls_ap_filter_number BLM_BLQ0_AP_FILTER_NUM
    .init_ctx 1 cls_ap_filter_number BLM_BLQ1_AP_FILTER_NUM
    .init_ctx 2 cls_ap_filter_number BLM_BLQ2_AP_FILTER_NUM
    .init_ctx 3 cls_ap_filter_number BLM_BLQ3_AP_FILTER_NUM
    .init_ctx 4 cls_ap_filter_number BLM_BLQ4_AP_FILTER_NUM
    .init_ctx 5 cls_ap_filter_number BLM_BLQ5_AP_FILTER_NUM
    .init_ctx 6 cls_ap_filter_number BLM_BLQ6_AP_FILTER_NUM
    .init_ctx 7 cls_ap_filter_number BLM_BLQ7_AP_FILTER_NUM

    /* Initialize stats base address */
    .init_ctx 0 blq_stats_base CTM_NBI_BLQ0_STATS_BASE
    .init_ctx 1 blq_stats_base CTM_NBI_BLQ0_STATS_BASE
    .init_ctx 2 blq_stats_base CTM_NBI_BLQ1_STATS_BASE
    .init_ctx 3 blq_stats_base CTM_NBI_BLQ1_STATS_BASE
    .init_ctx 4 blq_stats_base CTM_NBI_BLQ2_STATS_BASE
    .init_ctx 5 blq_stats_base CTM_NBI_BLQ2_STATS_BASE
    .init_ctx 6 blq_stats_base CTM_NBI_BLQ3_STATS_BASE
    .init_ctx 7 blq_stats_base CTM_NBI_BLQ3_STATS_BASE

    /* Initialize Ring number */
    #if BLM_INSTANCE_ID == 0
         .init_ctx 0  ringid BLM_NBI8_BLQ0_QID
         .init_ctx 1  ringid BLM_NBI8_BLQ0_QID
         .init_ctx 2  ringid BLM_NBI8_BLQ1_QID
         .init_ctx 3  ringid BLM_NBI8_BLQ1_QID
         .init_ctx 4  ringid BLM_NBI8_BLQ2_QID
         .init_ctx 5  ringid BLM_NBI8_BLQ2_QID
         .init_ctx 6  ringid BLM_NBI8_BLQ3_QID
         .init_ctx 7  ringid BLM_NBI8_BLQ3_QID
    #else
         .init_ctx 0  ringid BLM_NBI9_BLQ0_QID
         .init_ctx 1  ringid BLM_NBI9_BLQ0_QID
         .init_ctx 2  ringid BLM_NBI9_BLQ1_QID
         .init_ctx 3  ringid BLM_NBI9_BLQ1_QID
         .init_ctx 4  ringid BLM_NBI9_BLQ2_QID
         .init_ctx 5  ringid BLM_NBI9_BLQ2_QID
         .init_ctx 6  ringid BLM_NBI9_BLQ3_QID
         .init_ctx 7  ringid BLM_NBI9_BLQ3_QID
    #endif
#endif

/* -------------------------------- Main -----------------------------*/

.begin

    /* Relative GPR's */
    .reg blq_cnt_mask
    .reg blq_cnt_prev
    .reg blq_cnt_cur
    .reg blq_evnt_cnt
    .reg alarm_cnt
    .reg deficit
    .reg island
    .reg NbiNum
    .reg sub_id
    .reg count
    .reg addr
    .reg blq
    .reg event_data
    .reg is_aps
    .reg to_ticks

    /* Transfer Registers */
    .reg volatile $event_data[1]
    .reg volatile $xfer[1]
    .xfer_order $event_data
    .xfer_order $xfer
    .set $event_data[0]
    .reg $nbidmabuf[16]
    .xfer_order $nbidmabuf
    aggregate_directive(.set, $nbidmabuf, 16)

    /* Signals */
    .sig volatile auto_push_event_sig
    .sig volatile evtsig
    .sig sig_bufpush sig_memget0 sig_memget1
    .set_sig auto_push_event_sig
    .sig alarm_sig
    .set_sig alarm_sig

    blm_ctx_init()

/* -------------------------------- NBI EGRESS EVENT PROCESSING - TM BLQ EVENTS -----------------------------*/

blm_egress_blq_processing#:
    .reg $nbitmbuf[16]
    .xfer_order $nbitmbuf
    aggregate_directive(.set, $nbitmbuf, 16)
    .sig sig_bufpop sig_memput0 sig_memput1

    /* Setup TM Evnt Auto push */
    evntm_cls_event_filter_config(cls_ap_filter_number, AUTOPUSH_FILTER_MASK, \
                                  cls_ap_filter_match, CLS_AP_FILTER_TYPE) /* Match NBI-x/TM,  Evnt Type 5 */

    evntm_cls_autopush_monitor_config(cls_ap_filter_number, &$event_data[0], &auto_push_event_sig)

    /*
     * Indicate BLM setup is complete and ready to rcv Events.
     * Context-7 is the last one to setup autopush filters.
     */
    /* ############# TODO TODO TODO TODO TODO ############
     * Since thread execution is based on BLQ_MASK, it is not safe
     * to indicate INIT_DONE based on context number.
     */
    .if (ctx() == 7)
        /* Indicate BLM init is done */
        immed[count, BLM_INIT_DONE]
        local_csr_wr[MAILBOX0, count]
    .endif

    .while (1)
        evntm_cls_autopush_monitor_engage(cls_ap_filter_number, $xfer[0], evtsig, NONE)
        ctx_arb[evtsig, auto_push_event_sig],all
        alu[$event_data[0], --, b, $event_data[0]]
        //alu[sub_id, 0x3, AND, $event_data[0], >>16]
        alu[blq, 0x3, AND, $event_data[0], >>14]
        alu[blq_cnt_cur, blq_cnt_mask, AND, $event_data[0], >>4]
        alu[NbiNum, 0x3, AND, $event_data[0], >>18]
        alu[deficit, blq_cnt_cur, -, blq_cnt_prev]
        alu[deficit, deficit, AND, blq_cnt_mask]
        alu[blq_evnt_cnt, blq_evnt_cnt, +, 1]

        blm_stats(BLM_STATS_NUM_TM_EVNTS_RCVD)
        .if (BLM_BLQ_LM_REF[BLM_LM_BLQ_NULL_RECYCLE_OFFSET] & 2)
            .while (deficit >= NBI_BLQ_EVENT_THRESHOLD)
                blm_egress_null_buffer_recycle(NbiNum, blq, NBI_BLQ_EVENT_THRESHOLD)
                alu[deficit, deficit, -, NBI_BLQ_EVENT_THRESHOLD]
                blm_stats(BLM_STATS_TM_NULL_RECYCLE)
            .endw
        .else
            .while ( deficit >= NBI_BLQ_EVENT_THRESHOLD)
                /* If there is any pending DMA Evnt, push egress to ingress directly */
                .if (BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET] != 0)
                    blm_recycle(NbiNum, blq, NbiNum, blq, BLM_RECYCLE_LEN)
                    blm_decr_dma_evnt_pend_cnt()
                    blm_stats(BLM_STATS_RECYCLE_DIRECT)
                .else
                    blm_cache_acquire_lock()
                    alu[cache_cnt, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET]]
                    blm_cache_release_lock()
                    .if (cache_cnt >= cache_hwm)
                        blm_egress_pull_buffers_to_emu_ring(NbiNum, blq, addr, ringid)
                        blm_stats(BLM_STATS_RECYCLE_TM_TO_EMU)
                    .else
                        blm_egress_pull_buffers_to_cache(NbiNum, blq, NBI_BLQ_EVENT_THRESHOLD)
                        blm_stats(BLM_STATS_RECYCLE_TM_TO_CACHE)
                    .endif
                .endif
                alu[deficit, deficit, -, NBI_BLQ_EVENT_THRESHOLD]
            .endw
        .endif

        #ifdef DEBUG
            .reg $debug[8]
            .addr $debug[0] 24
            .addr $debug[1] 25
            .addr $debug[2] 26
            .addr $debug[3] 27
            .addr $debug[4] 28
            .addr $debug[5] 29
            .addr $debug[6] 30
            .addr $debug[7] 31
            alu[$debug[0], --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET]]
            alu[$debug[1], --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET]]
            alu[$debug[2], --, b, $event_data[0]]
            alu[$debug[3], --, b, blq_cnt_cur]
            alu[$debug[4], --, b, blq_cnt_prev]
            alu[$debug[5], --, b, deficit]
            alu[$debug[6], --, b, blq_evnt_cnt]
            alu[$debug[7], --, b, 0]
        #endif

        /* update deficit pointers */
        alu[blq_cnt_prev, --, b, blq_cnt_cur]
    .endw

/* -------------------------------- NBI INGRESS EVENT PROCESSING - DMA BLQ EVENTS (EVEN Contexts) -----------------------------*/

blm_ingress_blq_processing#:

    /* Setup DMA Evnt Auto push */
    evntm_cls_event_filter_config(cls_ap_filter_number, AUTOPUSH_FILTER_MASK, \
                                  cls_ap_filter_match, CLS_AP_FILTER_TYPE) /* Match NBI-x/DMA, Evnt Type 5 */

    evntm_cls_autopush_monitor_config(cls_ap_filter_number, &$event_data[0], &auto_push_event_sig)

    /* Fill cache, if shadow EMU ring has buffers */
    .while (BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET] < cache_size)
        blm_cache_fill(addr, ringid, sig_memget0, init_cache_fill_complete#)
    .endw
    init_cache_fill_complete#:

    move(event_data, 0xffffffff)

    evntm_cls_autopush_monitor_engage(cls_ap_filter_number, $xfer[0], evtsig, NONE)
    ctx_arb[evtsig]
    ctx_arb[auto_push_event_sig], any
    alarm(to_ticks,alarm_sig)

    .while (1)
        br_signal[auto_push_event_sig, process_dma_evnet#]
        evntm_cls_autopush_monitor_engage(cls_ap_filter_number, $xfer[0], evtsig, NONE)
        ctx_arb[evtsig]
        alarm(to_ticks,alarm_sig)
        ctx_arb[alarm_sig, auto_push_event_sig], any
        br_signal[auto_push_event_sig, process_dma_evnet#]
        immed[is_aps, 0]
        alu[alarm_cnt, alarm_cnt, +, 1]
        br_signal[alarm_sig, service_check#]
process_dma_evnet#:
        alu[blq_evnt_cnt, blq_evnt_cnt, +, 1]
        immed[is_aps, 1]
        alu[event_data, --, b, $event_data[0]]
service_check#:
        //alu[sub_id, 0x3, AND, event_data, >>16]
        alu[blq, 0x3, AND, event_data, >>14]
        alu[NbiNum, 0x3, AND, event_data, >>18]
        alu[blq_cnt_cur, blq_cnt_mask, AND, event_data, >>4]
        alu[deficit, blq_cnt_cur, -, blq_cnt_prev]
        alu[deficit, deficit, AND, blq_cnt_mask]

        D(MAILBOX1, 0x1111)
        .if (is_aps)
            blm_stats(BLM_STATS_NUM_DMA_EVNTS_RCVD)
        .else
            blm_stats(BLM_STATS_NUM_ALARMS)
        .endif

        .if (BLM_BLQ_LM_REF[BLM_LM_BLQ_NULL_RECYCLE_OFFSET] & 1)
            D(MAILBOX2, 0x2222)
            .while (deficit >= NBI_BLQ_EVENT_THRESHOLD)
                blm_ingress_null_buffer_recycle(NbiNum, blq, NBI_BLQ_EVENT_THRESHOLD)
                alu[deficit, deficit, -, NBI_BLQ_EVENT_THRESHOLD]
                blm_stats(BLM_STATS_DMA_NULL_RECYCLE)
            .endw
        .else
            D(MAILBOX2, 0x3333)
            .while (deficit >= NBI_BLQ_EVENT_THRESHOLD)
                alu[deficit, deficit, -, NBI_BLQ_EVENT_THRESHOLD]
                blm_incr_dma_evnt_pend_cnt()
            .endw

            .while (BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET] > BLM_MAX_DMA_PENDING_EVNTS)
                blm_cache_acquire_lock()
                alu[cache_cnt, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET]]
                blm_cache_release_lock()
                .if (cache_cnt >= NBI_BLQ_EVENT_THRESHOLD)
                    D(MAILBOX3, 0x1111)
                    blm_ingress_push_buffers_from_cache(NbiNum, blq, NBI_BLQ_EVENT_THRESHOLD)
                    blm_decr_dma_evnt_pend_cnt()
                    blm_stats(BLM_STATS_RECYCLE_CACHE_TO_DMA)
                .else
                    D(MAILBOX3, 0x2222)
                    blm_stats(BLM_STATS_RECYCLE_CACHE_LOW)
                    .break
                .endif
            .endw

            alu[--, is_aps, XOR, 1]
            beq[cache_fill_complete#]
            blm_cache_acquire_lock()
            alu[cache_cnt, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET]]
            blm_cache_release_lock()
            .while (cache_cnt < cache_hwm)
                D(MAILBOX3, 0x3333)
                blm_cache_fill(addr, ringid, sig_memget0, cache_fill_complete#)
                blm_cache_acquire_lock()
                alu[cache_cnt, --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET]]
                blm_cache_release_lock()
            .endw
            cache_fill_complete#:
        .endif
        D(MAILBOX1, 0xaaaa0000)

        #ifdef DEBUG
            .reg global $debugdma[8]
            .addr $debugdma[0] 24
            .addr $debugdma[1] 25
            .addr $debugdma[2] 26
            .addr $debugdma[3] 27
            .addr $debugdma[4] 28
            .addr $debugdma[5] 29
            .addr $debugdma[6] 30
            .addr $debugdma[7] 31
            alu[$debugdma[0], --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET]]
            alu[$debugdma[1], --, b, BLM_BLQ_LM_REF[BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET]]
            alu[$debugdma[2], --, b, $event_data[0]]
            alu[$debugdma[3], --, b, blq_cnt_cur]
            alu[$debugdma[4], --, b, blq_cnt_prev]
            alu[$debugdma[5], --, b, deficit]
            alu[$debugdma[6], --, b, blq_evnt_cnt]
            alu[$debugdma[7], --, b, alarm_cnt]
        #endif

        /* update deficit pointers */
        alu[blq_cnt_prev, --, b, blq_cnt_cur]
        local_csr_rd[MAILBOX1]
        immed[to_ticks, 0]
    .endw
.end

#endif /* __BLM_UC__ */
