/*
 * Copyright (C) 2014-2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file:   init_nbi_dma.uc
 * @brief:  Intermediate NBI DMA setup
 *
 * @note:   The Buffer List Manager (BLM) block takes care of following 3
 *          configurations:
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaBlqEvent
 *              NbiDmaCpp_NbiDmaBufferList_BLQueCtrl
 *              NbiDmaCpp_NbiDmaBDSRAM_NbiDmaBDSramEntry
 *          Thereafter, the following are fixed config and do not require
 *          explicit exposure to application developers. They can be
 *          initialized here to static values:
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaCfg
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaRate
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaCredit
 *          Finally, there are 3 sets of registers which are application
 *          dependent which are not configured by the BLM, hence we set these
 *          here based on values specified in init_config.h:
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaBpeChainEnd
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg
 *              NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg
 *
 * @note This code produces a 'NBI DMA BPE credit symbol'. It's required by
 *       Netronome NIC kernel module to offer safe NFP soft reset functionality
 *       (device quiescing). The symbol has the following structure:
 *
 *         struct nfp_nbiX_dma_bpe_credits {
 *             uint32_t version_magic;  // (0xDADA0000 | (version & 0xFF << 8) | (bpe_count & 0xFF))
 *             uint32_t NbiDmaBpe0Cfg;
 *             uint32_t NbiDmaBpe1Cfg;
 *             uint32_t NbiDmaBpe2Cfg;
 *             ....<bpe_count>
 *         }
 *
 *
 */

#ifndef _INIT_NBI_DMA_UC_
#define _INIT_NBI_DMA_UC_

#include "../init_config.h"
#include "init_nbi_dma_csr.uc"

/* Macro to help parse BPE configurations */
#define ENABLE_VAL(x,y,z) x
#define PKT_CREDIT_VAL(x,y,z) y
#define BUF_CREDIT_VAL(x,y,z) z
#define BLQ_PRIMARY(x,y) x
#define BLQ_SECONDARY(x,y) y

/* Sanity Checking below */
#ifndef SPLIT_LENGTH
    #error "SPLIT_LENGTH is not defined"
#endif

/* NBI DMA settings for C0 */
#if (__REVISION_MIN >= __REVISION_C0)

/* FIXME: C0 */
#error "C0 not implemented by NBI DMA init."

/* NBI DMA settings for A0-B0 */
#elif (__REVISION_MAX <= __REVISION_B0)

#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0)) > 256)
    #error "ME Island0's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0)) > 128)
    #error "ME Island0's CTM Buffer credits are not allocated appropriately"
#endif
#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1)) > 256)
    #error "ME Island1's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1)) > 128)
    #error "ME Island1's CTM Buffer credits are not allocated appropriately"
#endif
#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2)) > 256)
    #error "ME Island2's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2)) > 128)
    #error "ME Island2's CTM Buffer credits are not allocated appropriately"
#endif
#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3)) > 256)
    #error "ME Island3's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3)) > 128)
    #error "ME Island3's CTM Buffer credits are not allocated appropriately"
#endif
#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4)) > 256)
    #error "ME Island4's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4)) > 128)
    #error "ME Island4's CTM Buffer credits are not allocated appropriately"
#endif
#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5)) > 256)
    #error "ME Island5's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5)) > 128)
    #error "ME Island5's CTM Buffer credits are not allocated appropriately"
#endif
#if ( (PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6) + \
      PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6)) > 256)
    #error "ME Island6's CTM Packet credits are not allocated appropriately"
#endif
#if ( (BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6) + \
      BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6)) > 128)
    #error "ME Island6's CTM Buffer credits are not allocated appropriately"
#endif

/* NBI DMA settings for mixed A0-B0 and C0 */
#else

/* FIXME: C0 */
#error "Mixed A0-B0 and C0 not implemented by NBI DMA init."

#endif

/* A helper macro to populate the NBI DMA BPE credit symbol */
#macro Nbi_Dma_Bpe_Credits_Populate(NBI_ID, BPE, CTM, PKT_CREDIT, BUF_CREDIT)
    #define_eval OFFSET (BPE*4 + 4)
    #if (NBI_ID == 0)
        .init nbi0_dma_bpe_credits+OFFSET (((BPE & 0x1F)<< 27) | ((CTM & 0x3F) << 21) | ((PKT_CREDIT & 0x7FF) << 10) | (BUF_CREDIT & 0x3FF))
    #else
        .init nbi1_dma_bpe_credits+OFFSET (((BPE & 0x1F)<< 27) | ((CTM & 0x3F) << 21) | ((PKT_CREDIT & 0x7FF) << 10) | (BUF_CREDIT & 0x3FF))
    #endif
    #undef OFFSET
#endm

/** Nbi_Dma_Init
 *
 * DMA initialisation.
 *
 * @param NBI_ID        NBI number, can be 0 or 1
 */
#macro Nbi_Dma_Init(NBI_ID)
    #define NBI_DMA_LOOP 0


    /* The following 3 registers do not require modification by developers,
     * hence they can be defined statically here */
    NbiDmaXpb_NbiDmaCsr_NbiDmaCfg(NBI_ID,
    1,  //CTM Poll search enabled
    0,  //RateLimit Disabled
    0,  //CtmPollIntvl = 512us
    1   //Ctm Poll Enabled
    )
    NbiDmaXpb_NbiDmaCsr_NbiDmaRate(NBI_ID,
    0,  //CreditRate0
    0,  //CreditRate0
    0,  //CreditRate0
    0   //CreditRate0
    )
    NbiDmaXpb_NbiDmaCsr_NbiDmaCredit(NBI_ID,
    3,  //Credit0
    3,  //Credit1
    3,  //Credit2
    3,  //Credit3
    3,  //Threshold0
    3,  //Threshold1
    3,  //Threshold2
    3   //Threshold3
    )

    /* Buffer Pool Config:
     * These values are set to match Catamaran operation mode */

    /* Buffer Pool 0: Packets <= 192B */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        0,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP0_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP0_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP0_DROP_ENABLE
    )
    /* Buffer Pool 1: Packets <= 1984B */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        1,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP1_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP1_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP1_DROP_ENABLE
    )
    /* Buffer Pool 2: Packets > 1984B */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        2,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP2_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP2_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP2_DROP_ENABLE
    )
    /* Buffer Pool 3: Exceptiond Packets <= 192B */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        3,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP3_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP3_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP3_DROP_ENABLE
    )
    /* Buffer Pool 4: Exceptiond Packets <= 1984B */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        4,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP4_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP4_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP4_DROP_ENABLE
    )
    /* Buffer Pool 5: Exceptiond Packets > 1984B */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        5,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP5_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP5_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP5_DROP_ENABLE
    )
    /* Buffer Pool 6: Unused */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        6,  //Pool #
        0,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP6_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP6_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP6_DROP_ENABLE
    )

    #define NBI_DMA_BPE_NUM 32
    #define NBI_DMA_BPE_VERSION 0x01
    #define NBI_DMA_MAGIC_VAL 0xDADA0000

    /* Configure Buffer Pool Entries */
    #if (NBI_ID==0)
        /* NBI DMA BPE Credit symbol */
        .alloc_mem nbi0_dma_bpe_credits emem global ((NBI_DMA_BPE_NUM+1)*4) 8

        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            32, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 32, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND0))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            33, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 33, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND1))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            34, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 34, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND2))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            35, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 35, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND3))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            36, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 36, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND4))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            37, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 37, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND5))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            38, //Ctm
            BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6),
            PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 38, \
                    PKT_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6), \
                    BUF_CREDIT_VAL(NBI0_DMA_BPE_CONFIG_ME_ISLAND6))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif

        /* Lastly, a BPE with 0 credits, used by NBI to drop */
        NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP,    //BpeNum
            0,             //Ctm
            (0),            //BufCredit
            (0)             //PktCredit
        )

        /* Cap off BPE symbol and indicate how many BPEs we've used */
        .init nbi0_dma_bpe_credits+0 (NBI_DMA_MAGIC_VAL | ((NBI_DMA_BPE_VERSION & 0xFF) << 8) | (NBI_DMA_LOOP & 0xFF))

        /* Populate ChainEnd register bit for last valid BPE + unused BPEs */
        #define_eval CHAINEND ((0xFFFFFFFF << (NBI_DMA_LOOP-1)) & 0xFFFFFFFF)
        NbiDmaXpb_NbiDmaCsr_NbiDmaBpeChainEnd(NBI_ID, CHAINEND)
        #undef CHAINEND

    #elif(NBI_ID==1)
        /* NBI DMA BPE Credit symbol */
        .alloc_mem nbi1_dma_bpe_credits emem global ((NBI_DMA_BPE_NUM+1)*4) 8

        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            32, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 32, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND0))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            33, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 33, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND1))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            34, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 34, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND2))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            35, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 35, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND3))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            36, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 36, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND4))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            37, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 37, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND5))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif
        #if (ENABLE_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6) == 1)
            NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP, //BpeNum
            38, //Ctm
            BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6),
            PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6)
            )
            Nbi_Dma_Bpe_Credits_Populate(NBI_ID, NBI_DMA_LOOP, 38, \
                    PKT_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6), \
                    BUF_CREDIT_VAL(NBI1_DMA_BPE_CONFIG_ME_ISLAND6))
            #define_eval NBI_DMA_LOOP (NBI_DMA_LOOP+1)
        #endif

        /* Lastly, a BPE with 0 credits, used by NBI to drop */
        NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,
            NBI_DMA_LOOP,    //BpeNum
            0,             //Ctm
            (0),            //BufCredit
            (0)             //PktCredit
        )

        /* Cap off BPE symbol and indicate how many BPEs we've used */
        .init nbi1_dma_bpe_credits+0 (NBI_DMA_MAGIC_VAL | ((NBI_DMA_BPE_VERSION & 0xFF) << 8) | (NBI_DMA_LOOP & 0xFF))

        /* Populate ChainEnd register bit for last valid BPE + unused BPEs */
        #define_eval CHAINEND ((0xFFFFFFFF << (NBI_DMA_LOOP-1)) & 0xFFFFFFFF)
        NbiDmaXpb_NbiDmaCsr_NbiDmaBpeChainEnd(NBI_ID, CHAINEND)
        #undef CHAINEND

    #endif /* NBI_ID == 1 */
    #undef NBI_DMA_BPE_NUM
    #undef NBI_DMA_BPE_VERSION

    /* Now that we kow what the BPE number is for dropping, we can populate the
     * final Buffer Pool 7 to Drop */
    NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,
        7,  //Pool #
        NBI_DMA_LOOP,  //BpeHead
        SPLIT_LENGTH,
        BLQ_SECONDARY(NBI_DMA_BP7_BLQ_TARGET),
        BLQ_PRIMARY(NBI_DMA_BP7_BLQ_TARGET),
        CTM_OFFSET,
        NBI_DMA_BP7_DROP_ENABLE
    )

    #undef NBI_DMA_LOOP
#endm

#endif /* _INIT_NBI_DMA_UC_ */
