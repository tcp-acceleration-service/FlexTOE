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
 * @file:   init_config.h
 * @brief:  Configuration which specify the allocation of various resources
 *          across the NFP at firmware load-time. These are meant to be
 *          overridden by a custom version of this header file.
 *
 * @note:   This configuration assumes the use of the the Catamaran Picocode
 *          running on the NBI
 *
 */

#ifndef _INIT_CONFIG_H_
#define _INIT_CONFIG_H_

/*************************************************************************
 * General NFP Configuration
 *
 * These values affect global NFP behaviour, not solely applicable to one
 * component/island of the NFP
 ************************************************************************/

/* Determines how many NBIs to use for traffic.
 * Whether they are physically present is not checked */
#ifndef NBI_COUNT
    #define NBI_COUNT 1
#endif

/*************************************************************************
 * NBI DMA Configuration
 *
 * A large portion of NBI DMA configuration is performed by the Buffer List
 * Manager, and thus we only expose non-BLM related items which one may tweak.
 *
 ************************************************************************/

/* The number of bytes prepended to the start of the packet */
#ifndef NBI_PKT_PREPEND_BYTES
    #define NBI_PKT_PREPEND_BYTES 0
#endif

/* The starting offset of the packet data in CTM */
#ifndef PKT_NBI_OFFSET
    #define PKT_NBI_OFFSET 64
#endif

#if (__REVISION_MIN == __REVISION_A0)
    #if (PKT_NBI_OFFSET == 32)
        #define CTM_OFFSET 0
    #elif (PKT_NBI_OFFSET == 64)
        #define CTM_OFFSET 1
    #else
        #error "Invalid PKT_NBI_OFFSET must be 32 or 64"
    #endif
#elif (__REVISION_MIN == __REVISION_B0)
    #if (PKT_NBI_OFFSET == 64)
        #define CTM_OFFSET 1
    #elif (PKT_NBI_OFFSET == 128)
        #define CTM_OFFSET 0
    #else
        #error "Invalid PKT_NBI_OFFSET must be 64 or 128"
    #endif
#else
    #error "Invalid CHIP REVISION"
#endif

/* Maxmimum split length boundary between a packets CTM buffer and it's backing
 * IMEM/EMEM buffer:
 * 0 - Split at 256B boundary
 * 1 - Split at 512B boundary
 * 2 - Split at 1024B boundary
 * 3 - Split at 2048B boundary
 */
#ifndef SPLIT_LENGTH
    #define SPLIT_LENGTH 3
#endif

/* Set this to zero if we want the NBI to back-up and start emitting pause
 * frames when buffers are full */
#ifndef NBI_DMA_BP_DROP_ENABLE
    #define NBI_DMA_BP_DROP_ENABLE 0
#endif

/* Define this to disable/enable the NBI DMA Buffer Pool drop behavior on a
 * per-Buffer Pool basis; otherwise, the global NBI DMA Buffer Pool drop
 * behavior will be used by default. */
#ifndef NBI_DMA_BP0_DROP_ENABLE
    #define NBI_DMA_BP0_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP1_DROP_ENABLE
    #define NBI_DMA_BP1_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP2_DROP_ENABLE
    #define NBI_DMA_BP2_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP3_DROP_ENABLE
    #define NBI_DMA_BP3_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP4_DROP_ENABLE
    #define NBI_DMA_BP4_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP5_DROP_ENABLE
    #define NBI_DMA_BP5_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP6_DROP_ENABLE
    #define NBI_DMA_BP6_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif
#ifndef NBI_DMA_BP7_DROP_ENABLE
    #define NBI_DMA_BP7_DROP_ENABLE NBI_DMA_BP_DROP_ENABLE
#endif

/* The allocation of Primary and Secondary Buffer List Queues targets amongst
 * the available Buffer Pools. The NFP offers four available Buffer List Queues
 * per NBI. For targets with muliple NBIs we assume identical NBI
 * configuration.
 *
 * As guidance, note the offical load-balancing the Catamaran Picocode uses:
 *     Buffer Pool 0: Packets <= 192B
 *     Buffer Pool 1: Packets <= 1984B
 *     Buffer Pool 2: Packets > 1984B
 *     Buffer Pool 3: Exceptiond Packets <= 192B
 *     Buffer Pool 4: Exceptiond Packets <= 1984B
 *     Buffer Pool 5: Exceptiond Packets > 1984B
 *     Buffer Pool 6: Unused
 *     Buffer Pool 7: Drop
 *
 *     Syntax:
 *     <Primary Buffer List, Secondary Buffer List>
 */
#ifndef NBI_DMA_BP0_BLQ_TARGET
    #define NBI_DMA_BP0_BLQ_TARGET 0,1
    #define NBI_DMA_BP1_BLQ_TARGET 1,0
    #define NBI_DMA_BP2_BLQ_TARGET 2,3
    #define NBI_DMA_BP3_BLQ_TARGET 0,1
    #define NBI_DMA_BP4_BLQ_TARGET 1,0
    #define NBI_DMA_BP5_BLQ_TARGET 2,3
    #define NBI_DMA_BP6_BLQ_TARGET 2,3
    #define NBI_DMA_BP7_BLQ_TARGET 0,1
#endif

/* Allocation of per-CTM Packet and Buffer Credits shared between CTM and NBI
 *
 *     Here we expose granular config of the credit splits and targeting of
 *     each ME Island. This same mechanism is also used it specify which
 *     islands will recieve traffic directly from the NBI upon ingress.
 *
 *     Syntax:
 *     <RX from NBIx ? 1=yes/0=no, NBIx Packet Credits , NBIx Buffer Credits>
 * */
#ifndef NBI0_DMA_BPE_CONFIG_ME_ISLAND0
    #if (NBI_COUNT == 1)
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND0 1,256,63
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND1 1,256,63
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND2 1,256,63
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND3 1,256,63
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND4 1,256,63
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND5 1,256,63
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND6 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND0 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND1 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND2 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND3 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND4 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND5 0,0,0
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND6 0,0,0
    #elif(NBI_COUNT == 2)
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND0 1,128,31
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND1 1,128,31
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND2 1,128,31
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND3 1,128,31
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND4 1,128,31
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND5 1,128,31
        #define NBI0_DMA_BPE_CONFIG_ME_ISLAND6 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND0 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND1 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND2 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND3 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND4 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND5 1,128,31
        #define NBI1_DMA_BPE_CONFIG_ME_ISLAND6 1,128,31
    #else
        #error "Unexpected NBI_COUNT"
    #endif
#endif

/*************************************************************************
 * NBI Traffic Manager Configuration
 *
 * The following TM Configuration are deemed unnecessary to tweak and have
 * default values:
 *     -Minipacket Credit based Flow Control is enabled.
 *     -Scheduling is enabled.
 *     -MAC out of band flow control is disabled.
 *     -Poisoning of data and other propagation mechanisms in case of errors is
 *      disabled.
 * The following items must be configured at runtime:
 *     -ShaperEnable bit and invidual shaper configurations. Shaping is off at
 *      init time.
 *     -Individual Scheduler settings (weightings, thresholds etc)
 *
 ************************************************************************/

/* Packet Sequencing/Ordering.
 *
 * Options:
 *      0: 0  Sequencers - No reordering
 *      1: 1  Sequencers - Reorder 1024 packets
 *      2: 2  Sequencers - Reorder  512 packets per sequencer
 *      3: 4  Sequencers - Reorder  256 packets per sequencer
 *      4: 8  Sequencers - Reorder  128 packets per sequencer
 *      5: 16 Sequencers - Reorder   64 packets per sequencer
 *      6: 32 Sequencers - Reorder   32 packets per sequencer
 * */
#ifndef NBI_TM_NUM_SEQUENCERS
    #define NBI_TM_NUM_SEQUENCERS 0
#endif
/* Keep Sequencer0 off as a target to send packets not requiring ordering */
#ifndef NBI_TM_ENABLE_SEQUENCER0
    #define NBI_TM_ENABLE_SEQUENCER0 0
#endif

/* Channel level select bit.
 *
 * Select the definition of the MAC channel number and the MAC XOFF Flow
 * control to be either the 128 level 0 scheduler ports, or 128 ports on the
 * first 16 level 1 schedulers (or level 2 schedulers depending on the value of
 * NBI_TM_L1_INPUT_SELECT). Note: When NBI_TM_L1_INPUT_SELECT=0 and
 * NBI_TM_CHANNEL_LEVEL_SELECT=1, only 128 queues are allowed to be active in
 * this configuration.
 *
 * 0: MAC Channel number defined to be level 0 ports
 * 1: MAC Channel number defined to be level 1 ports
 * */
#ifndef NBI_TM_CHANNEL_LEVEL_SELECT
    #define NBI_TM_CHANNEL_LEVEL_SELECT 1
#endif

/* Enable/Disable Level 1 schedulers. */
#ifndef NBI_TM_L1_INPUT_SELECT
    #define NBI_TM_L1_INPUT_SELECT 1
#endif

/* Check if specifying number of queues for each half of NBI TM */
#ifdef NBI_TM_H_0_Q

    /* Make sure number of queues is valid */
    #if (NBI_TM_H_0_Q > 64)
        #if ((NBI_TM_CHANNEL_LEVEL_SELECT == 1) && (NBI_TM_L1_INPUT_SELECT == 0))
            #error "Can't use more then 64 queues in first half of TM when ChannelLevelSelect=0 and L1InputSelect=0"
        #elif (NBI_TM_H_0_Q > 512)
            #error "Number of queues in first half of TM cannot be greater than 512"
        #endif
    #endif
    #if (NBI_TM_H_1_Q > 64)
        #if ((NBI_TM_CHANNEL_LEVEL_SELECT == 1) && (NBI_TM_L1_INPUT_SELECT == 0))
            #error "Can't use more then 64 queues in second half of TM when ChannelLevelSelect=0 and L1InputSelect=0"
        #elif (NBI_TM_H_1_Q > 512)
            #error "Number of queues in second half of TM cannot be greater than 512"
        #endif
    #endif

    /* Evenly split queue space amongst all queues configured */
    #if ((NBI_TM_H_0_Q + NBI_TM_H_1_Q) <= 4)
        #define NBI_TM_Q_CFG_QSIZE 12
    #else
        #define NBI_TM_Q_CFG_QSIZE                              \
            (LOG2((16384 / (NBI_TM_H_0_Q + NBI_TM_H_1_Q)), -1))
    #endif

    #define NBI0_TM_Q_CFG_RANGE0 1,0,(NBI_TM_H_0_Q - 1),NBI_TM_Q_CFG_QSIZE
    #if (NBI_TM_H_1_Q > 0)
        #if ((NBI_TM_CHANNEL_LEVEL_SELECT == 1) && (NBI_TM_L1_INPUT_SELECT == 0))
            #define NBI0_TM_Q_CFG_RANGE1                   \
                1,64,(NBI_TM_H_1_Q - 1),NBI_TM_Q_CFG_QSIZE
        #else
            #define NBI0_TM_Q_CFG_RANGE1                    \
                1,512,(NBI_TM_H_1_Q - 1),NBI_TM_Q_CFG_QSIZE
        #endif
    #endif

    #if NBI_COUNT == 2
        #define NBI1_TM_Q_CFG_RANGE0 1,0,(NBI_TM_H_0_Q - 1),NBI_TM_Q_CFG_QSIZE
        #if (NBI_TM_H_1_Q > 0)
            #if ((NBI_TM_CHANNEL_LEVEL_SELECT == 1) && (NBI_TM_L1_INPUT_SELECT == 0))
                #define NBI1_TM_Q_CFG_RANGE1                   \
                    1,64,(NBI_TM_H_1_Q - 1),NBI_TM_Q_CFG_QSIZE
            #else
                #define NBI1_TM_Q_CFG_RANGE1                    \
                    1,512,(NBI_TM_H_1_Q - 1),NBI_TM_Q_CFG_QSIZE
            #endif
        #endif
    #endif

#endif


/* Make sure each TM queue config range is defined, even if unused.
 * Note: Default value is to have all queues active w/ 2^4 entries
 */
#ifndef NBI0_TM_Q_CFG_RANGE0
    #if ((NBI_TM_CHANNEL_LEVEL_SELECT == 1) && (NBI_TM_L1_INPUT_SELECT == 0))
        #define NBI0_TM_Q_CFG_RANGE0  1,0,127,7
    #else
        #define NBI0_TM_Q_CFG_RANGE0  1,0,1023,4
    #endif
#endif
#ifndef NBI0_TM_Q_CFG_RANGE1
#define NBI0_TM_Q_CFG_RANGE1  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE2
#define NBI0_TM_Q_CFG_RANGE2  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE3
#define NBI0_TM_Q_CFG_RANGE3  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE4
#define NBI0_TM_Q_CFG_RANGE4  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE5
#define NBI0_TM_Q_CFG_RANGE5  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE6
#define NBI0_TM_Q_CFG_RANGE6  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE7
#define NBI0_TM_Q_CFG_RANGE7  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE8
#define NBI0_TM_Q_CFG_RANGE8  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE9
#define NBI0_TM_Q_CFG_RANGE9  0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE10
#define NBI0_TM_Q_CFG_RANGE10 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE11
#define NBI0_TM_Q_CFG_RANGE11 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE12
#define NBI0_TM_Q_CFG_RANGE12 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE13
#define NBI0_TM_Q_CFG_RANGE13 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE14
#define NBI0_TM_Q_CFG_RANGE14 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE15
#define NBI0_TM_Q_CFG_RANGE15 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE16
#define NBI0_TM_Q_CFG_RANGE16 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE17
#define NBI0_TM_Q_CFG_RANGE17 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE18
#define NBI0_TM_Q_CFG_RANGE18 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE19
#define NBI0_TM_Q_CFG_RANGE19 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE20
#define NBI0_TM_Q_CFG_RANGE20 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE21
#define NBI0_TM_Q_CFG_RANGE21 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE22
#define NBI0_TM_Q_CFG_RANGE22 0,0,0,0
#endif
#ifndef NBI0_TM_Q_CFG_RANGE23
#define NBI0_TM_Q_CFG_RANGE23 0,0,0,0
#endif

#if NBI_COUNT == 2
    #ifndef NBI1_TM_Q_CFG_RANGE0
        #if ((NBI_TM_CHANNEL_LEVEL_SELECT == 1) && \
             (NBI_TM_L1_INPUT_SELECT == 0))
            #define NBI1_TM_Q_CFG_RANGE0  1,0,127,7
        #else
            #define NBI1_TM_Q_CFG_RANGE0  1,0,1023,4
        #endif
    #endif
#elif !defined(NBI1_TM_Q_CFG_RANGE0)
#define NBI1_TM_Q_CFG_RANGE0  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE1
#define NBI1_TM_Q_CFG_RANGE1  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE2
#define NBI1_TM_Q_CFG_RANGE2  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE3
#define NBI1_TM_Q_CFG_RANGE3  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE4
#define NBI1_TM_Q_CFG_RANGE4  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE5
#define NBI1_TM_Q_CFG_RANGE5  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE6
#define NBI1_TM_Q_CFG_RANGE6  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE7
#define NBI1_TM_Q_CFG_RANGE7  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE8
#define NBI1_TM_Q_CFG_RANGE8  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE9
#define NBI1_TM_Q_CFG_RANGE9  0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE10
#define NBI1_TM_Q_CFG_RANGE10 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE11
#define NBI1_TM_Q_CFG_RANGE11 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE12
#define NBI1_TM_Q_CFG_RANGE12 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE13
#define NBI1_TM_Q_CFG_RANGE13 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE14
#define NBI1_TM_Q_CFG_RANGE14 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE15
#define NBI1_TM_Q_CFG_RANGE15 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE16
#define NBI1_TM_Q_CFG_RANGE16 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE17
#define NBI1_TM_Q_CFG_RANGE17 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE18
#define NBI1_TM_Q_CFG_RANGE18 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE19
#define NBI1_TM_Q_CFG_RANGE19 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE20
#define NBI1_TM_Q_CFG_RANGE20 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE21
#define NBI1_TM_Q_CFG_RANGE21 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE22
#define NBI1_TM_Q_CFG_RANGE22 0,0,0,0
#endif
#ifndef NBI1_TM_Q_CFG_RANGE23
#define NBI1_TM_Q_CFG_RANGE23 0,0,0,0
#endif


/* Make sure each TM shaper config range is defined, even if unused.
 * Note: Default value is to have all shapers disabled
 */
#ifndef NBI0_TM_SHAPER_CFG_RANGE0
#define NBI0_TM_SHAPER_CFG_RANGE0  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE1
#define NBI0_TM_SHAPER_CFG_RANGE1  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE2
#define NBI0_TM_SHAPER_CFG_RANGE2  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE3
#define NBI0_TM_SHAPER_CFG_RANGE3  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE4
#define NBI0_TM_SHAPER_CFG_RANGE4  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE5
#define NBI0_TM_SHAPER_CFG_RANGE5  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE6
#define NBI0_TM_SHAPER_CFG_RANGE6  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE7
#define NBI0_TM_SHAPER_CFG_RANGE7  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE8
#define NBI0_TM_SHAPER_CFG_RANGE8  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE9
#define NBI0_TM_SHAPER_CFG_RANGE9  0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE10
#define NBI0_TM_SHAPER_CFG_RANGE10 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE11
#define NBI0_TM_SHAPER_CFG_RANGE11 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE12
#define NBI0_TM_SHAPER_CFG_RANGE12 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE13
#define NBI0_TM_SHAPER_CFG_RANGE13 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE14
#define NBI0_TM_SHAPER_CFG_RANGE14 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE15
#define NBI0_TM_SHAPER_CFG_RANGE15 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE16
#define NBI0_TM_SHAPER_CFG_RANGE16 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE17
#define NBI0_TM_SHAPER_CFG_RANGE17 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE18
#define NBI0_TM_SHAPER_CFG_RANGE18 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE19
#define NBI0_TM_SHAPER_CFG_RANGE19 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE20
#define NBI0_TM_SHAPER_CFG_RANGE20 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE21
#define NBI0_TM_SHAPER_CFG_RANGE21 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE22
#define NBI0_TM_SHAPER_CFG_RANGE22 0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_SHAPER_CFG_RANGE23
#define NBI0_TM_SHAPER_CFG_RANGE23 0,0,0,0,0,0,0
#endif

#ifndef NBI1_TM_SHAPER_CFG_RANGE0
#define NBI1_TM_SHAPER_CFG_RANGE0  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE1
#define NBI1_TM_SHAPER_CFG_RANGE1  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE2
#define NBI1_TM_SHAPER_CFG_RANGE2  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE3
#define NBI1_TM_SHAPER_CFG_RANGE3  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE4
#define NBI1_TM_SHAPER_CFG_RANGE4  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE5
#define NBI1_TM_SHAPER_CFG_RANGE5  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE6
#define NBI1_TM_SHAPER_CFG_RANGE6  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE7
#define NBI1_TM_SHAPER_CFG_RANGE7  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE8
#define NBI1_TM_SHAPER_CFG_RANGE8  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE9
#define NBI1_TM_SHAPER_CFG_RANGE9  0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE10
#define NBI1_TM_SHAPER_CFG_RANGE10 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE11
#define NBI1_TM_SHAPER_CFG_RANGE11 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE12
#define NBI1_TM_SHAPER_CFG_RANGE12 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE13
#define NBI1_TM_SHAPER_CFG_RANGE13 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE14
#define NBI1_TM_SHAPER_CFG_RANGE14 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE15
#define NBI1_TM_SHAPER_CFG_RANGE15 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE16
#define NBI1_TM_SHAPER_CFG_RANGE16 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE17
#define NBI1_TM_SHAPER_CFG_RANGE17 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE18
#define NBI1_TM_SHAPER_CFG_RANGE18 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE19
#define NBI1_TM_SHAPER_CFG_RANGE19 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE20
#define NBI1_TM_SHAPER_CFG_RANGE20 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE21
#define NBI1_TM_SHAPER_CFG_RANGE21 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE22
#define NBI1_TM_SHAPER_CFG_RANGE22 0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_SHAPER_CFG_RANGE23
#define NBI1_TM_SHAPER_CFG_RANGE23 0,0,0,0,0,0,0
#endif

#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE0
#define NBI0_TM_L2L1_SCHED_CFG_RANGE0 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE1
#define NBI0_TM_L2L1_SCHED_CFG_RANGE1 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE2
#define NBI0_TM_L2L1_SCHED_CFG_RANGE2 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE3
#define NBI0_TM_L2L1_SCHED_CFG_RANGE3 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE4
#define NBI0_TM_L2L1_SCHED_CFG_RANGE4 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE5
#define NBI0_TM_L2L1_SCHED_CFG_RANGE5 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE6
#define NBI0_TM_L2L1_SCHED_CFG_RANGE6 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE7
#define NBI0_TM_L2L1_SCHED_CFG_RANGE7 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE8
#define NBI0_TM_L2L1_SCHED_CFG_RANGE8 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE9
#define NBI0_TM_L2L1_SCHED_CFG_RANGE9 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE10
#define NBI0_TM_L2L1_SCHED_CFG_RANGE10 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE11
#define NBI0_TM_L2L1_SCHED_CFG_RANGE11 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE12
#define NBI0_TM_L2L1_SCHED_CFG_RANGE12 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE13
#define NBI0_TM_L2L1_SCHED_CFG_RANGE13 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE14
#define NBI0_TM_L2L1_SCHED_CFG_RANGE14 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE15
#define NBI0_TM_L2L1_SCHED_CFG_RANGE15 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE16
#define NBI0_TM_L2L1_SCHED_CFG_RANGE16 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE17
#define NBI0_TM_L2L1_SCHED_CFG_RANGE17 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE18
#define NBI0_TM_L2L1_SCHED_CFG_RANGE18 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE19
#define NBI0_TM_L2L1_SCHED_CFG_RANGE19 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE20
#define NBI0_TM_L2L1_SCHED_CFG_RANGE20 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE21
#define NBI0_TM_L2L1_SCHED_CFG_RANGE21 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE22
#define NBI0_TM_L2L1_SCHED_CFG_RANGE22 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI0_TM_L2L1_SCHED_CFG_RANGE23
#define NBI0_TM_L2L1_SCHED_CFG_RANGE23 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif

#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE0
#define NBI1_TM_L2L1_SCHED_CFG_RANGE0 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE1
#define NBI1_TM_L2L1_SCHED_CFG_RANGE1 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE2
#define NBI1_TM_L2L1_SCHED_CFG_RANGE2 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE3
#define NBI1_TM_L2L1_SCHED_CFG_RANGE3 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE4
#define NBI1_TM_L2L1_SCHED_CFG_RANGE4 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE5
#define NBI1_TM_L2L1_SCHED_CFG_RANGE5 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE6
#define NBI1_TM_L2L1_SCHED_CFG_RANGE6 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE7
#define NBI1_TM_L2L1_SCHED_CFG_RANGE7 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE8
#define NBI1_TM_L2L1_SCHED_CFG_RANGE8 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE9
#define NBI1_TM_L2L1_SCHED_CFG_RANGE9 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE10
#define NBI1_TM_L2L1_SCHED_CFG_RANGE10 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE11
#define NBI1_TM_L2L1_SCHED_CFG_RANGE11 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE12
#define NBI1_TM_L2L1_SCHED_CFG_RANGE12 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE13
#define NBI1_TM_L2L1_SCHED_CFG_RANGE13 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE14
#define NBI1_TM_L2L1_SCHED_CFG_RANGE14 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE15
#define NBI1_TM_L2L1_SCHED_CFG_RANGE15 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE16
#define NBI1_TM_L2L1_SCHED_CFG_RANGE16 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE17
#define NBI1_TM_L2L1_SCHED_CFG_RANGE17 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE18
#define NBI1_TM_L2L1_SCHED_CFG_RANGE18 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE19
#define NBI1_TM_L2L1_SCHED_CFG_RANGE19 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE20
#define NBI1_TM_L2L1_SCHED_CFG_RANGE20 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE21
#define NBI1_TM_L2L1_SCHED_CFG_RANGE21 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE22
#define NBI1_TM_L2L1_SCHED_CFG_RANGE22 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
#ifndef NBI1_TM_L2L1_SCHED_CFG_RANGE23
#define NBI1_TM_L2L1_SCHED_CFG_RANGE23 0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif

#endif /* _INIT_CONFIG_H_ */
