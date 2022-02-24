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
 * @file        blm_iface_asm.h
 * @brief
 */

#ifndef __NFP_BLM_IFACE_ASM_H__
#define __NFP_BLM_IFACE_ASM_H__

#include <nfp_chipres.h>

#if defined(__NFP_LANG_ASM)

/* BLM default configuration file */
#include "../blm_cfg.h"

#define BLM_INIT_DONE                           0xd043
#define NBI_BLQ_EVENT_THRESHOLD                 32 /* 0:16, 1:32, 2:64, 3:128 */


/* Check EMU buffers size and alignment */
#if (EMU_PKTBUF_ALIGNMENT % 2048)
    #error "EMU_PKTBUF_ALIGNMENT must be a multiple of 2048"
#endif

#if (NBI8_BLQ_EMU_0_PKTBUF_SIZE % 2048)
    #error "NBI8_BLQ_EMU_0_PKTBUF_SIZE must be a multiple of 2048"
#endif
#if (NBI8_BLQ_EMU_1_PKTBUF_SIZE % 2048)
    #error "NBI8_BLQ_EMU_1_PKTBUF_SIZE must be a multiple of 2048"
#endif
#if (NBI8_BLQ_EMU_2_PKTBUF_SIZE % 2048)
    #error "NBI8_BLQ_EMU_2_PKTBUF_SIZE must be a multiple of 2048"
#endif
#if (NBI8_BLQ_EMU_3_PKTBUF_SIZE % 2048)
    #error "NBI8_BLQ_EMU_3_PKTBUF_SIZE must be a multiple of 2048"
#endif

#if (NBI9_BLQ_EMU_0_PKTBUF_SIZE % 2048)
    #error "NBI9_BLQ_EMU_0_PKTBUF_SIZE must be a multiple of 2048"
#endif
#if (NBI9_BLQ_EMU_1_PKTBUF_SIZE % 2048)
    #error "NBI9_BLQ_EMU_1_PKTBUF_SIZE must be a multiple of 2048"
#endif
#if (NBI9_BLQ_EMU_2_PKTBUF_SIZE % 2048)
    #error "NBI9_BLQ_EMU_2_PKTBUF_SIZE must be a multiple of 2048"
#endif
#if (NBI9_BLQ_EMU_3_PKTBUF_SIZE % 2048)
    #error "NBI9_BLQ_EMU_3_PKTBUF_SIZE must be a multiple of 2048"
#endif


#ifdef SPLIT_EMU_RINGS
    /* resource declaration */
    /**
     * If two NBI's do not share same EMU rings for packet buffers
     * grab 8 rings, 4 for each NBI.
     */
    .declare_resource BLQ_EMU_RINGS global 8 emem0_queues+8

    /* allocate ring ids from local pool */
    /**
     * Allocate 4 rings per instance of BLM - one for each BLQ - island scope.
     *
     * Make sure the ring IDs are allocated in an ordered sequance.
     *
     */
    .alloc_resource BLM_NBI8_BLQ0_EMU_QID BLQ_EMU_RINGS+0 global 1
    .alloc_resource BLM_NBI8_BLQ1_EMU_QID BLQ_EMU_RINGS+1 global 1
    .alloc_resource BLM_NBI8_BLQ2_EMU_QID BLQ_EMU_RINGS+2 global 1
    .alloc_resource BLM_NBI8_BLQ3_EMU_QID BLQ_EMU_RINGS+3 global 1
    .alloc_resource BLM_NBI9_BLQ0_EMU_QID BLQ_EMU_RINGS+4 global 1
    .alloc_resource BLM_NBI9_BLQ1_EMU_QID BLQ_EMU_RINGS+5 global 1
    .alloc_resource BLM_NBI9_BLQ2_EMU_QID BLQ_EMU_RINGS+6 global 1
    .alloc_resource BLM_NBI9_BLQ3_EMU_QID BLQ_EMU_RINGS+7 global 1

    /* allocate memory for NBI8 EMU Ring Qdescriptors */
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i/**/BLM_NBI8_BLQ0_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE   global  16                      16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i/**/BLM_NBI8_BLQ1_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE   global  16                      16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i/**/BLM_NBI8_BLQ2_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE   global  16                      16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i/**/BLM_NBI8_BLQ3_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE   global  16                      16
    /* allocate memory for NBI9 EMU Ring Qdescriptors */
    .alloc_mem _BLM_NBI9_BLQ0_EMU_QD_BASE   i/**/BLM_NBI9_BLQ0_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    .alloc_mem _BLM_NBI9_BLQ1_EMU_QD_BASE   i/**/BLM_NBI9_BLQ1_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    .alloc_mem _BLM_NBI9_BLQ2_EMU_QD_BASE   i/**/BLM_NBI9_BLQ2_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    .alloc_mem _BLM_NBI9_BLQ3_EMU_QD_BASE   i/**/BLM_NBI9_BLQ3_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    /* allocate memory for NBI8 EMU Rings */
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i/**/BLM_NBI8_BLQ0_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ0_Q_SIZE    BLM_NBI8_BLQ0_Q_SIZE
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i/**/BLM_NBI8_BLQ1_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ1_Q_SIZE    BLM_NBI8_BLQ1_Q_SIZE
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i/**/BLM_NBI8_BLQ2_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ2_Q_SIZE    BLM_NBI8_BLQ2_Q_SIZE
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i/**/BLM_NBI8_BLQ3_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ3_Q_SIZE    BLM_NBI8_BLQ3_Q_SIZE
    /* allocate memory for NBI9 EMU Rings */
    .alloc_mem _BLM_NBI9_BLQ0_EMU_Q_BASE    i/**/BLM_NBI9_BLQ0_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI9_BLQ0_Q_SIZE    BLM_NBI9_BLQ0_Q_SIZE
    .alloc_mem _BLM_NBI9_BLQ1_EMU_Q_BASE    i/**/BLM_NBI9_BLQ1_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI9_BLQ1_Q_SIZE    BLM_NBI9_BLQ1_Q_SIZE
    .alloc_mem _BLM_NBI9_BLQ2_EMU_Q_BASE    i/**/BLM_NBI9_BLQ2_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI9_BLQ2_Q_SIZE    BLM_NBI9_BLQ2_Q_SIZE
    .alloc_mem _BLM_NBI9_BLQ3_EMU_Q_BASE    i/**/BLM_NBI9_BLQ3_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI9_BLQ3_Q_SIZE    BLM_NBI9_BLQ3_Q_SIZE

    #define_eval _NBI_  8
    #define_eval _RING_ 0
    #while (_NBI_ < 10)
        #define_eval _RING_ 0

        #while (_RING_ < 4)

            /* Allocate buffers for the EMU rings */

            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM0_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM0_BUFS_BASE imem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM1_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM1_BUFS_BASE imem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_BUFS_BASE emem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_BUFS_BASE emem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_BUFS_BASE emem2 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif

            /* Allocate buffers for the BLQ rings */

            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM0_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM0_BUFS_BASE imem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM1_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM1_BUFS_BASE imem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_BUFS_BASE emem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_BUFS_BASE emem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_BUFS_BASE emem2 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif

            #define_eval _RING_ (_RING_ + 1)

        #endloop

        #define_eval _NBI_ (_NBI_ + 1)
    #endloop
    #undef _NBI_
    #undef _RING_

#else
    /* resource declaration */
    /**
     * Two NBI's share EMU rings for packet buffers.
     * grab 4 rings, one for each BLQ - global scope.
     */
    .declare_resource BLQ_EMU_RINGS global 4 emem0_queues+4

    .alloc_resource BLM_NBI8_BLQ0_EMU_QID BLQ_EMU_RINGS+0 global 1
    .alloc_resource BLM_NBI8_BLQ1_EMU_QID BLQ_EMU_RINGS+1 global 1
    .alloc_resource BLM_NBI8_BLQ2_EMU_QID BLQ_EMU_RINGS+2 global 1
    .alloc_resource BLM_NBI8_BLQ3_EMU_QID BLQ_EMU_RINGS+3 global 1

    #define BLM_NBI9_BLQ0_EMU_QID           BLM_NBI8_BLQ0_EMU_QID
    #define BLM_NBI9_BLQ1_EMU_QID           BLM_NBI8_BLQ1_EMU_QID
    #define BLM_NBI9_BLQ2_EMU_QID           BLM_NBI8_BLQ2_EMU_QID
    #define BLM_NBI9_BLQ3_EMU_QID           BLM_NBI8_BLQ3_EMU_QID

    #define _BLM_NBI9_BLQ0_EMU_QD_BASE     _BLM_NBI8_BLQ0_EMU_QD_BASE
    #define _BLM_NBI9_BLQ1_EMU_QD_BASE     _BLM_NBI8_BLQ1_EMU_QD_BASE
    #define _BLM_NBI9_BLQ2_EMU_QD_BASE     _BLM_NBI8_BLQ2_EMU_QD_BASE
    #define _BLM_NBI9_BLQ3_EMU_QD_BASE     _BLM_NBI8_BLQ3_EMU_QD_BASE

    #define _BLM_NBI9_BLQ0_EMU_Q_BASE      _BLM_NBI8_BLQ0_EMU_Q_BASE
    #define _BLM_NBI9_BLQ1_EMU_Q_BASE      _BLM_NBI8_BLQ1_EMU_Q_BASE
    #define _BLM_NBI9_BLQ2_EMU_Q_BASE      _BLM_NBI8_BLQ2_EMU_Q_BASE
    #define _BLM_NBI9_BLQ3_EMU_Q_BASE      _BLM_NBI8_BLQ3_EMU_Q_BASE

    /* allocate memory for NBI8 EMU Ring Qdescriptors */
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i/**/BLM_NBI8_BLQ0_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i/**/BLM_NBI8_BLQ1_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i/**/BLM_NBI8_BLQ2_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i/**/BLM_NBI8_BLQ3_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  16                      16
    /* allocate memory for NBI8 EMU Rings */
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i/**/BLM_NBI8_BLQ0_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ0_Q_SIZE    BLM_NBI8_BLQ0_Q_SIZE
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i/**/BLM_NBI8_BLQ1_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ1_Q_SIZE    BLM_NBI8_BLQ1_Q_SIZE
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i/**/BLM_NBI8_BLQ2_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ2_Q_SIZE    BLM_NBI8_BLQ2_Q_SIZE
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i/**/BLM_NBI8_BLQ3_EMU_Q_ISLAND.BLM_BLQ_EMEM_TYPE  global  BLM_NBI8_BLQ3_Q_SIZE    BLM_NBI8_BLQ3_Q_SIZE

    #define_eval _NBI_  8
    #define_eval _RING_ 0
    #while (_NBI_ < 10)
        #define_eval _RING_ 0

        #while (_RING_ < 4)

            /* Allocate buffers for the EMU rings - ONLY FOR NBI8 */
            #if (_NBI_ == 8)
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM0_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM0_BUFS_BASE imem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM1_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM1_BUFS_BASE imem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_IMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_BUFS_BASE emem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_BUFS_BASE emem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_BUFS_BASE emem2 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
				/* Buffers from EMEM cache */
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_CACHE_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_CACHE_BUFS_BASE emem0.emem_cache_upper global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM0_CACHE_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_CACHE_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_CACHE_BUFS_BASE emem1.emem_cache_upper global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM1_CACHE_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
                #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_CACHE_NUM_BUFS > 0)
                    .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_CACHE_BUFS_BASE emem2.emem_cache_upper global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_EMU_EMEM2_CACHE_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
                #endif
            #endif

            /* Allocate buffers for the BLQ rings */

            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM0_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM0_BUFS_BASE imem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM1_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM1_BUFS_BASE imem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_IMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_BUFS_BASE emem0 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_BUFS_BASE emem1 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_BUFS_BASE emem2 global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_CACHE_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_CACHE_BUFS_BASE emem0.emem_cache_upper global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM0_CACHE_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_CACHE_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_CACHE_BUFS_BASE emem1.emem_cache_upper global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM1_CACHE_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif
            #if (BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_CACHE_NUM_BUFS > 0)
                .alloc_mem _BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_CACHE_BUFS_BASE emem2.emem_cache_upper global (NBI/**/_NBI_/**/_BLQ_EMU_/**/_RING_/**/_PKTBUF_SIZE * BLM_NBI/**/_NBI_/**/_BLQ/**/_RING_/**/_BDSRAM_EMEM2_CACHE_NUM_BUFS) EMU_PKTBUF_ALIGNMENT
            #endif

            #define_eval _RING_ (_RING_ + 1)

        #endloop

        #define_eval _NBI_ (_NBI_ + 1)
    #endloop
    #undef _NBI_
    #undef _RING_

#endif

#endif // __NFP_LANG_ASM

#endif // __NFP_BLM_IFACE_ASM_H__
