/*
 * Copyright (C) 2016-2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          nfp6000/nfp_nbi_tm.h
 * @brief         NFP6000 NBI Traffic Manager CSR definitions
 */

#ifndef _NFP6000__NFP_NBI_TM_H_
#define _NFP6000__NFP_NBI_TM_H_


/**
 * NBI Traffic Manager XPB BUS offset for a given NBI island
 */
#define NFP_NBI_TM_XPB_OFF(_isl)  ((_isl << 24) | 0x8140000)

/*
 * NBI Traffic Manager Register Block Offsets
 */
/* TrafficManagerReg: <base>.TrafficManagerReg */
#define NFP_NBI_TM_TRAFFIC_MANAGER_REG                     0x0000
/* TMQueueReg: <base>.TMQueueReg */
#define NFP_NBI_TM_QUEUE_REG                               0x10000
/* TMSchedulerReg: <base>.TMSchedulerReg */
#define NFP_NBI_TM_SCHEDULER_REG                           0x20000
/* SchedulerL1L2Weight: <base>.SchedulerL1Weight <base>.SchedulerL2Weight */
#define NFP_NBI_TM_SCHEDULER_L1_WEIGHT                     0x30000
#define NFP_NBI_TM_SCHEDULER_L2_WEIGHT                     0x60000
/* SchedulerL1L2Deficit: <base>.SchedulerL1Deficit <base>.SchedulerL2Deficit */
#define NFP_NBI_TM_SCHEDULER_L1_DEFICIT                    0x40000
#define NFP_NBI_TM_SCHEDULER_L2_DEFICIT                    0x70000
/* ECCControlMany: <base>.SchedulerL1ECC <base>.SchedulerL2ECC <base>.DescriptorMemECC <base>.HdTlMemECC <base>.EgressPktBufECC <base>.SlowDMemECC <base>.ReorderMemECC */
#define NFP_NBI_TM_SCHEDULER_L1_ECC                        0x50000
#define NFP_NBI_TM_SCHEDULER_L2_ECC                        0x80000
#define NFP_NBI_TM_DESCRIPTOR_MEM_ECC                      0xa0000
#define NFP_NBI_TM_HD_TL_MEM_ECC                           0xb0000
#define NFP_NBI_TM_EGRESS_PKT_BUF_ECC                      0xd0000
#define NFP_NBI_TM_SLOWD_MEM_ECC                           0xe0000
#define NFP_NBI_TM_REORDER_MEM_ECC                         0xf0000
/* TMShaperReg: <base>.TMShaperReg */
#define NFP_NBI_TM_SHAPER_REG                              0x90000
/* ECCControl: <base>.BLQMemECC <base>.DropCntMemECC */
#define NFP_NBI_TM_BLQ_MEM_ECC                             0xc0000
#define NFP_NBI_TM_DROP_CNT_MEM_ECC                        0x110000


/*
 * NBI Traffic Manager CSRs
 */

/*
 * Register: TrafficManagerConfig
 *   [16]      OOBFCEnable
 *   [15:13]   L0ChannelMap
 *   [12]      LevelCheckEnable
 *   [11]      DescQueuesEnable
 *   [10]      Sequencer0Enable
 *   [9]       ErrPoisonEna
 *   [8]       SchedulerEnable
 *   [7]       ShaperEnable
 *   [6]       MiniPacketFCEnable
 *   [5]       ChannelLevelSelect
 *   [3:1]     NumSequencers
 *   [0]       L1InputSelect
 *
 * Name(s):
 * <base>.TrafficManagerConfig
 */
#define NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG                  0x0000
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_OOBFCENABLE      (1 << 16)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_OOBFCENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_OOBFCENABLE_Enable (0x10000)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_OOBFCENABLE_bf 0, 16, 16
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_OOBFCENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_OOBFCENABLE_bit (16)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP(x)  (((x) & 7) << 13)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_of(x) (((x) >> 13) & 7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_0 (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_1 (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_2 (2)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_3 (3)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_4 (4)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_5 (5)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_6 (6)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_7 (7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_bf 0, 15, 13
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_msk (0x7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L0CHANNELMAP_shf (13)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE (1 << 12)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE_Enable (0x1000)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE_bf 0, 12, 12
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE_bit (12)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE (1 << 11)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE_Enable (0x800)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE_bf 0, 11, 11
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE_bit (11)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE (1 << 10)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE_Enable (0x400)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE_bf 0, 10, 10
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE_bit (10)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA   (1 << 9)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA_Enable (0x200)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA_bf 0, 9, 9
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA_bit (9)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE  (1 << 8)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE_Enable (0x100)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE_bf 0, 8, 8
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE_bit (8)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE     (1 << 7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE_Enable (0x80)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE_bf 0, 7, 7
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE_bit (7)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE (1 << 6)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE_Enable (0x40)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE_bf 0, 6, 6
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE_bit (6)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT (1 << 5)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT_Level_0 (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT_Level_1 (0x20)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT_bf 0, 5, 5
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT_bit (5)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS(x) (((x) & 7) << 1)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_of(x) (((x) >> 1) & 7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_0 (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_1 (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_2 (2)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_4 (3)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_8 (4)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_16 (5)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_32 (6)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_NA (7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_bf 0, 3, 1
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_msk (0x7)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_NUMSEQUENCERS_shf (1)
#define   NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT    (1 << 0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT_L1_Disable (0)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT_L1_Enable (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT_bf 0, 0, 0
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT_msk (0x1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT_bit (0)


/*
 * Register: DropRate
 *   [23:21]   DropRateRangeB3
 *   [20:18]   DropRateRangeB2
 *   [17:15]   DropRateRangeB1
 *   [14:12]   DropRateRangeB0
 *   [11:9]    DropRateRangeA3
 *   [8:6]     DropRateRangeA2
 *   [5:3]     DropRateRangeA1
 *   [2:0]     DropRateRangeA0
 *
 * Name(s):
 * <base>.DropRate
 */
#define NFP_NBI_TM_DROP_RATE                               0x0004
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3(x)            (((x) & 7) << 21)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_of(x)         (((x) >> 21) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_bf          0, 23, 21
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB3_shf         (21)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2(x)            (((x) & 7) << 18)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_of(x)         (((x) >> 18) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_bf          0, 20, 18
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB2_shf         (18)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1(x)            (((x) & 7) << 15)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_of(x)         (((x) >> 15) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_bf          0, 17, 15
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB1_shf         (15)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0(x)            (((x) & 7) << 12)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_of(x)         (((x) >> 12) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_bf          0, 14, 12
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEB0_shf         (12)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3(x)            (((x) & 7) << 9)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_of(x)         (((x) >> 9) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_bf          0, 11, 9
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA3_shf         (9)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2(x)            (((x) & 7) << 6)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_of(x)         (((x) >> 6) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_bf          0, 8, 6
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA2_shf         (6)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1(x)            (((x) & 7) << 3)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_of(x)         (((x) >> 3) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_bf          0, 5, 3
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA1_shf         (3)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0(x)            (((x) & 7) << 0)
#define   NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_of(x)         (((x) >> 0) & 7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_0           (0)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_12_5        (1)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_25          (2)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_37_5        (3)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_50          (4)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_62_5        (5)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_75          (6)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_87_5        (7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_bf          0, 2, 0
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_msk         (0x7)
#define     NFP_NBI_TM_DROP_RATE_DROPRATERANGEA0_shf         (0)


/*
 * Register: BlqEvent
 *   [20]      DisBLQEventFifoFullAssert
 *   [19:18]   BLQEventThreshold3
 *   [17:16]   BLQEventThreshold2
 *   [15:14]   BLQEventThreshold1
 *   [13:12]   BLQEventThreshold0
 *   [7:4]     BLQEventNotEmptyEnable
 *   [3:0]     BLQEventStatusEnable
 *
 * Name(s):
 * <base>.BlqEvent
 */
#define NFP_NBI_TM_BLQ_EVENT                               0x0008
#define   NFP_NBI_TM_BLQ_EVENT_DIS_BLQ_EVENT_FIFO_FULL_ASSERT (1 << 20)
#define     NFP_NBI_TM_BLQ_EVENT_DIS_BLQ_EVENT_FIFO_FULL_ASSERT_bf 0, 20, 20
#define     NFP_NBI_TM_BLQ_EVENT_DIS_BLQ_EVENT_FIFO_FULL_ASSERT_msk (0x1)
#define     NFP_NBI_TM_BLQ_EVENT_DIS_BLQ_EVENT_FIFO_FULL_ASSERT_bit (20)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3(x)        (((x) & 3) << 18)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_of(x)     (((x) >> 18) & 3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_16      (0)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_32      (1)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_64      (2)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_128     (3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_bf      0, 19, 18
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_msk     (0x3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD3_shf     (18)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2(x)        (((x) & 3) << 16)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_of(x)     (((x) >> 16) & 3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_16      (0)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_32      (1)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_64      (2)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_128     (3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_bf      0, 17, 16
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_msk     (0x3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD2_shf     (16)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1(x)        (((x) & 3) << 14)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_of(x)     (((x) >> 14) & 3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_16      (0)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_32      (1)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_64      (2)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_128     (3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_bf      0, 15, 14
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_msk     (0x3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD1_shf     (14)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0(x)        (((x) & 3) << 12)
#define   NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_of(x)     (((x) >> 12) & 3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_16      (0)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_32      (1)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_64      (2)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_128     (3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_bf      0, 13, 12
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_msk     (0x3)
#define     NFP_NBI_TM_BLQ_EVENT_BLQEVENT_THRESHOLD0_shf     (12)
#define   NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE(x) (((x) & 0xf) << 4)
#define   NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE_of(x) (((x) >> 4) & 0xf)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE_Disable (0)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE_Enable (1)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE_bf 0, 7, 4
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE_msk (0xf)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_NOT_EMPTY_ENABLE_shf (4)
#define   NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE(x)    (((x) & 0xf) << 0)
#define   NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE_of(x) (((x) >> 0) & 0xf)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE_Disable (0)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE_Enable (1)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE_bf  0, 3, 0
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE_msk (0xf)
#define     NFP_NBI_TM_BLQ_EVENT_BLQ_EVENT_STATUS_ENABLE_shf (0)


/*
 * Register: EgressRateLimit
 *   [2]       EgressRateLimitOff
 *   [1:0]     EgressRate
 *
 * Name(s):
 * <base>.EgressRateLimit
 */
#define NFP_NBI_TM_EGRESS_RATE_LIMIT                       0x0074
#define   NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_LIMIT_OFF (1 << 2)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_LIMIT_OFF_on (0)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_LIMIT_OFF_off (4)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_LIMIT_OFF_bf 0, 2, 2
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_LIMIT_OFF_msk (0x1)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_LIMIT_OFF_bit (2)
#define   NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE(x)        (((x) & 3) << 0)
#define   NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_of(x)     (((x) >> 0) & 3)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_64G     (0)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_32G     (1)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_16G     (2)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_8G      (3)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_bf      0, 1, 0
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_msk     (0x3)
#define     NFP_NBI_TM_EGRESS_RATE_LIMIT_EGRESS_RATE_shf     (0)


/*
 * Register: EgressPullIdPortEnable
 *   [1:0]     EgressPullIdPortEnable
 *
 * Name(s):
 * <base>.EgressPullIdPortEnable
 */
#define NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE              0x0078
#define   NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE(x) (((x) & 3) << 0)
#define   NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_of(x) (((x) >> 0) & 3)
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_P6_and_P7 (0)
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_P6 (1)
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_P7 (2)
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_unused (3)
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_bf 0, 1, 0
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_msk (0x3)
#define     NFP_NBI_TM_EGRESS_PULL_ID_PORT_ENABLE_EGRESS_PULL_ID_PORT_ENABLE_shf (0)


/*
 * Register: OutOfOrderCount
 *   [15:0]    OutOfOrderCount
 *
 * Name(s):
 * <base>.OutOfOrderCount0...
 */
#define NFP_NBI_TM_OUT_OF_ORDER_COUNT(x)                   (0x0100 + ((x) * 0x4))
#define   NFP_NBI_TM_OUT_OF_ORDER_COUNT_OUTOFORDERCOUNT(x)   (((x) & 0xffff) << 0)
#define   NFP_NBI_TM_OUT_OF_ORDER_COUNT_OUTOFORDERCOUNT_of(x) (((x) >> 0) & 0xffff)
#define     NFP_NBI_TM_OUT_OF_ORDER_COUNT_OUTOFORDERCOUNT_bf 0, 15, 0
#define     NFP_NBI_TM_OUT_OF_ORDER_COUNT_OUTOFORDERCOUNT_msk (0xffff)
#define     NFP_NBI_TM_OUT_OF_ORDER_COUNT_OUTOFORDERCOUNT_shf (0)


/*
 * Register: OutOfOrderCountClear
 *   [15:0]    OutOfOrderCountClear
 *
 * Name(s):
 * <base>.OutOfOrderCountClear0...
 */
#define NFP_NBI_TM_OUT_OF_ORDER_COUNT_CLEAR(x)             (0x0200 + ((x) * 0x4))
#define   NFP_NBI_TM_OUT_OF_ORDER_COUNT_CLEAR_OUTOFORDERCOUNTCLEAR(x) (((x) & 0xffff) << 0)
#define   NFP_NBI_TM_OUT_OF_ORDER_COUNT_CLEAR_OUTOFORDERCOUNTCLEAR_of(x) (((x) >> 0) & 0xffff)
#define     NFP_NBI_TM_OUT_OF_ORDER_COUNT_CLEAR_OUTOFORDERCOUNTCLEAR_bf 0, 15, 0
#define     NFP_NBI_TM_OUT_OF_ORDER_COUNT_CLEAR_OUTOFORDERCOUNTCLEAR_msk (0xffff)
#define     NFP_NBI_TM_OUT_OF_ORDER_COUNT_CLEAR_OUTOFORDERCOUNTCLEAR_shf (0)


/*
 * Register: MiniPktCreditConfig
 *   [24]      MiniPacketFCMode
 *   [21:12]   FPCreditLimit
 *   [9:0]     CreditLimit
 *
 * Name(s):
 * <base>.MiniPktCreditConfig
 */
#define NFP_NBI_TM_MINIPKT_CREDIT_CONFIG                   0x0300
#define   NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_MINIPACKETFCMODE  (1 << 24)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_MINIPACKETFCMODE_Mode_0 (0)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_MINIPACKETFCMODE_Mode_1 (0x1000000)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_MINIPACKETFCMODE_bf 0, 24, 24
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_MINIPACKETFCMODE_msk (0x1)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_MINIPACKETFCMODE_bit (24)
#define   NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_FPCREDITLIMIT(x)  (((x) & 0x3ff) << 12)
#define   NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_FPCREDITLIMIT_of(x) (((x) >> 12) & 0x3ff)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_FPCREDITLIMIT_bf 0, 21, 12
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_FPCREDITLIMIT_msk (0x3ff)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_FPCREDITLIMIT_shf (12)
#define   NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_CREDITLIMIT(x)    (((x) & 0x3ff) << 0)
#define   NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_CREDITLIMIT_of(x) (((x) >> 0) & 0x3ff)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_CREDITLIMIT_bf  0, 9, 0
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_CREDITLIMIT_msk (0x3ff)
#define     NFP_NBI_TM_MINIPKT_CREDIT_CONFIG_CREDITLIMIT_shf (0)


/*
 * Register: MiniPktFreePoolCredit
 *   [9:0]     FPCreditCounter
 *
 * Name(s):
 * <base>.MiniPktFreePoolCredit0...
 */
#define NFP_NBI_TM_MINIPKT_FREE_POOL_CREDIT(x)             (0x0304 + ((x) * 0x4))
#define   NFP_NBI_TM_MINIPKT_FREE_POOL_CREDIT_FPCREDITCOUNT(x) (((x) & 0x3ff) << 0)
#define   NFP_NBI_TM_MINIPKT_FREE_POOL_CREDIT_FPCREDITCOUNT_of(x) (((x) >> 0) & 0x3ff)
#define     NFP_NBI_TM_MINIPKT_FREE_POOL_CREDIT_FPCREDITCOUNT_bf 0, 9, 0
#define     NFP_NBI_TM_MINIPKT_FREE_POOL_CREDIT_FPCREDITCOUNT_msk (0x3ff)
#define     NFP_NBI_TM_MINIPKT_FREE_POOL_CREDIT_FPCREDITCOUNT_shf (0)


/*
 * Register: MiniPktChannelCredit
 *   [9:0]     CreditCounter
 *
 * Name(s):
 * <base>.MiniPktChannelCredit0...
 */
#define NFP_NBI_TM_MINIPKT_CHANNEL_CREDIT(x)               (0x0400 + ((x) * 0x4))
#define   NFP_NBI_TM_MINIPKT_CHANNEL_CREDIT_CREDITCOUNT(x)   (((x) & 0x3ff) << 0)
#define   NFP_NBI_TM_MINIPKT_CHANNEL_CREDIT_CREDITCOUNT_of(x) (((x) >> 0) & 0x3ff)
#define     NFP_NBI_TM_MINIPKT_CHANNEL_CREDIT_CREDITCOUNT_bf 0, 9, 0
#define     NFP_NBI_TM_MINIPKT_CHANNEL_CREDIT_CREDITCOUNT_msk (0x3ff)
#define     NFP_NBI_TM_MINIPKT_CHANNEL_CREDIT_CREDITCOUNT_shf (0)


/*
 * Register: ReorderActivity
 *   [31:0]    ReorderActivity
 *
 * Name(s):
 * <base>.ReorderActivity
 */
#define NFP_NBI_TM_REORDER_ACTIVITY                        0x0600
#define   NFP_NBI_TM_REORDER_ACTIVITY_REORDERACTIVITY(x)     (((x) & 0xffffffff) << 0)
#define   NFP_NBI_TM_REORDER_ACTIVITY_REORDERACTIVITY_of(x)  (((x) >> 0) & 0xffffffff)
#define     NFP_NBI_TM_REORDER_ACTIVITY_REORDERACTIVITY_bf   0, 31, 0
#define     NFP_NBI_TM_REORDER_ACTIVITY_REORDERACTIVITY_msk  (0xffffffff)
#define     NFP_NBI_TM_REORDER_ACTIVITY_REORDERACTIVITY_shf  (0)


/*
 * NBI Traffic Manager Queue CSRs
 */

/*
 * Register: QueueStatus
 *   [13]      QueueFull
 *   [12:0]    QueueLevel
 *
 * Name(s):
 * <base>.QueueStatus0...
 */
#define NFP_NBI_TM_QUEUE_STATUS(x)                         (0x0000 + ((x) * 0x4))
#define   NFP_NBI_TM_QUEUE_STATUS_QUEUEFULL                  (1 << 13)
#define     NFP_NBI_TM_QUEUE_STATUS_QUEUEFULL_bf             0, 13, 13
#define     NFP_NBI_TM_QUEUE_STATUS_QUEUEFULL_msk            (0x1)
#define     NFP_NBI_TM_QUEUE_STATUS_QUEUEFULL_bit            (13)
#define   NFP_NBI_TM_QUEUE_STATUS_QUEUELEVEL(x)              (((x) & 0x1fff) << 0)
#define   NFP_NBI_TM_QUEUE_STATUS_QUEUELEVEL_of(x)           (((x) >> 0) & 0x1fff)
#define     NFP_NBI_TM_QUEUE_STATUS_QUEUELEVEL_bf            0, 12, 0
#define     NFP_NBI_TM_QUEUE_STATUS_QUEUELEVEL_msk           (0x1fff)
#define     NFP_NBI_TM_QUEUE_STATUS_QUEUELEVEL_shf           (0)


/*
 * Register: QueueConfig
 *   [10]      DropRateRangeSelect
 *   [9:6]     QueueSize
 *   [5:3]     DMAPacketThreshold
 *   [2]       REDEnable
 *   [1]       DropEnable
 *   [0]       QueueEnable
 *
 * Name(s):
 * <base>.QueueConfig0...
 */
#define NFP_NBI_TM_QUEUE_CONFIG(x)                         (0x1000 + ((x) * 0x4))
#define   NFP_NBI_TM_QUEUE_CONFIG_DROPRATERANGESELECT        (1 << 10)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPRATERANGESELECT_A    (0)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPRATERANGESELECT_B    (0x400)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPRATERANGESELECT_bf   0, 10, 10
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPRATERANGESELECT_msk  (0x1)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPRATERANGESELECT_bit  (10)
#define   NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE(x)               (((x) & 0xf) << 6)
#define   NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_of(x)            (((x) >> 6) & 0xf)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_n_a0           (0)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_n_a1           (1)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_n_a2           (2)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_8              (3)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_16             (4)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_32             (5)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_64             (6)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_128            (7)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_256            (8)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_512            (9)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_1024           (0xa)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_2048           (0xb)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_4096           (0xc)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_8192           (0xd)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_n_a14          (0xe)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_n_a15          (0xf)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_bf             0, 9, 6
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_msk            (0xf)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUESIZE_shf            (6)
#define   NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD(x)      (((x) & 7) << 3)
#define   NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_of(x)   (((x) >> 3) & 7)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_default (0)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_12_5  (1)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_25    (2)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_37_5  (3)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_50    (4)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_62_5  (5)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_75    (6)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_87_5  (7)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_bf    0, 5, 3
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_msk   (0x7)
#define     NFP_NBI_TM_QUEUE_CONFIG_DMAPACKETTHRESHOLD_shf   (3)
#define   NFP_NBI_TM_QUEUE_CONFIG_REDENABLE                  (1 << 2)
#define     NFP_NBI_TM_QUEUE_CONFIG_REDENABLE_Disabled       (0)
#define     NFP_NBI_TM_QUEUE_CONFIG_REDENABLE_Enabled        (4)
#define     NFP_NBI_TM_QUEUE_CONFIG_REDENABLE_bf             0, 2, 2
#define     NFP_NBI_TM_QUEUE_CONFIG_REDENABLE_msk            (0x1)
#define     NFP_NBI_TM_QUEUE_CONFIG_REDENABLE_bit            (2)
#define   NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE                 (1 << 1)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE_Disabled      (0)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE_Enabled       (2)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE_bf            0, 1, 1
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE_msk           (0x1)
#define     NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE_bit           (1)
#define   NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE                (1 << 0)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE_Disable      (0)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE_Enable       (1)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE_bf           0, 0, 0
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE_msk          (0x1)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE_bit          (0)


/*
 * Register: QueueDropCount
 *   [31:0]    DropCount
 *
 * Name(s):
 * <base>.QueueDropCount0...
 */
#define NFP_NBI_TM_QUEUE_DROP_COUNT(x)                     (0x2000 + ((x) * 0x4))
#define   NFP_NBI_TM_QUEUE_DROP_COUNT_DROPCOUNT(x)           (((x) & 0xffffffff) << 0)
#define   NFP_NBI_TM_QUEUE_DROP_COUNT_DROPCOUNT_of(x)        (((x) >> 0) & 0xffffffff)
#define     NFP_NBI_TM_QUEUE_DROP_COUNT_DROPCOUNT_bf         0, 31, 0
#define     NFP_NBI_TM_QUEUE_DROP_COUNT_DROPCOUNT_msk        (0xffffffff)
#define     NFP_NBI_TM_QUEUE_DROP_COUNT_DROPCOUNT_shf        (0)


/*
 * Register: QueueDropCountClear
 *   [31:0]    DropCountClear
 *
 * Name(s):
 * <base>.QueueDropCountClear0...
 */
#define NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR(x)               (0x3000 + ((x) * 0x4))
#define   NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR_DROPCOUNTCLEAR(x) (((x) & 0xffffffff) << 0)
#define   NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR_DROPCOUNTCLEAR_of(x) (((x) >> 0) & 0xffffffff)
#define     NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR_DROPCOUNTCLEAR_bf 0, 31, 0
#define     NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR_DROPCOUNTCLEAR_msk (0xffffffff)
#define     NFP_NBI_TM_QUEUE_DROP_COUNT_CLEAR_DROPCOUNTCLEAR_shf (0)


/*
 * NBI Traffic Manager Scheduler CSRs
 */

/*
 * Register: SchedulerConfig
 *   [2]       SP1Enable
 *   [1]       SP0Enable
 *   [0]       DWRREnable
 *
 * Name(s):
 * <base>.SchedulerConfig0...
 */
#define NFP_NBI_TM_SCHEDULER_CONFIG(x)                     (0x0000 + ((x) * 0x4))
#define   NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE              (1 << 2)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE_Disable    (0)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE_Enable     (4)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE_bf         0, 2, 2
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE_msk        (0x1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE_bit        (2)
#define   NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE              (1 << 1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE_Disable    (0)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE_Enable     (2)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE_bf         0, 1, 1
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE_msk        (0x1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE_bit        (1)
#define   NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE             (1 << 0)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE_Disable   (0)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE_Enable    (1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE_bf        0, 0, 0
#define     NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE_msk       (0x1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE_bit       (0)


/*
 * Register: SchedulerWeight
 *   [23:0]    Weight
 *
 * Name(s):
 * <base>.SchedulerWeight0...
 */
#define NFP_NBI_TM_SCHEDULER_WEIGHT(x)                     (0x0800 + ((x) * 0x4))
#define   NFP_NBI_TM_SCHEDULER_WEIGHT_WEIGHT(x)              (((x) & 0xffffff) << 0)
#define   NFP_NBI_TM_SCHEDULER_WEIGHT_WEIGHT_of(x)           (((x) >> 0) & 0xffffff)
#define     NFP_NBI_TM_SCHEDULER_WEIGHT_WEIGHT_bf            0, 23, 0
#define     NFP_NBI_TM_SCHEDULER_WEIGHT_WEIGHT_msk           (0xffffff)
#define     NFP_NBI_TM_SCHEDULER_WEIGHT_WEIGHT_shf           (0)


/*
 * Register: SchedulerDeficit
 *   [24:0]    Deficit
 *
 * Name(s):
 * <base>.SchedulerDeficit0...
 */
#define NFP_NBI_TM_SCHEDULER_DEFICIT(x)                    (0x1000 + ((x) * 0x4))
#define   NFP_NBI_TM_SCHEDULER_DEFICIT_DEFICIT(x)            (((x) & 0x1ffffff) << 0)
#define   NFP_NBI_TM_SCHEDULER_DEFICIT_DEFICIT_of(x)         (((x) >> 0) & 0x1ffffff)
#define     NFP_NBI_TM_SCHEDULER_DEFICIT_DEFICIT_bf          0, 24, 0
#define     NFP_NBI_TM_SCHEDULER_DEFICIT_DEFICIT_msk         (0x1ffffff)
#define     NFP_NBI_TM_SCHEDULER_DEFICIT_DEFICIT_shf         (0)



/*
 * Register: SchedulerWeight
 *   [23:0]    Weight
 *
 * Name(s):
 * <base>.SchedulerWeight0...
 */
#define NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT(x)               (0x0000 + ((x) * 0x4))
#define   NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT_WEIGHT(x)        (((x) & 0xffffff) << 0)
#define   NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT_WEIGHT_of(x)     (((x) >> 0) & 0xffffff)
#define     NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT_WEIGHT_bf      0, 23, 0
#define     NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT_WEIGHT_msk     (0xffffff)
#define     NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT_WEIGHT_shf     (0)



/*
 * Register: SchedulerDeficit
 *   [24:0]    Deficit
 *
 * Name(s):
 * <base>.SchedulerDeficit0...
 */
#define NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT(x)              (0x0000 + ((x) * 0x4))
#define   NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT_DEFICIT(x)      (((x) & 0x1ffffff) << 0)
#define   NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT_DEFICIT_of(x)   (((x) >> 0) & 0x1ffffff)
#define     NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT_DEFICIT_bf    0, 24, 0
#define     NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT_DEFICIT_msk   (0x1ffffff)
#define     NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT_DEFICIT_shf   (0)


/*
 * NBI Traffic Manager Shaper CSRs
 */

/*
 * Register: ShaperRate
 *   [13:0]    Rate
 *
 * Name(s):
 * <base>.ShaperRate0...
 */
#define NFP_NBI_TM_SHAPER_RATE(x)                          (0x0000 + ((x) * 0x4))
#define   NFP_NBI_TM_SHAPER_RATE_RATE(x)                     (((x) & 0x3fff) << 0)
#define   NFP_NBI_TM_SHAPER_RATE_RATE_of(x)                  (((x) >> 0) & 0x3fff)
#define     NFP_NBI_TM_SHAPER_RATE_RATE_bf                   0, 13, 0
#define     NFP_NBI_TM_SHAPER_RATE_RATE_msk                  (0x3fff)
#define     NFP_NBI_TM_SHAPER_RATE_RATE_shf                  (0)


/*
 * Register: ShaperThreshold
 *   [2:0]     Threshold
 *
 * Name(s):
 * <base>.ShaperThreshold0...
 */
#define NFP_NBI_TM_SHAPER_THRESHOLD(x)                     (0x0800 + ((x) * 0x4))
#define   NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD(x)           (((x) & 7) << 0)
#define   NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_of(x)        (((x) >> 0) & 7)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_8192       (0)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_16384      (1)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_24576      (2)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_32768      (3)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_40960      (4)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_49152      (5)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_57344      (6)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_65535      (7)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_bf         0, 2, 0
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_msk        (0x7)
#define     NFP_NBI_TM_SHAPER_THRESHOLD_THRESHOLD_shf        (0)


/*
 * Register: ShaperMaxOvershoot
 *   [2:0]     MaxOvershoot
 *
 * Name(s):
 * <base>.ShaperMaxOvershoot0...
 */
#define NFP_NBI_TM_SHAPER_MAX_OVERSHOOT(x)                 (0x1000 + ((x) * 0x4))
#define   NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT(x)    (((x) & 7) << 0)
#define   NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_of(x) (((x) >> 0) & 7)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_16384 (0)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_24576 (1)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_32768 (2)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_40960 (3)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_49152 (4)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_57344 (5)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_65536 (6)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_131071 (7)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_bf  0, 2, 0
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_msk (0x7)
#define     NFP_NBI_TM_SHAPER_MAX_OVERSHOOT_MAXOVERSHOOT_shf (0)


/*
 * Register: ShaperRateAdjust
 *   [9:0]     RateAdjust
 *
 * Name(s):
 * <base>.ShaperRateAdjust0...
 */
#define NFP_NBI_TM_SHAPER_RATE_ADJUST(x)                   (0x1800 + ((x) * 0x4))
#define   NFP_NBI_TM_SHAPER_RATE_ADJUST_RATE(x)              (((x) & 0x3ff) << 0)
#define   NFP_NBI_TM_SHAPER_RATE_ADJUST_RATE_of(x)           (((x) >> 0) & 0x3ff)
#define     NFP_NBI_TM_SHAPER_RATE_ADJUST_RATE_bf            0, 9, 0
#define     NFP_NBI_TM_SHAPER_RATE_ADJUST_RATE_msk           (0x3ff)
#define     NFP_NBI_TM_SHAPER_RATE_ADJUST_RATE_shf           (0)


/*
 * Register: ShaperStatus
 *   [17]      ShaperOpen
 *   [16:0]    Level
 *
 * Name(s):
 * <base>.ShaperStatus0...
 */
#define NFP_NBI_TM_SHAPER_STATUS(x)                        (0x2000 + ((x) * 0x4))
#define   NFP_NBI_TM_SHAPER_STATUS_SHAPEROPEN                (1 << 17)
#define     NFP_NBI_TM_SHAPER_STATUS_SHAPEROPEN_Closed       (0)
#define     NFP_NBI_TM_SHAPER_STATUS_SHAPEROPEN_Open         (0x20000)
#define     NFP_NBI_TM_SHAPER_STATUS_SHAPEROPEN_bf           0, 17, 17
#define     NFP_NBI_TM_SHAPER_STATUS_SHAPEROPEN_msk          (0x1)
#define     NFP_NBI_TM_SHAPER_STATUS_SHAPEROPEN_bit          (17)
#define   NFP_NBI_TM_SHAPER_STATUS_LEVEL(x)                  (((x) & 0x1ffff) << 0)
#define   NFP_NBI_TM_SHAPER_STATUS_LEVEL_of(x)               (((x) >> 0) & 0x1ffff)
#define     NFP_NBI_TM_SHAPER_STATUS_LEVEL_bf                0, 16, 0
#define     NFP_NBI_TM_SHAPER_STATUS_LEVEL_msk               (0x1ffff)
#define     NFP_NBI_TM_SHAPER_STATUS_LEVEL_shf               (0)


#if defined(__NFP_LANG_MICROC)

/*
 * NBI Traffic Manager register structures
 */
struct nfp_nbi_tm_traffic_manager_config {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_17:15;
            unsigned int oobfcenable:1;
            unsigned int l0channelmap:3;
            unsigned int levelcheckenable:1;
            unsigned int descqueuesenable:1;
            unsigned int sequencer0enable:1;
            unsigned int err_poison_ena:1;
            unsigned int schedulerenable:1;
            unsigned int shaperenable:1;
            unsigned int minipacketfcenable:1;
            unsigned int channellevelselect:1;
            unsigned int __reserved_4:1;
            unsigned int numsequencers:3;
            unsigned int l1inputselect:1;
#           else
            unsigned int l1inputselect:1;
            unsigned int numsequencers:3;
            unsigned int __reserved_4:1;
            unsigned int channellevelselect:1;
            unsigned int minipacketfcenable:1;
            unsigned int shaperenable:1;
            unsigned int schedulerenable:1;
            unsigned int err_poison_ena:1;
            unsigned int sequencer0enable:1;
            unsigned int descqueuesenable:1;
            unsigned int levelcheckenable:1;
            unsigned int l0channelmap:3;
            unsigned int oobfcenable:1;
            unsigned int __reserved_17:15;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_drop_rate {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_24:8;
            unsigned int dropraterangeb3:3;
            unsigned int dropraterangeb2:3;
            unsigned int dropraterangeb1:3;
            unsigned int dropraterangeb0:3;
            unsigned int dropraterangea3:3;
            unsigned int dropraterangea2:3;
            unsigned int dropraterangea1:3;
            unsigned int dropraterangea0:3;
#           else
            unsigned int dropraterangea0:3;
            unsigned int dropraterangea1:3;
            unsigned int dropraterangea2:3;
            unsigned int dropraterangea3:3;
            unsigned int dropraterangeb0:3;
            unsigned int dropraterangeb1:3;
            unsigned int dropraterangeb2:3;
            unsigned int dropraterangeb3:3;
            unsigned int __reserved_24:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_blq_event {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_21:11;
            unsigned int dis_blq_event_fifo_full_assert:1;
            unsigned int blqevent_threshold3:2;
            unsigned int blqevent_threshold2:2;
            unsigned int blqevent_threshold1:2;
            unsigned int blqevent_threshold0:2;
            unsigned int __reserved_8:4;
            unsigned int blq_event_not_empty_enable:4;
            unsigned int blq_event_status_enable:4;
#           else
            unsigned int blq_event_status_enable:4;
            unsigned int blq_event_not_empty_enable:4;
            unsigned int __reserved_8:4;
            unsigned int blqevent_threshold0:2;
            unsigned int blqevent_threshold1:2;
            unsigned int blqevent_threshold2:2;
            unsigned int blqevent_threshold3:2;
            unsigned int dis_blq_event_fifo_full_assert:1;
            unsigned int __reserved_21:11;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_egress_rate_limit {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_3:29;
            unsigned int egress_rate_limit_off:1;
            unsigned int egress_rate:2;
#           else
            unsigned int egress_rate:2;
            unsigned int egress_rate_limit_off:1;
            unsigned int __reserved_3:29;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_egress_pull_id_port_enable {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_2:30;
            unsigned int egress_pull_id_port_enable:2;
#           else
            unsigned int egress_pull_id_port_enable:2;
            unsigned int __reserved_2:30;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_out_of_order_count {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int outofordercount:16;
#           else
            unsigned int outofordercount:16;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_out_of_order_count_clear {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int outofordercountclear:16;
#           else
            unsigned int outofordercountclear:16;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_minipkt_credit_config {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_25:7;
            unsigned int minipacketfcmode:1;
            unsigned int __reserved_22:2;
            unsigned int fpcreditlimit:10;
            unsigned int __reserved_10:2;
            unsigned int creditlimit:10;
#           else
            unsigned int creditlimit:10;
            unsigned int __reserved_10:2;
            unsigned int fpcreditlimit:10;
            unsigned int __reserved_22:2;
            unsigned int minipacketfcmode:1;
            unsigned int __reserved_25:7;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_minipkt_free_pool_credit {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_10:22;
            unsigned int fpcreditcount:10;
#           else
            unsigned int fpcreditcount:10;
            unsigned int __reserved_10:22;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_minipkt_channel_credit {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_10:22;
            unsigned int creditcount:10;
#           else
            unsigned int creditcount:10;
            unsigned int __reserved_10:22;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_reorder_activity {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int reorderactivity:32;
#           else
            unsigned int reorderactivity:32;
#           endif
        };
        unsigned int __raw;
    };
};


/*
 * NBI Traffic Manager Queue register structures
 */
struct nfp_nbi_tm_queue_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_14:18;
            unsigned int queuefull:1;
            unsigned int queuelevel:13;
#           else
            unsigned int queuelevel:13;
            unsigned int queuefull:1;
            unsigned int __reserved_14:18;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_queue_config {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_11:21;
            unsigned int dropraterangeselect:1;
            unsigned int queuesize:4;
            unsigned int dmapacketthreshold:3;
            unsigned int redenable:1;
            unsigned int dropenable:1;
            unsigned int queueenable:1;
#           else
            unsigned int queueenable:1;
            unsigned int dropenable:1;
            unsigned int redenable:1;
            unsigned int dmapacketthreshold:3;
            unsigned int queuesize:4;
            unsigned int dropraterangeselect:1;
            unsigned int __reserved_11:21;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_queue_drop_count {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int dropcount:32;
#           else
            unsigned int dropcount:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_queue_drop_count_clear {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int dropcountclear:32;
#           else
            unsigned int dropcountclear:32;
#           endif
        };
        unsigned int __raw;
    };
};


/*
 * NBI Traffic Manager Scheduler register structures
 */
struct nfp_nbi_tm_scheduler_config {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_3:29;
            unsigned int sp1enable:1;
            unsigned int sp0enable:1;
            unsigned int dwrrenable:1;
#           else
            unsigned int dwrrenable:1;
            unsigned int sp0enable:1;
            unsigned int sp1enable:1;
            unsigned int __reserved_3:29;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_scheduler_weight {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_24:8;
            unsigned int weight:24;
#           else
            unsigned int weight:24;
            unsigned int __reserved_24:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_scheduler_deficit {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_25:7;
            unsigned int deficit:25;
#           else
            unsigned int deficit:25;
            unsigned int __reserved_25:7;
#           endif
        };
        unsigned int __raw;
    };
};


struct nfp_nbi_tm_l1_l2_scheduler_weight {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_24:8;
            unsigned int weight:24;
#           else
            unsigned int weight:24;
            unsigned int __reserved_24:8;
#           endif
        };
        unsigned int __raw;
    };
};


struct nfp_nbi_tm_l1_l2_scheduler_deficit {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_25:7;
            unsigned int deficit:25;
#           else
            unsigned int deficit:25;
            unsigned int __reserved_25:7;
#           endif
        };
        unsigned int __raw;
    };
};


/*
 * NBI Traffic Manager Shaper register structures
 */
struct nfp_nbi_tm_shaper_rate {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_14:18;
            unsigned int rate:14;
#           else
            unsigned int rate:14;
            unsigned int __reserved_14:18;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_shaper_threshold {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_3:29;
            unsigned int threshold:3;
#           else
            unsigned int threshold:3;
            unsigned int __reserved_3:29;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_shaper_max_overshoot {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_3:29;
            unsigned int maxovershoot:3;
#           else
            unsigned int maxovershoot:3;
            unsigned int __reserved_3:29;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_shaper_rate_adjust {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_10:22;
            unsigned int rate:10;
#           else
            unsigned int rate:10;
            unsigned int __reserved_10:22;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_nbi_tm_shaper_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_18:14;
            unsigned int shaperopen:1;
            unsigned int level:17;
#           else
            unsigned int level:17;
            unsigned int shaperopen:1;
            unsigned int __reserved_18:14;
#           endif
        };
        unsigned int __raw;
    };
};


#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_NBI_TM_H_ */
