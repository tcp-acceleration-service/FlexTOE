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
 * @file        blm_internal.h
 * @brief       Internal definitions for BLM
 */

#ifndef __NFP_BLM_INTERNAL_H__
#define __NFP_BLM_INTERNAL_H__

/* Internal use constants */
#define BLM_BLQ_NON_SPLIT_MODE  1
#define BLM_BLQ_SPLIT_MODE      2
#define BLM_SINGLE_NBI_MODE     1
#define BLM_DUAL_NBI_MODE       2

/* Force to use SINGLE_NBI MODE */
#define SINGLE_NBI              1

/* CLS Autopush constants */
#define CLS_AP_FILTER_TYPE               5
#define AUTOPUSH_FILTER_MASK             0xFFC00F

/* Event related constants */
#define_eval NBI_BLQ_EVENT_THRESHOLD_ENCODING   (LOG2(NBI_BLQ_EVENT_THRESHOLD) - LOG2(16))
#define_eval BLM_RECYCLE_LEN                    ((NBI_BLQ_EVENT_THRESHOLD / 2) - 1)
#define_eval BLM_RECYCLE_LEN_PLUS_ONE           (NBI_BLQ_EVENT_THRESHOLD / 2)
#define MASK_10b        0x3ff
#define_eval NBII_lsb   (NBII & 1)

/* Local memory constants */
#define BLM_BLQ_LM_INDEX        0
#define_eval BLM_BLQ_LM_CSR          ACTIVE_LM_ADDR_/**/BLM_BLQ_LM_INDEX
#define_eval BLM_BLQ_LM_REF          *l$index/**/BLM_BLQ_LM_INDEX

/* Local memory offsets for BLM state info */
/*
.declare_resource BLM_LM_OFFSETS me 8
.alloc_resource BLM_LM_BLQ_CACHE_ADDR_OFFSET            BLM_LM_OFFSETS me 1
.alloc_resource BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET       BLM_LM_OFFSETS me 1
.alloc_resource BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET     BLM_LM_OFFSETS me 1
.alloc_resource BLM_LM_BLQ_NULL_RECYCLE_OFFSET          BLM_LM_OFFSETS me 1
*/
.declare_resource BLM_EMU_RING_INIT_CNT global 1

#define BLM_LM_BLQ_CACHE_ADDR_OFFSET            0
#define BLM_LM_BLQ_CACHE_ENTRY_CNT_OFFSET       1
#define BLM_LM_BLQ_DMA_EVNT_PEND_CNT_OFFSET     2
#define BLM_LM_BLQ_NULL_RECYCLE_OFFSET          3
#define BLM_LM_BLQ_CACHE_RDWR_BUSY_OFFSET       4

#define BLM_MAX_DMA_PENDING_EVNTS               5
#define BLM_NBI_BLQ_CACHE_DEFICIT               47

/* BLM Stats */
#define BLQ_STATS_SIZE                    128

.declare_resource BLQ_STATS_OFFSETS island BLQ_STATS_SIZE
.alloc_resource BLM_STATS_CACHE_REFILLS               BLQ_STATS_OFFSETS           island 8 /* Offset-0  */
.alloc_resource BLM_STATS_EMU_RING_UNDERFLOW          BLQ_STATS_OFFSETS           island 8 /* Offset-1  */
.alloc_resource BLM_STATS_RECYCLE_DIRECT              BLQ_STATS_OFFSETS           island 8 /* Offset-2  */
.alloc_resource BLM_STATS_RECYCLE_TM_TO_EMU           BLQ_STATS_OFFSETS           island 8 /* Offset-3  */
.alloc_resource BLM_STATS_RECYCLE_TM_TO_CACHE         BLQ_STATS_OFFSETS           island 8 /* Offset-4  */
.alloc_resource BLM_STATS_RECYCLE_CACHE_TO_DMA        BLQ_STATS_OFFSETS           island 8 /* Offset-5  */
.alloc_resource BLM_STATS_RECYCLE_CACHE_LOW           BLQ_STATS_OFFSETS           island 8 /* Offset-6  */
.alloc_resource BLM_STATS_NUM_DMA_EVNTS_RCVD          BLQ_STATS_OFFSETS           island 8 /* Offset-7  */
.alloc_resource BLM_STATS_NUM_TM_EVNTS_RCVD           BLQ_STATS_OFFSETS           island 8 /* Offset-8  */
.alloc_resource BLM_STATS_DMA_NULL_RECYCLE            BLQ_STATS_OFFSETS           island 8 /* Offset-9  */
.alloc_resource BLM_STATS_TM_NULL_RECYCLE             BLQ_STATS_OFFSETS           island 8 /* Offset-10 */
.alloc_resource BLM_STATS_NUM_ALARMS                  BLQ_STATS_OFFSETS           island 8 /* Offset-11 */
.alloc_resource BLM_STATS_OFFSET_RFU1                 BLQ_STATS_OFFSETS           island 8 /* Offset-12 */
.alloc_resource BLM_STATS_OFFSET_RFU2                 BLQ_STATS_OFFSETS           island 8 /* Offset-13 */
.alloc_resource BLM_STATS_OFFSET_RFU3                 BLQ_STATS_OFFSETS           island 8 /* Offset-14 */
.alloc_resource BLM_STATS_OFFSET_RFU4                 BLQ_STATS_OFFSETS           island 8 /* Offset-15 */

/* BLM CLS Autopush filters */
.declare_resource BLM_AP_FILTERS island 8 cls_apfilters
.alloc_resource BLM_BLQ0_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ1_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ2_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ3_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ4_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ5_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ6_AP_FILTER_NUM BLM_AP_FILTERS island 1
.alloc_resource BLM_BLQ7_AP_FILTER_NUM BLM_AP_FILTERS island 1

/*
 * blm_preproc_check
 */
#macro blm_preproc_check()

    /* --------- Compile time error checks --------- */

    #if (__NFP_TOOL_NFAS < NFP_SW_VERSION(5,2,0,0))
        #error 4 "BLM requires NFAS version 5.2.0.0 or later."
    #endif

    #if (!IS_NFPTYPE(__NFP6000))
        #error 4 "BLM is compatible only with NFP6000 chip family:" __IXPTYPE
    #endif

    #ifndef __NFP_INDIRECT_REF_FORMAT_NFP6000
        #error 4 "BLM requires NFP6000 indirect reference format. Use command line switch '-indirect_ref_format_nfp6000' for nfas."
    #endif

    #ifndef NBII
        #error 4 "NBII not defined. Valid values = [8,9]."
    #else
        #if ((NBII < 8) || (NBII > 9))
            #error 4 "Invalid NBII. Valid values = [8,9]."
        #endif
    #endif

    #ifndef BLM_INSTANCE_ID
        #error 4 "Define BLM_INSTANCE_ID. Valid values=[0,1]"
    #endif

    #if ((BLM_INSTANCE_ID < 0) || (BLM_INSTANCE_ID > 1))
        #error 4 "BLM Instance ID should 0 or 1:"BLM_INSTANCE_ID
    #endif

    #if (((BLM_INSTANCE_ID) ^ (NBII & 1)) != 0)
        #error 4 "NBII should be 8 for BLM_INSTACE-ID=0 AND NBII should be 9 for BLM_INSTANCE_ID=1."
    #endif
#endm /* blm_preproc_check */

/*
 * blm_linker_check
 */
#macro blm_linker_check()
    /* --------- Link time error checks --------- */

.alloc_mem BLM_/**/BLM_INSTANCE_ID/**/_ISLAND_ID   i0.ctm  global  8
/* BLM Instance-0 must run on Island-33: adjacent to NBI-8 */
#if BLM_INSTANCE_ID == 0
#ifdef BLM_FORCE_EVENT_ROUTING
    #ifdef BLM_0_ISLAND_48
        .assert((__ISLAND == 48)) "BLM Instance-0 should run on Island-48!"
    #else
        .assert((__ISLAND == 33)) "BLM Instance-0 should run on Island-33!"
    #endif
#else
    .assert((__ISLAND == 48)) "BLM Instance-0 should run on Island-48!"
#endif /* BLM_FORCE_EVENT_ROUTING */
#endif

/* BLM Instance-1 must run on Island-37: adjacent to NBI-9 */
#if BLM_INSTANCE_ID == 1
#ifdef BLM_FORCE_EVENT_ROUTING
    #ifdef BLM_1_ISLAND_49
        .assert((__ISLAND == 49)) "BLM Instance-1 should run on Island-49!"
    #else
        .assert((__ISLAND == 37)) "BLM Instance-1 should run on Island-37!"
    #endif
#else
    .assert((__ISLAND == 49)) "BLM Instance-1 should run on Island-49!"
#endif /* BLM_FORCE_EVENT_ROUTING */
#endif
#endm /* blm_linker_check */

/*
 * blm_loader_check
 */
#macro blm_loader_check()
    /* --------- Load time error checks --------- */

    /* Make sure NOT entire CTM is used for packet buffers. Leave room for BLM memory requirements */
    .init_csr xpb:CTMXpbMap.MuPacketReg.MUPEMemConfig 1 const
    .init_csr xpb:CTMXpbMap.MuPacketReg.MUPEMemConfig 0 invalid

    /* If event routing is not set, set it to default */

#if BLM_INSTANCE_ID == 0
#ifdef BLM_INIT_EMU_RINGS
    .alloc_resource BLM_EMU_RING_INIT_CHK_0               BLM_EMU_RING_INIT_CNT           global 1
#endif
#ifdef BLM_FORCE_EVENT_ROUTING
    /*
     * The following event routing configuration via init_csr supports running BLM_0 on 33 or 48
     */
    .init_csr xpbm:Nbi0IsldXpbmMap.Overlay.IslandControl.EventRouteConfig 0x00001000 const
    .init_csr xpbm:Me1IsldXpbmMap.Overlay.IslandControl.EventRouteConfig  0x00010040 const
    .init_csr xpbm:Ila0IsldXpbmMap.Overlay.IslandControl.EventRouteConfig 0x00030200 const
#endif /* BLM_FORCE_EVENT_ROUTING */
#endif /* BLM_INSTANCE_ID */

#if BLM_INSTANCE_ID == 1
#ifdef BLM_INIT_EMU_RINGS
    .alloc_resource BLM_EMU_RING_INIT_CHK_1              BLM_EMU_RING_INIT_CNT           global 1
#endif
#ifdef BLM_FORCE_EVENT_ROUTING
    /*
     * The following event routing configuration via init_csr supports running BLM_1 on 37 or 49
     */
    .init_csr xpbm:Nbi1IsldXpbmMap.Overlay.IslandControl.EventRouteConfig 0x00001000 const
    .init_csr xpbm:Me5IsldXpbmMap.Overlay.IslandControl.EventRouteConfig  0x00031000 const
    .init_csr xpbm:Ila1IsldXpbmMap.Overlay.IslandControl.EventRouteConfig 0x00030200 const
#endif /* BLM_FORCE_EVENT_ROUTING */
#endif /* BLM_INSTANCE_ID */

#endm /* blm_loader_check */

/*
 * blm_compatability_check
 */
#macro blm_compatability_check()
    /*
     * Compile time checks.
     * Make sure:
     * 1. Required NFAS version is used
     * 2. CHIP = NFP6000
     * 3. NFP600 indirect ref
     * 4. Minimum required compile flags defined
     */

    blm_preproc_check()

    /*
     * Perform link time checks:
     * 1. make sure BLM runs on right Islands
     */
    blm_linker_check()

    /*
     * Perform load time checks:
     * 1. make sure required csr's are set with right values.
     */
    blm_loader_check()
#endm /* blm_compatability_check */

    /*
     * Debug macro. Write a value to mailbox
     */
#macro D(mbox, value)
    #ifdef DEBUGM
        .begin
        .reg val
        move(val, value)
        local_csr_wr[mbox, val]
        .end
    #endif
#endm

#endif /* __NFP_BLM_INTERNAL_H__ */
