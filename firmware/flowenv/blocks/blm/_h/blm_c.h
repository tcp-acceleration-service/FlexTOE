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
 * @file        blm_c.h
 * @brief       Internal BLM defines needed by clients
 */

#ifndef __NFP_BLM_C_H__
#define __NFP_BLM_C_H__

#if !defined(__NFP_LANG_MICROC)
#error "This file is MicroC only"
#endif

/*
 * NOTE :
 * All resource declaration and allocation happens in the ASM version
 * of this file.  The resources are declared and allocated since the
 * BLM code includes the ASM version of this file.  For NFCC
 * applications we only need to supply a way to get the EMU ring
 * numbers
 */

#include <nfp_chipres.h>

/* BLM default configuration file */
#include "../blm_cfg.h"


#define BLM_INIT_DONE                           0xd043

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
__asm {
    .declare_resource BLQ_EMU_RINGS global 8 emem0_queues+8
    .alloc_resource BLM_NBI8_BLQ0_EMU_QID BLQ_EMU_RINGS+0 global 1
    .alloc_resource BLM_NBI8_BLQ1_EMU_QID BLQ_EMU_RINGS+1 global 1
    .alloc_resource BLM_NBI8_BLQ2_EMU_QID BLQ_EMU_RINGS+2 global 1
    .alloc_resource BLM_NBI8_BLQ3_EMU_QID BLQ_EMU_RINGS+3 global 1
    .alloc_resource BLM_NBI9_BLQ0_EMU_QID BLQ_EMU_RINGS+4 global 1
    .alloc_resource BLM_NBI9_BLQ1_EMU_QID BLQ_EMU_RINGS+5 global 1
    .alloc_resource BLM_NBI9_BLQ2_EMU_QID BLQ_EMU_RINGS+6 global 1
    .alloc_resource BLM_NBI9_BLQ3_EMU_QID BLQ_EMU_RINGS+7 global 1
}

/* Get a BLM ring number for a given NBI (8/9) and BLQ (0-3) */
#define BLM_EMU_RING_ID(_NBI_,_BLQ_)                            \
    __link_sym("BLM_NBI" #_NBI_ "_BLQ" #_BLQ_ "_EMU_QID")

/* The NFCC application can get the 8 ring numbers using :
 *
 * BLM_EMU_RING_ID(8,0);
 * BLM_EMU_RING_ID(8,1);
 * BLM_EMU_RING_ID(8,2);
 * BLM_EMU_RING_ID(8,3);
 *
 * BLM_EMU_RING_ID(9,0);
 * BLM_EMU_RING_ID(9,1);
 * BLM_EMU_RING_ID(9,2);
 * BLM_EMU_RING_ID(9,3);
 */

#if (BLM_BLQ_EMEM_TYPE == BLM_MEM_TYPE_EMEM)

#define EXPORT_BLQ_EMU_MEM_HELP(NAME,ISLAND,ALIGN,SIZE)                 \
    volatile __declspec(export, shared, i##ISLAND##.emem, addr40,       \
                        aligned(ALIGN)) unsigned char NAME[SIZE]
#define EXPORT_BLQ_EMU_MEM(NAME,ISLAND,ALIGN,SIZE)      \
    EXPORT_BLQ_EMU_MEM_HELP(NAME,ISLAND,ALIGN,SIZE)

#if (BLM_NBI8_BLQ0_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ0_EMU_QD_BASE, BLM_NBI8_BLQ0_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ0_EMU_Q_BASE, BLM_NBI8_BLQ0_EMU_Q_ISLAND,
                   BLM_NBI8_BLQ0_Q_SIZE, BLM_NBI8_BLQ0_Q_SIZE);
#endif

#if (BLM_NBI8_BLQ1_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ1_EMU_QD_BASE, BLM_NBI8_BLQ1_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ1_EMU_Q_BASE, BLM_NBI8_BLQ1_EMU_Q_ISLAND, \
                   BLM_NBI8_BLQ1_Q_SIZE, BLM_NBI8_BLQ1_Q_SIZE);
#endif

#if (BLM_NBI8_BLQ2_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ2_EMU_QD_BASE, BLM_NBI8_BLQ2_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ2_EMU_Q_BASE, BLM_NBI8_BLQ2_EMU_Q_ISLAND, \
                   BLM_NBI8_BLQ2_Q_SIZE, BLM_NBI8_BLQ2_Q_SIZE);
#endif

#if (BLM_NBI8_BLQ3_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ3_EMU_QD_BASE, BLM_NBI8_BLQ3_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ3_EMU_Q_BASE, BLM_NBI8_BLQ3_EMU_Q_ISLAND, \
                   BLM_NBI8_BLQ3_Q_SIZE, BLM_NBI8_BLQ3_Q_SIZE);
#endif

#if (BLM_NBI9_BLQ0_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ0_EMU_QD_BASE, BLM_NBI9_BLQ0_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ0_EMU_Q_BASE, BLM_NBI9_BLQ0_EMU_Q_ISLAND, \
                   BLM_NBI9_BLQ0_Q_SIZE, BLM_NBI9_BLQ0_Q_SIZE);
#endif

#if (BLM_NBI9_BLQ1_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ1_EMU_QD_BASE, BLM_NBI9_BLQ1_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ1_EMU_Q_BASE, BLM_NBI9_BLQ1_EMU_Q_ISLAND, \
                   BLM_NBI9_BLQ1_Q_SIZE, BLM_NBI9_BLQ1_Q_SIZE);
#endif

#if (BLM_NBI9_BLQ2_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ2_EMU_QD_BASE, BLM_NBI9_BLQ2_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ2_EMU_Q_BASE, BLM_NBI9_BLQ2_EMU_Q_ISLAND, \
                   BLM_NBI9_BLQ2_Q_SIZE, BLM_NBI9_BLQ2_Q_SIZE);
#endif

#if (BLM_NBI9_BLQ3_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ3_EMU_QD_BASE, BLM_NBI9_BLQ3_EMU_Q_ISLAND, \
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI9_BLQ3_EMU_Q_BASE, BLM_NBI9_BLQ3_EMU_Q_ISLAND, \
                   BLM_NBI9_BLQ3_Q_SIZE, BLM_NBI9_BLQ3_Q_SIZE);
#endif



#elif (BLM_BLQ_EMEM_TYPE == BLM_MEM_TYPE_EMEM_CACHE_UPPER)
/* NFCC does not support allocating memory in emem_cache_upper, so
 * we use __asm to do that */
#if (BLM_NBI8_BLQ0_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ0_Q_SIZE BLM_NBI8_BLQ0_Q_SIZE
}
#elif (BLM_NBI8_BLQ0_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ0_Q_SIZE BLM_NBI8_BLQ0_Q_SIZE
}
#elif (BLM_NBI8_BLQ0_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ0_Q_SIZE BLM_NBI8_BLQ0_Q_SIZE
}
#endif

#if (BLM_NBI8_BLQ1_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ1_Q_SIZE BLM_NBI8_BLQ1_Q_SIZE
}
#elif (BLM_NBI8_BLQ1_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ1_Q_SIZE BLM_NBI8_BLQ1_Q_SIZE
}
#elif (BLM_NBI8_BLQ1_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ1_Q_SIZE BLM_NBI8_BLQ1_Q_SIZE
}
#endif

#if (BLM_NBI8_BLQ2_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ2_Q_SIZE BLM_NBI8_BLQ2_Q_SIZE
 }
#elif (BLM_NBI8_BLQ2_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ2_Q_SIZE BLM_NBI8_BLQ2_Q_SIZE
}
#elif (BLM_NBI8_BLQ2_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ2_Q_SIZE BLM_NBI8_BLQ2_Q_SIZE
}
#endif

#if (BLM_NBI8_BLQ3_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ3_Q_SIZE BLM_NBI8_BLQ3_Q_SIZE
}
#elif (BLM_NBI8_BLQ3_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ3_Q_SIZE BLM_NBI8_BLQ3_Q_SIZE
}
#elif (BLM_NBI8_BLQ3_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ3_Q_SIZE BLM_NBI8_BLQ3_Q_SIZE
}
#endif


#if (BLM_NBI9_BLQ0_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI9_BLQ0_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ0_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI9_BLQ0_Q_SIZE BLM_NBI9_BLQ0_Q_SIZE
}
#elif (BLM_NBI9_BLQ0_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI9_BLQ0_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ0_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI9_BLQ0_Q_SIZE BLM_NBI9_BLQ0_Q_SIZE
}
#elif (BLM_NBI9_BLQ0_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI9_BLQ0_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ0_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI9_BLQ0_Q_SIZE BLM_NBI9_BLQ0_Q_SIZE
}
#endif

#if (BLM_NBI9_BLQ1_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI9_BLQ1_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ1_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI9_BLQ1_Q_SIZE BLM_NBI9_BLQ1_Q_SIZE
}
#elif (BLM_NBI9_BLQ1_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI9_BLQ1_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ1_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI9_BLQ1_Q_SIZE BLM_NBI9_BLQ1_Q_SIZE
}
#elif (BLM_NBI9_BLQ1_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI9_BLQ1_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ1_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI9_BLQ1_Q_SIZE BLM_NBI9_BLQ1_Q_SIZE
}
#endif

#if (BLM_NBI9_BLQ2_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI9_BLQ2_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ2_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI9_BLQ2_Q_SIZE BLM_NBI9_BLQ2_Q_SIZE
  }
#elif (BLM_NBI9_BLQ2_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI9_BLQ2_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ2_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI9_BLQ2_Q_SIZE BLM_NBI9_BLQ2_Q_SIZE
}
#elif (BLM_NBI9_BLQ2_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI9_BLQ2_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ2_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI9_BLQ2_Q_SIZE BLM_NBI9_BLQ2_Q_SIZE
}
#endif

#if (BLM_NBI9_BLQ3_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI9_BLQ3_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ3_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI9_BLQ3_Q_SIZE BLM_NBI9_BLQ3_Q_SIZE
}
#elif (BLM_NBI9_BLQ3_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI9_BLQ3_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ3_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI9_BLQ3_Q_SIZE BLM_NBI9_BLQ3_Q_SIZE
}
#elif (BLM_NBI9_BLQ3_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI9_BLQ3_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI9_BLQ3_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI9_BLQ3_Q_SIZE BLM_NBI9_BLQ3_Q_SIZE
}
#endif

/* Make the above accessible from MicroC */
#define BLM_NBI8_BLQ0_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ0_EMU_QD_BASE")
#define BLM_NBI8_BLQ1_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ1_EMU_QD_BASE")
#define BLM_NBI8_BLQ2_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ2_EMU_QD_BASE")
#define BLM_NBI8_BLQ3_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ3_EMU_QD_BASE")

#define BLM_NBI8_BLQ0_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ0_EMU_Q_BASE")
#define BLM_NBI8_BLQ1_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ1_EMU_Q_BASE")
#define BLM_NBI8_BLQ2_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ2_EMU_Q_BASE")
#define BLM_NBI8_BLQ3_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ3_EMU_Q_BASE")

#define BLM_NBI9_BLQ0_EMU_QD_BASE __link_sym("_BLM_NBI9_BLQ0_EMU_QD_BASE")
#define BLM_NBI9_BLQ1_EMU_QD_BASE __link_sym("_BLM_NBI9_BLQ1_EMU_QD_BASE")
#define BLM_NBI9_BLQ2_EMU_QD_BASE __link_sym("_BLM_NBI9_BLQ2_EMU_QD_BASE")
#define BLM_NBI9_BLQ3_EMU_QD_BASE __link_sym("_BLM_NBI9_BLQ3_EMU_QD_BASE")

#define BLM_NBI9_BLQ0_EMU_Q_BASE __link_sym("_BLM_NBI9_BLQ0_EMU_Q_BASE")
#define BLM_NBI9_BLQ1_EMU_Q_BASE __link_sym("_BLM_NBI9_BLQ1_EMU_Q_BASE")
#define BLM_NBI9_BLQ2_EMU_Q_BASE __link_sym("_BLM_NBI9_BLQ2_EMU_Q_BASE")
#define BLM_NBI9_BLQ3_EMU_Q_BASE __link_sym("_BLM_NBI9_BLQ3_EMU_Q_BASE")

#else /* BLM_BLQ_EMEM_TYPE  */
#error "Unsupported memory type"
#endif /* BLM_BLQ_EMEM_TYPE  */


/* Allocate buffers memory for ring 0 */
#if (BLM_NBI8_BLQ0_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
   BLM_NBI8_BLQ0_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                     BLM_NBI8_BLQ0_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_IMEM1_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ0_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ0_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 1 */
#if (BLM_NBI8_BLQ1_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ1_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 2 */
#if (BLM_NBI8_BLQ2_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ2_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 3 */
#if (BLM_NBI8_BLQ3_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ3_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 0 */
#if (BLM_NBI9_BLQ0_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_EMU_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ0_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_EMU_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ0_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_EMU_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ0_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_EMU_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ0_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_EMU_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ0_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI9_BLQ0_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_BDSRAM_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ0_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_BDSRAM_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ0_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_BDSRAM_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ0_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_BDSRAM_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ0_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ0_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ0_BDSRAM_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ0_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 1 */
#if (BLM_NBI9_BLQ1_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_EMU_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ1_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_EMU_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ1_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_EMU_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ1_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_EMU_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ1_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_EMU_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ1_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI9_BLQ1_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_BDSRAM_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ1_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_BDSRAM_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ1_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_BDSRAM_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ1_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_BDSRAM_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ1_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ1_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ1_BDSRAM_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ1_BDSRAM_EMEM2_NUM_BUFS];
#endif


    /* Allocate buffers memory for ring 2 */
#if (BLM_NBI9_BLQ2_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_EMU_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ2_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_EMU_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ2_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_EMU_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ2_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_EMU_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ2_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_EMU_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ2_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI9_BLQ2_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_BDSRAM_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ2_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_BDSRAM_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ2_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_BDSRAM_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ2_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_BDSRAM_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ2_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ2_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ2_BDSRAM_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ2_BDSRAM_EMEM2_NUM_BUFS];
#endif

/* Allocate buffers memory for ring 3 */
#if (BLM_NBI9_BLQ3_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_EMU_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ3_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_EMU_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ3_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_EMU_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ3_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_EMU_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ3_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_EMU_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI9_BLQ3_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI9_BLQ3_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_BDSRAM_IMEM0_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ3_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_BDSRAM_IMEM1_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ3_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_BDSRAM_EMEM0_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ3_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_BDSRAM_EMEM1_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ3_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI9_BLQ3_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI9_BLQ3_BDSRAM_EMEM2_BUFS_BASE[NBI9_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI9_BLQ3_BDSRAM_EMEM2_NUM_BUFS];
#endif


#else /* SPLIT_EMU_RINGS */

__asm {
    .declare_resource BLQ_EMU_RINGS global 4 emem0_queues+4
    .alloc_resource BLM_NBI8_BLQ0_EMU_QID BLQ_EMU_RINGS+0 global 1
    .alloc_resource BLM_NBI8_BLQ1_EMU_QID BLQ_EMU_RINGS+1 global 1
    .alloc_resource BLM_NBI8_BLQ2_EMU_QID BLQ_EMU_RINGS+2 global 1
    .alloc_resource BLM_NBI8_BLQ3_EMU_QID BLQ_EMU_RINGS+3 global 1
}

/* Get a BLM ring number for a given NBI (8/9) and BLQ (0-3) */
#define BLM_EMU_RING_ID(_NBI_,_BLQ_)                    \
    __link_sym("BLM_NBI8_BLQ" #_BLQ_ "_EMU_QID")

/* The NFCC application can get the 4 ring numbers using :
 *
 * BLM_EMU_RING_ID(8,0);
 * BLM_EMU_RING_ID(8,1);
 * BLM_EMU_RING_ID(8,2);
 * BLM_EMU_RING_ID(8,3);
 *
 * BLM_EMU_RING_ID(9,0);
 * BLM_EMU_RING_ID(9,1);
 * BLM_EMU_RING_ID(9,2);
 * BLM_EMU_RING_ID(9,3);
 */

#define BLM_NBI9_BLQ0_EMU_QD_BASE     BLM_NBI8_BLQ0_EMU_QD_BASE
#define BLM_NBI9_BLQ1_EMU_QD_BASE     BLM_NBI8_BLQ1_EMU_QD_BASE
#define BLM_NBI9_BLQ2_EMU_QD_BASE     BLM_NBI8_BLQ2_EMU_QD_BASE
#define BLM_NBI9_BLQ3_EMU_QD_BASE     BLM_NBI8_BLQ3_EMU_QD_BASE

#define BLM_NBI9_BLQ0_EMU_Q_BASE      BLM_NBI8_BLQ0_EMU_Q_BASE
#define BLM_NBI9_BLQ1_EMU_Q_BASE      BLM_NBI8_BLQ1_EMU_Q_BASE
#define BLM_NBI9_BLQ2_EMU_Q_BASE      BLM_NBI8_BLQ2_EMU_Q_BASE
#define BLM_NBI9_BLQ3_EMU_Q_BASE      BLM_NBI8_BLQ3_EMU_Q_BASE

#if (BLM_BLQ_EMEM_TYPE == BLM_MEM_TYPE_EMEM)

#define EXPORT_BLQ_EMU_MEM_HELP(NAME,ISLAND,ALIGN,SIZE)           \
    volatile __declspec(export, shared, i##ISLAND##.emem, addr40, \
                        aligned(ALIGN)) unsigned char NAME[SIZE]
#define EXPORT_BLQ_EMU_MEM(NAME,ISLAND,ALIGN,SIZE) \
    EXPORT_BLQ_EMU_MEM_HELP(NAME,ISLAND,ALIGN,SIZE)

#if (BLM_NBI8_BLQ0_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ0_EMU_QD_BASE, BLM_NBI8_BLQ0_EMU_Q_ISLAND,
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ0_EMU_Q_BASE, BLM_NBI8_BLQ0_EMU_Q_ISLAND,
                   BLM_NBI8_BLQ0_Q_SIZE, BLM_NBI8_BLQ0_Q_SIZE);
#endif

#if (BLM_NBI8_BLQ1_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ1_EMU_QD_BASE, BLM_NBI8_BLQ1_EMU_Q_ISLAND,
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ1_EMU_Q_BASE, BLM_NBI8_BLQ1_EMU_Q_ISLAND,
                   BLM_NBI8_BLQ1_Q_SIZE, BLM_NBI8_BLQ1_Q_SIZE);
#endif

#if (BLM_NBI8_BLQ2_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ2_EMU_QD_BASE, BLM_NBI8_BLQ2_EMU_Q_ISLAND,
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ2_EMU_Q_BASE, BLM_NBI8_BLQ2_EMU_Q_ISLAND,
                   BLM_NBI8_BLQ2_Q_SIZE, BLM_NBI8_BLQ2_Q_SIZE);
#endif

#if (BLM_NBI8_BLQ3_Q_SIZE > 0)
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ3_EMU_QD_BASE, BLM_NBI8_BLQ3_EMU_Q_ISLAND,
                   16, 16);
EXPORT_BLQ_EMU_MEM(BLM_NBI8_BLQ3_EMU_Q_BASE, BLM_NBI8_BLQ3_EMU_Q_ISLAND,
                   BLM_NBI8_BLQ3_Q_SIZE, BLM_NBI8_BLQ3_Q_SIZE);
#endif

#elif (BLM_BLQ_EMEM_TYPE == BLM_MEM_TYPE_EMEM_CACHE_UPPER)
/* NFCC does not support allocating memory in emem_cache_upper, so
 * we use __asm to do that */
#if (BLM_NBI8_BLQ0_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ0_Q_SIZE BLM_NBI8_BLQ0_Q_SIZE
}
#elif (BLM_NBI8_BLQ0_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ0_Q_SIZE BLM_NBI8_BLQ0_Q_SIZE
}
#elif (BLM_NBI8_BLQ0_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ0_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ0_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ0_Q_SIZE BLM_NBI8_BLQ0_Q_SIZE
}
#endif

#if (BLM_NBI8_BLQ1_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ1_Q_SIZE BLM_NBI8_BLQ1_Q_SIZE
}
#elif (BLM_NBI8_BLQ1_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ1_Q_SIZE BLM_NBI8_BLQ1_Q_SIZE
}
#elif (BLM_NBI8_BLQ1_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ1_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ1_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ1_Q_SIZE BLM_NBI8_BLQ1_Q_SIZE
}
#endif

#if (BLM_NBI8_BLQ2_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ2_Q_SIZE BLM_NBI8_BLQ2_Q_SIZE
}
#elif (BLM_NBI8_BLQ2_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ2_Q_SIZE BLM_NBI8_BLQ2_Q_SIZE
}
#elif (BLM_NBI8_BLQ2_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ2_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ2_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ2_Q_SIZE BLM_NBI8_BLQ2_Q_SIZE
}
#endif

#if (BLM_NBI8_BLQ3_EMU_Q_ISLAND == 24)
__asm {
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i24.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i24.emem_cache_upper global \
        BLM_NBI8_BLQ3_Q_SIZE BLM_NBI8_BLQ3_Q_SIZE
}
#elif (BLM_NBI8_BLQ3_EMU_Q_ISLAND == 25)
__asm {
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i25.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i25.emem_cache_upper global \
        BLM_NBI8_BLQ3_Q_SIZE BLM_NBI8_BLQ3_Q_SIZE
}
#elif (BLM_NBI8_BLQ3_EMU_Q_ISLAND == 26)
__asm {
    .alloc_mem _BLM_NBI8_BLQ3_EMU_QD_BASE   i26.emem_cache_upper global 16 16
    .alloc_mem _BLM_NBI8_BLQ3_EMU_Q_BASE    i26.emem_cache_upper global \
        BLM_NBI8_BLQ3_Q_SIZE BLM_NBI8_BLQ3_Q_SIZE
}
#endif

/* Make the above accessible from MicroC */
#define BLM_NBI8_BLQ0_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ0_EMU_QD_BASE")
#define BLM_NBI8_BLQ1_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ1_EMU_QD_BASE")
#define BLM_NBI8_BLQ2_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ2_EMU_QD_BASE")
#define BLM_NBI8_BLQ3_EMU_QD_BASE __link_sym("_BLM_NBI8_BLQ3_EMU_QD_BASE")

#define BLM_NBI8_BLQ0_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ0_EMU_Q_BASE")
#define BLM_NBI8_BLQ1_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ1_EMU_Q_BASE")
#define BLM_NBI8_BLQ2_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ2_EMU_Q_BASE")
#define BLM_NBI8_BLQ3_EMU_Q_BASE __link_sym("_BLM_NBI8_BLQ3_EMU_Q_BASE")

#else /* BLM_BLQ_EMEM_TYPE */
#error "Unsupported memory type"
#endif

/* Allocate buffers memory for ring 0 */
#if (BLM_NBI8_BLQ0_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ0_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ0_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ0_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ0_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_0_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ0_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 1 */
#if (BLM_NBI8_BLQ1_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ1_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ1_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ1_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ1_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_1_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ1_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 2 */
#if (BLM_NBI8_BLQ2_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ2_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ2_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ2_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ2_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_2_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ2_BDSRAM_EMEM2_NUM_BUFS];
#endif


/* Allocate buffers memory for ring 3 */
#if (BLM_NBI8_BLQ3_EMU_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_EMU_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_EMU_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                      BLM_NBI8_BLQ3_EMU_EMEM2_NUM_BUFS];
#endif

#if (BLM_NBI8_BLQ3_BDSRAM_IMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, imem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_IMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_IMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_IMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, imem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_IMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_IMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_EMEM0_NUM_BUFS > 0)
volatile __declspec(export, shared, emem0, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_EMEM0_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_EMEM0_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_EMEM1_NUM_BUFS > 0)
volatile __declspec(export, shared, emem1, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_EMEM1_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_EMEM1_NUM_BUFS];
#endif
#if (BLM_NBI8_BLQ3_BDSRAM_EMEM2_NUM_BUFS > 0)
volatile __declspec(export, shared, emem2, addr40,
                    aligned(EMU_PKTBUF_ALIGNMENT)) unsigned char
    BLM_NBI8_BLQ3_BDSRAM_EMEM2_BUFS_BASE[NBI8_BLQ_EMU_3_PKTBUF_SIZE *
                                         BLM_NBI8_BLQ3_BDSRAM_EMEM2_NUM_BUFS];
#endif

#endif  /* SPLIT_EMU_RINGS */

#endif /* __NFP_BLM_C_H__ */
