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
 * @file: init_nbi_tm_csr.uc
 * @brief: Contains macros to initialise the NBI Traffic Manager. All
 * macros use the .init_csr directive to set the registers
 */

#ifndef _INIT_NBI_TM_CSR_UC_
#define _INIT_NBI_TM_CSR_UC_

/** Nbi_TrafficManager_TrafficManagerReg_TrafficManagerConfig
 *
 * Configures the NBI Traffic Manager Config Register. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ID                The NBI number, can be 0 or 1
 * @param CHANNELLEVELSELECT    Channel level select bit. Select the definition of the MAC channel number and the MAC XOFF Flow control to be either the 128 level 0 scheduler ports, or 128 ports on the first 16 level 1 schedulers
 * @param ERRPOISONENA          It enables the poisoning of data and/or enables the other error propagation mechanisms in case of errors.
 * @param L0CHANNELMAP          MAC channel number to Level 0 Scheduler port number mapping register.
 * @param L1INPUTSELECT         Select the operation of the Level 1 schedulers. This bit disables or enables the Level 1 schedulers
 * @param MINIPACKETFCENABLE    Minipacket Credit based Flow Control enable bit. Clear this bit to disable minipacket flow control
 * @param NUMSEQUENCERS         Number of active sequencers. Specifies the number of active sequencers in the traffic manager.
 * @param OOBFCENABLE           MAC Out of Band Flow Control Enable.
 * @param SCHEDULERENABLE       Scheduler Enable bit. Set this bit to enable the scheduling of traffic, clear to disable.
 * @param SEQUENCER0ENABLE      Sequencer 0 Enable bit. Set this bit to enable reordering for sequencer 0.
 * @param SHAPERENABLE          Shaper Enable bit. Enables the Rate Shaper to start rate limiting traffic based on the configured values.
 */
#macro Nbi_TrafficManager_TrafficManagerReg_TrafficManagerConfig(NBI_ID,CHANNELLEVELSELECT,ERRPOISONENA,L0CHANNELMAP,L1INPUTSELECT,MINIPACKETFCENABLE,NUMSEQUENCERS,OOBFCENABLE,SCHEDULERENABLE,SEQUENCER0ENABLE,SHAPERENABLE)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.ChannelLevelSelect     CHANNELLEVELSELECT const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.ErrPoisonEna           ERRPOISONENA       const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.L0ChannelMap           L0CHANNELMAP       const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.L1InputSelect          L1INPUTSELECT      const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.MiniPacketFCEnable     MINIPACKETFCENABLE const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.NumSequencers          NUMSEQUENCERS      const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.OOBFCEnable            OOBFCENABLE        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.SchedulerEnable        SCHEDULERENABLE    const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.Sequencer0Enable       SEQUENCER0ENABLE   const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.TrafficManagerConfig.ShaperEnable           SHAPERENABLE       const

#endm


/** Nbi_TrafficManager_TMQueueReg_QueueConfig
 *
 * Configures the NBI TM Queue config Register. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ID                 The NBI number, can be 0 or 1
 * @param QUEUE_NUM              Queue number
 * @param QUEUEENABLE            Enable bit for this particular queue. Set this bit to enable the queue, clear to disable.
 * @param DROPENABLE             Enables packet dropping for this particular queue.
 * @param REDENABLE              Enables random early detection for packet dropping for this particular queue.
 * @param DMAPACKETTHRESHOLD     Queue level percentage threshold to determine when the packet data should be moved from CTM memory to main memory.
 * @param QUEUESIZE              Configures the maximum size of this particular queue.
 * @param DROPRATERANGESELECT    Select the Drop Rate Range set to use for this queue.
 */
#macro Nbi_TrafficManager_TMQueueReg_QueueConfig(NBI_ID,QUEUE_NUM,QUEUEENABLE,DROPENABLE,REDENABLE,DMAPACKETTHRESHOLD,QUEUESIZE,DROPRATERANGESELECT)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueConfig/**/QUEUE_NUM.QueueEnable           QUEUEENABLE         const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueConfig/**/QUEUE_NUM.DropEnable            DROPENABLE          const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueConfig/**/QUEUE_NUM.REDEnable             REDENABLE           const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueConfig/**/QUEUE_NUM.DMAPacketThreshold    DMAPACKETTHRESHOLD  const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueConfig/**/QUEUE_NUM.QueueSize             QUEUESIZE           const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueConfig/**/QUEUE_NUM.DropRateRangeSelect   DROPRATERANGESELECT const

#endm


/** Nbi_TrafficManager_TMQueueReg_QueueDropCountClear
 *
 * Clear the NBI TM Queue Drop Count Register. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ID                 The NBI number, can be 0 or 1
 * @param QUEUE_NUM              Queue number
 */
#macro Nbi_TrafficManager_TMQueueReg_QueueDropCountClear(NBI_ID,QUEUE_NUM)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMQueueReg.QueueDropCountClear/**/QUEUE_NUM               0xFFFFFFFF          invalid
#endm


/** Nbi_TrafficManager_TMShaperReg_ShaperEntry
 *
 * Configure the NBI TM Shaper Registers.  See the NFP-6xxx Databook section on
 * the NBI Traffic Manager
 *
 * @param NBI_ID      The NBI number, can be 0 or 1
 * @param SHAPER_NUM  Shaper/scheduler number
 * @param RATE        Maximum rate (in 10 Mbps increments when PClk = 1 GHz):
 *                        maximum rate = RATE * 10 Mbps * (Pclk/1 GHz)
 * @param THRESHOLD   Threshold to trigger rate limiter:
 *                        0 = 8192 B   1 = 16384 B  2 = 24576 B  3 = 32768 B
 *                        4 = 40960 B  5 = 49152 B  6 = 57344 B  7 = 65535 B
 * @param OVERSHOOT   Maximum overshoot tracked:
 *                        0 = 16384 B  1 = 24576 B  2 = 32768 B  3 = 40960 B
 *                        4 = 49152 B  5 = 57344 B  6 = 65536 B  7 = 131071 B
 * @param RATE_ADJ    Amount of bytes to ignore from each packet
 */
#macro Nbi_TrafficManager_TMShaperReg_ShaperEntry(NBI_ID,SHAPER_NUM,RATE,THRESHOLD,OVERSHOOT,RATE_ADJ)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMShaperReg.ShaperRate/**/SHAPER_NUM.Rate                  RATE                const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMShaperReg.ShaperThreshold/**/SHAPER_NUM.Threshold        THRESHOLD           const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMShaperReg.ShaperMaxOvershoot/**/SHAPER_NUM.MaxOvershoot  OVERSHOOT           const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMShaperReg.ShaperRateAdjust/**/SHAPER_NUM.RateAdjust      RATE_ADJ            const
#endm


/** NBITMCPP_TMHeadTailSram_TMHeadTailEntry
 *
 * The NBI Traffic Manager contains a 4KB array of registers to store the head and tail pointers for the descriptor queues. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ISLAND_ID     The NBI island number. Can be 8 or 9
 * @param HEAD_TAIL_ENTRY   Queue number
 * @param TAILPOINTER       14-bit Descriptor Queue Tail Pointer.
 * @param HEADPOINTER       14-bit Descriptor Queue Head Pointer.
 */
#macro NBITMCPP_TMHeadTailSram_TMHeadTailEntry(NBI_ISLAND_ID,HEAD_TAIL_ENTRY,TAILPOINTER,HEADPOINTER)
    .init_csr nbi:i/**/NBI_ISLAND_ID.NBITMCPP.TMHeadTailSram.TMHeadTailEntry/**/HEAD_TAIL_ENTRY.TailPointer     TAILPOINTER     const
    .init_csr nbi:i/**/NBI_ISLAND_ID.NBITMCPP.TMHeadTailSram.TMHeadTailEntry/**/HEAD_TAIL_ENTRY.HeadPointer     HEADPOINTER     const

#endm


/** Nbi_TrafficManager_TrafficManagerReg_MiniPktCreditConfig
 *
 * Configures the NBI TM Minipacket Credit Config Register. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ID                    The NBI number, can be 0 or 1
 * @param CREDITLIMIT               MiniPacket Transmit Channel Credit Counter Limit. Programmable limit for all of the channel credit counters
 * @param FPCREDITLIMIT             MiniPacket Transmit Free Pool Credit Counter Limit.
 * @param MINIPACKETFCMODE          MiniPacket Credit Flow Control Mode bit. Select the free pool crediting mode to use.

 */
#macro Nbi_TrafficManager_TrafficManagerReg_MiniPktCreditConfig(NBI_ID,CREDITLIMIT,FPCREDITLIMIT,MINIPACKETFCMODE)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.MiniPktCreditConfig.CreditLimit             CREDITLIMIT         const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.MiniPktCreditConfig.FPCreditLimit           FPCREDITLIMIT       const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.MiniPktCreditConfig.MiniPacketFCMode        MINIPACKETFCMODE    const

#endm


/** Nbi_TrafficManager_TrafficManager_TrafficManagerReg_BlqEvent
 *
 * Configures thresholds, event type and enables/disables for the buffer list queues. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ID                    The NBI number, can be 0 or 1
 * @param BLQEVENTSTATUSENABLE      Enables BLQ Status Events when set.
 * @param BLQEVENTNOTEMPTYENABLE    Enables the generation of BLQ Events when the queue transitions from empty to not empty.
 * @param BLQEVENTTHRESHOLD0        An event is generated for every N buffer descriptors written to the buffer list queue 0 with
 *                                  N configurable as 16, 32, 64 or 128. Can be 0 (16), 1 (32), 2 (64) or 4 (128).
 * @param BLQEVENTTHRESHOLD1        An event is generated for every N buffer descriptors written to the buffer list queue 1 with
 *                                  N configurable as 16, 32, 64 or 128. Can be 0 (16), 1 (32), 2 (64) or 4 (128).
 * @param BLQEVENTTHRESHOLD2        An event is generated for every N buffer descriptors written to the buffer list queue 2 with
 *                                  N configurable as 16, 32, 64 or 128. Can be 0 (16), 1 (32), 2 (64) or 4 (128).
 * @param BLQEVENTTHRESHOLD3        An event is generated for every N buffer descriptors written to the buffer list queue 3 with
 *                                  N configurable as 16, 32, 64 or 128. Can be 0 (16), 1 (32), 2 (64) or 4 (128).
 * @param DISBLQEVENTFIFOFULLASSERT Set this bit to disable the BLQ Event FIFO overflow assertion.
 *
 */
#macro Nbi_TrafficManager_TrafficManager_TrafficManagerReg_BlqEvent(NBI_ID, BLQEVENTSTATUSENABLE, BLQEVENTNOTEMPTYENABLE, BLQEVENTTHRESHOLD0, BLQEVENTTHRESHOLD1, BLQEVENTTHRESHOLD2, BLQEVENTTHRESHOLD3, DISBLQEVENTFIFOFULLASSERT)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.BLQEventStatusEnable         BLQEVENTSTATUSENABLE      const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.BLQEventNotEmptyEnable       BLQEVENTNOTEMPTYENABLE    const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.BLQEventThreshold0           BLQEVENTTHRESHOLD0        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.BLQEventThreshold1           BLQEVENTTHRESHOLD1        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.BLQEventThreshold2           BLQEVENTTHRESHOLD2        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.BLQEventThreshold3           BLQEVENTTHRESHOLD3        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TrafficManagerReg.BlqEvent.DisBLQEventFifoFullAssert    DISBLQEVENTFIFOFULLASSERT const

#endm

/** Nbi_TrafficManager_TMSchedulerReg_L2L1SchedulerEntry
 *
 * Configures L2 and L1 schedulers. See the NFP-6xxx Databook section on the NBI Traffic Manager
 *
 * @param NBI_ID      The NBI number, can be 0 or 1
 * @param SCHED_NUM   L2 (0 to 127) or L1 (128 to 143) scheduler number
 * @param SP0         Strict Priority Zero Enable bit. Set this bit to enable
 *                    scheduler port 0 as the strict priority 0 port. SP0 will
 *                    have priority over all other ports, including SP1.
 * @param SP1         Strict Priority One Enable bit. Set this bit to enable
 *                    scheduler port 1 as the strict priority 1 port. SP1 will
 *                    have priority over all other ports except for SP0.
 * @param DWRR_EN     Deficit Weighted Round Robin Enable bit. Set this bit to
 *                    enable deficit weighted round operation for the particular
 *                    scheduler.
 * @param DWRR[0..7]  Scheduler weight value to configure the relative
 *                    bandwidth for each scheduler port.
 */
#macro Nbi_TrafficManager_TMSchedulerReg_L2L1SchedulerEntry(NBI_ID, SCHED_NUM, SP0, SP1, DWRR_EN, DWRR0, DWRR1, DWRR2, DWRR3, DWRR4, DWRR5, DWRR6, DWRR7)

    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMSchedulerReg.SchedulerConfig/**/SCHED_NUM.SP1Enable     SP0     const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMSchedulerReg.SchedulerConfig/**/SCHED_NUM.SP0Enable     SP1     const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.TMSchedulerReg.SchedulerConfig/**/SCHED_NUM.DWRREnable    DWRR_EN const
    #if (DWRR_EN == 1)
        #if (SCHED_NUM < 128)
            #define_eval S_W_OFFS (SCHED_NUM * 8)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR0   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 1)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR1   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 2)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR2   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 3)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR3   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 4)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR4   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 5)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR5   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 6)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR6   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS ((SCHED_NUM * 8) + 7)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR7   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL2Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #undef S_W_OFFS

        #elif (SCHED_NUM < 143)
             #define_eval S_W_OFFS ((128 - SCHED_NUM) * 8)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR0   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 1)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR1   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 2)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR2   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 3)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR3   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 4)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR4   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 5)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR5   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 6)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR6   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #define_eval S_W_OFFS (((128 - SCHED_NUM) * 8) + 7)
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Weight.SchedulerWeight/**/S_W_OFFS.Weight   DWRR7   const
            .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.TrafficManager.SchedulerL1Deficit.SchedulerDeficit/**/S_W_OFFS.Deficit   0   const

            #undef S_W_OFFS
        #endif

    #endif

#endm

#endif /* _INIT_NBI_TM_CSR_UC_ */
