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
 * @file          lib/nfp/macstats.h
 * @brief         Interface for MAC statistics.
 */

#ifndef _NFP__MACSTATS_H_
#define _NFP__MACSTATS_H_

#include <nfp.h>
#include <stdint.h>
#include <nfp6000/nfp_mac.h>

/**
 * This file contains the API for reading and accumulating MAC statistics.
 * For a detailed description of the MAC please see NFP-6xxx Databook section
 * 10 (Networking).
 * Each MAC supports up to 24 eth ports (12 SerDes lanes per core) and up
 * to 128 NBI channels (64 per core). The used port and channel numbers
 * depends on the core configuration (which is capable of being configured to
 * 100GE/3x40GE/12x10GE/12x1GE) and the per-port channel assignment (see
 * MacPortChanAssign CSR for details). Each of the ports can be assigned with
 * a starting channel number and up to 8 channels from the start.
 * Each port and each channel has a total of 256 bytes address space of
 * statistics.
 * Those MAC statistics are cleared on read and should be read/accumulated
 * periodically to prevent overflow. Each of the statistics is a 32/40 bits
 * counter, while the accumulating structure reserves 64 bits per counter.
 */

/* Per port statistics
 * See NFP-6xxx Databook Section 10.3.3.1.1 EthPortStatsSeg for details.
 * Each segment (12 per MAC core) has 256 bytes of stats counters.
 * This structure is used to read the current port stats.
 */
struct macstats_port {
    uint32_t RxPIfInOctetsLo;
    uint8_t RxPIfInOctetsHi;
    uint8_t RxPIfInOctetsHi_res[3];
    uint32_t RxFrameTooLongErrors;
    uint32_t RxInRangeLengthErrors;
    uint32_t RxVlanReceivedOK;
    uint32_t RxPIfInErrors;
    uint32_t RxPIfInBroadCastPkts;
    uint32_t RxPStatsDropEvents;
    uint32_t RxAlignmentErrors;
    uint32_t RxPauseMacCtlFrames;
    uint32_t RxFramesReceivedOK;
    uint32_t RxFrameCheckSequenceErrors;
    uint32_t RxPIfInUniCastPkts;
    uint32_t RxPIfInMultiCastPkts;
    uint32_t RxPStatsPkts;
    uint32_t RxPStatsUndersizePkts;
    uint32_t RxPStatsPkts64octets;
    uint32_t RxPStatsPkts65to127octets;
    uint32_t RxPStatsPkts512to1023octets;
    uint32_t RxPStatsPkts1024to1518octets;
    uint32_t RxPStatsJabbers;
    uint32_t RxPStatsFragments;
    uint32_t RxCBFCPauseFramesReceived2;
    uint32_t RxCBFCPauseFramesReceived3;
    uint32_t RxPStatsPkts128to255octets;
    uint32_t RxPStatsPkts256to511octets;
    uint32_t RxPStatsPkts1519toMaxoctets;
    uint32_t RxPStatsOversizePkts;
    uint32_t RxCBFCPauseFramesReceived0;
    uint32_t RxCBFCPauseFramesReceived1;
    uint32_t RxCBFCPauseFramesReceived4;
    uint32_t RxCBFCPauseFramesReceived5;
    uint32_t RxCBFCPauseFramesReceived6;
    uint32_t RxCBFCPauseFramesReceived7;
    uint32_t RxMacCtlFramesReceived;
    uint32_t unused0;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t unused3;
    uint32_t unused4;
    uint32_t TxPIfOutOctetsLo;
    uint8_t TxPIfOutOctetsHi;
    uint8_t TxPIfOutOctetsHi_res[3];
    uint32_t TxVlanTransmittedOK;
    uint32_t TxPIfOutErrors;
    uint32_t TxPIfOutBroadCastPkts;
    uint32_t TxPStatsPkts64octets;
    uint32_t TxPStatsPkts256to511octets;
    uint32_t TxPStatsPkts512to1023octets;
    uint32_t TxPauseMacCtlFramesTransmitted;
    uint32_t TxFramesTransmittedOK;
    uint32_t TxPIfOutUniCastPkts;
    uint32_t TxPIfOutMultiCastPkts;
    uint32_t TxPStatsPkts65to127octets;
    uint32_t TxPStatsPkts128to255octets;
    uint32_t TxPStatsPkts1024to1518octets;
    uint32_t TxPStatsPkts1518toMAXoctets;
    uint32_t TxCBFCPauseFramesTransmitted0;
    uint32_t TxCBFCPauseFramesTransmitted1;
    uint32_t TxCBFCPauseFramesTransmitted4;
    uint32_t TxCBFCPauseFramesTransmitted5;
    uint32_t TxCBFCPauseFramesTransmitted2;
    uint32_t TxCBFCPauseFramesTransmitted3;
    uint32_t TxCBFCPauseFramesTransmitted6;
    uint32_t TxCBFCPauseFramesTransmitted7;
};

/* Per port statistics accumulate structure */
struct macstats_port_accum {
    uint64_t RxPIfInOctets;
    uint64_t RxPIfInOctets_unused;
    uint64_t RxFrameTooLongErrors;
    uint64_t RxInRangeLengthErrors;
    uint64_t RxVlanReceivedOK;
    uint64_t RxPIfInErrors;
    uint64_t RxPIfInBroadCastPkts;
    uint64_t RxPStatsDropEvents;
    uint64_t RxAlignmentErrors;
    uint64_t RxPauseMacCtlFrames;
    uint64_t RxFramesReceivedOK;
    uint64_t RxFrameCheckSequenceErrors;
    uint64_t RxPIfInUniCastPkts;
    uint64_t RxPIfInMultiCastPkts;
    uint64_t RxPStatsPkts;
    uint64_t RxPStatsUndersizePkts;
    uint64_t RxPStatsPkts64octets;
    uint64_t RxPStatsPkts65to127octets;
    uint64_t RxPStatsPkts512to1023octets;
    uint64_t RxPStatsPkts1024to1518octets;
    uint64_t RxPStatsJabbers;
    uint64_t RxPStatsFragments;
    uint64_t RxCBFCPauseFramesReceived2;
    uint64_t RxCBFCPauseFramesReceived3;
    uint64_t RxPStatsPkts128to255octets;
    uint64_t RxPStatsPkts256to511octets;
    uint64_t RxPStatsPkts1519toMaxoctets;
    uint64_t RxPStatsOversizePkts;
    uint64_t RxCBFCPauseFramesReceived0;
    uint64_t RxCBFCPauseFramesReceived1;
    uint64_t RxCBFCPauseFramesReceived4;
    uint64_t RxCBFCPauseFramesReceived5;
    uint64_t RxCBFCPauseFramesReceived6;
    uint64_t RxCBFCPauseFramesReceived7;
    uint64_t RxMacCtlFramesReceived;
    uint64_t RxMacHeadDrop;
    uint64_t unused0;
    uint64_t unused1;
    uint64_t unused2;
    uint64_t TxQueueDrop;
    uint64_t TxPIfOutOctets;
    uint64_t TxPIfOutOctets_unused;
    uint64_t TxVlanTransmittedOK;
    uint64_t TxPIfOutErrors;
    uint64_t TxPIfOutBroadCastPkts;
    uint64_t TxPStatsPkts64octets;
    uint64_t TxPStatsPkts256to511octets;
    uint64_t TxPStatsPkts512to1023octets;
    uint64_t TxPauseMacCtlFramesTransmitted;
    uint64_t TxFramesTransmittedOK;
    uint64_t TxPIfOutUniCastPkts;
    uint64_t TxPIfOutMultiCastPkts;
    uint64_t TxPStatsPkts65to127octets;
    uint64_t TxPStatsPkts128to255octets;
    uint64_t TxPStatsPkts1024to1518octets;
    uint64_t TxPStatsPkts1518toMAXoctets;
    uint64_t TxCBFCPauseFramesTransmitted0;
    uint64_t TxCBFCPauseFramesTransmitted1;
    uint64_t TxCBFCPauseFramesTransmitted4;
    uint64_t TxCBFCPauseFramesTransmitted5;
    uint64_t TxCBFCPauseFramesTransmitted2;
    uint64_t TxCBFCPauseFramesTransmitted3;
    uint64_t TxCBFCPauseFramesTransmitted6;
    uint64_t TxCBFCPauseFramesTransmitted7;
};

/* Per channel statistics
 * See NFP-6xxx Databook Section 10.3.3.2 ChannelStatsSeg for details.
 * Each channel (total of 128 channels) has 128 bytes of stats counters.
 * This structure is used to read the current channel stats.
 */
struct macstats_channel {
    uint32_t RxCIfInOctetsLo;
    uint8_t RxCIfInOctetsHi;
    uint8_t RxCIfInOctetsHi_res[3];
    uint32_t RxCStatsOctetsLo;
    uint8_t RxCStatsOctetsHi;
    uint8_t RxCStatsOctetsHi_res[3];
    uint32_t RxCIfInErrors;
    uint32_t RxCIfInUniCastPkts;
    uint32_t RxCIfInMultiCastPkts;
    uint32_t RxCIfInBroadCastPkts;
    uint32_t RxCStatsPkts;
    uint32_t RxCStatsPkts64octets;
    uint32_t RxCStatsPkts65to127octets;
    uint32_t RxCStatsPkts128to255octets;
    uint32_t RxCStatsPkts256to511octets;
    uint32_t RxCStatsPkts512to1023octets;
    uint32_t RxCStatsPkts1024to1518octets;
    uint32_t RxCStatsPkts1519toMaxoctets;
    uint32_t RxChanFramesReceivedOK;
    uint32_t RxChanVlanReceivedOK;
    uint32_t unused0;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t unused3;
    uint32_t unused4;
    uint32_t unused5;
    uint32_t TxCIfOutOctetsLo;
    uint8_t TxCIfOutOctetsHi;
    uint8_t TxCIfOutOctetsHi_res[3];
    uint32_t TxCIfOutErrors;
    uint32_t TxCIfOutUniCastPkts;
    uint32_t TxChanFramesTransmittedOK;
    uint32_t TxChanVlanTransmittedOK;
    uint32_t TxCIfOutMultiCastPkts;
    uint32_t TxCIfOutBroadCastPkts;
};

/* Per channel statistics accumulate structure */
struct macstats_channel_accum {
    uint64_t RxCIfInOctets;
    uint64_t RxCIfInOctets_unused;
    uint64_t RxCStatsOctets;
    uint64_t RxCStatsOctets_unused;
    uint64_t RxCIfInErrors;
    uint64_t RxCIfInUniCastPkts;
    uint64_t RxCIfInMultiCastPkts;
    uint64_t RxCIfInBroadCastPkts;
    uint64_t RxCStatsPkts;
    uint64_t RxCStatsPkts64octets;
    uint64_t RxCStatsPkts65to127octets;
    uint64_t RxCStatsPkts128to255octets;
    uint64_t RxCStatsPkts256to511octets;
    uint64_t RxCStatsPkts512to1023octets;
    uint64_t RxCStatsPkts1024to1518octets;
    uint64_t RxCStatsPkts1519toMaxoctets;
    uint64_t RxChanFramesReceivedOK;
    uint64_t RxChanVlanReceivedOK;
    uint64_t unused0;
    uint64_t unused1;
    uint64_t unused2;
    uint64_t unused3;
    uint64_t unused4;
    uint64_t unused5;
    uint64_t TxCIfOutOctets;
    uint64_t TxCIfOutOctets_unused;
    uint64_t TxCIfOutErrors;
    uint64_t TxCIfOutUniCastPkts;
    uint64_t TxChanFramesTransmittedOK;
    uint64_t TxChanVlanTransmittedOK;
    uint64_t TxCIfOutMultiCastPkts;
    uint64_t TxCIfOutBroadCastPkts;
};

/* Per port mac head drop counters accumulation */
struct macstats_head_drop_accum {
    /* We have 12 ports */
    uint64_t ports_drop[NFP_MAX_ETH_PORTS_PER_MAC_CORE];
};

/**
 * Accumulates the per port mac head drop counters.
 * @param nbi               The nbi to read from (0/1)
 * @param core              The MAC core to read from (0/1)
 * @param ports_mask        A 16 bits ports mask configuration used, each bit
 *                          indicates if a port is in use. Only bits 0-11 are
 *                          used.
 * @param port_stats        A pointer to the accumulate stats struct to update,
 *                          must be in ctm/imem/emem only
 * @param break_cpp_burst   A flag indicating if this function should sleep
 *                          between loops to avoid a burst of CPP commands.
 *                          This should only be used if the current ctx is not
 *                          already waiting for an alarm to fire.
 *                          (using the CTX_FUTURE_COUNT mechanism).
 *
 * @return 0 on success, -1 on error
 */
int __macstats_head_drop_accum(unsigned int nbi, unsigned int core,
        unsigned short ports_mask,
        __mem40 struct macstats_head_drop_accum *port_stats,
        unsigned int break_cpp_burst);

/**
 * Accumulates the per port mac head drop counters.
 * @param nbi               The nbi to read from (0/1)
 * @param core              The MAC core to read from (0/1)
 * @param ports_mask        A 16 bits ports mask configuration used, each bit
 *                          indicates if a port is in use. Only bits 0-11 are
 *                          used.
 * @param port_stats        A pointer to the accumulate stats struct to update,
 *                          must be in ctm/imem/emem only
 *
 * This API simply calls the __macstats_head_drop_accum with the
 * break_cpp_burst set to 0.
 *
 * @return 0 on success, -1 on error
 */
int macstats_head_drop_accum(unsigned int nbi, unsigned int core,
        unsigned short ports_mask,
        __mem40 struct macstats_head_drop_accum *port_stats);

/**
 * Reads and clears MAC stats for a given port.
 * @param mac         The mac to read from (0/1)
 * @param port        The port number (0-23)
 * @param port_stats  A pointer to the stats struct to fill,
 *                    must be in ctm/imem/emem only
 * @return 0 on success, -1 on error
 */
int macstats_port_read(unsigned int mac, unsigned int port,
                       __mem40 struct macstats_port *port_stats);

/**
 * Accumulate MAC stats for a given port.
 * @param mac         The mac to read from (0/1)
 * @param port        The port number (0-23)
 * @param port_stats  A pointer to the accumulate stats struct to
 *                    update, must be in ctm/imem/emem only
 * @return 0 on success, -1 on error
 */
int macstats_port_accum(unsigned int mac, unsigned int port,
                        __mem40 struct macstats_port_accum *port_stats);


/**
 * Reads and clears MAC stats for a given channel.
 * @param mac            The mac to read from (0/1)
 * @param channel        The channel number (0-127)
 * @param channel_stats  A pointer to the stats struct to fill,
 *                       must be in ctm/imem/emem only
 * @return 0 on success, -1 on error
 */
int macstats_channel_read(unsigned int mac, unsigned int channel,
                          __mem40 struct macstats_channel *channel_stats);

/**
 * Accumulate MAC stats for a given channel.
 * @param mac            The mac to read from (0/1)
 * @param channel        The channel number (0-127)
 * @param channel_stats  A pointer to the accumulate stats struct to
 *                       update, must be in ctm/imem/emem only
 * @return 0 on success, -1 on error
 */
int macstats_channel_accum(unsigned int mac, unsigned int channel,
        __mem40 struct macstats_channel_accum *channel_stats);

#endif /* !_NFP__MACSTATS_H_ */
