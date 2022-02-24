/*
 * Copyright (C) 2015-2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          nfp6000/nfp_mac.h
 * @brief         Partial NFP6000 MAC CSR definitions
 *
 * This files is auto-generated. Do not edit!
 */

#ifndef _NFP6000__NFP_MAC_H_
#define _NFP6000__NFP_MAC_H_


/**
 * The NFP6000 can have up to two MAC islands. Each MAC island has 2
 * MAC cores and each MAC core may support up to 12 (10G) Ethernet
 * ports.  These NFP MAC ports can be combined to provide higher speed
 * external Ethernet ports (e.g., 40G or 100G).
 *
 * How external ports are wired to NFP MAC Ethernet ports is board
 * specific.  The macros defined in this file only operated on NFP MAC
 * ports and a user has to take the board specific mapping of external
 * ports to NFP MAC ports into account.
 */

/* Maximum Ethernet ports per MAC core and MAC island */
#define NFP_MAX_ETH_PORTS_PER_MAC_CORE 12
#define NFP_MAX_MAC_CORES_PER_MAC_ISL  2
#define NFP_MAX_ETH_PORTS_PER_MAC_ISL                               \
    (NFP_MAX_ETH_PORTS_PER_MAC_CORE * NFP_MAX_MAC_CORES_PER_MAC_ISL)

/* Maximum number of MAC islands supported on the NFP */
#define NFP_MAC_MAX_ISLANDS_PER_NFP    2

/**
 * Macros to determine the MAC core number/core port for a given port number
 */
#define NFP_MAC_ETH_PORT_TO_MAC_CORE(_p)                \
    (_p < NFP_MAX_ETH_PORTS_PER_MAC_CORE ? 0 : 1)
#define NFP_MAC_ETH_PORT_TO_MAC_CORE_PORT(_p)           \
    (_p < NFP_MAX_ETH_PORTS_PER_MAC_CORE                \
     ? _p : _p - NFP_MAX_ETH_PORTS_PER_MAC_CORE)

/**
 * XPB BUS offset for a given MAC island
 */
#define NFP_MAC_XPB_OFF(_isl)  ((_isl << 24) | 0x8400000)


/*
 * MAC Checksum prepend word format
 */

/**
 * Header length definitions
 * @NFP_MAC_CSUM_PREPEND_LEN     RX/TX Checksum prepend length
 * @NFP_MAC_CSUM_PREPEND_LEN32   RX/TX Checksum prepend length
 */
#define NFP_MAC_CSUM_PREPEND_LEN    4
#define NFP_MAC_CSUM_PREPEND_LEN32  (PKT_CSUM_PREPEND_LEN / 4)

/**
 * Format of the CSUM RX prepend word as per section "10.2.10.4 Ingress
 * Parser" of the DB.
 *
 * The NFP MAC block can optionally check the L3/L4 checksum for
 * selected protocols and prepends the result to the buffer.  The
 * format is defined below.
 *
 * @NFP_MAC_RX_CSUM_L4_SUM_of       Summary of L4 checksum
 * @NFP_MAC_RX_CSUM_L3_SUM_of       Summary of L3 checksum
 * @NFP_MAC_RX_CSUM_MPLS_of         Number of MPLS labels (3 = 3 or more)
 * @NFP_MAC_RX_CSUM_VLANS_of        Number of VLANs present (3 = 3 or more)
 * @NFP_MAC_RX_CSUM_CSUM_of         16-bit L4 TCP/UDP checksum if TCP/UDP is
 *                                  parse-able. If not 16-bit checksum for
 *                                  received packet excluding CRC.
 */

#define NFP_MAC_RX_CSUM_L4_SUM_of(_x)           (((_x) >> 29) & 0x7)
#define   NFP_MAC_RX_CSUM_L4_UNSUP              (0)
#define   NFP_MAC_RX_CSUM_L4_ESP                (1)
#define   NFP_MAC_RX_CSUM_L4_TCP_OK             (2)
#define   NFP_MAC_RX_CSUM_L4_TCP_FAIL           (3)
/* Note UDP_OK is also returned when the checksum is not populated (=0) */
#define   NFP_MAC_RX_CSUM_L4_UDP_OK             (4)
#define   NFP_MAC_RX_CSUM_L4_UDP_FAIL           (5)
#define   NFP_MAC_RX_CSUM_L4_AUTH               (6)
#define   NFP_MAC_RX_CSUM_L4_FRAGMENT           (7)


#define NFP_MAC_RX_CSUM_L3_SUM_of(_x)           (((_x) >> 20) & 0x3)
#define   NFP_MAC_RX_CSUM_L3_UNKNOWN            (0)
#define   NFP_MAC_RX_CSUM_L3_IPV6               (1)
#define   NFP_MAC_RX_CSUM_L3_IPV4_FAIL          (2)
#define   NFP_MAC_RX_CSUM_L3_IPV4_OK            (3)

#define NFP_MAC_RX_CSUM_MPLS_of(_x)             (((_x) >> 18) & 0x3)
#define NFP_MAC_RX_CSUM_VLANS_of(_x)            (((_x) >> 16) & 0x3)
#define NFP_MAC_RX_CSUM_CSUM_of(_x)             ((_x) & 0xffff)


/*
 * MAC CSRs
 */
/* MacXPB: <base>.MacCsr */
#define NFP_MAC_CSR                                        0x0000
/* InterruptManagerMap: <base>.MacIntrMng */
#define NFP_MAC_INTR_MNG                                   0x10000
/* MacInterlaken: <base>.MacInterlaken0... */
#define NFP_MAC_ILK(x)                                     (0x20000 + ((x) * 0x10000))
/* MacEthernet: <base>.MacEthernet0... */
#define NFP_MAC_ETH(x)                                     (0x40000 + ((x) * 0x20000))
/* MacMemCtrl: <base>.MacCoresCsrMemCtrl0... */
#define NFP_MAC_MEM_CTRL(x)                                (0x80000 + ((x) * 0x10000))
/* ECCControlMany: <base>.MacCdsEccMon0... */
#define NFP_MAC_ECC_MON(x)                                 (0xa0000 + ((x) * 0x10000))
/* IslandOverlayExtMap: <base>.MacOvlExt */
#define NFP_MAC_OVL_EXT                                    0x300000


/*
 * Macros for NFP_MAC_ETH(x)
 */

/*
 * Register: EthRevision
 *   [31:16]   EthCustVer
 *   [15:8]    EthCoreVer
 *   [7:0]     EthCoreRev
 *
 * Name(s):
 * <base>.MacEthSeg0.EthRevision...
 */
#define NFP_MAC_ETH_SEG_REVISON(_seg)                      (0x0000 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_REVISON_CUSTOM(x)                  (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_REVISON_CUSTOM_of(x)               (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_REVISON_CUSTOM_bf                0, 31, 16
#define     NFP_MAC_ETH_SEG_REVISON_CUSTOM_msk               (0xffff)
#define     NFP_MAC_ETH_SEG_REVISON_CUSTOM_shf               (16)
#define   NFP_MAC_ETH_SEG_REVISON_CORE(x)                    (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_SEG_REVISON_CORE_of(x)                 (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_SEG_REVISON_CORE_bf                  0, 15, 8
#define     NFP_MAC_ETH_SEG_REVISON_CORE_msk                 (0xff)
#define     NFP_MAC_ETH_SEG_REVISON_CORE_shf                 (8)
#define   NFP_MAC_ETH_SEG_REVISON_CORE_REV(x)                (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_SEG_REVISON_CORE_REV_of(x)             (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_SEG_REVISON_CORE_REV_bf              0, 7, 0
#define     NFP_MAC_ETH_SEG_REVISON_CORE_REV_msk             (0xff)
#define     NFP_MAC_ETH_SEG_REVISON_CORE_REV_shf             (0)


/*
 * Register: EthScratch
 *
 * Name(s):
 * <base>.MacEthSeg0.EthScratch...
 */
#define NFP_MAC_ETH_SEG_SCRATCH(_seg)                      (0x0004 + ((_seg) * 0x400))

/*
 * Register: EthCmdConfig
 *   [29]      EthForceSendRf
 *   [28]      EthForceSendLf
 *   [27]      EthDisableFltHdl
 *   [22]      EthTxFlush
 *   [21]      EthRxSfdAny
 *   [20]      EthPausePfcComp
 *   [19]      EthPfcMode
 *   [18]      EthRsColCntExt
 *   [17]      EthNoLgthCheck
 *   [16]      EthSendIdle
 *   [15]      EthPhyTxEna
 *   [14]      EthRxErrDiscard
 *   [13]      EthCmdFrameEna
 *   [12]      EthSwReset
 *   [11]      EthTxPadEn
 *   [10]      EthLoopBackEn
 *   [9]       EthTxAddrIns
 *   [8]       EthPauseIgnore
 *   [7]       EthPauseFwd
 *   [6]       EthCrcFwd
 *   [5]       EthPadEn
 *   [4]       EthPromisEn
 *   [3]       EthWanMode
 *   [1]       EthRxEna
 *   [0]       EthTxEna
 *
 * Name(s):
 * <base>.MacEthSeg0.EthCmdConfig...
 */
#define NFP_MAC_ETH_SEG_CMD_CONFIG(_seg)                   (0x0008 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_RF           (1 << 29)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_RF_bf      0, 29, 29
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_RF_msk     (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_RF_bit     (29)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_LF           (1 << 28)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_LF_bf      0, 28, 28
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_LF_msk     (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_FORCE_SEND_LF_bit     (28)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_DISABLE_FLT_HDL         (1 << 27)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_DISABLE_FLT_HDL_bf    0, 27, 27
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_DISABLE_FLT_HDL_msk   (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_DISABLE_FLT_HDL_bit   (27)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_TX_FLUSH                (1 << 22)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_FLUSH_bf           0, 22, 22
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_FLUSH_msk          (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_FLUSH_bit          (22)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_RX_SFD_ANY              (1 << 21)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_SFD_ANY_bf         0, 21, 21
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_SFD_ANY_msk        (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_SFD_ANY_bit        (21)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_PFC_COMP          (1 << 20)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_PFC_COMP_bf     0, 20, 20
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_PFC_COMP_msk    (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_PFC_COMP_bit    (20)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PFC_MODE                (1 << 19)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PFC_MODE_bf           0, 19, 19
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PFC_MODE_msk          (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PFC_MODE_bit          (19)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_RS_COL_CNT_EXT          (1 << 18)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RS_COL_CNT_EXT_bf     0, 18, 18
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RS_COL_CNT_EXT_msk    (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RS_COL_CNT_EXT_bit    (18)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_NO_LEN_CHECK            (1 << 17)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_NO_LEN_CHECK_bf       0, 17, 17
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_NO_LEN_CHECK_msk      (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_NO_LEN_CHECK_bit      (17)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_SEND_IDLE               (1 << 16)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_SEND_IDLE_bf          0, 16, 16
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_SEND_IDLE_msk         (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_SEND_IDLE_bit         (16)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PHY_TX_ENABLE           (1 << 15)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PHY_TX_ENABLE_bf      0, 15, 15
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PHY_TX_ENABLE_msk     (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PHY_TX_ENABLE_bit     (15)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ERR_DISCARD          (1 << 14)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ERR_DISCARD_bf     0, 14, 14
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ERR_DISCARD_msk    (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ERR_DISCARD_bit    (14)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_CMD_FRAME_ENABLE        (1 << 13)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_CMD_FRAME_ENABLE_bf   0, 13, 13
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_CMD_FRAME_ENABLE_msk  (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_CMD_FRAME_ENABLE_bit  (13)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_SW_RESET                (1 << 12)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_SW_RESET_bf           0, 12, 12
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_SW_RESET_msk          (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_SW_RESET_bit          (12)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_TX_PAD_ENABLE           (1 << 11)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_PAD_ENABLE_bf      0, 11, 11
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_PAD_ENABLE_msk     (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_PAD_ENABLE_bit     (11)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_LOOPBACK_ENABLE         (1 << 10)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_LOOPBACK_ENABLE_bf    0, 10, 10
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_LOOPBACK_ENABLE_msk   (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_LOOPBACK_ENABLE_bit   (10)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ADDR_INSERT          (1 << 9)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ADDR_INSERT_bf     0, 9, 9
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ADDR_INSERT_msk    (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ADDR_INSERT_bit    (9)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_IGNORE            (1 << 8)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_IGNORE_bf       0, 8, 8
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_IGNORE_msk      (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_IGNORE_bit      (8)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_FWD               (1 << 7)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_FWD_bf          0, 7, 7
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_FWD_msk         (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAUSE_FWD_bit         (7)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_CRC_FWD                 (1 << 6)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_CRC_FWD_bf            0, 6, 6
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_CRC_FWD_msk           (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_CRC_FWD_bit           (6)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PAD_ENABLE              (1 << 5)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAD_ENABLE_bf         0, 5, 5
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAD_ENABLE_msk        (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PAD_ENABLE_bit        (5)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_PROMISC_ENABLE          (1 << 4)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PROMISC_ENABLE_bf     0, 4, 4
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PROMISC_ENABLE_msk    (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_PROMISC_ENABLE_bit    (4)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_WAN_MODE                (1 << 3)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_WAN_MODE_bf           0, 3, 3
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_WAN_MODE_msk          (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_WAN_MODE_bit          (3)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ENABLE               (1 << 1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ENABLE_bf          0, 1, 1
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ENABLE_msk         (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_RX_ENABLE_bit         (1)
#define   NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ENABLE               (1 << 0)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ENABLE_bf          0, 0, 0
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ENABLE_msk         (0x1)
#define     NFP_MAC_ETH_SEG_CMD_CONFIG_TX_ENABLE_bit         (0)


/*
 * Register: EthMacAddr0
 *   [31:0]    EthMacAddr0
 *
 * Name(s):
 * <base>.MacEthSeg0.EthMacAddr0...
 */
#define NFP_MAC_ETH_SEG_MAC_ADDR_0(_seg)                   (0x000c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_MAC_ADDR_0_VAL(x)                  (((x) & 0xffffffff) << 0)
#define   NFP_MAC_ETH_SEG_MAC_ADDR_0_VAL_of(x)               (((x) >> 0) & 0xffffffff)
#define     NFP_MAC_ETH_SEG_MAC_ADDR_0_VAL_bf                0, 31, 0
#define     NFP_MAC_ETH_SEG_MAC_ADDR_0_VAL_msk               (0xffffffff)
#define     NFP_MAC_ETH_SEG_MAC_ADDR_0_VAL_shf               (0)


/*
 * Register: EthMacAddr1
 *   [15:0]    EthMacAddr1
 *
 * Name(s):
 * <base>.MacEthSeg0.EthMacAddr1...
 */
#define NFP_MAC_ETH_SEG_MAC_ADDR_1(_seg)                   (0x0010 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_MAC_ADDR_1_VAL(x)                  (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_MAC_ADDR_1_VAL_of(x)               (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_MAC_ADDR_1_VAL_bf                0, 15, 0
#define     NFP_MAC_ETH_SEG_MAC_ADDR_1_VAL_msk               (0xffff)
#define     NFP_MAC_ETH_SEG_MAC_ADDR_1_VAL_shf               (0)


/*
 * Register: EthFrmLength
 *   [15:0]    EthFrmLength
 *
 * Name(s):
 * <base>.MacEthSeg0.EthFrmLength...
 */
#define NFP_MAC_ETH_SEG_FRAME_LEN(_seg)                    (0x0014 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_FRAME_LEN_VAL(x)                   (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_FRAME_LEN_VAL_of(x)                (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_FRAME_LEN_VAL_bf                 0, 15, 0
#define     NFP_MAC_ETH_SEG_FRAME_LEN_VAL_msk                (0xffff)
#define     NFP_MAC_ETH_SEG_FRAME_LEN_VAL_shf                (0)


/*
 * Register: EthRxFifoSections
 *   [31:16]   EthRxSectionEmptyWm
 *   [15:0]    EthRxSectionAvailWm
 *
 * Name(s):
 * <base>.MacEthSeg0.EthRxFifoSections...
 */
#define NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS(_seg)             (0x001c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_EMPTY_WM(x)       (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_EMPTY_WM_of(x)    (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_EMPTY_WM_bf     0, 31, 16
#define     NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_EMPTY_WM_msk    (0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_EMPTY_WM_shf    (16)
#define   NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_AVAIL_WM(x)       (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_AVAIL_WM_of(x)    (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_AVAIL_WM_bf     0, 15, 0
#define     NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_AVAIL_WM_msk    (0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_SECTIONS_AVAIL_WM_shf    (0)


/*
 * Register: EthTxFifoSections
 *   [31:16]   EthTxSectionEmptyWm
 *   [15:0]    EthTxSectionAvailWm
 *
 * Name(s):
 * <base>.MacEthSeg0.EthTxFifoSections...
 */
#define NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS(_seg)             (0x0020 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_EMPTY_WM(x)       (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_EMPTY_WM_of(x)    (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_EMPTY_WM_bf     0, 31, 16
#define     NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_EMPTY_WM_msk    (0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_EMPTY_WM_shf    (16)
#define   NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_AVAIL_WM(x)       (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_AVAIL_WM_of(x)    (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_AVAIL_WM_bf     0, 15, 0
#define     NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_AVAIL_WM_msk    (0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_SECTIONS_AVAIL_WM_shf    (0)


/*
 * Register: EthRxFifoAlmostFE
 *   [31:16]   EthRxFifoAlmostFullWm
 *   [15:0]    EthRxFifoAlmostEmptyWm
 *
 * Name(s):
 * <base>.MacEthSeg0.EthRxFifoAlmostFE...
 */
#define NFP_MAC_ETH_SEG_RX_FIFO_ALMOST(_seg)               (0x0024 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_FULL_WM(x)          (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_FULL_WM_of(x)       (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_FULL_WM_bf        0, 31, 16
#define     NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_FULL_WM_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_FULL_WM_shf       (16)
#define   NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_EMPTY_WM(x)         (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_EMPTY_WM_of(x)      (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_EMPTY_WM_bf       0, 15, 0
#define     NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_EMPTY_WM_msk      (0xffff)
#define     NFP_MAC_ETH_SEG_RX_FIFO_ALMOST_EMPTY_WM_shf      (0)


/*
 * Register: EthTxFifoAlmostFE
 *   [31:16]   EthTxFifoAlmostFullWm
 *   [15:0]    EthTxFifoAlmostEmptyWm
 *
 * Name(s):
 * <base>.MacEthSeg0.EthTxFifoAlmostFE...
 */
#define NFP_MAC_ETH_SEG_TX_FIFO_ALMOST(_seg)               (0x0028 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_FULL_WM(x)          (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_FULL_WM_of(x)       (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_FULL_WM_bf        0, 31, 16
#define     NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_FULL_WM_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_FULL_WM_shf       (16)
#define   NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_EMPTY_WM(x)         (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_EMPTY_WM_of(x)      (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_EMPTY_WM_bf       0, 15, 0
#define     NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_EMPTY_WM_msk      (0xffff)
#define     NFP_MAC_ETH_SEG_TX_FIFO_ALMOST_EMPTY_WM_shf      (0)


/*
 * Register: EthHashTableLoad
 *   [8]       EthHashTableMcEn
 *   [5:0]     EthHashTableAddr
 *
 * Name(s):
 * <base>.MacEthSeg0.EthHashTableLoad...
 */
#define NFP_MAC_ETH_SEG_HASH_TABLE_LOAD(_seg)              (0x002c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_MC_EN              (1 << 8)
#define     NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_MC_EN_bf         0, 8, 8
#define     NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_MC_EN_msk        (0x1)
#define     NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_MC_EN_bit        (8)
#define   NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_ADDR(x)            (((x) & 0x3f) << 0)
#define   NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_ADDR_of(x)         (((x) >> 0) & 0x3f)
#define     NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_ADDR_bf          0, 5, 0
#define     NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_ADDR_msk         (0x3f)
#define     NFP_MAC_ETH_SEG_HASH_TABLE_LOAD_ADDR_shf         (0)


/*
 * Register: EthStatus
 *   [3]       EthTsAvail
 *   [2]       EthPhyLos
 *   [1]       EthRxRemFault
 *   [0]       EthRxLocFault
 *
 * Name(s):
 * <base>.MacEthSeg0.EthStatus...
 */
#define NFP_MAC_ETH_SEG_STS(_seg)                          (0x0040 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_STS_TS_AVAIL                       (1 << 3)
#define     NFP_MAC_ETH_SEG_STS_TS_AVAIL_bf                  0, 3, 3
#define     NFP_MAC_ETH_SEG_STS_TS_AVAIL_msk                 (0x1)
#define     NFP_MAC_ETH_SEG_STS_TS_AVAIL_bit                 (3)
#define   NFP_MAC_ETH_SEG_STS_PHY_LOS                        (1 << 2)
#define     NFP_MAC_ETH_SEG_STS_PHY_LOS_bf                   0, 2, 2
#define     NFP_MAC_ETH_SEG_STS_PHY_LOS_msk                  (0x1)
#define     NFP_MAC_ETH_SEG_STS_PHY_LOS_bit                  (2)
#define   NFP_MAC_ETH_SEG_STS_RX_REMOTE_FAULT                (1 << 1)
#define     NFP_MAC_ETH_SEG_STS_RX_REMOTE_FAULT_bf           0, 1, 1
#define     NFP_MAC_ETH_SEG_STS_RX_REMOTE_FAULT_msk          (0x1)
#define     NFP_MAC_ETH_SEG_STS_RX_REMOTE_FAULT_bit          (1)
#define   NFP_MAC_ETH_SEG_STS_RX_LOCAL_FAULT                 (1 << 0)
#define     NFP_MAC_ETH_SEG_STS_RX_LOCAL_FAULT_bf            0, 0, 0
#define     NFP_MAC_ETH_SEG_STS_RX_LOCAL_FAULT_msk           (0x1)
#define     NFP_MAC_ETH_SEG_STS_RX_LOCAL_FAULT_bit           (0)


/*
 * Register: EthTxIpgLength
 *   [6:0]     EthTxIpgLength
 *
 * Name(s):
 * <base>.MacEthSeg0.EthTxIpgLength...
 */
#define NFP_MAC_ETH_SEG_TX_IPG_LEN(_seg)                   (0x0044 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_TX_IPG_LEN_VAL(x)                  (((x) & 0x7f) << 0)
#define   NFP_MAC_ETH_SEG_TX_IPG_LEN_VAL_of(x)               (((x) >> 0) & 0x7f)
#define     NFP_MAC_ETH_SEG_TX_IPG_LEN_VAL_bf                0, 6, 0
#define     NFP_MAC_ETH_SEG_TX_IPG_LEN_VAL_msk               (0x7f)
#define     NFP_MAC_ETH_SEG_TX_IPG_LEN_VAL_shf               (0)


/*
 * Register: EthCreditTrigger
 *   [0]       EthCreditTrigger
 *
 * Name(s):
 * <base>.MacEthSeg0.EthCreditTrigger...
 */
#define NFP_MAC_ETH_SEG_CREDIT_TRIGGER(_seg)               (0x0048 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_CREDIT_TRIGGER_VAL                 (1 << 0)
#define     NFP_MAC_ETH_SEG_CREDIT_TRIGGER_VAL_bf            0, 0, 0
#define     NFP_MAC_ETH_SEG_CREDIT_TRIGGER_VAL_msk           (0x1)
#define     NFP_MAC_ETH_SEG_CREDIT_TRIGGER_VAL_bit           (0)


/*
 * Register: EthInitCredit
 *   [7:0]     EthInitCredit
 *
 * Name(s):
 * <base>.MacEthSeg0.EthInitCredit...
 */
#define NFP_MAC_ETH_SEG_INIT_CREDIT(_seg)                  (0x004c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_INIT_CREDIT_VAL(x)                 (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_SEG_INIT_CREDIT_VAL_of(x)              (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_SEG_INIT_CREDIT_VAL_bf               0, 7, 0
#define     NFP_MAC_ETH_SEG_INIT_CREDIT_VAL_msk              (0xff)
#define     NFP_MAC_ETH_SEG_INIT_CREDIT_VAL_shf              (0)


/*
 * Register: EthCreditReg
 *   [7:0]     EthCreditReg
 *
 * Name(s):
 * <base>.MacEthSeg0.EthCreditReg...
 */
#define NFP_MAC_ETH_SEG_CREDIT_REG(_seg)                   (0x0050 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_CREDIT_REG_VAL(x)                  (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_SEG_CREDIT_REG_VAL_of(x)               (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_SEG_CREDIT_REG_VAL_bf                0, 7, 0
#define     NFP_MAC_ETH_SEG_CREDIT_REG_VAL_msk               (0xff)
#define     NFP_MAC_ETH_SEG_CREDIT_REG_VAL_shf               (0)


/*
 * Register: EthPauseQuantaCL01
 *   [31:16]   EthPauseQuantaCL1
 *   [15:0]    EthPauseQuantaCL0
 *
 * Name(s):
 * <base>.MacEthSeg0.EthPauseQuantaCL01...
 */
#define NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01(_seg)            (0x0054 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL1(x)           (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL1_of(x)        (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL1_bf         0, 31, 16
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL1_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL1_shf        (16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL0(x)           (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL0_of(x)        (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL0_bf         0, 15, 0
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL0_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL01_CL0_shf        (0)


/*
 * Register: EthPauseQuantaCL23
 *   [31:16]   EthPauseQuantaCL3
 *   [15:0]    EthPauseQuantaCL2
 *
 * Name(s):
 * <base>.MacEthSeg0.EthPauseQuantaCL23...
 */
#define NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23(_seg)            (0x0058 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL3(x)           (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL3_of(x)        (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL3_bf         0, 31, 16
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL3_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL3_shf        (16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL2(x)           (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL2_of(x)        (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL2_bf         0, 15, 0
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL2_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL23_CL2_shf        (0)


/*
 * Register: EthPauseQuantaCL45
 *   [31:16]   EthPauseQuantaCL5
 *   [15:0]    EthPauseQuantaCL4
 *
 * Name(s):
 * <base>.MacEthSeg0.EthPauseQuantaCL45...
 */
#define NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45(_seg)            (0x005c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL5(x)           (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL5_of(x)        (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL5_bf         0, 31, 16
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL5_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL5_shf        (16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL4(x)           (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL4_of(x)        (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL4_bf         0, 15, 0
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL4_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL45_CL4_shf        (0)


/*
 * Register: EthPauseQuantaCL67
 *   [31:16]   EthPauseQuantaCL7
 *   [15:0]    EthPauseQuantaCL6
 *
 * Name(s):
 * <base>.MacEthSeg0.EthPauseQuantaCL67...
 */
#define NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67(_seg)            (0x0060 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL7(x)           (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL7_of(x)        (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL7_bf         0, 31, 16
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL7_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL7_shf        (16)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL6(x)           (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL6_of(x)        (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL6_bf         0, 15, 0
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL6_msk        (0xffff)
#define     NFP_MAC_ETH_SEG_PAUSE_QUANTA_CL67_CL6_shf        (0)


/*
 * Register: EthQuantaThreshCL01
 *   [31:16]   EthQuantaThreshCL1
 *   [15:0]    EthQuantaThreshCL0
 *
 * Name(s):
 * <base>.MacEthSeg0.EthQuantaThreshCL01...
 */
#define NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01(_seg)           (0x0064 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL1(x)          (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL1_of(x)       (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL1_bf        0, 31, 16
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL1_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL1_shf       (16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL0(x)          (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL0_of(x)       (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL0_bf        0, 15, 0
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL0_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL01_CL0_shf       (0)


/*
 * Register: EthQuantaThreshCL23
 *   [31:16]   EthQuantaThreshCL3
 *   [15:0]    EthQuantaThreshCL2
 *
 * Name(s):
 * <base>.MacEthSeg0.EthQuantaThreshCL23...
 */
#define NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23(_seg)           (0x0068 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL3(x)          (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL3_of(x)       (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL3_bf        0, 31, 16
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL3_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL3_shf       (16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL2(x)          (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL2_of(x)       (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL2_bf        0, 15, 0
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL2_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL23_CL2_shf       (0)


/*
 * Register: EthQuantaThreshCL45
 *   [31:16]   EthQuantaThreshCL5
 *   [15:0]    EthQuantaThreshCL4
 *
 * Name(s):
 * <base>.MacEthSeg0.EthQuantaThreshCL45...
 */
#define NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45(_seg)           (0x006c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL5(x)          (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL5_of(x)       (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL5_bf        0, 31, 16
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL5_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL5_shf       (16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL4(x)          (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL4_of(x)       (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL4_bf        0, 15, 0
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL4_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL45_CL4_shf       (0)


/*
 * Register: EthQuantaThreshCL67
 *   [31:16]   EthQuantaThreshCL7
 *   [15:0]    EthQuantaThreshCL6
 *
 * Name(s):
 * <base>.MacEthSeg0.EthQuantaThreshCL67...
 */
#define NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67(_seg)           (0x0070 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL7(x)          (((x) & 0xffff) << 16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL7_of(x)       (((x) >> 16) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL7_bf        0, 31, 16
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL7_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL7_shf       (16)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL6(x)          (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL6_of(x)       (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL6_bf        0, 15, 0
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL6_msk       (0xffff)
#define     NFP_MAC_ETH_SEG_QUANTA_THRESH_CL67_CL6_shf       (0)


/*
 * Register: EthRxPauseStatus
 *   [7:0]     EthRxPauseStatus
 *
 * Name(s):
 * <base>.MacEthSeg0.EthRxPauseStatus...
 */
#define NFP_MAC_ETH_SEG_RX_PAUSE_STS(_seg)                 (0x0074 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_RX_PAUSE_STS_VAL(x)                (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_SEG_RX_PAUSE_STS_VAL_of(x)             (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_SEG_RX_PAUSE_STS_VAL_bf              0, 7, 0
#define     NFP_MAC_ETH_SEG_RX_PAUSE_STS_VAL_msk             (0xff)
#define     NFP_MAC_ETH_SEG_RX_PAUSE_STS_VAL_shf             (0)


/*
 * Register: EthTimestamp
 *
 * Name(s):
 * <base>.MacEthSeg0.EthTimestamp...
 */
#define NFP_MAC_ETH_SEG_TIMESTAMP(_seg)                    (0x007c + ((_seg) * 0x400))

/*
 * Register: EthTxPreamble0
 *
 * Name(s):
 * <base>.MacEthSeg0.EthTxPreamble0...
 */
#define NFP_MAC_ETH_SEG_TX_PREAMBLE_0(_seg)                (0x0280 + ((_seg) * 0x400))

/*
 * Register: EthTxPreamble1
 *   [23:0]    EthTxPreamble1
 *
 * Name(s):
 * <base>.MacEthSeg0.EthTxPreamble1...
 */
#define NFP_MAC_ETH_SEG_TX_PREAMBLE_1(_seg)                (0x0284 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_TX_PREAMBLE_1_VAL(x)               (((x) & 0xffffff) << 0)
#define   NFP_MAC_ETH_SEG_TX_PREAMBLE_1_VAL_of(x)            (((x) >> 0) & 0xffffff)
#define     NFP_MAC_ETH_SEG_TX_PREAMBLE_1_VAL_bf             0, 23, 0
#define     NFP_MAC_ETH_SEG_TX_PREAMBLE_1_VAL_msk            (0xffffff)
#define     NFP_MAC_ETH_SEG_TX_PREAMBLE_1_VAL_shf            (0)


/*
 * Register: EthSgmiiPcsCtl
 *   [15]      EthPcsReset
 *   [14]      EthPhyLoopBack
 *   [13]      EthSgmiiSpeedSel0
 *   [12]      EthAutoNegEnable
 *   [11]      EthPowerDown
 *   [10]      EthIsolate
 *   [9]       EthRestartAutoNeg
 *   [8]       EthDuplexMode
 *   [7]       EthCollisionTest
 *   [6]       EthSgmiiSpeedSel1
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiPcsCtl...
 */
#define NFP_MAC_ETH_SEG_SGMII_PCS_CTRL(_seg)               (0x0300 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESET               (1 << 15)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESET_bf          0, 15, 15
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESET_msk         (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESET_bit         (15)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_PHY_LOOPBACK        (1 << 14)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_PHY_LOOPBACK_bf   0, 14, 14
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_PHY_LOOPBACK_msk  (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_PHY_LOOPBACK_bit  (14)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_0         (1 << 13)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_0_bf    0, 13, 13
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_0_msk   (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_0_bit   (13)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_AUTONEG_ENABLE      (1 << 12)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_AUTONEG_ENABLE_bf 0, 12, 12
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_AUTONEG_ENABLE_msk (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_AUTONEG_ENABLE_bit (12)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_POWER_DOWN          (1 << 11)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_POWER_DOWN_bf     0, 11, 11
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_POWER_DOWN_msk    (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_POWER_DOWN_bit    (11)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_ISOLATE             (1 << 10)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_ISOLATE_bf        0, 10, 10
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_ISOLATE_msk       (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_ISOLATE_bit       (10)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESTART_AUTONEG     (1 << 9)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESTART_AUTONEG_bf 0, 9, 9
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESTART_AUTONEG_msk (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_RESTART_AUTONEG_bit (9)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_DUPLEX_MODE         (1 << 8)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_DUPLEX_MODE_bf    0, 8, 8
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_DUPLEX_MODE_msk   (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_DUPLEX_MODE_bit   (8)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_COLLISION_TEST      (1 << 7)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_COLLISION_TEST_bf 0, 7, 7
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_COLLISION_TEST_msk (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_COLLISION_TEST_bit (7)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_1         (1 << 6)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_1_bf    0, 6, 6
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_1_msk   (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_CTRL_SPEED_SEL_1_bit   (6)


/*
 * Register: EthSgmiiPcsStatus
 *   [15]      Eth100T4
 *   [14:13]   Eth100XHalfDuplex
 *   [12:11]   Eth10MbHalfDuplex
 *   [10:9]    Eth100T2HalfDuplex
 *   [8]       EthExtendedStatus
 *   [5]       EthAutoNegComplete
 *   [4]       EthRemoteFault
 *   [3]       EthAutoNegAbility
 *   [2]       EthLinkStatus
 *   [1]       EthJabberDetect
 *   [0]       EthExtCapable
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiPcsStatus...
 */
#define NFP_MAC_ETH_SEG_SGMII_PCS_STS(_seg)                (0x0304 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET4            (1 << 15)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET4_bf       0, 15, 15
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET4_msk      (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET4_bit      (15)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASEX_HALF_DUPLEX(x) (((x) & 3) << 13)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASEX_HALF_DUPLEX_of(x) (((x) >> 13) & 3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASEX_HALF_DUPLEX_bf 0, 14, 13
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASEX_HALF_DUPLEX_msk (0x3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASEX_HALF_DUPLEX_shf (13)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_10M_HALF_DUPLEX(x)   (((x) & 3) << 11)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_10M_HALF_DUPLEX_of(x) (((x) >> 11) & 3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_10M_HALF_DUPLEX_bf 0, 12, 11
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_10M_HALF_DUPLEX_msk (0x3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_10M_HALF_DUPLEX_shf (11)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET2_HALF_DUPLEX(x) (((x) & 3) << 9)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET2_HALF_DUPLEX_of(x) (((x) >> 9) & 3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET2_HALF_DUPLEX_bf 0, 10, 9
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET2_HALF_DUPLEX_msk (0x3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_100BASET2_HALF_DUPLEX_shf (9)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXTENDED_STS         (1 << 8)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXTENDED_STS_bf    0, 8, 8
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXTENDED_STS_msk   (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXTENDED_STS_bit   (8)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_COMPLETE     (1 << 5)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_COMPLETE_bf 0, 5, 5
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_COMPLETE_msk (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_COMPLETE_bit (5)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_REMOTE_FAULT         (1 << 4)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_REMOTE_FAULT_bf    0, 4, 4
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_REMOTE_FAULT_msk   (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_REMOTE_FAULT_bit   (4)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_ABILITY      (1 << 3)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_ABILITY_bf 0, 3, 3
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_ABILITY_msk (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_AUTONEG_ABILITY_bit (3)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_LINK_STS             (1 << 2)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_LINK_STS_bf        0, 2, 2
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_LINK_STS_msk       (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_LINK_STS_bit       (2)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_JABBER_DETECT        (1 << 1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_JABBER_DETECT_bf   0, 1, 1
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_JABBER_DETECT_msk  (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_JABBER_DETECT_bit  (1)
#define   NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXT_CAPABLE          (1 << 0)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXT_CAPABLE_bf     0, 0, 0
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXT_CAPABLE_msk    (0x1)
#define     NFP_MAC_ETH_SEG_SGMII_PCS_STS_EXT_CAPABLE_bit    (0)


/*
 * Register: EthSgmiiPhyIdent0
 *   [15:0]    EthPhyIdentifier0
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiPhyIdent0...
 */
#define NFP_MAC_ETH_SEG_SGMII_PHY_IDENT0(_seg)             (0x0308 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_SGMII_PHY_IDENT0_VAL(x)            (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_SGMII_PHY_IDENT0_VAL_of(x)         (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_SGMII_PHY_IDENT0_VAL_bf          0, 15, 0
#define     NFP_MAC_ETH_SEG_SGMII_PHY_IDENT0_VAL_msk         (0xffff)
#define     NFP_MAC_ETH_SEG_SGMII_PHY_IDENT0_VAL_shf         (0)


/*
 * Register: EthSgmiiPhyIdent1
 *   [15:0]    EthPhyIdentifier1
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiPhyIdent1...
 */
#define NFP_MAC_ETH_SEG_SGMII_PHY_IDENT1(_seg)             (0x030c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_SGMII_PHY_IDENT1_VAL(x)            (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_SGMII_PHY_IDENT1_VAL_of(x)         (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_SGMII_PHY_IDENT1_VAL_bf          0, 15, 0
#define     NFP_MAC_ETH_SEG_SGMII_PHY_IDENT1_VAL_msk         (0xffff)
#define     NFP_MAC_ETH_SEG_SGMII_PHY_IDENT1_VAL_shf         (0)


/*
 * Register: EthSgmiiDevAbility
 *   [15]      EthNextPageCapable
 *   [14]      EthACK
 *   [13:12]   EthRemoteFault
 *   [8]       EthPS2
 *   [7]       EthPS1
 *   [6]       EthHalfDuplex
 *   [5]       EthFullDuplex
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiDevAbility...
 */
#define NFP_MAC_ETH_SEG_DEV_ABILITY(_seg)                  (0x0310 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_NP                     (1 << 15)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_NP_bf                0, 15, 15
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_NP_msk               (0x1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_NP_bit               (15)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_ACK                    (1 << 14)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_ACK_bf               0, 14, 14
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_ACK_msk              (0x1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_ACK_bit              (14)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT(x)        (((x) & 3) << 12)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_of(x)     (((x) >> 12) & 3)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_no_error (0)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_link_failure (1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_offline (2)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_autonegotiation_error (3)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_bf      0, 13, 12
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_msk     (0x3)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_REMOTE_FAULT_shf     (12)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT2         (1 << 8)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT2_bf    0, 8, 8
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT2_msk   (0x1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT2_bit   (8)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT1         (1 << 7)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT1_bf    0, 7, 7
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT1_msk   (0x1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_PAUSE_SUPPORT1_bit   (7)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_HALF_DUPLEX            (1 << 6)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_HALF_DUPLEX_bf       0, 6, 6
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_HALF_DUPLEX_msk      (0x1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_HALF_DUPLEX_bit      (6)
#define   NFP_MAC_ETH_SEG_DEV_ABILITY_FULL_DUPLEX            (1 << 5)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_FULL_DUPLEX_bf       0, 5, 5
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_FULL_DUPLEX_msk      (0x1)
#define     NFP_MAC_ETH_SEG_DEV_ABILITY_FULL_DUPLEX_bit      (5)


/*
 * Register: EthSgmiiPartnerAbility
 *   [15]      EthCopperLinkStatus
 *   [14]      EthACK
 *   [12]      EthCopperDuplexStatus
 *   [11:10]   EthCopperSpeed
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiPartnerAbility...
 */
#define NFP_MAC_ETH_SEG_PARTNER_ABILITY(_seg)              (0x0314 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_LINK_STS    (1 << 15)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_LINK_STS_bf 0, 15, 15
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_LINK_STS_msk (0x1)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_LINK_STS_bit (15)
#define   NFP_MAC_ETH_SEG_PARTNER_ABILITY_ACK                (1 << 14)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_ACK_bf           0, 14, 14
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_ACK_msk          (0x1)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_ACK_bit          (14)
#define   NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_DUPLEX_STS  (1 << 12)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_DUPLEX_STS_bf 0, 12, 12
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_DUPLEX_STS_msk (0x1)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_DUPLEX_STS_bit (12)
#define   NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED(x)    (((x) & 3) << 10)
#define   NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_of(x) (((x) >> 10) & 3)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_10M (0)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_100M (1)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_1G  (2)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_bf  0, 11, 10
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_msk (0x3)
#define     NFP_MAC_ETH_SEG_PARTNER_ABILITY_COPPER_SPEED_shf (10)


/*
 * Register: EthSgmiiAnExpansion
 *   [1]       EthLatchedHiPageRcvd
 *   [0]       EthRealTimePageRcvd
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiAnExpansion...
 */
#define NFP_MAC_ETH_SEG_AN_EXPANSION(_seg)                 (0x0318 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_AN_EXPANSION_LATCHED_PAGE_RCVD     (1 << 1)
#define     NFP_MAC_ETH_SEG_AN_EXPANSION_LATCHED_PAGE_RCVD_bf 0, 1, 1
#define     NFP_MAC_ETH_SEG_AN_EXPANSION_LATCHED_PAGE_RCVD_msk (0x1)
#define     NFP_MAC_ETH_SEG_AN_EXPANSION_LATCHED_PAGE_RCVD_bit (1)
#define   NFP_MAC_ETH_SEG_AN_EXPANSION_REAL_TIME_PAGE_RCVD   (1 << 0)
#define     NFP_MAC_ETH_SEG_AN_EXPANSION_REAL_TIME_PAGE_RCVD_bf 0, 0, 0
#define     NFP_MAC_ETH_SEG_AN_EXPANSION_REAL_TIME_PAGE_RCVD_msk (0x1)
#define     NFP_MAC_ETH_SEG_AN_EXPANSION_REAL_TIME_PAGE_RCVD_bit (0)


/*
 * Register: EthSgmiiNoSupport
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiDeviceNextPage...
 * <base>.MacEthSeg0.EthSgmiiPartnerNextPage...
 * <base>.MacEthSeg0.EthSgmiiExtendedStatus...
 */
#define NFP_MAC_ETH_SEG_DEVICE_NEXT_PAGE(_seg)             (0x031c + ((_seg) * 0x400))
#define NFP_MAC_ETH_SEG_PARTNER_NEXT_PAGE(_seg)            (0x0320 + ((_seg) * 0x400))
#define NFP_MAC_ETH_SEG_EXTENDED_STS(_seg)                 (0x033c + ((_seg) * 0x400))

/*
 * Register: EthSgmiiLinkTimerLo
 *   [15:0]    EthLinkTimerLo
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiLinkTimerLo...
 */
#define NFP_MAC_ETH_SEG_LINK_TIMER_LO(_seg)                (0x0348 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_LINK_TIMER_LO_VAL(x)               (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_SEG_LINK_TIMER_LO_VAL_of(x)            (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_SEG_LINK_TIMER_LO_VAL_bf             0, 15, 0
#define     NFP_MAC_ETH_SEG_LINK_TIMER_LO_VAL_msk            (0xffff)
#define     NFP_MAC_ETH_SEG_LINK_TIMER_LO_VAL_shf            (0)


/*
 * Register: EthSgmiiLinkTimerHi
 *   [4:0]     EthLinkTimerHi
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiLinkTimerHi...
 */
#define NFP_MAC_ETH_SEG_INK_TIMER_HI(_seg)                 (0x034c + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_INK_TIMER_HI_VAL(x)                (((x) & 0x1f) << 0)
#define   NFP_MAC_ETH_SEG_INK_TIMER_HI_VAL_of(x)             (((x) >> 0) & 0x1f)
#define     NFP_MAC_ETH_SEG_INK_TIMER_HI_VAL_bf              0, 4, 0
#define     NFP_MAC_ETH_SEG_INK_TIMER_HI_VAL_msk             (0x1f)
#define     NFP_MAC_ETH_SEG_INK_TIMER_HI_VAL_shf             (0)


/*
 * Register: EthSgmiiIfMode
 *   [5]       EthSgmiiPcsEnable
 *   [4]       EthSgmiiHDuplex
 *   [3:2]     EthSgmiiSpeed
 *   [1]       EthUseSgmiiAn
 *   [0]       EthSgmiiEna
 *
 * Name(s):
 * <base>.MacEthSeg0.EthSgmiiIfMode...
 */
#define NFP_MAC_ETH_SEG_IF_MODE(_seg)                      (0x0350 + ((_seg) * 0x400))
#define   NFP_MAC_ETH_SEG_IF_MODE_SGMII_PCS_ENABLE           (1 << 5)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_PCS_ENABLE_bf      0, 5, 5
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_PCS_ENABLE_msk     (0x1)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_PCS_ENABLE_bit     (5)
#define   NFP_MAC_ETH_SEG_IF_MODE_SGMII_HDUPLEX              (1 << 4)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_HDUPLEX_bf         0, 4, 4
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_HDUPLEX_msk        (0x1)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_HDUPLEX_bit        (4)
#define   NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED(x)             (((x) & 3) << 2)
#define   NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_of(x)          (((x) >> 2) & 3)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_10M          (0)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_100M         (1)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_1G           (2)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_bf           0, 3, 2
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_msk          (0x3)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_SPEED_shf          (2)
#define   NFP_MAC_ETH_SEG_IF_MODE_USE_SGMII_AN               (1 << 1)
#define     NFP_MAC_ETH_SEG_IF_MODE_USE_SGMII_AN_bf          0, 1, 1
#define     NFP_MAC_ETH_SEG_IF_MODE_USE_SGMII_AN_msk         (0x1)
#define     NFP_MAC_ETH_SEG_IF_MODE_USE_SGMII_AN_bit         (1)
#define   NFP_MAC_ETH_SEG_IF_MODE_SGMII_ENA                  (1 << 0)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_ENA_bf             0, 0, 0
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_ENA_msk            (0x1)
#define     NFP_MAC_ETH_SEG_IF_MODE_SGMII_ENA_bit            (0)


/*
 * Register: EthActCtlSeg
 *   [11:0]    EthActivateSegment
 *
 * Name(s):
 * <base>.MacEthGlobal.EthActCtlSeg
 */
#define NFP_MAC_ETH_GLOBAL_ETH_ACT_CTL_SEG                 0x3000
#define   NFP_MAC_ETH_GLOBAL_ETH_ACT_CTL_SEG_VAL(x)          (((x) & 0xfff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_ACT_CTL_SEG_VAL_of(x)       (((x) >> 0) & 0xfff)
#define     NFP_MAC_ETH_GLOBAL_ETH_ACT_CTL_SEG_VAL_bf        0, 11, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_ACT_CTL_SEG_VAL_msk       (0xfff)
#define     NFP_MAC_ETH_GLOBAL_ETH_ACT_CTL_SEG_VAL_shf       (0)


/*
 * Register: EthModeCtlSeg
 *   [23:0]    EthSegmentModeCtl
 *
 * Name(s):
 * <base>.MacEthGlobal.EthModeCtlSeg
 */
#define NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG                0x3004
#define   NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL(x)         (((x) & 0xffffff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_of(x)      (((x) >> 0) & 0xffffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_10G      (0)
#define     NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_40G      (1)
#define     NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_100G     (2)
#define     NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_bf       0, 23, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_msk      (0xffffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_MODE_CTL_SEG_VAL_shf      (0)


/*
 * Register: EthTxClkCtlSeg
 *   [23:0]    EthSegmentTxClkCtl
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTxClkCtlSeg
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG               0x3008
#define   NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL(x)        (((x) & 0xffffff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_of(x)     (((x) >> 0) & 0xffffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_AUTO    (0)
#define     NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_1G      (1)
#define     NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_OTHER   (2)
#define     NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_bf      0, 23, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_msk     (0xffffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TXCLK_CTL_SEG_VAL_shf     (0)


/*
 * Register: EthPrmblCtlSeg
 *   [23:0]    EthSegmentPrmblCtl
 *
 * Name(s):
 * <base>.MacEthGlobal.EthPrmblCtlSeg
 */
#define NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG               0x300c
#define   NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL(x)        (((x) & 0xffffff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_of(x)     (((x) >> 0) & 0xffffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_DEFAULT (0)
#define     NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_REG     (1)
#define     NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_FF      (2)
#define     NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_bf      0, 23, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_msk     (0xffffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_PRMBL_CTL_SEG_VAL_shf     (0)


/*
 * Register: EthWanRsColCnt
 *   [15:0]    EthWanRsColCnt
 *
 * Name(s):
 * <base>.MacEthGlobal.EthWanRsColCnt
 */
#define NFP_MAC_ETH_GLOBAL_ETH_WAN_RS_COL_CNT              0x3020
#define   NFP_MAC_ETH_GLOBAL_ETH_WAN_RS_COL_CNT_VAL(x)       (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_WAN_RS_COL_CNT_VAL_of(x)    (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_WAN_RS_COL_CNT_VAL_bf     0, 15, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_WAN_RS_COL_CNT_VAL_msk    (0xffff)
#define     NFP_MAC_ETH_GLOBAL_ETH_WAN_RS_COL_CNT_VAL_shf    (0)


/*
 * Register: EthTdmInitCredit0
 *   [31:24]   EthTdmInitCredit3
 *   [23:16]   EthTdmInitCredit2
 *   [15:8]    EthTdmInitCredit1
 *   [7:0]     EthTdmInitCredit0
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTdmInitCredit0
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0            0x3024
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_3(x)       (((x) & 0xff) << 24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_3_of(x)    (((x) >> 24) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_3_bf     0, 31, 24
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_3_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_3_shf    (24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_2(x)       (((x) & 0xff) << 16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_2_of(x)    (((x) >> 16) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_2_bf     0, 23, 16
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_2_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_2_shf    (16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_1(x)       (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_1_of(x)    (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_1_bf     0, 15, 8
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_1_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_1_shf    (8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_0(x)       (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_0_of(x)    (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_0_bf     0, 7, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_0_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT0_0_shf    (0)


/*
 * Register: EthTdmInitCredit1
 *   [31:24]   EthTdmInitCredit7
 *   [23:16]   EthTdmInitCredit6
 *   [15:8]    EthTdmInitCredit5
 *   [7:0]     EthTdmInitCredit4
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTdmInitCredit1
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1            0x3028
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_7(x)       (((x) & 0xff) << 24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_7_of(x)    (((x) >> 24) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_7_bf     0, 31, 24
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_7_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_7_shf    (24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_6(x)       (((x) & 0xff) << 16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_6_of(x)    (((x) >> 16) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_6_bf     0, 23, 16
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_6_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_6_shf    (16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_5(x)       (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_5_of(x)    (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_5_bf     0, 15, 8
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_5_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_5_shf    (8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_4(x)       (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_4_of(x)    (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_4_bf     0, 7, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_4_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT1_4_shf    (0)


/*
 * Register: EthTdmInitCredit2
 *   [31:24]   EthTdmInitCredit11
 *   [23:16]   EthTdmInitCredit10
 *   [15:8]    EthTdmInitCredit9
 *   [7:0]     EthTdmInitCredit8
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTdmInitCredit2
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2            0x302c
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_11(x)      (((x) & 0xff) << 24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_11_of(x)   (((x) >> 24) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_11_bf    0, 31, 24
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_11_msk   (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_11_shf   (24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_10(x)      (((x) & 0xff) << 16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_10_of(x)   (((x) >> 16) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_10_bf    0, 23, 16
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_10_msk   (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_10_shf   (16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_9(x)       (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_9_of(x)    (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_9_bf     0, 15, 8
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_9_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_9_shf    (8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_8(x)       (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_8_of(x)    (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_8_bf     0, 7, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_8_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_INIT_CREDIT2_8_shf    (0)


/*
 * Register: EthTdmCreditReg0
 *   [31:24]   EthTdmCredit3
 *   [23:16]   EthTdmCredit2
 *   [15:8]    EthTdmCredit1
 *   [7:0]     EthTdmCredit0
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTdmCreditReg0
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0             0x3030
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_3(x)        (((x) & 0xff) << 24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_3_of(x)     (((x) >> 24) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_3_bf      0, 31, 24
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_3_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_3_shf     (24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_2(x)        (((x) & 0xff) << 16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_2_of(x)     (((x) >> 16) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_2_bf      0, 23, 16
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_2_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_2_shf     (16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_1(x)        (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_1_of(x)     (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_1_bf      0, 15, 8
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_1_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_1_shf     (8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_0(x)        (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_0_of(x)     (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_0_bf      0, 7, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_0_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG0_0_shf     (0)


/*
 * Register: EthTdmCreditReg1
 *   [31:24]   EthTdmCredit7
 *   [23:16]   EthTdmCredit6
 *   [15:8]    EthTdmCredit5
 *   [7:0]     EthTdmCredit4
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTdmCreditReg1
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1             0x3034
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_7(x)        (((x) & 0xff) << 24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_7_of(x)     (((x) >> 24) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_7_bf      0, 31, 24
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_7_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_7_shf     (24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_6(x)        (((x) & 0xff) << 16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_6_of(x)     (((x) >> 16) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_6_bf      0, 23, 16
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_6_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_6_shf     (16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_5(x)        (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_5_of(x)     (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_5_bf      0, 15, 8
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_5_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_5_shf     (8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_4(x)        (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_4_of(x)     (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_4_bf      0, 7, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_4_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG1_4_shf     (0)


/*
 * Register: EthTdmCreditReg2
 *   [31:24]   EthTdmCredit11
 *   [23:16]   EthTdmCredit10
 *   [15:8]    EthTdmCredit9
 *   [7:0]     EthTdmCredit8
 *
 * Name(s):
 * <base>.MacEthGlobal.EthTdmCreditReg2
 */
#define NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2             0x3038
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_11(x)       (((x) & 0xff) << 24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_11_of(x)    (((x) >> 24) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_11_bf     0, 31, 24
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_11_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_11_shf    (24)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_10(x)       (((x) & 0xff) << 16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_10_of(x)    (((x) >> 16) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_10_bf     0, 23, 16
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_10_msk    (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_10_shf    (16)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_9(x)        (((x) & 0xff) << 8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_9_of(x)     (((x) >> 8) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_9_bf      0, 15, 8
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_9_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_9_shf     (8)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_8(x)        (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_8_of(x)     (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_8_bf      0, 7, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_8_msk     (0xff)
#define     NFP_MAC_ETH_GLOBAL_ETH_TDM_CREDIT_REG2_8_shf     (0)


/*
 * Register: EthVLIntvl
 *   [0]       EthVLIntvl
 *
 * Name(s):
 * <base>.MacEthGlobal.EthVLIntvl
 */
#define NFP_MAC_ETH_GLOBAL_ETH_VL_INTERVAL                 0x3040
#define   NFP_MAC_ETH_GLOBAL_ETH_VL_INTERVAL_VAL             (1 << 0)
#define     NFP_MAC_ETH_GLOBAL_ETH_VL_INTERVAL_VAL_bf        0, 0, 0
#define     NFP_MAC_ETH_GLOBAL_ETH_VL_INTERVAL_VAL_msk       (0x1)
#define     NFP_MAC_ETH_GLOBAL_ETH_VL_INTERVAL_VAL_bit       (0)

/* MacEthMdioCtl: <base>.MacEthMdioCtl */
#define NFP_MAC_ETH_MDIO                                   0x3400

/*
 * Register: EthVlanTpid
 *   [15:0]    EthVlanTpid
 *
 * Name(s):
 * <base>.MacEthVlanTpidCfg.EthVlanTpid0...
 */
#define NFP_MAC_ETH_VLAN_TPID_CFG(x)                       (0x3800 + ((x) * 0x4))
#define   NFP_MAC_ETH_VLAN_TPID_CFG_VAL(x)                   (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_VLAN_TPID_CFG_VAL_of(x)                (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_VLAN_TPID_CFG_VAL_bf                 0, 15, 0
#define     NFP_MAC_ETH_VLAN_TPID_CFG_VAL_msk                (0xffff)
#define     NFP_MAC_ETH_VLAN_TPID_CFG_VAL_shf                (0)

/* MacEthChPcs: <base>.MacEthChPcsSeg0... */
#define NFP_MAC_ETH_CHAN_PCS_SEG(x)                        (0x4000 + ((x) * 0x400))
/* MacEthAutoNegSeg: <base>.MacEthAutoNeg.MacEthAutoNegSeg0... */
#define NFP_MAC_ETH_AUTO_NEG_ETH_AN_SEG(x)                 (0x7000 + ((x) * 0x40))
/* MacEthFecLT: <base>.MacEthFecLT */
#define NFP_MAC_ETH_FEC_LT                                 0x7400
/* MacEthPrbs: <base>.MacEthPrbs */
#define NFP_MAC_ETH_PRBS                                   0x7800


/*
 * Macros for NFP_MAC_ETH_CHAN_PCS(x)
 */

/*
 * Register: EthChPcsCtl1
 *   [15]      EthPcsReset
 *   [14]      EthPcsLoopback
 *   [13]      EthPcsSpeedSelection13
 *   [11]      EthPcsLowPower
 *   [6]       EthPcsSpeedSelection6
 *   [5:2]     EthPcsSpeedSel
 *
 * Name(s):
 * <base>.EthChPcsCtl1
 */
#define NFP_MAC_ETH_CHAN_PCS_CTL1                          0x0000
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_RESET            (1 << 15)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_RESET_bf       0, 15, 15
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_RESET_msk      (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_RESET_bit      (15)
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOOPBACK         (1 << 14)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOOPBACK_bf    0, 14, 14
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOOPBACK_msk   (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOOPBACK_bit   (14)
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION13 (1 << 13)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION13_bf 0, 13, 13
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION13_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION13_bit (13)
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOW_POWER        (1 << 11)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOW_POWER_bf   0, 11, 11
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOW_POWER_msk  (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_LOW_POWER_bit  (11)
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION6 (1 << 6)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION6_bf 0, 6, 6
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION6_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SELECTION6_bit (6)
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL(x)     (((x) & 0xf) << 2)
#define   NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_of(x)  (((x) >> 2) & 0xf)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_Mode0 (0)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_Mode1 (1)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_Mode2 (2)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_Mode3 (3)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_Mode4 (4)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_bf   0, 5, 2
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_msk  (0xf)
#define     NFP_MAC_ETH_CHAN_PCS_CTL1_ETH_PCS_SPEED_SEL_shf  (2)


/*
 * Register: EthChPcsStatus1
 *   [7]       EthPcsFault
 *   [2]       EthPcsRcvLinkStatus
 *   [1]       EthPcsLowPowerAble
 *
 * Name(s):
 * <base>.EthChPcsStatus1
 */
#define NFP_MAC_ETH_CHAN_PCS_STS1                          0x0004
#define   NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_FAULT            (1 << 7)
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_FAULT_bf       0, 7, 7
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_FAULT_msk      (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_FAULT_bit      (7)
#define   NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_RCV_LINK_STS     (1 << 2)
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_RCV_LINK_STS_bf 0, 2, 2
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_RCV_LINK_STS_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_RCV_LINK_STS_bit (2)
#define   NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_LOW_POWER_ABILITY (1 << 1)
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_LOW_POWER_ABILITY_bf 0, 1, 1
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_LOW_POWER_ABILITY_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS1_ETH_PCS_LOW_POWER_ABILITY_bit (1)


/*
 * Register: EthChPcsDevId0
 *   [3:0]     EthPcsDevId
 *
 * Name(s):
 * <base>.EthChPcsDevId0
 */
#define NFP_MAC_ETH_CHAN_PCS_DEV_ID0                       0x0008
#define   NFP_MAC_ETH_CHAN_PCS_DEV_ID0_ETH_PCS_DEV_ID(x)     (((x) & 0xf) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_ID0_ETH_PCS_DEV_ID_of(x)  (((x) >> 0) & 0xf)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_ID0_ETH_PCS_DEV_ID_bf   0, 3, 0
#define     NFP_MAC_ETH_CHAN_PCS_DEV_ID0_ETH_PCS_DEV_ID_msk  (0xf)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_ID0_ETH_PCS_DEV_ID_shf  (0)


/*
 * Register: EthChPcsDevId1
 *
 * Name(s):
 * <base>.EthChPcsDevId1
 */
#define NFP_MAC_ETH_CHAN_PCS_DEV_ID1                       0x000c


/*
 * Register: EthChPcsSpeedAbility
 *   [8]       EthPcsSpeed100GCapable
 *   [7]       EthPcsSpeed40GCapable
 *   [1]       EthPcsSpeed10PassTs2BaseTlCapable
 *   [0]       EthPcsSpeed10GCapable
 *
 * Name(s):
 * <base>.EthChPcsSpeedAbility
 */
#define NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY                0x0010
#define   NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_100G_CAPABLE (1 << 8)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_100G_CAPABLE_bf 0, 8, 8
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_100G_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_100G_CAPABLE_bit (8)
#define   NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_40G_CAPABLE (1 << 7)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_40G_CAPABLE_bf 0, 7, 7
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_40G_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_40G_CAPABLE_bit (7)
#define   NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10PASS_TS_2BASE_TL_CAPABLE (1 << 1)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10PASS_TS_2BASE_TL_CAPABLE_bf 0, 1, 1
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10PASS_TS_2BASE_TL_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10PASS_TS_2BASE_TL_CAPABLE_bit (1)
#define   NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10G_CAPABLE (1 << 0)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10G_CAPABLE_bf 0, 0, 0
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10G_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_SPEED_ABLILITY_ETH_PCS_SPEED_10G_CAPABLE_bit (0)


/*
 * Register: EthChPcsDevInPkg1
 *   [6]       EthPcsTcPresent
 *   [5]       EthPcsDteXsPresent
 *   [4]       EthPcsPhyXsPresent
 *   [3]       EthPcsPCSPresent
 *   [2]       EthPcsWisPresent
 *   [1]       EthPcsPmdPmaPresent
 *   [0]       EthPcsClause22RegsPresent
 *
 * Name(s):
 * <base>.EthChPcsDevInPkg1
 */
#define NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1                   0x0014
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_TC_PRESENT (1 << 6)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_TC_PRESENT_bf 0, 6, 6
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_TC_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_TC_PRESENT_bit (6)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_DTE_XS_PRESENT (1 << 5)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_DTE_XS_PRESENT_bf 0, 5, 5
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_DTE_XS_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_DTE_XS_PRESENT_bit (5)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PHY_XS_PRESENT (1 << 4)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PHY_XS_PRESENT_bf 0, 4, 4
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PHY_XS_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PHY_XS_PRESENT_bit (4)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PCS_PRESENT (1 << 3)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PCS_PRESENT_bf 0, 3, 3
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PCS_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PCS_PRESENT_bit (3)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_WIS_PRESENT (1 << 2)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_WIS_PRESENT_bf 0, 2, 2
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_WIS_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_WIS_PRESENT_bit (2)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PMD_PMA_PRESENT (1 << 1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PMD_PMA_PRESENT_bf 0, 1, 1
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PMD_PMA_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_PMD_PMA_PRESENT_bit (1)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_CLAUSE22_REGS_PRESENT (1 << 0)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_CLAUSE22_REGS_PRESENT_bf 0, 0, 0
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_CLAUSE22_REGS_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG1_ETH_PCS_CLAUSE22_REGS_PRESENT_bit (0)


/*
 * Register: EthChPcsDevInPkg2
 *   [15]      EthPcsVndrDev2Present
 *   [14]      EthPcsVndrDev1Present
 *   [13]      EthPcsClause22ExtPresent
 *
 * Name(s):
 * <base>.EthChPcsDevInPkg2
 */
#define NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2                   0x0018
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV2_PRESENT (1 << 15)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV2_PRESENT_bf 0, 15, 15
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV2_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV2_PRESENT_bit (15)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV1_PRESENT (1 << 14)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV1_PRESENT_bf 0, 14, 14
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV1_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_VENDOR_DEV1_PRESENT_bit (14)
#define   NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_CLAUSE22_EXT_PRESENT (1 << 13)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_CLAUSE22_EXT_PRESENT_bf 0, 13, 13
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_CLAUSE22_EXT_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_DEV_IN_PKG2_ETH_PCS_CLAUSE22_EXT_PRESENT_bit (13)


/*
 * Register: EthChPcsCtl2
 *   [2:0]     EthPcsTypeSelection
 *
 * Name(s):
 * <base>.EthChPcsCtl2
 */
#define NFP_MAC_ETH_CHAN_PCS_CTL2                          0x001c
#define   NFP_MAC_ETH_CHAN_PCS_CTL2_ETH_PCS_TYPE_SELECTION(x) (((x) & 7) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_CTL2_ETH_PCS_TYPE_SELECTION_of(x) (((x) >> 0) & 7)
#define     NFP_MAC_ETH_CHAN_PCS_CTL2_ETH_PCS_TYPE_SELECTION_bf 0, 2, 0
#define     NFP_MAC_ETH_CHAN_PCS_CTL2_ETH_PCS_TYPE_SELECTION_msk (0x7)
#define     NFP_MAC_ETH_CHAN_PCS_CTL2_ETH_PCS_TYPE_SELECTION_shf (0)


/*
 * Register: EthChPcsStatus2
 *   [15:14]   EthPcsDevPresent
 *   [5]       EthPcs100GBaseRCapable
 *   [4]       EthPcs40GBaseRCapable
 *   [3]       EthPcs10GBaseTCapable
 *   [2]       EthPcs10GBaseWCapable
 *   [1]       EthPcs10GBaseXCapable
 *   [0]       EthPcs10GBaseRCapable
 *
 * Name(s):
 * <base>.EthChPcsStatus2
 */
#define NFP_MAC_ETH_CHAN_PCS_STS2                          0x0020
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_DEV_PRESENT(x)   (((x) & 3) << 14)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_DEV_PRESENT_of(x) (((x) >> 14) & 3)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_DEV_PRESENT_bf 0, 15, 14
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_DEV_PRESENT_msk (0x3)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_DEV_PRESENT_shf (14)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_100GBASE_R_CAPABLE (1 << 5)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_100GBASE_R_CAPABLE_bf 0, 5, 5
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_100GBASE_R_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_100GBASE_R_CAPABLE_bit (5)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_40GBASE_R_CAPABLE (1 << 4)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_40GBASE_R_CAPABLE_bf 0, 4, 4
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_40GBASE_R_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_40GBASE_R_CAPABLE_bit (4)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_T_CAPABLE (1 << 3)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_T_CAPABLE_bf 0, 3, 3
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_T_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_T_CAPABLE_bit (3)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_W_CAPABLE (1 << 2)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_W_CAPABLE_bf 0, 2, 2
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_W_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_W_CAPABLE_bit (2)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_X_CAPABLE (1 << 1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_X_CAPABLE_bf 0, 1, 1
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_X_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_X_CAPABLE_bit (1)
#define   NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_R_CAPABLE (1 << 0)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_R_CAPABLE_bf 0, 0, 0
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_R_CAPABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_STS2_ETH_PCS_10GBASE_R_CAPABLE_bit (0)


/*
 * Register: EthChPcsBaseRStatus1
 *   [12]      EthPcsRcvLinkStatus
 *   [3]       EthPcsPrbs9Able
 *   [2]       EthPcsPrbs31Able
 *   [1]       EthPcsHighBer
 *   [0]       EthPcsBlockLocked
 *
 * Name(s):
 * <base>.EthChPcsBaseRStatus1
 */
#define NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1                   0x0080
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_RCV_LINK_STS (1 << 12)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_RCV_LINK_STS_bf 0, 12, 12
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_RCV_LINK_STS_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_RCV_LINK_STS_bit (12)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS9_ABLE (1 << 3)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS9_ABLE_bf 0, 3, 3
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS9_ABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS9_ABLE_bit (3)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS31_ABLE (1 << 2)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS31_ABLE_bf 0, 2, 2
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS31_ABLE_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_PRBS31_ABLE_bit (2)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_HIGH_BER  (1 << 1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_HIGH_BER_bf 0, 1, 1
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_HIGH_BER_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_HIGH_BER_bit (1)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_BLOCK_LOCKED (1 << 0)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_BLOCK_LOCKED_bf 0, 0, 0
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_BLOCK_LOCKED_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS1_ETH_PCS_BLOCK_LOCKED_bit (0)


/*
 * Register: EthChPcsBaseRStatus2
 *   [15]      EthPcsLatchedBlockLock
 *   [14]      EthPcsLatchedHighBerr
 *   [13:8]    EthPcsBerCounter
 *   [7:0]     EthPcsErrorBlockCounter
 *
 * Name(s):
 * <base>.EthChPcsBaseRStatus2
 */
#define NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2                   0x0084
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_BLOCK_LOCK (1 << 15)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_BLOCK_LOCK_bf 0, 15, 15
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_BLOCK_LOCK_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_BLOCK_LOCK_bit (15)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_HIGH_BER (1 << 14)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_HIGH_BER_bf 0, 14, 14
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_HIGH_BER_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_LATCHED_HIGH_BER_bit (14)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_BER_CNTR(x) (((x) & 0x3f) << 8)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_BER_CNTR_of(x) (((x) >> 8) & 0x3f)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_BER_CNTR_bf 0, 13, 8
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_BER_CNTR_msk (0x3f)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_BER_CNTR_shf (8)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_ERRORED_BLOCK_CNTR(x) (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_ERRORED_BLOCK_CNTR_of(x) (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_ERRORED_BLOCK_CNTR_bf 0, 7, 0
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_ERRORED_BLOCK_CNTR_msk (0xff)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_STS2_ETH_PCS_ERRORED_BLOCK_CNTR_shf (0)


/*
 * Register: EthChPcsTestPatSeedData
 *   [15:0]    EthPcsTestPatSeedData
 *
 * Name(s):
 * <base>.EthChPcs10GSeedA0 <base>.EthChPcs10GSeedA1 <base>.EthChPcs10GSeedA2
 * <base>.EthChPcs10GSeedB0 <base>.EthChPcs10GSeedB1 <base>.EthChPcs10GSeedB2
 */
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_A0                   0x0088
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_A1                   0x008c
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_A2                   0x0090
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_B0                   0x0098
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_B1                   0x009c
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_B2                   0x00a0
#define   NFP_MAC_ETH_CHAN_PCS_10G_SEED_A0_ETH_PCS_TEST_PAT_SEED_DATA(x) (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_10G_SEED_A0_ETH_PCS_TEST_PAT_SEED_DATA_of(x) (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_10G_SEED_A0_ETH_PCS_TEST_PAT_SEED_DATA_bf 0, 15, 0
#define     NFP_MAC_ETH_CHAN_PCS_10G_SEED_A0_ETH_PCS_TEST_PAT_SEED_DATA_msk (0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_10G_SEED_A0_ETH_PCS_TEST_PAT_SEED_DATA_shf (0)


/*
 * Register: EthChPcsTestPatSeedDataH
 *   [9:0]     EthPcsTestPatSeedDataH
 *
 * Name(s):
 * <base>.EthChPcs10GSeedA3 <base>.EthChPcs10GSeedB3
 */
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_A3                   0x0094
#define NFP_MAC_ETH_CHAN_PCS_10G_SEED_B3                   0x00a4
#define   NFP_MAC_ETH_CHAN_PCS_10G_SEED_A3_ETH_PCS_TEST_PAT_SEED_DATA_H(x) (((x) & 0x3ff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_10G_SEED_A3_ETH_PCS_TEST_PAT_SEED_DATA_H_of(x) (((x) >> 0) & 0x3ff)
#define     NFP_MAC_ETH_CHAN_PCS_10G_SEED_A3_ETH_PCS_TEST_PAT_SEED_DATA_H_bf 0, 9, 0
#define     NFP_MAC_ETH_CHAN_PCS_10G_SEED_A3_ETH_PCS_TEST_PAT_SEED_DATA_H_msk (0x3ff)
#define     NFP_MAC_ETH_CHAN_PCS_10G_SEED_A3_ETH_PCS_TEST_PAT_SEED_DATA_H_shf (0)


/*
 * Register: EthChPcsBaseRTestCtl
 *   [6]       EthPcsPrbs9TransmitPatEn
 *   [5]       EthPcsPrbs31ReceivePatEn
 *   [4]       EthPcsPrbs31TransmitPatEn
 *   [3]       EthPcsTransmitTestPatEn
 *   [2]       EthPcsReceiveTestPatEn
 *   [1]       EthPcsTestPatSel
 *   [0]       EthPcsDataPatSel
 *
 * Name(s):
 * <base>.EthChPcsBaseRTestCtl
 */
#define NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL               0x00a8
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS9_TRANSMIT_PAT_EN (1 << 6)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS9_TRANSMIT_PAT_EN_bf 0, 6, 6
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS9_TRANSMIT_PAT_EN_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS9_TRANSMIT_PAT_EN_bit (6)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_RECEIVE_PAT_EN (1 << 5)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_RECEIVE_PAT_EN_bf 0, 5, 5
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_RECEIVE_PAT_EN_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_RECEIVE_PAT_EN_bit (5)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_TRANSMIT_PAT_EN (1 << 4)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_TRANSMIT_PAT_EN_bf 0, 4, 4
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_TRANSMIT_PAT_EN_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_PRBS31_TRANSMIT_PAT_EN_bit (4)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TRANSMIT_TEST_PAT_EN (1 << 3)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TRANSMIT_TEST_PAT_EN_bf 0, 3, 3
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TRANSMIT_TEST_PAT_EN_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TRANSMIT_TEST_PAT_EN_bit (3)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_RECEIVE_TEST_PAT_EN (1 << 2)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_RECEIVE_TEST_PAT_EN_bf 0, 2, 2
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_RECEIVE_TEST_PAT_EN_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_RECEIVE_TEST_PAT_EN_bit (2)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TEST_PAT_SEL (1 << 1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TEST_PAT_SEL_bf 0, 1, 1
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TEST_PAT_SEL_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_TEST_PAT_SEL_bit (1)
#define   NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_DATA_PAT_SEL (1 << 0)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_DATA_PAT_SEL_bf 0, 0, 0
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_DATA_PAT_SEL_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_BASE_R_TEST_CTL_ETH_PCS_DATA_PAT_SEL_bit (0)


/*
 * Register: EthChPcsTestErrCnt
 *   [15:0]    EthPcsTestErrCnt
 *
 * Name(s):
 * <base>.EthChPcsTestCnt
 */
#define NFP_MAC_ETH_CHAN_PCS_TEST_ERR_CNT                  0x00ac
#define   NFP_MAC_ETH_CHAN_PCS_TEST_ERR_CNT_ETH_PCS_TEST_ERR_CNT(x) (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_TEST_ERR_CNT_ETH_PCS_TEST_ERR_CNT_of(x) (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_TEST_ERR_CNT_ETH_PCS_TEST_ERR_CNT_bf 0, 15, 0
#define     NFP_MAC_ETH_CHAN_PCS_TEST_ERR_CNT_ETH_PCS_TEST_ERR_CNT_msk (0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_TEST_ERR_CNT_ETH_PCS_TEST_ERR_CNT_shf (0)


/*
 * Register: EthChPcsBerHiOrderCnt
 *   [15:0]    EthPcsBerHiOrderCnt
 *
 * Name(s):
 * <base>.EthChPcsBerHiOrderCnt
 */
#define NFP_MAC_ETH_CHAN_PCS_BER_HI_ORDER_CNT              0x00b0
#define   NFP_MAC_ETH_CHAN_PCS_BER_HI_ORDER_CNT_ETH_PCS_BER_HI_ORDER_CNT(x) (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_BER_HI_ORDER_CNT_ETH_PCS_BER_HI_ORDER_CNT_of(x) (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_BER_HI_ORDER_CNT_ETH_PCS_BER_HI_ORDER_CNT_bf 0, 15, 0
#define     NFP_MAC_ETH_CHAN_PCS_BER_HI_ORDER_CNT_ETH_PCS_BER_HI_ORDER_CNT_msk (0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_BER_HI_ORDER_CNT_ETH_PCS_BER_HI_ORDER_CNT_shf (0)


/*
 * Register: EthChPcsErrBlkHiOrderCnt
 *   [15]      EthPcsHiOrderCntPresent
 *   [13:0]    EthPcsErrBlkHiOrderCnt
 *
 * Name(s):
 * <base>.EthChPcsErrBlkHiOrderCnt
 */
#define NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT          0x00b4
#define   NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_HI_ORDER_CNT_PRESENT (1 << 15)
#define     NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_HI_ORDER_CNT_PRESENT_bf 0, 15, 15
#define     NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_HI_ORDER_CNT_PRESENT_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_HI_ORDER_CNT_PRESENT_bit (15)
#define   NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_ERR_BLK_HI_ORDER_CNT(x) (((x) & 0x3fff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_ERR_BLK_HI_ORDER_CNT_of(x) (((x) >> 0) & 0x3fff)
#define     NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_ERR_BLK_HI_ORDER_CNT_bf 0, 13, 0
#define     NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_ERR_BLK_HI_ORDER_CNT_msk (0x3fff)
#define     NFP_MAC_ETH_CHAN_PCS_ERR_BLK_HI_ORDER_CNT_ETH_PCS_ERR_BLK_HI_ORDER_CNT_shf (0)


/*
 * Register: EthChPcsLaneAlignStat1
 *   [12]      EthPcsLaneAlignmentStatus
 *   [7:0]     EthPcsLaneBlockLock
 *
 * Name(s):
 * <base>.EthChPcsLaneAlignStat1
 */
#define NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1        0x00c8
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_ALIGN_STS (1 << 12)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_ALIGN_STS_bf 0, 12, 12
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_ALIGN_STS_msk (0x1)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_ALIGN_STS_bit (12)
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_BLOCK_LOCK(x) (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_BLOCK_LOCK_of(x) (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_BLOCK_LOCK_bf 0, 7, 0
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_BLOCK_LOCK_msk (0xff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_1_ETH_PCS_LANE_BLOCK_LOCK_shf (0)


/*
 * Register: EthChPcsLaneAlignStat2
 *   [11:0]    EthPcsLaneBlockLock
 *
 * Name(s):
 * <base>.EthChPcsLaneAlignStat2
 */
#define NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_2        0x00cc
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_2_ETH_PCS_LANE_BLOCK_LOCK(x) (((x) & 0xfff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_2_ETH_PCS_LANE_BLOCK_LOCK_of(x) (((x) >> 0) & 0xfff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_2_ETH_PCS_LANE_BLOCK_LOCK_bf 0, 11, 0
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_2_ETH_PCS_LANE_BLOCK_LOCK_msk (0xfff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_2_ETH_PCS_LANE_BLOCK_LOCK_shf (0)


/*
 * Register: EthChPcsLaneAlignStat3
 *   [7:0]     EthPcsLaneAlignMkrLock
 *
 * Name(s):
 * <base>.EthChPcsLaneAlignStat3
 */
#define NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_3        0x00d0
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_3_ETH_PCS_LANE_ALIGN_MARKER_LOCK(x) (((x) & 0xff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_3_ETH_PCS_LANE_ALIGN_MARKER_LOCK_of(x) (((x) >> 0) & 0xff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_3_ETH_PCS_LANE_ALIGN_MARKER_LOCK_bf 0, 7, 0
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_3_ETH_PCS_LANE_ALIGN_MARKER_LOCK_msk (0xff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_3_ETH_PCS_LANE_ALIGN_MARKER_LOCK_shf (0)


/*
 * Register: EthChPcsLaneAlignStat4
 *   [11:0]    EthPcsLaneAlignMkrLock
 *
 * Name(s):
 * <base>.EthChPcsLaneAlignStat4
 */
#define NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_4        0x00d4
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_4_ETH_PCS_LANE_ALIGN_MARKER_LOCK(x) (((x) & 0xfff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_4_ETH_PCS_LANE_ALIGN_MARKER_LOCK_of(x) (((x) >> 0) & 0xfff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_4_ETH_PCS_LANE_ALIGN_MARKER_LOCK_bf 0, 11, 0
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_4_ETH_PCS_LANE_ALIGN_MARKER_LOCK_msk (0xfff)
#define     NFP_MAC_ETH_CHAN_PCS_MULTILANE_ALIGN_STAT_4_ETH_PCS_LANE_ALIGN_MARKER_LOCK_shf (0)


/*
 * Register: EthChPcsBipErrCnt
 *   [15:0]    EthPcsBipErrCnt
 *
 * Name(s):
 * <base>.EthChPcsBipErrCntLane0 <base>.EthChPcsBipErrCntLane1
 * <base>.EthChPcsBipErrCntLane2 <base>.EthChPcsBipErrCntLane3
 * <base>.EthChPcsBipErrCntLane4 <base>.EthChPcsBipErrCntLane5
 * <base>.EthChPcsBipErrCntLane6 <base>.EthChPcsBipErrCntLane7
 * <base>.EthChPcsBipErrCntLane8 <base>.EthChPcsBipErrCntLane9
 * <base>.EthChPcsBipErrCntLane10 <base>.EthChPcsBipErrCntLane11
 * <base>.EthChPcsBipErrCntLane12 <base>.EthChPcsBipErrCntLane13
 * <base>.EthChPcsBipErrCntLane14 <base>.EthChPcsBipErrCntLane15
 * <base>.EthChPcsBipErrCntLane16 <base>.EthChPcsBipErrCntLane17
 * <base>.EthChPcsBipErrCntLane18 <base>.EthChPcsBipErrCntLane19
 */
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE0             0x0168
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE1             0x016c
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE2             0x0170
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE3             0x0174
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE4             0x0178
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE5             0x017c
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE6             0x0180
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE7             0x0184
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE8             0x0188
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE9             0x018c
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE10            0x0190
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE11            0x0194
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE12            0x0198
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE13            0x019c
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE14            0x01a0
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE15            0x01a4
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE16            0x01a8
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE17            0x01ac
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE18            0x01b0
#define NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE19            0x01b4
#define   NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE0_ETH_PCS_BIP_ERR_CNT(x) (((x) & 0xffff) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE0_ETH_PCS_BIP_ERR_CNT_of(x) (((x) >> 0) & 0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE0_ETH_PCS_BIP_ERR_CNT_bf 0, 15, 0
#define     NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE0_ETH_PCS_BIP_ERR_CNT_msk (0xffff)
#define     NFP_MAC_ETH_CHAN_PCS_BIP_ERR_CNT_LANE0_ETH_PCS_BIP_ERR_CNT_shf (0)


/*
 * Register: EthChPcsLaneMap
 *   [4:0]     EthPcsLaneMap
 *
 * Name(s):
 * <base>.EthChPcsLaneMapLane0 <base>.EthChPcsLaneMapLane1
 * <base>.EthChPcsLaneMapLane2 <base>.EthChPcsLaneMapLane3
 * <base>.EthChPcsLaneMapLane4 <base>.EthChPcsLaneMapLane5
 * <base>.EthChPcsLaneMapLane6 <base>.EthChPcsLaneMapLane7
 * <base>.EthChPcsLaneMapLane8 <base>.EthChPcsLaneMapLane9
 * <base>.EthChPcsLaneMapLane10 <base>.EthChPcsLaneMapLane11
 * <base>.EthChPcsLaneMapLane12 <base>.EthChPcsLaneMapLane13
 * <base>.EthChPcsLaneMapLane14 <base>.EthChPcsLaneMapLane15
 * <base>.EthChPcsLaneMapLane16 <base>.EthChPcsLaneMapLane17
 * <base>.EthChPcsLaneMapLane18 <base>.EthChPcsLaneMapLane19
 */
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE0                0x01b8
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE1                0x01bc
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE2                0x01c0
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE3                0x01c4
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE4                0x01c8
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE5                0x01cc
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE6                0x01d0
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE7                0x01d4
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE8                0x01d8
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE9                0x01dc
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE10               0x01e0
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE11               0x01e4
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE12               0x01e8
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE13               0x01ec
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE14               0x01f0
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE15               0x01f4
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE16               0x01f8
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE17               0x01fc
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE18               0x0200
#define NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE19               0x0204
#define   NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE0_ETH_PCS_LANE_MAP(x) (((x) & 0x1f) << 0)
#define   NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE0_ETH_PCS_LANE_MAP_of(x) (((x) >> 0) & 0x1f)
#define     NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE0_ETH_PCS_LANE_MAP_bf 0, 4, 0
#define     NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE0_ETH_PCS_LANE_MAP_msk (0x1f)
#define     NFP_MAC_ETH_CHAN_PCS_LANE_MAP_LANE0_ETH_PCS_LANE_MAP_shf (0)


/*
 * Macros for NFP_MAC_XPB(x)
 */

/*
 * Register: MacBlkReset
 *   [23]      MacHy1StatRst
 *   [22]      MacHy0StatRst
 *   [21]      MacTxRstMpb
 *   [20]      MacRxRstMpb
 *   [19]      MacTxRstCore
 *   [18]      MacRxRstCore
 *   [17]      MacFcX2RstLk1
 *   [16]      MacFcX2RstLk0
 *   [15]      MacRxRstLk1
 *   [14]      MacRxRstLk0
 *   [13]      MacTxRstLk1
 *   [12]      MacTxRstLk0
 *   [11]      MacRstLk1
 *   [10]      MacRstLk0
 *   [9]       MacX2ClkEnLk1
 *   [8]       MacX2ClkEnLk0
 *   [7]       MacCoreClkEnLk1
 *   [6]       MacCoreClkEnLk0
 *   [5]       MacCoreClkEnHy1
 *   [4]       MacCoreClkEnHy0
 *   [2]       MacSerDesRst
 *   [1]       MacSReset
 *   [0]       MacHReset
 *
 * Name(s):
 * <base>.MacBlkReset
 */
#define NFP_MAC_CSR_BLOCK_RST                              0x0000
#define   NFP_MAC_CSR_BLOCK_RST_MAC_HY1_STAT_RST             (1 << 23)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_HY1_STAT_RST_bf        0, 23, 23
#define     NFP_MAC_CSR_BLOCK_RST_MAC_HY1_STAT_RST_msk       (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_HY1_STAT_RST_bit       (23)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_HY0_STAT_RST             (1 << 22)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_HY0_STAT_RST_bf        0, 22, 22
#define     NFP_MAC_CSR_BLOCK_RST_MAC_HY0_STAT_RST_msk       (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_HY0_STAT_RST_bit       (22)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_MPB               (1 << 21)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_MPB_bf          0, 21, 21
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_MPB_msk         (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_MPB_bit         (21)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_MPB               (1 << 20)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_MPB_bf          0, 20, 20
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_MPB_msk         (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_MPB_bit         (20)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_CORE              (1 << 19)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_CORE_bf         0, 19, 19
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_CORE_msk        (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TX_RST_CORE_bit        (19)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_CORE              (1 << 18)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_CORE_bf         0, 18, 18
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_CORE_msk        (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RX_RST_CORE_bit        (18)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK1              (1 << 17)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK1_bf         0, 17, 17
#define     NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK1_msk        (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK1_bit        (17)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK0              (1 << 16)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK0_bf         0, 16, 16
#define     NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK0_msk        (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_FCX2RST_LK0_bit        (16)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK1                (1 << 15)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK1_bf           0, 15, 15
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK1_msk          (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK1_bit          (15)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK0                (1 << 14)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK0_bf           0, 14, 14
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK0_msk          (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RXRST_LK0_bit          (14)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK1                (1 << 13)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK1_bf           0, 13, 13
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK1_msk          (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK1_bit          (13)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK0                (1 << 12)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK0_bf           0, 12, 12
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK0_msk          (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_TXRST_LK0_bit          (12)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK1                  (1 << 11)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK1_bf             0, 11, 11
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK1_msk            (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK1_bit            (11)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK0                  (1 << 10)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK0_bf             0, 10, 10
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK0_msk            (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_RST_LK0_bit            (10)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK1              (1 << 9)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK1_bf         0, 9, 9
#define     NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK1_msk        (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK1_bit        (9)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK0              (1 << 8)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK0_bf         0, 8, 8
#define     NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK0_msk        (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_X2CLKEN_LK0_bit        (8)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK1            (1 << 7)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK1_bf       0, 7, 7
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK1_msk      (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK1_bit      (7)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK0            (1 << 6)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK0_bf       0, 6, 6
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK0_msk      (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_LK0_bit      (6)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY1            (1 << 5)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY1_bf       0, 5, 5
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY1_msk      (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY1_bit      (5)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY0            (1 << 4)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY0_bf       0, 4, 4
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY0_msk      (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_CORECLKEN_HY0_bit      (4)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_SERDES_RST               (1 << 2)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_SERDES_RST_bf          0, 2, 2
#define     NFP_MAC_CSR_BLOCK_RST_MAC_SERDES_RST_msk         (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_SERDES_RST_bit         (2)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_S_RST                    (1 << 1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_S_RST_bf               0, 1, 1
#define     NFP_MAC_CSR_BLOCK_RST_MAC_S_RST_msk              (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_S_RST_bit              (1)
#define   NFP_MAC_CSR_BLOCK_RST_MAC_H_RST                    (1 << 0)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_H_RST_bf               0, 0, 0
#define     NFP_MAC_CSR_BLOCK_RST_MAC_H_RST_msk              (0x1)
#define     NFP_MAC_CSR_BLOCK_RST_MAC_H_RST_bit              (0)


/*
 * Register: MacHydBlkReset
 *   [31:20]   MacHydRxSerDesIfRst
 *   [15:4]    MacHydTxSerDesIfRst
 *   [3]       MacHydRxFFRst
 *   [2]       MacHydTxFFRst
 *   [1]       MacHydRegRst
 *   [0]       MacHydRefRst
 *
 * Name(s):
 * <base>.MacHyd0BlkReset <base>.MacHyd1BlkReset
 */
#define NFP_MAC_CSR_HYD0_BLOCK_RST                         0x0004
#define NFP_MAC_CSR_HYD1_BLOCK_RST                         0x0008
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_SERDES(x)        (((x) & 0xfff) << 20)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_SERDES_of(x)     (((x) >> 20) & 0xfff)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_SERDES_bf      0, 31, 20
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_SERDES_msk     (0xfff)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_SERDES_shf     (20)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_SERDES(x)        (((x) & 0xfff) << 4)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_SERDES_of(x)     (((x) >> 4) & 0xfff)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_SERDES_bf      0, 15, 4
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_SERDES_msk     (0xfff)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_SERDES_shf     (4)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_FF               (1 << 3)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_FF_bf          0, 3, 3
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_FF_msk         (0x1)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_RX_FF_bit         (3)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_FF               (1 << 2)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_FF_bf          0, 2, 2
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_FF_msk         (0x1)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_TX_FF_bit         (2)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REG                 (1 << 1)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REG_bf            0, 1, 1
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REG_msk           (0x1)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REG_bit           (1)
#define   NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REF                 (1 << 0)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REF_bf            0, 0, 0
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REF_msk           (0x1)
#define     NFP_MAC_CSR_HYD0_BLOCK_RST_MAC_REF_bit           (0)


/*
 * Register: MacMuxCtrl
 *   [24]      LASelect
 *   [23:0]    MacSerDesIntlknSel
 *
 * Name(s):
 * <base>.MacMuxCtrl
 */
#define NFP_MAC_CSR_MUX_CTRL                               0x000c
#define   NFP_MAC_CSR_MUX_CTRL_ILA_SEL                       (1 << 24)
#define     NFP_MAC_CSR_MUX_CTRL_ILA_SEL_MAC                 (0)
#define     NFP_MAC_CSR_MUX_CTRL_ILA_SEL_ILA                 (0x1000000)
#define     NFP_MAC_CSR_MUX_CTRL_ILA_SEL_bf                  0, 24, 24
#define     NFP_MAC_CSR_MUX_CTRL_ILA_SEL_msk                 (0x1)
#define     NFP_MAC_CSR_MUX_CTRL_ILA_SEL_bit                 (24)
#define   NFP_MAC_CSR_MUX_CTRL_INLK_SEL(x)                   (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_MUX_CTRL_INLK_SEL_of(x)                (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_MUX_CTRL_INLK_SEL_bf                 0, 23, 0
#define     NFP_MAC_CSR_MUX_CTRL_INLK_SEL_msk                (0xffffff)
#define     NFP_MAC_CSR_MUX_CTRL_INLK_SEL_shf                (0)


/*
 * Register: MacSerDesEn
 *   [23:0]    SerDesEnable
 *
 * Name(s):
 * <base>.MacSerDesEn
 */
#define NFP_MAC_CSR_SERDES                                 0x0010
#define   NFP_MAC_CSR_SERDES_ENABLE(x)                       (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_SERDES_ENABLE_of(x)                    (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_SERDES_ENABLE_bf                     0, 23, 0
#define     NFP_MAC_CSR_SERDES_ENABLE_msk                    (0xffffff)
#define     NFP_MAC_CSR_SERDES_ENABLE_shf                    (0)


/*
 * Register: MacSysSupCtrl
 *   [31:24]   MacSysSupCtrlC
 *   [23:20]   MacMpbFreeBufFifoLowWm
 *   [19]      MacIgLnkLstFreezeOnErrN
 *   [18]      MacEgLnkLstFreezeOnErrN
 *   [17]      DwrrArbiterDisable
 *   [16]      DwrrWeightWrEnable
 *   [15]      MacIlkLiveIntSel
 *   [14]      Lk1IgDqSegmentedEn
 *   [13]      Lk0IgDqSegmentedEn
 *   [12]      Lk1LinklistEn
 *   [11]      Lk0LinklistEn
 *   [10]      Hy1LinklistEn
 *   [9]       Hy0LinklistEn
 *   [8]       SplitMemIG
 *   [7]       ExtraEthHistMode
 *   [6:4]     MacSysSupCtrlA
 *   [3]       TimeStampFrc
 *   [2]       TimeStampSet
 *   [1]       TimeStampRst
 *   [0]       TimeStampEn
 *
 * Name(s):
 * <base>.MacSysSupCtrl
 */
#define NFP_MAC_CSR_SYS_SUP_CTRL                           0x0014
#define   NFP_MAC_CSR_SYS_SUP_CTRL_CTRLC(x)                  (((x) & 0xff) << 24)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_CTRLC_of(x)               (((x) >> 24) & 0xff)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_CTRLC_bf                0, 31, 24
#define     NFP_MAC_CSR_SYS_SUP_CTRL_CTRLC_msk               (0xff)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_CTRLC_shf               (24)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_MPB_FREE_LOW_WM(x)        (((x) & 0xf) << 20)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_MPB_FREE_LOW_WM_of(x)     (((x) >> 20) & 0xf)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_MPB_FREE_LOW_WM_bf      0, 23, 20
#define     NFP_MAC_CSR_SYS_SUP_CTRL_MPB_FREE_LOW_WM_msk     (0xf)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_MPB_FREE_LOW_WM_shf     (20)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_IG_LL_FREEZE_ON_ERR       (1 << 19)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_IG_LL_FREEZE_ON_ERR_bf  0, 19, 19
#define     NFP_MAC_CSR_SYS_SUP_CTRL_IG_LL_FREEZE_ON_ERR_msk (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_IG_LL_FREEZE_ON_ERR_bit (19)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_EG_LL_FREEZE_ON_ERR       (1 << 18)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_EG_LL_FREEZE_ON_ERR_bf  0, 18, 18
#define     NFP_MAC_CSR_SYS_SUP_CTRL_EG_LL_FREEZE_ON_ERR_msk (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_EG_LL_FREEZE_ON_ERR_bit (18)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_ARBITER_DIS          (1 << 17)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_ARBITER_DIS_bf     0, 17, 17
#define     NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_ARBITER_DIS_msk    (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_ARBITER_DIS_bit    (17)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_WEIGHT_WR_EN         (1 << 16)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_WEIGHT_WR_EN_bf    0, 16, 16
#define     NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_WEIGHT_WR_EN_msk   (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_DWRR_WEIGHT_WR_EN_bit   (16)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_ILK_LIVE_INT_SEL          (1 << 15)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_ILK_LIVE_INT_SEL_bf     0, 15, 15
#define     NFP_MAC_CSR_SYS_SUP_CTRL_ILK_LIVE_INT_SEL_msk    (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_ILK_LIVE_INT_SEL_bit    (15)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_LK1_IG_DQ_SEG_EN          (1 << 14)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK1_IG_DQ_SEG_EN_bf     0, 14, 14
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK1_IG_DQ_SEG_EN_msk    (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK1_IG_DQ_SEG_EN_bit    (14)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_LK0_IG_DQ_SEG_EN          (1 << 13)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK0_IG_DQ_SEG_EN_bf     0, 13, 13
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK0_IG_DQ_SEG_EN_msk    (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK0_IG_DQ_SEG_EN_bit    (13)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_LK1_LL_EN                 (1 << 12)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK1_LL_EN_bf            0, 12, 12
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK1_LL_EN_msk           (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK1_LL_EN_bit           (12)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_LK0_LL_EN                 (1 << 11)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK0_LL_EN_bf            0, 11, 11
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK0_LL_EN_msk           (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_LK0_LL_EN_bit           (11)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_HY1_LL_EN                 (1 << 10)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_HY1_LL_EN_bf            0, 10, 10
#define     NFP_MAC_CSR_SYS_SUP_CTRL_HY1_LL_EN_msk           (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_HY1_LL_EN_bit           (10)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_HY0_LL_EN                 (1 << 9)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_HY0_LL_EN_bf            0, 9, 9
#define     NFP_MAC_CSR_SYS_SUP_CTRL_HY0_LL_EN_msk           (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_HY0_LL_EN_bit           (9)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_SPLIT_MEM_IG              (1 << 8)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_SPLIT_MEM_IG_bf         0, 8, 8
#define     NFP_MAC_CSR_SYS_SUP_CTRL_SPLIT_MEM_IG_msk        (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_SPLIT_MEM_IG_bit        (8)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_EXTRA_ETH_HIST            (1 << 7)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_EXTRA_ETH_HIST_bf       0, 7, 7
#define     NFP_MAC_CSR_SYS_SUP_CTRL_EXTRA_ETH_HIST_msk      (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_EXTRA_ETH_HIST_bit      (7)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_CTRLA(x)                  (((x) & 7) << 4)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_CTRLA_of(x)               (((x) >> 4) & 7)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_CTRLA_bf                0, 6, 4
#define     NFP_MAC_CSR_SYS_SUP_CTRL_CTRLA_msk               (0x7)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_CTRLA_shf               (4)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_FRC             (1 << 3)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_FRC_bf        0, 3, 3
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_FRC_msk       (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_FRC_bit       (3)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_SET             (1 << 2)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_SET_bf        0, 2, 2
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_SET_msk       (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_SET_bit       (2)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_RST             (1 << 1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_RST_bf        0, 1, 1
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_RST_msk       (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_RST_bit       (1)
#define   NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_EN              (1 << 0)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_EN_bf         0, 0, 0
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_EN_msk        (0x1)
#define     NFP_MAC_CSR_SYS_SUP_CTRL_TIMESTAMP_EN_bit        (0)


/*
 * Register: MacSysSupStat
 *
 * Name(s):
 * <base>.MacSysSupStat
 */
#define NFP_MAC_CSR_SYS_SUP_STAT                           0x0018

/*
 * Register: MacTimeStampNsec
 *
 * Name(s):
 * <base>.MacTimeStampNsec
 */
#define NFP_MAC_CSR_TS_NSEC                                0x001c

/*
 * Register: MacTimeStampSec
 *
 * Name(s):
 * <base>.MacTimeStampSec
 */
#define NFP_MAC_CSR_TS_SEC                                 0x0020

/*
 * Register: MacTimeStampIncr
 *   [19:16]   IncrNsec
 *   [15:0]    IncrFraction
 *
 * Name(s):
 * <base>.MacTimeStampIncr
 */
#define NFP_MAC_CSR_TS_INCR                                0x0024
#define   NFP_MAC_CSR_TS_INCR_NSEC(x)                        (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_TS_INCR_NSEC_of(x)                     (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_TS_INCR_NSEC_bf                      0, 19, 16
#define     NFP_MAC_CSR_TS_INCR_NSEC_msk                     (0xf)
#define     NFP_MAC_CSR_TS_INCR_NSEC_shf                     (16)
#define   NFP_MAC_CSR_TS_INCR_FRAC(x)                        (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_TS_INCR_FRAC_of(x)                     (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_TS_INCR_FRAC_bf                      0, 15, 0
#define     NFP_MAC_CSR_TS_INCR_FRAC_msk                     (0xffff)
#define     NFP_MAC_CSR_TS_INCR_FRAC_shf                     (0)


/*
 * Register: MacTimeStampSetNsec
 *
 * Name(s):
 * <base>.MacTimeStampSetNsec
 */
#define NFP_MAC_CSR_TS_SET_NSEC                            0x0028

/*
 * Register: MacTimeStampSetSec
 *
 * Name(s):
 * <base>.MacTimeStampSetSec
 */
#define NFP_MAC_CSR_TS_SET_SEC                             0x002c

/*
 * Register: MacTdmCycleWord3100
 *   [31:28]   MacTdmPortSlot7
 *   [27:24]   MacTdmPortSlot6
 *   [23:20]   MacTdmPortSlot5
 *   [19:16]   MacTdmPortSlot4
 *   [15:12]   MacTdmPortSlot3
 *   [11:8]    MacTdmPortSlot2
 *   [7:4]     MacTdmPortSlot1
 *   [3:0]     MacTdmPortSlot0
 *
 * Name(s):
 * <base>.MacTdm0CycleWord3100 <base>.MacTdm1CycleWord3100
 */
#define NFP_MAC_CSR_TDM0_CYCLE_WORD_3100                   0x0030
#define NFP_MAC_CSR_TDM1_CYCLE_WORD_3100                   0x0038
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT7(x)          (((x) & 0xf) << 28)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT7_of(x)       (((x) >> 28) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT7_bf        0, 31, 28
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT7_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT7_shf       (28)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT6(x)          (((x) & 0xf) << 24)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT6_of(x)       (((x) >> 24) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT6_bf        0, 27, 24
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT6_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT6_shf       (24)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT5(x)          (((x) & 0xf) << 20)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT5_of(x)       (((x) >> 20) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT5_bf        0, 23, 20
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT5_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT5_shf       (20)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT4(x)          (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT4_of(x)       (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT4_bf        0, 19, 16
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT4_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT4_shf       (16)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT3(x)          (((x) & 0xf) << 12)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT3_of(x)       (((x) >> 12) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT3_bf        0, 15, 12
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT3_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT3_shf       (12)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT2(x)          (((x) & 0xf) << 8)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT2_of(x)       (((x) >> 8) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT2_bf        0, 11, 8
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT2_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT2_shf       (8)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT1(x)          (((x) & 0xf) << 4)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT1_of(x)       (((x) >> 4) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT1_bf        0, 7, 4
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT1_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT1_shf       (4)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT0(x)          (((x) & 0xf) << 0)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT0_of(x)       (((x) >> 0) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT0_bf        0, 3, 0
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT0_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_3100_SLOT0_shf       (0)


/*
 * Register: MacTdmCycleWord4732
 *   [15:12]   MacTdmPortSlot11
 *   [11:8]    MacTdmPortSlot10
 *   [7:4]     MacTdmPortSlot9
 *   [3:0]     MacTdmPortSlot8
 *
 * Name(s):
 * <base>.MacTdm0CycleWord4732 <base>.MacTdm1CycleWord4732
 */
#define NFP_MAC_CSR_TDM0_CYCLE_WORD_4732                   0x0034
#define NFP_MAC_CSR_TDM1_CYCLE_WORD_4732                   0x003c
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT11(x)         (((x) & 0xf) << 12)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT11_of(x)      (((x) >> 12) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT11_bf       0, 15, 12
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT11_msk      (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT11_shf      (12)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT10(x)         (((x) & 0xf) << 8)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT10_of(x)      (((x) >> 8) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT10_bf       0, 11, 8
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT10_msk      (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT10_shf      (8)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT9(x)          (((x) & 0xf) << 4)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT9_of(x)       (((x) >> 4) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT9_bf        0, 7, 4
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT9_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT9_shf       (4)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT8(x)          (((x) & 0xf) << 0)
#define   NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT8_of(x)       (((x) >> 0) & 0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT8_bf        0, 3, 0
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT8_msk       (0xf)
#define     NFP_MAC_CSR_TDM0_CYCLE_WORD_4732_SLOT8_shf       (0)


/*
 * Register: MacTdmMode0900
 *   [29:27]   MacTdmModePort9
 *   [26:24]   MacTdmModePort8
 *   [23:21]   MacTdmModePort7
 *   [20:18]   MacTdmModePort6
 *   [17:15]   MacTdmModePort5
 *   [14:12]   MacTdmModePort4
 *   [11:9]    MacTdmModePort3
 *   [8:6]     MacTdmModePort2
 *   [5:3]     MacTdmModePort1
 *   [2:0]     MacTdmModePort0
 *
 * Name(s):
 * <base>.MacTdm0Mode0900 <base>.MacTdm1Mode0900
 */
#define NFP_MAC_CSR_TDM0_MODE_0900                         0x0040
#define NFP_MAC_CSR_TDM1_MODE_0900                         0x0048
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT9(x)                (((x) & 7) << 27)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT9_of(x)             (((x) >> 27) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_bf              0, 29, 27
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT9_shf             (27)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT8(x)                (((x) & 7) << 24)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT8_of(x)             (((x) >> 24) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_bf              0, 26, 24
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT8_shf             (24)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT7(x)                (((x) & 7) << 21)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT7_of(x)             (((x) >> 21) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_bf              0, 23, 21
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT7_shf             (21)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT6(x)                (((x) & 7) << 18)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT6_of(x)             (((x) >> 18) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_bf              0, 20, 18
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT6_shf             (18)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT5(x)                (((x) & 7) << 15)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT5_of(x)             (((x) >> 15) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_bf              0, 17, 15
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT5_shf             (15)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT4(x)                (((x) & 7) << 12)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT4_of(x)             (((x) >> 12) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_bf              0, 14, 12
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT4_shf             (12)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT3(x)                (((x) & 7) << 9)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT3_of(x)             (((x) >> 9) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_bf              0, 11, 9
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT3_shf             (9)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT2(x)                (((x) & 7) << 6)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT2_of(x)             (((x) >> 6) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_bf              0, 8, 6
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT2_shf             (6)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT1(x)                (((x) & 7) << 3)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT1_of(x)             (((x) >> 3) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_bf              0, 5, 3
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT1_shf             (3)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT0(x)                (((x) & 7) << 0)
#define   NFP_MAC_CSR_TDM0_MODE_0900_PORT0_of(x)             (((x) >> 0) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_OFF             (0)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_1G              (1)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_10G             (2)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_40G             (3)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_100G            (4)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_bf              0, 2, 0
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_msk             (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_0900_PORT0_shf             (0)


/*
 * Register: MacTdmMode1110Crc
 *   [27:16]   MacEgressPortCrcEn
 *   [5:3]     MacTdmModePort1
 *   [2:0]     MacTdmModePort0
 *
 * Name(s):
 * <base>.MacTdm0Mode1110CrcEn <base>.MacTdm1Mode1110CrcEn
 */
#define NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN                  0x0044
#define NFP_MAC_CSR_TDM1_MODE_1110_CRC_EN                  0x004c
#define   NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_MAC_EG_PORT(x)   (((x) & 0xfff) << 16)
#define   NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_MAC_EG_PORT_of(x) (((x) >> 16) & 0xfff)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_MAC_EG_PORT_bf 0, 27, 16
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_MAC_EG_PORT_msk (0xfff)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_MAC_EG_PORT_shf (16)
#define   NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1(x)         (((x) & 7) << 3)
#define   NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_of(x)      (((x) >> 3) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_OFF      (0)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_1G       (1)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_10G      (2)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_40G      (3)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_100G     (4)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_bf       0, 5, 3
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_msk      (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT1_shf      (3)
#define   NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0(x)         (((x) & 7) << 0)
#define   NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_of(x)      (((x) >> 0) & 7)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_OFF      (0)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_1G       (1)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_10G      (2)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_40G      (3)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_100G     (4)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_bf       0, 2, 0
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_msk      (0x7)
#define     NFP_MAC_CSR_TDM0_MODE_1110_CRC_EN_PORT0_shf      (0)


/*
 * Register: MacPortChanAssign
 *   [29:26]   PortNumOfChannels2
 *   [25:20]   PortBaseChan2
 *   [19:16]   PortNumOfChannels1
 *   [15:10]   PortBaseChan1
 *   [9:6]     PortNumOfChannels0
 *   [5:0]     PortBaseChan0
 *
 * Name(s):
 * <base>.MacPort2to0ChanAssign <base>.MacPort5to3ChanAssign
 * <base>.MacPort8to6ChanAssign <base>.MacPort11to9ChanAssign
 * <base>.MacPort14to12ChanAssign <base>.MacPort17to15ChanAssign
 * <base>.MacPort20to18ChanAssign <base>.MacPort23to21ChanAssign
 * <base>.MacEgPort2to0ChanAssign <base>.MacEgPort5to3ChanAssign
 * <base>.MacEgPort8to6ChanAssign <base>.MacEgPort11to9ChanAssign
 * <base>.MacEgPort14to12ChanAssign <base>.MacEgPort17to15ChanAssign
 * <base>.MacEgPort20to18ChanAssign <base>.MacEgPort23to21ChanAssign
 */
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_2_TO_0                0x0050
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_5_TO_3                0x0054
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_8_TO_6                0x0058
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_11_TO_9               0x005c
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_14_TO_12              0x0060
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_17_TO_15              0x0064
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_20_TO_18              0x0068
#define NFP_MAC_CSR_CHAN_ASSIGN_PORT_23_TO_21              0x006c
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_2_TO_0             0x0240
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_5_TO_3             0x0244
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_8_TO_6             0x0248
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_11_TO_9            0x024c
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_14_TO_12           0x0250
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_17_TO_15           0x0254
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_20_TO_18           0x0258
#define NFP_MAC_CSR_EG_CHAN_ASSIGN_PORT_23_TO_21           0x025c
#define   NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS2(x)              (((x) & 0xf) << 26)
#define   NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS2_of(x)           (((x) >> 26) & 0xf)
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS2_bf            0, 29, 26
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS2_msk           (0xf)
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS2_shf           (26)
#define   NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN2(x)              (((x) & 0x3f) << 20)
#define   NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN2_of(x)           (((x) >> 20) & 0x3f)
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN2_bf            0, 25, 20
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN2_msk           (0x3f)
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN2_shf           (20)
#define   NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS1(x)              (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS1_of(x)           (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS1_bf            0, 19, 16
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS1_msk           (0xf)
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS1_shf           (16)
#define   NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN1(x)              (((x) & 0x3f) << 10)
#define   NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN1_of(x)           (((x) >> 10) & 0x3f)
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN1_bf            0, 15, 10
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN1_msk           (0x3f)
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN1_shf           (10)
#define   NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS0(x)              (((x) & 0xf) << 6)
#define   NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS0_of(x)           (((x) >> 6) & 0xf)
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS0_bf            0, 9, 6
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS0_msk           (0xf)
#define     NFP_MAC_CSR_CHAN_ASSIGN_NUM_CHANS0_shf           (6)
#define   NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN0(x)              (((x) & 0x3f) << 0)
#define   NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN0_of(x)           (((x) >> 0) & 0x3f)
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN0_bf            0, 5, 0
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN0_msk           (0x3f)
#define     NFP_MAC_CSR_CHAN_ASSIGN_BASE_CHAN0_shf           (0)


/*
 * Register: MacPrePendCtl1
 *   [31:28]   EGSkipOctetsPort3
 *   [27:24]   IGSkipOctetsPort3
 *   [23:20]   EGSkipOctetsPort2
 *   [19:16]   IGSkipOctetsPort2
 *   [15:12]   EGSkipOctetsPort1
 *   [11:8]    IGSkipOctetsPort1
 *   [7:4]     EGSkipOctetsPort0
 *   [3:0]     IGSkipOctetsPort0
 *
 * Name(s):
 * <base>.MacPrePendCtl03to00 <base>.MacPrePendCtl15to12
 */
#define NFP_MAC_CSR_PREPEND_CTRL_03_TO_00                  0x0070
#define NFP_MAC_CSR_PREPEND_CTRL_15_TO_12                  0x007c
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT3(x)     (((x) & 0xf) << 28)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT3_of(x)  (((x) >> 28) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT3_bf   0, 31, 28
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT3_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT3_shf  (28)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT3(x)     (((x) & 0xf) << 24)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT3_of(x)  (((x) >> 24) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT3_bf   0, 27, 24
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT3_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT3_shf  (24)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT2(x)     (((x) & 0xf) << 20)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT2_of(x)  (((x) >> 20) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT2_bf   0, 23, 20
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT2_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT2_shf  (20)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT2(x)     (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT2_of(x)  (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT2_bf   0, 19, 16
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT2_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT2_shf  (16)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT1(x)     (((x) & 0xf) << 12)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT1_of(x)  (((x) >> 12) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT1_bf   0, 15, 12
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT1_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT1_shf  (12)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT1(x)     (((x) & 0xf) << 8)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT1_of(x)  (((x) >> 8) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT1_bf   0, 11, 8
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT1_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT1_shf  (8)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT0(x)     (((x) & 0xf) << 4)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT0_of(x)  (((x) >> 4) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT0_bf   0, 7, 4
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT0_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT0_shf  (4)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT0(x)     (((x) & 0xf) << 0)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT0_of(x)  (((x) >> 0) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT0_bf   0, 3, 0
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT0_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT0_shf  (0)


/*
 * Register: MacPrePendCtl2
 *   [31:28]   EGSkipOctetsPort7
 *   [27:24]   IGSkipOctetsPort7
 *   [23:20]   EGSkipOctetsPort6
 *   [19:16]   IGSkipOctetsPort6
 *   [15:12]   EGSkipOctetsPort5
 *   [11:8]    IGSkipOctetsPort5
 *   [7:4]     EGSkipOctetsPort4
 *   [3:0]     IGSkipOctetsPort4
 *
 * Name(s):
 * <base>.MacPrePendCtl07to04 <base>.MacPrePendCtl19to16
 */
#define NFP_MAC_CSR_PREPEND_CTRL_07_TO_04                  0x0074
#define NFP_MAC_CSR_PREPEND_CTRL_19_TO_16                  0x0080
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT7(x)     (((x) & 0xf) << 28)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT7_of(x)  (((x) >> 28) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT7_bf   0, 31, 28
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT7_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT7_shf  (28)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT7(x)     (((x) & 0xf) << 24)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT7_of(x)  (((x) >> 24) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT7_bf   0, 27, 24
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT7_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT7_shf  (24)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT6(x)     (((x) & 0xf) << 20)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT6_of(x)  (((x) >> 20) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT6_bf   0, 23, 20
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT6_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT6_shf  (20)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT6(x)     (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT6_of(x)  (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT6_bf   0, 19, 16
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT6_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT6_shf  (16)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT5(x)     (((x) & 0xf) << 12)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT5_of(x)  (((x) >> 12) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT5_bf   0, 15, 12
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT5_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT5_shf  (12)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT5(x)     (((x) & 0xf) << 8)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT5_of(x)  (((x) >> 8) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT5_bf   0, 11, 8
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT5_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT5_shf  (8)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT4(x)     (((x) & 0xf) << 4)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT4_of(x)  (((x) >> 4) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT4_bf   0, 7, 4
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT4_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT4_shf  (4)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT4(x)     (((x) & 0xf) << 0)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT4_of(x)  (((x) >> 0) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT4_bf   0, 3, 0
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT4_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT4_shf  (0)


/*
 * Register: MacPrePendCtl3
 *   [31:28]   EGSkipOctetsPort11
 *   [27:24]   IGSkipOctetsPort11
 *   [23:20]   EGSkipOctetsPort10
 *   [19:16]   IGSkipOctetsPort10
 *   [15:12]   EGSkipOctetsPort9
 *   [11:8]    IGSkipOctetsPort9
 *   [7:4]     EGSkipOctetsPort8
 *   [3:0]     IGSkipOctetsPort8
 *
 * Name(s):
 * <base>.MacPrePendCtl11to08 <base>.MacPrePendCtl23to20
 */
#define NFP_MAC_CSR_PREPEND_CTRL_11_TO_08                  0x0078
#define NFP_MAC_CSR_PREPEND_CTRL_23_TO_20                  0x0084
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT11(x)    (((x) & 0xf) << 28)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT11_of(x) (((x) >> 28) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT11_bf  0, 31, 28
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT11_msk (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT11_shf (28)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT11(x)    (((x) & 0xf) << 24)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT11_of(x) (((x) >> 24) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT11_bf  0, 27, 24
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT11_msk (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT11_shf (24)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT10(x)    (((x) & 0xf) << 20)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT10_of(x) (((x) >> 20) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT10_bf  0, 23, 20
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT10_msk (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT10_shf (20)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT10(x)    (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT10_of(x) (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT10_bf  0, 19, 16
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT10_msk (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT10_shf (16)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT9(x)     (((x) & 0xf) << 12)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT9_of(x)  (((x) >> 12) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT9_bf   0, 15, 12
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT9_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT9_shf  (12)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT9(x)     (((x) & 0xf) << 8)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT9_of(x)  (((x) >> 8) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT9_bf   0, 11, 8
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT9_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT9_shf  (8)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT8(x)     (((x) & 0xf) << 4)
#define   NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT8_of(x)  (((x) >> 4) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT8_bf   0, 7, 4
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT8_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_EG_SKIP_OCTS_PORT8_shf  (4)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT8(x)     (((x) & 0xf) << 0)
#define   NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT8_of(x)  (((x) >> 0) & 0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT8_bf   0, 3, 0
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT8_msk  (0xf)
#define     NFP_MAC_CSR_PREPEND_CTRL_IG_SKIP_OCTS_PORT8_shf  (0)


/*
 * Register: MacPrePendDsaCtl1
 *   [31:30]   DsaTagModePort15
 *   [29:28]   DsaTagModePort14
 *   [27:26]   DsaTagModePort13
 *   [25:24]   DsaTagModePort12
 *   [23:22]   DsaTagModePort11
 *   [21:20]   DsaTagModePort10
 *   [19:18]   DsaTagModePort9
 *   [17:16]   DsaTagModePort8
 *   [15:14]   DsaTagModePort7
 *   [13:12]   DsaTagModePort6
 *   [11:10]   DsaTagModePort5
 *   [9:8]     DsaTagModePort4
 *   [7:6]     DsaTagModePort3
 *   [5:4]     DsaTagModePort2
 *   [3:2]     DsaTagModePort1
 *   [1:0]     DsaTagModePort0
 *
 * Name(s):
 * <base>.MacPrePendDsaCtl15to00 <base>.MacEgPrePendDsaCtl15to00
 */
#define NFP_MAC_CSR_DSA_CTRL_15_TO_00                      0x0088
#define NFP_MAC_CSR_EG_DSA_CTRL_15_TO_00                   0x01cc
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT15(x)                 (((x) & 3) << 30)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT15_of(x)              (((x) >> 30) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT15_bf               0, 31, 30
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT15_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT15_shf              (30)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT14(x)                 (((x) & 3) << 28)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT14_of(x)              (((x) >> 28) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT14_bf               0, 29, 28
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT14_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT14_shf              (28)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT13(x)                 (((x) & 3) << 26)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT13_of(x)              (((x) >> 26) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT13_bf               0, 27, 26
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT13_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT13_shf              (26)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT12(x)                 (((x) & 3) << 24)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT12_of(x)              (((x) >> 24) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT12_bf               0, 25, 24
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT12_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT12_shf              (24)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT11(x)                 (((x) & 3) << 22)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT11_of(x)              (((x) >> 22) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT11_bf               0, 23, 22
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT11_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT11_shf              (22)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT10(x)                 (((x) & 3) << 20)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT10_of(x)              (((x) >> 20) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT10_bf               0, 21, 20
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT10_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT10_shf              (20)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT9(x)                  (((x) & 3) << 18)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT9_of(x)               (((x) >> 18) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT9_bf                0, 19, 18
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT9_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT9_shf               (18)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT8(x)                  (((x) & 3) << 16)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT8_of(x)               (((x) >> 16) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT8_bf                0, 17, 16
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT8_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT8_shf               (16)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT7(x)                  (((x) & 3) << 14)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT7_of(x)               (((x) >> 14) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT7_bf                0, 15, 14
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT7_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT7_shf               (14)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT6(x)                  (((x) & 3) << 12)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT6_of(x)               (((x) >> 12) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT6_bf                0, 13, 12
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT6_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT6_shf               (12)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT5(x)                  (((x) & 3) << 10)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT5_of(x)               (((x) >> 10) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT5_bf                0, 11, 10
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT5_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT5_shf               (10)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT4(x)                  (((x) & 3) << 8)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT4_of(x)               (((x) >> 8) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT4_bf                0, 9, 8
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT4_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT4_shf               (8)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT3(x)                  (((x) & 3) << 6)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT3_of(x)               (((x) >> 6) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT3_bf                0, 7, 6
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT3_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT3_shf               (6)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT2(x)                  (((x) & 3) << 4)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT2_of(x)               (((x) >> 4) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT2_bf                0, 5, 4
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT2_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT2_shf               (4)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT1(x)                  (((x) & 3) << 2)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT1_of(x)               (((x) >> 2) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT1_bf                0, 3, 2
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT1_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT1_shf               (2)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT0(x)                  (((x) & 3) << 0)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT0_of(x)               (((x) >> 0) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_DISABLED          (0)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_4B                (1)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_8B                (2)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_INV               (3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_bf                0, 1, 0
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_msk               (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT0_shf               (0)


/*
 * Register: MacPrePendDsaCtl2
 *   [19:18]   DsaTagModeLkCore1
 *   [17:16]   DsaTagModeLkCore0
 *   [15:14]   DsaTagModePort23
 *   [13:12]   DsaTagModePort22
 *   [11:10]   DsaTagModePort21
 *   [9:8]     DsaTagModePort20
 *   [7:6]     DsaTagModePort19
 *   [5:4]     DsaTagModePort18
 *   [3:2]     DsaTagModePort17
 *   [1:0]     DsaTagModePort16
 *
 * Name(s):
 * <base>.MacPrePendDsaCtlLkand23to16 <base>.MacEgPrePendDsaCtlLkand23to16
 */
#define NFP_MAC_CSR_DSA_CTRL_23_TO_16                      0x008c
#define NFP_MAC_CSR_EG_DSA_CTRL_23_TO_16                   0x01d0
#define   NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_1(x)              (((x) & 3) << 18)
#define   NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_1_of(x)           (((x) >> 18) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_1_bf            0, 19, 18
#define     NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_1_msk           (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_1_shf           (18)
#define   NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_0(x)              (((x) & 3) << 16)
#define   NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_0_of(x)           (((x) >> 16) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_0_bf            0, 17, 16
#define     NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_0_msk           (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_LK_CORE_0_shf           (16)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT23(x)                 (((x) & 3) << 14)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT23_of(x)              (((x) >> 14) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT23_bf               0, 15, 14
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT23_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT23_shf              (14)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT22(x)                 (((x) & 3) << 12)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT22_of(x)              (((x) >> 12) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT22_bf               0, 13, 12
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT22_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT22_shf              (12)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT21(x)                 (((x) & 3) << 10)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT21_of(x)              (((x) >> 10) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT21_bf               0, 11, 10
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT21_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT21_shf              (10)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT20(x)                 (((x) & 3) << 8)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT20_of(x)              (((x) >> 8) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT20_bf               0, 9, 8
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT20_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT20_shf              (8)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT19(x)                 (((x) & 3) << 6)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT19_of(x)              (((x) >> 6) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT19_bf               0, 7, 6
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT19_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT19_shf              (6)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT18(x)                 (((x) & 3) << 4)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT18_of(x)              (((x) >> 4) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT18_bf               0, 5, 4
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT18_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT18_shf              (4)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT17(x)                 (((x) & 3) << 2)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT17_of(x)              (((x) >> 2) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT17_bf               0, 3, 2
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT17_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT17_shf              (2)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT16(x)                 (((x) & 3) << 0)
#define   NFP_MAC_CSR_DSA_TAG_MODE_PORT16_of(x)              (((x) >> 0) & 3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_DISABLED         (0)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_4B               (1)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_8B               (2)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_INV              (3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_bf               0, 1, 0
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_msk              (0x3)
#define     NFP_MAC_CSR_DSA_TAG_MODE_PORT16_shf              (0)


/*
 * Register: MacInterlakenCtl1
 *   [31:29]   LkBurstMaxCore1
 *   [28:22]   LkNumChannelsUpper64
 *   [21:16]   LkBaseChannelUpper64
 *   [15:13]   LkBurstMaxCore0
 *   [12:6]    LkNumChannelsLower64
 *   [5:0]     LkBaseChannelLower64
 *
 * Name(s):
 * <base>.MacInterlakenCtl1
 */
#define NFP_MAC_CSR_ILA_CTRL_1                             0x0090
#define   NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1(x)          (((x) & 7) << 29)
#define   NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_of(x)       (((x) >> 29) & 7)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_64B       (0)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_128B      (1)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_196B      (2)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_256B      (3)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_320B      (4)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_384B      (5)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_448B      (6)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_512B      (7)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_bf        0, 31, 29
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_msk       (0x7)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_1_shf       (29)
#define   NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_UPPER_64(x)       (((x) & 0x7f) << 22)
#define   NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_UPPER_64_of(x)    (((x) >> 22) & 0x7f)
#define     NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_UPPER_64_bf     0, 28, 22
#define     NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_UPPER_64_msk    (0x7f)
#define     NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_UPPER_64_shf    (22)
#define   NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_UPPER_64(x)       (((x) & 0x3f) << 16)
#define   NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_UPPER_64_of(x)    (((x) >> 16) & 0x3f)
#define     NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_UPPER_64_bf     0, 21, 16
#define     NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_UPPER_64_msk    (0x3f)
#define     NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_UPPER_64_shf    (16)
#define   NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0(x)          (((x) & 7) << 13)
#define   NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_of(x)       (((x) >> 13) & 7)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_64B       (0)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_128B      (1)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_196B      (2)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_256B      (3)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_320B      (4)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_384B      (5)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_448B      (6)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_512B      (7)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_bf        0, 15, 13
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_msk       (0x7)
#define     NFP_MAC_CSR_ILA_CTRL_1_BURSTMAX_CORE_0_shf       (13)
#define   NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_LOWER_64(x)       (((x) & 0x7f) << 6)
#define   NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_LOWER_64_of(x)    (((x) >> 6) & 0x7f)
#define     NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_LOWER_64_bf     0, 12, 6
#define     NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_LOWER_64_msk    (0x7f)
#define     NFP_MAC_CSR_ILA_CTRL_1_NUM_CHANS_LOWER_64_shf    (6)
#define   NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_LOWER_64(x)       (((x) & 0x3f) << 0)
#define   NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_LOWER_64_of(x)    (((x) >> 0) & 0x3f)
#define     NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_LOWER_64_bf     0, 5, 0
#define     NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_LOWER_64_msk    (0x3f)
#define     NFP_MAC_CSR_ILA_CTRL_1_BASE_CHAN_LOWER_64_shf    (0)


/*
 * Register: MacInterlakenCtl2
 *   [22]      LkNbiChanSwapEn1
 *   [21]      IgOobFcSelCore1
 *   [20]      EgOobFcSelCore1
 *   [16]      EgAtomicLkCore1
 *   [6]       LkNbiChanSwapEn0
 *   [5]       IgOobFcSelCore0
 *   [4]       EgOobFcSelCore0
 *   [3:1]     EgTdmModeLkCore0
 *   [0]       EgAtomicLkCore0
 *
 * Name(s):
 * <base>.MacInterlakenCtl2
 */
#define NFP_MAC_CSR_ILA_CTRL_2                             0x0094
#define   NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_1       (1 << 22)
#define     NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_1_bf  0, 22, 22
#define     NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_1_msk (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_1_bit (22)
#define   NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_1        (1 << 21)
#define     NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_1_bf   0, 21, 21
#define     NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_1_msk  (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_1_bit  (21)
#define   NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_1        (1 << 20)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_1_bf   0, 20, 20
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_1_msk  (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_1_bit  (20)
#define   NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_1         (1 << 16)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_1_bf    0, 16, 16
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_1_msk   (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_1_bit   (16)
#define   NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_0       (1 << 6)
#define     NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_0_bf  0, 6, 6
#define     NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_0_msk (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_LK_NBI_CHAN_SWAP_EN_0_bit (6)
#define   NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_0        (1 << 5)
#define     NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_0_bf   0, 5, 5
#define     NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_0_msk  (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_IG_OOB_FC_SEL_CORE_0_bit  (5)
#define   NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_0        (1 << 4)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_0_bf   0, 4, 4
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_0_msk  (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_OOB_FC_SEL_CORE_0_bit  (4)
#define   NFP_MAC_CSR_ILA_CTRL_2_EG_TDM_MODE_LK_CORE_0(x)    (((x) & 7) << 1)
#define   NFP_MAC_CSR_ILA_CTRL_2_EG_TDM_MODE_LK_CORE_0_of(x) (((x) >> 1) & 7)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_TDM_MODE_LK_CORE_0_bf  0, 3, 1
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_TDM_MODE_LK_CORE_0_msk (0x7)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_TDM_MODE_LK_CORE_0_shf (1)
#define   NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_0         (1 << 0)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_0_bf    0, 0, 0
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_0_msk   (0x1)
#define     NFP_MAC_CSR_ILA_CTRL_2_EG_ATOMIC_LK_CORE_0_bit   (0)


/*
 * Register: EgBufferCreditPoolCount
 *   [31]      EgBufferLinklistReady
 *   [29:16]   EgBufferCreditCount1
 *   [13:0]    EgBufferCreditCount
 *
 * Name(s):
 * <base>.EgBufferCreditPoolCount
 */
#define NFP_MAC_CSR_EG_BUF_CREDIT_POOL                     0x0098
#define   NFP_MAC_CSR_EG_BUF_CREDIT_POOL_LL_READY            (1 << 31)
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_LL_READY_bf       0, 31, 31
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_LL_READY_msk      (0x1)
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_LL_READY_bit      (31)
#define   NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT1(x)           (((x) & 0x3fff) << 16)
#define   NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT1_of(x)        (((x) >> 16) & 0x3fff)
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT1_bf         0, 29, 16
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT1_msk        (0x3fff)
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT1_shf        (16)
#define   NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT(x)            (((x) & 0x3fff) << 0)
#define   NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT_of(x)         (((x) >> 0) & 0x3fff)
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT_bf          0, 13, 0
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT_msk         (0x3fff)
#define     NFP_MAC_CSR_EG_BUF_CREDIT_POOL_COUNT_shf         (0)


/*
 * Register: TxMpbCreditInit
 *   [19:16]   TxMpbCreditDataInit
 *   [11:6]    TxMpbCreditMaxPktInit
 *   [5:0]     TxMpbCreditPktInit
 *
 * Name(s):
 * <base>.TxMpbCreditInit
 */
#define NFP_MAC_CSR_TX_MPB_CREDIT                          0x009c
#define   NFP_MAC_CSR_TX_MPB_CREDIT_DATA_INIT(x)             (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_TX_MPB_CREDIT_DATA_INIT_of(x)          (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_TX_MPB_CREDIT_DATA_INIT_bf           0, 19, 16
#define     NFP_MAC_CSR_TX_MPB_CREDIT_DATA_INIT_msk          (0xf)
#define     NFP_MAC_CSR_TX_MPB_CREDIT_DATA_INIT_shf          (16)
#define   NFP_MAC_CSR_TX_MPB_CREDIT_MAX_PKT_INIT(x)          (((x) & 0x3f) << 6)
#define   NFP_MAC_CSR_TX_MPB_CREDIT_MAX_PKT_INIT_of(x)       (((x) >> 6) & 0x3f)
#define     NFP_MAC_CSR_TX_MPB_CREDIT_MAX_PKT_INIT_bf        0, 11, 6
#define     NFP_MAC_CSR_TX_MPB_CREDIT_MAX_PKT_INIT_msk       (0x3f)
#define     NFP_MAC_CSR_TX_MPB_CREDIT_MAX_PKT_INIT_shf       (6)
#define   NFP_MAC_CSR_TX_MPB_CREDIT_PKT_INIT(x)              (((x) & 0x3f) << 0)
#define   NFP_MAC_CSR_TX_MPB_CREDIT_PKT_INIT_of(x)           (((x) >> 0) & 0x3f)
#define     NFP_MAC_CSR_TX_MPB_CREDIT_PKT_INIT_bf            0, 5, 0
#define     NFP_MAC_CSR_TX_MPB_CREDIT_PKT_INIT_msk           (0x3f)
#define     NFP_MAC_CSR_TX_MPB_CREDIT_PKT_INIT_shf           (0)


/*
 * Register: IgBufferCreditPoolCount
 *   [31]      IgBufferLinklistReady
 *   [29:16]   IgBufferCreditCount1
 *   [13:0]    IgBufferCreditCount
 *
 * Name(s):
 * <base>.IgBufferCreditPoolCount
 */
#define NFP_MAC_CSR_IG_BUF_CREDIT_POOL                     0x00a0
#define   NFP_MAC_CSR_IG_BUF_CREDIT_POOL_LL_READY            (1 << 31)
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_LL_READY_bf       0, 31, 31
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_LL_READY_msk      (0x1)
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_LL_READY_bit      (31)
#define   NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT1(x)           (((x) & 0x3fff) << 16)
#define   NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT1_of(x)        (((x) >> 16) & 0x3fff)
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT1_bf         0, 29, 16
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT1_msk        (0x3fff)
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT1_shf        (16)
#define   NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT(x)            (((x) & 0x3fff) << 0)
#define   NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT_of(x)         (((x) >> 0) & 0x3fff)
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT_bf          0, 13, 0
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT_msk         (0x3fff)
#define     NFP_MAC_CSR_IG_BUF_CREDIT_POOL_COUNT_shf         (0)


/*
 * Register: RxMpbCreditInit
 *   [29:16]   RxMpbCreditDataInit
 *   [13:0]    RxMpbCreditBufInit
 *
 * Name(s):
 * <base>.RxMpbCreditInit
 */
#define NFP_MAC_CSR_RX_MPB_CREDIT                          0x00a4
#define   NFP_MAC_CSR_RX_MPB_CREDIT_DATA_INIT(x)             (((x) & 0x3fff) << 16)
#define   NFP_MAC_CSR_RX_MPB_CREDIT_DATA_INIT_of(x)          (((x) >> 16) & 0x3fff)
#define     NFP_MAC_CSR_RX_MPB_CREDIT_DATA_INIT_bf           0, 29, 16
#define     NFP_MAC_CSR_RX_MPB_CREDIT_DATA_INIT_msk          (0x3fff)
#define     NFP_MAC_CSR_RX_MPB_CREDIT_DATA_INIT_shf          (16)
#define   NFP_MAC_CSR_RX_MPB_CREDIT_BUF_INIT(x)              (((x) & 0x3fff) << 0)
#define   NFP_MAC_CSR_RX_MPB_CREDIT_BUF_INIT_of(x)           (((x) >> 0) & 0x3fff)
#define     NFP_MAC_CSR_RX_MPB_CREDIT_BUF_INIT_bf            0, 13, 0
#define     NFP_MAC_CSR_RX_MPB_CREDIT_BUF_INIT_msk           (0x3fff)
#define     NFP_MAC_CSR_RX_MPB_CREDIT_BUF_INIT_shf           (0)


/*
 * Register: TDMRateCreditInit
 *   [31:24]   TDM100GECreditInit
 *   [23:16]   TDM40GECreditInit
 *   [15:8]    TDM10GECreditInit
 *   [7:0]     TDM1GECreditInit
 *
 * Name(s):
 * <base>.MacTdmRateCreditInit
 */
#define NFP_MAC_CSR_TDM_RATE_CREDIT_INIT                   0x00a8
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_100G(x)           (((x) & 0xff) << 24)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_100G_of(x)        (((x) >> 24) & 0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_100G_bf         0, 31, 24
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_100G_msk        (0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_100G_shf        (24)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_40G(x)            (((x) & 0xff) << 16)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_40G_of(x)         (((x) >> 16) & 0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_40G_bf          0, 23, 16
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_40G_msk         (0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_40G_shf         (16)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_10G(x)            (((x) & 0xff) << 8)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_10G_of(x)         (((x) >> 8) & 0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_10G_bf          0, 15, 8
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_10G_msk         (0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_10G_shf         (8)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_1G(x)             (((x) & 0xff) << 0)
#define   NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_1G_of(x)          (((x) >> 0) & 0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_1G_bf           0, 7, 0
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_1G_msk          (0xff)
#define     NFP_MAC_CSR_TDM_RATE_CREDIT_INIT_1G_shf          (0)


/*
 * Register: MacInterruptErrStatus0
 *
 * Name(s):
 * <base>.MacInterruptErrStatus0
 */
#define NFP_MAC_CSR_INTR_ERR_STS_0                         0x00ac

/*
 * Register: MacInterruptErrStatus1
 *   [23:12]   MacLinTrainingInthy0
 *   [11:0]    MacLinTrainingInthy1
 *
 * Name(s):
 * <base>.MacInterruptErrStatus1
 */
#define NFP_MAC_CSR_INTR_ERR_STS_1                         0x00b0
#define   NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY0(x)       (((x) & 0xfff) << 12)
#define   NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY0_of(x)    (((x) >> 12) & 0xfff)
#define     NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY0_bf     0, 23, 12
#define     NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY0_msk    (0xfff)
#define     NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY0_shf    (12)
#define   NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY1(x)       (((x) & 0xfff) << 0)
#define   NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY1_of(x)    (((x) >> 0) & 0xfff)
#define     NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY1_bf     0, 11, 0
#define     NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY1_msk    (0xfff)
#define     NFP_MAC_CSR_INTR_ERR_STS_1_LINK_TRAIN_HY1_shf    (0)


/*
 * Register: MacInterruptErrEn0
 *
 * Name(s):
 * <base>.MacInterruptErrEn0
 */
#define NFP_MAC_CSR_INTR_ERR_EN_0                          0x00b4

/*
 * Register: MacInterruptErrEn1
 *
 * Name(s):
 * <base>.MacInterruptErrEn1
 */
#define NFP_MAC_CSR_INTR_ERR_EN_1                          0x00b8

/*
 * Register: MacLiveStatus0
 *
 * Name(s):
 * <base>.MacLiveStatus0
 */
#define NFP_MAC_CSR_LIVE_STATUS_0                          0x00bc

/*
 * Register: MacLiveStatus1
 *   [31:0]    MacLiveStatus1
 *
 * Name(s):
 * <base>.MacLiveStatus1
 */
#define NFP_MAC_CSR_LIVE_STATUS_1                          0x00c0
#define   NFP_MAC_CSR_LIVE_STATUS_1_MAC_LIVE_STS_1(x)        (((x) & 0xffffffff) << 0)
#define   NFP_MAC_CSR_LIVE_STATUS_1_MAC_LIVE_STS_1_of(x)     (((x) >> 0) & 0xffffffff)
#define     NFP_MAC_CSR_LIVE_STATUS_1_MAC_LIVE_STS_1_bf      0, 31, 0
#define     NFP_MAC_CSR_LIVE_STATUS_1_MAC_LIVE_STS_1_msk     (0xffffffff)
#define     NFP_MAC_CSR_LIVE_STATUS_1_MAC_LIVE_STS_1_shf     (0)


/*
 * Register: MacChanRdAddr
 *   [22:16]   IgChanRdAddr
 *   [6:0]     EgChanRdAddr
 *
 * Name(s):
 * <base>.MacChanRdAddr
 */
#define NFP_MAC_CSR_CHAN_RD_ADDR                           0x00c4
#define   NFP_MAC_CSR_CHAN_RD_ADDR_IG_CHAN(x)                (((x) & 0x7f) << 16)
#define   NFP_MAC_CSR_CHAN_RD_ADDR_IG_CHAN_of(x)             (((x) >> 16) & 0x7f)
#define     NFP_MAC_CSR_CHAN_RD_ADDR_IG_CHAN_bf              0, 22, 16
#define     NFP_MAC_CSR_CHAN_RD_ADDR_IG_CHAN_msk             (0x7f)
#define     NFP_MAC_CSR_CHAN_RD_ADDR_IG_CHAN_shf             (16)
#define   NFP_MAC_CSR_CHAN_RD_ADDR_EG_CHAN(x)                (((x) & 0x7f) << 0)
#define   NFP_MAC_CSR_CHAN_RD_ADDR_EG_CHAN_of(x)             (((x) >> 0) & 0x7f)
#define     NFP_MAC_CSR_CHAN_RD_ADDR_EG_CHAN_bf              0, 6, 0
#define     NFP_MAC_CSR_CHAN_RD_ADDR_EG_CHAN_msk             (0x7f)
#define     NFP_MAC_CSR_CHAN_RD_ADDR_EG_CHAN_shf             (0)


/*
 * Register: MacChanBufCount
 *   [26:16]   IgChanRdBufCnt
 *   [10:0]    EgChanRdBufCnt
 *
 * Name(s):
 * <base>.MacChanBufCount
 */
#define NFP_MAC_CSR_CHAN_BUF_COUNT                         0x00c8
#define   NFP_MAC_CSR_CHAN_BUF_COUNT_IG_CHAN(x)              (((x) & 0x7ff) << 16)
#define   NFP_MAC_CSR_CHAN_BUF_COUNT_IG_CHAN_of(x)           (((x) >> 16) & 0x7ff)
#define     NFP_MAC_CSR_CHAN_BUF_COUNT_IG_CHAN_bf            0, 26, 16
#define     NFP_MAC_CSR_CHAN_BUF_COUNT_IG_CHAN_msk           (0x7ff)
#define     NFP_MAC_CSR_CHAN_BUF_COUNT_IG_CHAN_shf           (16)
#define   NFP_MAC_CSR_CHAN_BUF_COUNT_EG_CHAN(x)              (((x) & 0x7ff) << 0)
#define   NFP_MAC_CSR_CHAN_BUF_COUNT_EG_CHAN_of(x)           (((x) >> 0) & 0x7ff)
#define     NFP_MAC_CSR_CHAN_BUF_COUNT_EG_CHAN_bf            0, 10, 0
#define     NFP_MAC_CSR_CHAN_BUF_COUNT_EG_CHAN_msk           (0x7ff)
#define     NFP_MAC_CSR_CHAN_BUF_COUNT_EG_CHAN_shf           (0)


/*
 * Register: PauseWaterMark
 *   [27:16]   PauseWaterMark1
 *   [11:0]    PauseWaterMark0
 *
 * Name(s):
 * <base>.PauseWaterMark0...
 */
#define NFP_MAC_CSR_PAUSE_WATERMARK(x)                     (0x00cc + ((x) * 0x4))
#define   NFP_MAC_CSR_PAUSE_WATERMARK_WM1(x)                 (((x) & 0xfff) << 16)
#define   NFP_MAC_CSR_PAUSE_WATERMARK_WM1_of(x)              (((x) >> 16) & 0xfff)
#define     NFP_MAC_CSR_PAUSE_WATERMARK_WM1_bf               0, 27, 16
#define     NFP_MAC_CSR_PAUSE_WATERMARK_WM1_msk              (0xfff)
#define     NFP_MAC_CSR_PAUSE_WATERMARK_WM1_shf              (16)
#define   NFP_MAC_CSR_PAUSE_WATERMARK_WM0(x)                 (((x) & 0xfff) << 0)
#define   NFP_MAC_CSR_PAUSE_WATERMARK_WM0_of(x)              (((x) >> 0) & 0xfff)
#define     NFP_MAC_CSR_PAUSE_WATERMARK_WM0_bf               0, 11, 0
#define     NFP_MAC_CSR_PAUSE_WATERMARK_WM0_msk              (0xfff)
#define     NFP_MAC_CSR_PAUSE_WATERMARK_WM0_shf              (0)


/*
 * Register: BufferCounterRw
 *   [31:24]   CounterAddr
 *   [21]      CounterRdBusy
 *
 * Name(s):
 * <base>.IgChanUsedBufferCreditsRw
 */
#define NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW            0x01d4
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_ADDR(x)    (((x) & 0xff) << 24)
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_ADDR_of(x) (((x) >> 24) & 0xff)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_ADDR_bf  0, 31, 24
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_ADDR_msk (0xff)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_ADDR_shf (24)
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_RD_BUSY    (1 << 21)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_RD_BUSY_bf 0, 21, 21
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_RD_BUSY_msk (0x1)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RW_RD_BUSY_bit (21)


/*
 * Register: BufferCounterRdData
 *   [31:24]   CounterAddr
 *   [21]      BufferCounterRdDataValid
 *   [15:0]    CounterRdData
 *
 * Name(s):
 * <base>.IgChanUsedBufferCreditsRdData
 */
#define NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD            0x01d8
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_ADDR(x)    (((x) & 0xff) << 24)
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_ADDR_of(x) (((x) >> 24) & 0xff)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_ADDR_bf  0, 31, 24
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_ADDR_msk (0xff)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_ADDR_shf (24)
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_VALID      (1 << 21)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_VALID_bf 0, 21, 21
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_VALID_msk (0x1)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_VALID_bit (21)
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_DATA(x)    (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_DATA_of(x) (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_DATA_bf  0, 15, 0
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_DATA_msk (0xffff)
#define     NFP_MAC_CSR_IG_CHAN_USED_BUF_CREDITS_RD_DATA_shf (0)


/*
 * Register: IgPrependEn
 *   [25:24]   PrependLk
 *   [23:22]   PrependEn11
 *   [21:20]   PrependEn10
 *   [19:18]   PrependEn9
 *   [17:16]   PrependEn8
 *   [15:14]   PrependEn7
 *   [13:12]   PrependEn6
 *   [11:10]   PrependEn5
 *   [9:8]     PrependEn4
 *   [7:6]     PrependEn3
 *   [5:4]     PrependEn2
 *   [3:2]     PrependEn1
 *   [1:0]     PrependEn0
 *
 * Name(s):
 * <base>.IgPortPrependEn0 <base>.IgPortPrependEn1
 */
#define NFP_MAC_CSR_IG_PREPEND_EN0                         0x01dc
#define NFP_MAC_CSR_IG_PREPEND_EN1                         0x01e0
#define   NFP_MAC_CSR_IG_PREPEND_EN0_LK(x)                   (((x) & 3) << 24)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_LK_of(x)                (((x) >> 24) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_LK_bf                 0, 25, 24
#define     NFP_MAC_CSR_IG_PREPEND_EN0_LK_msk                (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_LK_shf                (24)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT11(x)               (((x) & 3) << 22)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT11_of(x)            (((x) >> 22) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT11_bf             0, 23, 22
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT11_msk            (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT11_shf            (22)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT10(x)               (((x) & 3) << 20)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT10_of(x)            (((x) >> 20) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT10_bf             0, 21, 20
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT10_msk            (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT10_shf            (20)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT9(x)                (((x) & 3) << 18)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT9_of(x)             (((x) >> 18) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT9_bf              0, 19, 18
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT9_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT9_shf             (18)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT8(x)                (((x) & 3) << 16)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT8_of(x)             (((x) >> 16) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT8_bf              0, 17, 16
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT8_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT8_shf             (16)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT7(x)                (((x) & 3) << 14)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT7_of(x)             (((x) >> 14) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT7_bf              0, 15, 14
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT7_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT7_shf             (14)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT6(x)                (((x) & 3) << 12)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT6_of(x)             (((x) >> 12) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT6_bf              0, 13, 12
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT6_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT6_shf             (12)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT5(x)                (((x) & 3) << 10)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT5_of(x)             (((x) >> 10) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT5_bf              0, 11, 10
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT5_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT5_shf             (10)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT4(x)                (((x) & 3) << 8)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT4_of(x)             (((x) >> 8) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT4_bf              0, 9, 8
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT4_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT4_shf             (8)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT3(x)                (((x) & 3) << 6)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT3_of(x)             (((x) >> 6) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT3_bf              0, 7, 6
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT3_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT3_shf             (6)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT2(x)                (((x) & 3) << 4)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT2_of(x)             (((x) >> 4) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT2_bf              0, 5, 4
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT2_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT2_shf             (4)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT1(x)                (((x) & 3) << 2)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT1_of(x)             (((x) >> 2) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT1_bf              0, 3, 2
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT1_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT1_shf             (2)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT0(x)                (((x) & 3) << 0)
#define   NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_of(x)             (((x) >> 0) & 3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_OFF             (0)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_CHK             (1)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_TS              (2)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_TS_CHK          (3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_bf              0, 1, 0
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_msk             (0x3)
#define     NFP_MAC_CSR_IG_PREPEND_EN0_PORT0_shf             (0)


/*
 * Register: VlanMatchReg
 *   [31:16]   VlanMask
 *   [15:0]    VlanMatch
 *
 * Name(s):
 * <base>.EgVlanMatchReg0 <base>.IgVlanMatchReg0
 */
#define NFP_MAC_CSR_EG_VLAN_MATCH_REG0                     0x01e8
#define NFP_MAC_CSR_IG_VLAN_MATCH_REG0                     0x01f0
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG0_MASK(x)             (((x) & 0xffff) << 16)
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG0_MASK_of(x)          (((x) >> 16) & 0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG0_MASK_bf           0, 31, 16
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG0_MASK_msk          (0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG0_MASK_shf          (16)
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG0_VAL(x)              (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG0_VAL_of(x)           (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG0_VAL_bf            0, 15, 0
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG0_VAL_msk           (0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG0_VAL_shf           (0)


/*
 * Register: VlanMatchReg1
 *   [31:16]   VlanMask1
 *   [15:0]    VlanMatch1
 *
 * Name(s):
 * <base>.EgVlanMatchReg1 <base>.IgVlanMatchReg1
 */
#define NFP_MAC_CSR_EG_VLAN_MATCH_REG1                     0x01ec
#define NFP_MAC_CSR_IG_VLAN_MATCH_REG1                     0x01f4
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG1_MASK(x)             (((x) & 0xffff) << 16)
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG1_MASK_of(x)          (((x) >> 16) & 0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG1_MASK_bf           0, 31, 16
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG1_MASK_msk          (0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG1_MASK_shf          (16)
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG1_VAL(x)              (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_EG_VLAN_MATCH_REG1_VAL_of(x)           (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG1_VAL_bf            0, 15, 0
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG1_VAL_msk           (0xffff)
#define     NFP_MAC_CSR_EG_VLAN_MATCH_REG1_VAL_shf           (0)


/*
 * Register: EgCmdPrependEn
 *
 * Name(s):
 * <base>.EgCmdPrependEn0Lo <base>.EgCmdPrependEn0Hi <base>.EgCmdPrependEn1Lo
 * <base>.EgCmdPrependEn1Hi
 */
#define NFP_MAC_CSR_EG_CMD_PREPEND_EN0_LO                  0x0200
#define NFP_MAC_CSR_EG_CMD_PREPEND_EN0_HI                  0x0204
#define NFP_MAC_CSR_EG_CMD_PREPEND_EN1_LO                  0x0208
#define NFP_MAC_CSR_EG_CMD_PREPEND_EN1_HI                  0x020c

/*
 * Register: MacEgIlkChanAssign
 *   [28:22]   LkNumChannelsUpper64
 *   [21:16]   LkBaseChannelUpper64
 *   [12:6]    LkNumChannelsLower64
 *   [5:0]     LkBaseChannelLower64
 *
 * Name(s):
 * <base>.MacEgIlkChanAssign
 */
#define NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN                     0x0260
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_NUM_CHANS_UPPER_64(x) (((x) & 0x7f) << 22)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_NUM_CHANS_UPPER_64_of(x) (((x) >> 22) & 0x7f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_NUM_CHANS_UPPER_64_bf 0, 28, 22
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_NUM_CHANS_UPPER_64_msk (0x7f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_NUM_CHANS_UPPER_64_shf (22)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_UPPER_64(x) (((x) & 0x3f) << 16)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_UPPER_64_of(x) (((x) >> 16) & 0x3f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_UPPER_64_bf 0, 21, 16
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_UPPER_64_msk (0x3f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_UPPER_64_shf (16)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_CHANS_LOWER_64(x)   (((x) & 0x7f) << 6)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_CHANS_LOWER_64_of(x) (((x) >> 6) & 0x7f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_CHANS_LOWER_64_bf 0, 12, 6
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_CHANS_LOWER_64_msk (0x7f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_CHANS_LOWER_64_shf (6)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_LOWER_64(x) (((x) & 0x3f) << 0)
#define   NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_LOWER_64_of(x) (((x) >> 0) & 0x3f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_LOWER_64_bf 0, 5, 0
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_LOWER_64_msk (0x3f)
#define     NFP_MAC_CSR_EG_ILK_CHAN_ASSIGN_BASE_CHAN_LOWER_64_shf (0)


/*
 * Register: MacEgPortRR
 *   [23:0]    EgPortRR
 *
 * Name(s):
 * <base>.MacEgPortRR
 */
#define NFP_MAC_CSR_EG_PORT_RR                             0x0264
#define   NFP_MAC_CSR_EG_PORT_RR_VAL(x)                      (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_EG_PORT_RR_VAL_of(x)                   (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_EG_PORT_RR_VAL_bf                    0, 23, 0
#define     NFP_MAC_CSR_EG_PORT_RR_VAL_msk                   (0xffffff)
#define     NFP_MAC_CSR_EG_PORT_RR_VAL_shf                   (0)


/*
 * Register: MacOobFcTmCntl
 *   [31:22]   Oob1023To512RFU
 *   [21:18]   Oob1023To512Mod32M1
 *   [17]      Oob1023To512MsgEn
 *   [16]      Oob1023To512En
 *   [15:6]    Oob511To0RFU
 *   [5:2]     Oob511To0Mod32M1
 *   [1]       Oob511To0MsgEn
 *   [0]       Oob511To0En
 *
 * Name(s):
 * <base>.MacOobFcTmCntl
 */
#define NFP_MAC_CSR_OOB_FC_TM_CNTL                         0x0268
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_OOB_1023_TO_512_RFU(x)  (((x) & 0x3ff) << 22)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_OOB_1023_TO_512_RFU_of(x) (((x) >> 22) & 0x3ff)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_OOB_1023_TO_512_RFU_bf 0, 31, 22
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_OOB_1023_TO_512_RFU_msk (0x3ff)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_OOB_1023_TO_512_RFU_shf (22)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MOD32_M1(x) (((x) & 0xf) << 18)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MOD32_M1_of(x) (((x) >> 18) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MOD32_M1_bf 0, 21, 18
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MOD32_M1_msk (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MOD32_M1_shf (18)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MSG_EN      (1 << 17)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MSG_EN_bf 0, 17, 17
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MSG_EN_msk (0x1)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_MSG_EN_bit (17)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_EN          (1 << 16)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_EN_bf     0, 16, 16
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_EN_msk    (0x1)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_1023_TO_512_EN_bit    (16)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_RFU(x)         (((x) & 0x3ff) << 6)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_RFU_of(x)      (((x) >> 6) & 0x3ff)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_RFU_bf       0, 15, 6
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_RFU_msk      (0x3ff)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_RFU_shf      (6)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MOD32_M1(x)    (((x) & 0xf) << 2)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MOD32_M1_of(x) (((x) >> 2) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MOD32_M1_bf  0, 5, 2
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MOD32_M1_msk (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MOD32_M1_shf (2)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MSG_EN         (1 << 1)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MSG_EN_bf    0, 1, 1
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MSG_EN_msk   (0x1)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_MSG_EN_bit   (1)
#define   NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_EN             (1 << 0)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_EN_bf        0, 0, 0
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_EN_msk       (0x1)
#define     NFP_MAC_CSR_OOB_FC_TM_CNTL_511_TO_0_EN_bit       (0)


/*
 * Register: MacOobFcTmReMap
 *   [31:28]   TmFcAddr7Sel
 *   [27:24]   TmFcAddr6Sel
 *   [23:20]   TmFcAddr5Sel
 *   [19:16]   TmFcAddr4Sel
 *   [15:12]   TmFcAddr3Sel
 *   [11:8]    TmFcAddr2Sel
 *   [7:4]     TmFcAddr1Sel
 *   [3:0]     TmFcAddr0Sel
 *
 * Name(s):
 * <base>.MacOobFcTmReMap
 */
#define NFP_MAC_CSR_OOB_FC_TM_REMAP                        0x026c
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR7(x)               (((x) & 0xf) << 28)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR7_of(x)            (((x) >> 28) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR7_bf             0, 31, 28
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR7_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR7_shf            (28)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR6(x)               (((x) & 0xf) << 24)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR6_of(x)            (((x) >> 24) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR6_bf             0, 27, 24
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR6_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR6_shf            (24)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR5(x)               (((x) & 0xf) << 20)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR5_of(x)            (((x) >> 20) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR5_bf             0, 23, 20
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR5_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR5_shf            (20)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR4(x)               (((x) & 0xf) << 16)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR4_of(x)            (((x) >> 16) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR4_bf             0, 19, 16
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR4_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR4_shf            (16)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR3(x)               (((x) & 0xf) << 12)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR3_of(x)            (((x) >> 12) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR3_bf             0, 15, 12
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR3_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR3_shf            (12)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR2(x)               (((x) & 0xf) << 8)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR2_of(x)            (((x) >> 8) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR2_bf             0, 11, 8
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR2_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR2_shf            (8)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR1(x)               (((x) & 0xf) << 4)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR1_of(x)            (((x) >> 4) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR1_bf             0, 7, 4
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR1_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR1_shf            (4)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR0(x)               (((x) & 0xf) << 0)
#define   NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR0_of(x)            (((x) >> 0) & 0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR0_bf             0, 3, 0
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR0_msk            (0xf)
#define     NFP_MAC_CSR_OOB_FC_TM_REMAP_ADDR0_shf            (0)


/*
 * Register: MacEqInh
 *   [25]      MacEqInhIlk1
 *   [24]      MacEqInhIlk0
 *   [23:0]    MacEqInhEthPort
 *
 * Name(s):
 * <base>.MacEqInh
 */
#define NFP_MAC_CSR_EQ_INH                                 0x0278
#define   NFP_MAC_CSR_EQ_INH_ILK1                            (1 << 25)
#define     NFP_MAC_CSR_EQ_INH_ILK1_bf                       0, 25, 25
#define     NFP_MAC_CSR_EQ_INH_ILK1_msk                      (0x1)
#define     NFP_MAC_CSR_EQ_INH_ILK1_bit                      (25)
#define   NFP_MAC_CSR_EQ_INH_ILK0                            (1 << 24)
#define     NFP_MAC_CSR_EQ_INH_ILK0_bf                       0, 24, 24
#define     NFP_MAC_CSR_EQ_INH_ILK0_msk                      (0x1)
#define     NFP_MAC_CSR_EQ_INH_ILK0_bit                      (24)
#define   NFP_MAC_CSR_EQ_INH_ETH_PORT(x)                     (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_EQ_INH_ETH_PORT_of(x)                  (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_EQ_INH_ETH_PORT_bf                   0, 23, 0
#define     NFP_MAC_CSR_EQ_INH_ETH_PORT_msk                  (0xffffff)
#define     NFP_MAC_CSR_EQ_INH_ETH_PORT_shf                  (0)


/*
 * Register: MacEqInhDone
 *   [25]      MacEqInhDoneIlk1
 *   [24]      MacEqInhDoneIlk0
 *   [23:0]    MacEqInhDoneEthPort
 *
 * Name(s):
 * <base>.MacEqInhDone
 */
#define NFP_MAC_CSR_EQ_INH_DONE                            0x027c
#define   NFP_MAC_CSR_EQ_INH_DONE_ILK1                       (1 << 25)
#define     NFP_MAC_CSR_EQ_INH_DONE_ILK1_bf                  0, 25, 25
#define     NFP_MAC_CSR_EQ_INH_DONE_ILK1_msk                 (0x1)
#define     NFP_MAC_CSR_EQ_INH_DONE_ILK1_bit                 (25)
#define   NFP_MAC_CSR_EQ_INH_DONE_ILK0                       (1 << 24)
#define     NFP_MAC_CSR_EQ_INH_DONE_ILK0_bf                  0, 24, 24
#define     NFP_MAC_CSR_EQ_INH_DONE_ILK0_msk                 (0x1)
#define     NFP_MAC_CSR_EQ_INH_DONE_ILK0_bit                 (24)
#define   NFP_MAC_CSR_EQ_INH_DONE_ETH_PORT(x)                (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_EQ_INH_DONE_ETH_PORT_of(x)             (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_EQ_INH_DONE_ETH_PORT_bf              0, 23, 0
#define     NFP_MAC_CSR_EQ_INH_DONE_ETH_PORT_msk             (0xffffff)
#define     NFP_MAC_CSR_EQ_INH_DONE_ETH_PORT_shf             (0)


/*
 * Register: MacHeadDropCounters
 *   [31:16]   MacHeadDropCounter1
 *   [15:0]    MacHeadDropCounter0
 *
 * Name(s):
 * <base>.MacHy0EthIgPktHeadDropCntrPair0...
 * <base>.MacHy1EthIgPktHeadDropCntrPair0... <base>.MacIlkIgPktHeadDropCntrPair
 */
#define NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR(x)         (0x0280 + ((x) * 0x4))
#define NFP_MAC_CSR_ETH1_IG_HEAD_DROP_CNTR_PAIR(x)         (0x02a0 + ((x) * 0x4))
#define NFP_MAC_CSR_ILK_IG_HEAD_DROP_CNTR_PAIR             0x02b8
#define   NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_1(x)       (((x) & 0xffff) << 16)
#define   NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_1_of(x)    (((x) >> 16) & 0xffff)
#define     NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_1_bf     0, 31, 16
#define     NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_1_msk    (0xffff)
#define     NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_1_shf    (16)
#define   NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_0(x)       (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_0_of(x)    (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_0_bf     0, 15, 0
#define     NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_0_msk    (0xffff)
#define     NFP_MAC_CSR_ETH0_IG_HEAD_DROP_CNTR_PAIR_0_shf    (0)


/*
 * Register: MacEthFifoIfErr
 *   [24]      RemLocFaultSticky
 *   [23:12]   EthTxIfOvr
 *   [11:0]    EthTxIfUnf
 *
 * Name(s):
 * <base>.MacEthFifoIfErr0 <base>.MacEthFifoIfErr1
 */
#define NFP_MAC_CSR_ETH_FIFO_ERR_0                         0x0400
#define NFP_MAC_CSR_ETH_FIFO_ERR_1                         0x0404
#define   NFP_MAC_CSR_ETH_FIFO_ERR_REM_LOC_FAULT             (1 << 24)
#define     NFP_MAC_CSR_ETH_FIFO_ERR_REM_LOC_FAULT_bf        0, 24, 24
#define     NFP_MAC_CSR_ETH_FIFO_ERR_REM_LOC_FAULT_msk       (0x1)
#define     NFP_MAC_CSR_ETH_FIFO_ERR_REM_LOC_FAULT_bit       (24)
#define   NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_OVR(x)              (((x) & 0xfff) << 12)
#define   NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_OVR_of(x)           (((x) >> 12) & 0xfff)
#define     NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_OVR_bf            0, 23, 12
#define     NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_OVR_msk           (0xfff)
#define     NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_OVR_shf           (12)
#define   NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_UNF(x)              (((x) & 0xfff) << 0)
#define   NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_UNF_of(x)           (((x) >> 0) & 0xfff)
#define     NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_UNF_bf            0, 11, 0
#define     NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_UNF_msk           (0xfff)
#define     NFP_MAC_CSR_ETH_FIFO_ERR_TX_IF_UNF_shf           (0)


/*
 * Register: MacEthAnStatus
 *   [23:12]   EthAnInt
 *   [11:0]    EthAnDone
 *
 * Name(s):
 * <base>.MacEthAnStatus0 <base>.MacEthAnStatus1
 */
#define NFP_MAC_CSR_ETH_AN_STS_0                           0x0408
#define NFP_MAC_CSR_ETH_AN_STS_1                           0x040c
#define   NFP_MAC_CSR_ETH_AN_STS_INT(x)                      (((x) & 0xfff) << 12)
#define   NFP_MAC_CSR_ETH_AN_STS_INT_of(x)                   (((x) >> 12) & 0xfff)
#define     NFP_MAC_CSR_ETH_AN_STS_INT_bf                    0, 23, 12
#define     NFP_MAC_CSR_ETH_AN_STS_INT_msk                   (0xfff)
#define     NFP_MAC_CSR_ETH_AN_STS_INT_shf                   (12)
#define   NFP_MAC_CSR_ETH_AN_STS_DONE(x)                     (((x) & 0xfff) << 0)
#define   NFP_MAC_CSR_ETH_AN_STS_DONE_of(x)                  (((x) >> 0) & 0xfff)
#define     NFP_MAC_CSR_ETH_AN_STS_DONE_bf                   0, 11, 0
#define     NFP_MAC_CSR_ETH_AN_STS_DONE_msk                  (0xfff)
#define     NFP_MAC_CSR_ETH_AN_STS_DONE_shf                  (0)


/*
 * Register: MacOobFcIlkStatus
 *   [19]      MacOobFcCrcErr1
 *   [18]      MacOobFcFrmErr1
 *   [17]      MacOobFcCrcErr0
 *   [16]      MacOobFcFrmErr0
 *   [7]       IlkInt2ndRx1
 *   [6]       IlkInt2ndTx1
 *   [5]       IlkIntRx1
 *   [4]       IlkIntTx1
 *   [3]       IlkInt2ndRx0
 *   [2]       IlkInt2ndTx0
 *   [1]       IlkIntRx0
 *   [0]       IlkIntTx0
 *
 * Name(s):
 * <base>.MacOobFcIlkStatus
 */
#define NFP_MAC_CSR_OOB_FC_ILK_STS                         0x0410
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_1           (1 << 19)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_1_bf      0, 19, 19
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_1_msk     (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_1_bit     (19)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_1           (1 << 18)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_1_bf      0, 18, 18
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_1_msk     (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_1_bit     (18)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_0           (1 << 17)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_0_bf      0, 17, 17
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_0_msk     (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_CRC_ERR_0_bit     (17)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_0           (1 << 16)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_0_bf      0, 16, 16
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_0_msk     (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_MAC_FRM_ERR_0_bit     (16)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_1        (1 << 7)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_1_bf   0, 7, 7
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_1_msk  (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_1_bit  (7)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_1        (1 << 6)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_1_bf   0, 6, 6
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_1_msk  (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_1_bit  (6)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_1            (1 << 5)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_1_bf       0, 5, 5
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_1_msk      (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_1_bit      (5)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_1            (1 << 4)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_1_bf       0, 4, 4
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_1_msk      (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_1_bit      (4)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_0        (1 << 3)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_0_bf   0, 3, 3
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_0_msk  (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_RX_0_bit  (3)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_0        (1 << 2)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_0_bf   0, 2, 2
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_0_msk  (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_2ND_TX_0_bit  (2)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_0            (1 << 1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_0_bf       0, 1, 1
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_0_msk      (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_RX_0_bit      (1)
#define   NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_0            (1 << 0)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_0_bf       0, 0, 0
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_0_msk      (0x1)
#define     NFP_MAC_CSR_OOB_FC_ILK_STS_ILK_INT_TX_0_bit      (0)


/*
 * Register: MacStatsHalfFull
 *   [24]      TxStatHalfFullVld
 *   [23:16]   TxStatAddr
 *   [8]       RxStatHalfFullVld
 *   [7:0]     RxStatAddr
 *
 * Name(s):
 * <base>.MacStatsHalfFullPort0011 <base>.MacStatsHalfFullPort1223
 * <base>.MacStatsHalfFullChan0063 <base>.MacStatsHalfFullChan63127
 */
#define NFP_MAC_CSR_STATS_HALF_FULL_PORT_00_11             0x0420
#define NFP_MAC_CSR_STATS_HALF_FULL_PORT_12_23             0x0424
#define NFP_MAC_CSR_STATS_HALF_FULL_CHAN_00_63             0x0428
#define NFP_MAC_CSR_STATS_HALF_FULL_CHAN_63_127            0x042c
#define   NFP_MAC_CSR_STATS_HALF_FULL_TX_VALID               (1 << 24)
#define     NFP_MAC_CSR_STATS_HALF_FULL_TX_VALID_bf          0, 24, 24
#define     NFP_MAC_CSR_STATS_HALF_FULL_TX_VALID_msk         (0x1)
#define     NFP_MAC_CSR_STATS_HALF_FULL_TX_VALID_bit         (24)
#define   NFP_MAC_CSR_STATS_HALF_FULL_TX_ADDR(x)             (((x) & 0xff) << 16)
#define   NFP_MAC_CSR_STATS_HALF_FULL_TX_ADDR_of(x)          (((x) >> 16) & 0xff)
#define     NFP_MAC_CSR_STATS_HALF_FULL_TX_ADDR_bf           0, 23, 16
#define     NFP_MAC_CSR_STATS_HALF_FULL_TX_ADDR_msk          (0xff)
#define     NFP_MAC_CSR_STATS_HALF_FULL_TX_ADDR_shf          (16)
#define   NFP_MAC_CSR_STATS_HALF_FULL_RX_VALID               (1 << 8)
#define     NFP_MAC_CSR_STATS_HALF_FULL_RX_VALID_bf          0, 8, 8
#define     NFP_MAC_CSR_STATS_HALF_FULL_RX_VALID_msk         (0x1)
#define     NFP_MAC_CSR_STATS_HALF_FULL_RX_VALID_bit         (8)
#define   NFP_MAC_CSR_STATS_HALF_FULL_RX_ADDR(x)             (((x) & 0xff) << 0)
#define   NFP_MAC_CSR_STATS_HALF_FULL_RX_ADDR_of(x)          (((x) >> 0) & 0xff)
#define     NFP_MAC_CSR_STATS_HALF_FULL_RX_ADDR_bf           0, 7, 0
#define     NFP_MAC_CSR_STATS_HALF_FULL_RX_ADDR_msk          (0xff)
#define     NFP_MAC_CSR_STATS_HALF_FULL_RX_ADDR_shf          (0)


/*
 * Register: MacPcpReMap
 *   [29:24]   UntaggedChan
 *   [23:21]   PcpReMap7
 *   [20:18]   PcpReMap6
 *   [17:15]   PcpReMap5
 *   [14:12]   PcpReMap4
 *   [11:9]    PcpReMap3
 *   [8:6]     PcpReMap2
 *   [5:3]     PcpReMap1
 *   [2:0]     PcpReMap0
 *
 * Name(s):
 * <base>.MacPcpReMap0...
 */
#define NFP_MAC_CSR_PCP_REMAP(x)                           (0x0680 + ((x) * 0x4))
#define   NFP_MAC_CSR_PCP_REMAP_UNTAGGED(x)                  (((x) & 0x3f) << 24)
#define   NFP_MAC_CSR_PCP_REMAP_UNTAGGED_of(x)               (((x) >> 24) & 0x3f)
#define     NFP_MAC_CSR_PCP_REMAP_UNTAGGED_bf                0, 29, 24
#define     NFP_MAC_CSR_PCP_REMAP_UNTAGGED_msk               (0x3f)
#define     NFP_MAC_CSR_PCP_REMAP_UNTAGGED_shf               (24)
#define   NFP_MAC_CSR_PCP_REMAP_7(x)                         (((x) & 7) << 21)
#define   NFP_MAC_CSR_PCP_REMAP_7_of(x)                      (((x) >> 21) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_7_bf                       0, 23, 21
#define     NFP_MAC_CSR_PCP_REMAP_7_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_7_shf                      (21)
#define   NFP_MAC_CSR_PCP_REMAP_6(x)                         (((x) & 7) << 18)
#define   NFP_MAC_CSR_PCP_REMAP_6_of(x)                      (((x) >> 18) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_6_bf                       0, 20, 18
#define     NFP_MAC_CSR_PCP_REMAP_6_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_6_shf                      (18)
#define   NFP_MAC_CSR_PCP_REMAP_5(x)                         (((x) & 7) << 15)
#define   NFP_MAC_CSR_PCP_REMAP_5_of(x)                      (((x) >> 15) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_5_bf                       0, 17, 15
#define     NFP_MAC_CSR_PCP_REMAP_5_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_5_shf                      (15)
#define   NFP_MAC_CSR_PCP_REMAP_4(x)                         (((x) & 7) << 12)
#define   NFP_MAC_CSR_PCP_REMAP_4_of(x)                      (((x) >> 12) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_4_bf                       0, 14, 12
#define     NFP_MAC_CSR_PCP_REMAP_4_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_4_shf                      (12)
#define   NFP_MAC_CSR_PCP_REMAP_3(x)                         (((x) & 7) << 9)
#define   NFP_MAC_CSR_PCP_REMAP_3_of(x)                      (((x) >> 9) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_3_bf                       0, 11, 9
#define     NFP_MAC_CSR_PCP_REMAP_3_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_3_shf                      (9)
#define   NFP_MAC_CSR_PCP_REMAP_2(x)                         (((x) & 7) << 6)
#define   NFP_MAC_CSR_PCP_REMAP_2_of(x)                      (((x) >> 6) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_2_bf                       0, 8, 6
#define     NFP_MAC_CSR_PCP_REMAP_2_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_2_shf                      (6)
#define   NFP_MAC_CSR_PCP_REMAP_1(x)                         (((x) & 7) << 3)
#define   NFP_MAC_CSR_PCP_REMAP_1_of(x)                      (((x) >> 3) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_1_bf                       0, 5, 3
#define     NFP_MAC_CSR_PCP_REMAP_1_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_1_shf                      (3)
#define   NFP_MAC_CSR_PCP_REMAP_0(x)                         (((x) & 7) << 0)
#define   NFP_MAC_CSR_PCP_REMAP_0_of(x)                      (((x) >> 0) & 7)
#define     NFP_MAC_CSR_PCP_REMAP_0_bf                       0, 2, 0
#define     NFP_MAC_CSR_PCP_REMAP_0_msk                      (0x7)
#define     NFP_MAC_CSR_PCP_REMAP_0_shf                      (0)


/*
 * Register: MacPortHwm
 *   [31:27]   PortDropDelta1
 *   [26:16]   PortHwm1
 *   [15:11]   PortDropDelta0
 *   [10:0]    PortHwm0
 *
 * Name(s):
 * <base>.MacPortHwm0...
 */
#define NFP_MAC_CSR_PORT_HWM(x)                            (0x0700 + ((x) * 0x4))
#define   NFP_MAC_CSR_PORT_HWM_DROP_DELTA1(x)                (((x) & 0x1f) << 27)
#define   NFP_MAC_CSR_PORT_HWM_DROP_DELTA1_of(x)             (((x) >> 27) & 0x1f)
#define     NFP_MAC_CSR_PORT_HWM_DROP_DELTA1_bf              0, 31, 27
#define     NFP_MAC_CSR_PORT_HWM_DROP_DELTA1_msk             (0x1f)
#define     NFP_MAC_CSR_PORT_HWM_DROP_DELTA1_shf             (27)
#define   NFP_MAC_CSR_PORT_HWM_HWM1(x)                       (((x) & 0x7ff) << 16)
#define   NFP_MAC_CSR_PORT_HWM_HWM1_of(x)                    (((x) >> 16) & 0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_HWM1_bf                     0, 26, 16
#define     NFP_MAC_CSR_PORT_HWM_HWM1_msk                    (0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_HWM1_shf                    (16)
#define   NFP_MAC_CSR_PORT_HWM_DROP_DELTA0(x)                (((x) & 0x1f) << 11)
#define   NFP_MAC_CSR_PORT_HWM_DROP_DELTA0_of(x)             (((x) >> 11) & 0x1f)
#define     NFP_MAC_CSR_PORT_HWM_DROP_DELTA0_bf              0, 15, 11
#define     NFP_MAC_CSR_PORT_HWM_DROP_DELTA0_msk             (0x1f)
#define     NFP_MAC_CSR_PORT_HWM_DROP_DELTA0_shf             (11)
#define   NFP_MAC_CSR_PORT_HWM_HWM0(x)                       (((x) & 0x7ff) << 0)
#define   NFP_MAC_CSR_PORT_HWM_HWM0_of(x)                    (((x) >> 0) & 0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_HWM0_bf                     0, 10, 0
#define     NFP_MAC_CSR_PORT_HWM_HWM0_msk                    (0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_HWM0_shf                    (0)


/*
 * Register: MacPortHwm1
 *   [31:27]   PortDropDelta1
 *   [26:16]   PortHwm1
 *   [15:11]   PortDropDelta0
 *   [10:0]    PortHwm0
 *
 * Name(s):
 * <base>.MacPortHwmLk1Lk0
 */
#define NFP_MAC_CSR_PORT_HWM_ILK                           0x0730
#define   NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA1(x)            (((x) & 0x1f) << 27)
#define   NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA1_of(x)         (((x) >> 27) & 0x1f)
#define     NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA1_bf          0, 31, 27
#define     NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA1_msk         (0x1f)
#define     NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA1_shf         (27)
#define   NFP_MAC_CSR_PORT_HWM_ILK_HWM1(x)                   (((x) & 0x7ff) << 16)
#define   NFP_MAC_CSR_PORT_HWM_ILK_HWM1_of(x)                (((x) >> 16) & 0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_ILK_HWM1_bf                 0, 26, 16
#define     NFP_MAC_CSR_PORT_HWM_ILK_HWM1_msk                (0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_ILK_HWM1_shf                (16)
#define   NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA0(x)            (((x) & 0x1f) << 11)
#define   NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA0_of(x)         (((x) >> 11) & 0x1f)
#define     NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA0_bf          0, 15, 11
#define     NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA0_msk         (0x1f)
#define     NFP_MAC_CSR_PORT_HWM_ILK_DROP_DELTA0_shf         (11)
#define   NFP_MAC_CSR_PORT_HWM_ILK_HWM0(x)                   (((x) & 0x7ff) << 0)
#define   NFP_MAC_CSR_PORT_HWM_ILK_HWM0_of(x)                (((x) >> 0) & 0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_ILK_HWM0_bf                 0, 10, 0
#define     NFP_MAC_CSR_PORT_HWM_ILK_HWM0_msk                (0x7ff)
#define     NFP_MAC_CSR_PORT_HWM_ILK_HWM0_shf                (0)


/*
 * Register: LLMemRdData
 *   [29:18]   LLRdOffsetAddr
 *   [17]      LLRdDataValid
 *   [15:0]    LLRdData
 *
 * Name(s):
 * <base>.EgLnkLstRdData <base>.IgLnkLstRdData
 */
#define NFP_MAC_CSR_EG_LL_RDDATA                           0x07b0
#define NFP_MAC_CSR_IG_LL_RDDATA                           0x07b4
#define   NFP_MAC_CSR_LL_RDDATA_OFF(x)                       (((x) & 0xfff) << 18)
#define   NFP_MAC_CSR_LL_RDDATA_OFF_of(x)                    (((x) >> 18) & 0xfff)
#define     NFP_MAC_CSR_LL_RDDATA_OFF_bf                     0, 29, 18
#define     NFP_MAC_CSR_LL_RDDATA_OFF_msk                    (0xfff)
#define     NFP_MAC_CSR_LL_RDDATA_OFF_shf                    (18)
#define   NFP_MAC_CSR_LL_RDDATA_VALID                        (1 << 17)
#define     NFP_MAC_CSR_LL_RDDATA_VALID_bf                   0, 17, 17
#define     NFP_MAC_CSR_LL_RDDATA_VALID_msk                  (0x1)
#define     NFP_MAC_CSR_LL_RDDATA_VALID_bit                  (17)
#define   NFP_MAC_CSR_LL_RDDATA_DATA(x)                      (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_LL_RDDATA_DATA_of(x)                   (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_LL_RDDATA_DATA_bf                    0, 15, 0
#define     NFP_MAC_CSR_LL_RDDATA_DATA_msk                   (0xffff)
#define     NFP_MAC_CSR_LL_RDDATA_DATA_shf                   (0)


/*
 * Register: LLMemRdWr
 *   [29:18]   LLOffsetAddr
 *   [17]      LLRdBusy
 *   [16]      LLWrBusy
 *   [15:0]    LLWrData
 *
 * Name(s):
 * <base>.EgLnkLstRdWr <base>.IgLnkLstRdWr
 */
#define NFP_MAC_CSR_EG_LL_RDWR                             0x07b8
#define NFP_MAC_CSR_IG_LL_RDWR                             0x07bc
#define   NFP_MAC_CSR_LL_RDWR_OFF(x)                         (((x) & 0xfff) << 18)
#define   NFP_MAC_CSR_LL_RDWR_OFF_of(x)                      (((x) >> 18) & 0xfff)
#define     NFP_MAC_CSR_LL_RDWR_OFF_bf                       0, 29, 18
#define     NFP_MAC_CSR_LL_RDWR_OFF_msk                      (0xfff)
#define     NFP_MAC_CSR_LL_RDWR_OFF_shf                      (18)
#define   NFP_MAC_CSR_LL_RDWR_RD_BUSY                        (1 << 17)
#define     NFP_MAC_CSR_LL_RDWR_RD_BUSY_bf                   0, 17, 17
#define     NFP_MAC_CSR_LL_RDWR_RD_BUSY_msk                  (0x1)
#define     NFP_MAC_CSR_LL_RDWR_RD_BUSY_bit                  (17)
#define   NFP_MAC_CSR_LL_RDWR_WR_BUSY                        (1 << 16)
#define     NFP_MAC_CSR_LL_RDWR_WR_BUSY_bf                   0, 16, 16
#define     NFP_MAC_CSR_LL_RDWR_WR_BUSY_msk                  (0x1)
#define     NFP_MAC_CSR_LL_RDWR_WR_BUSY_bit                  (16)
#define   NFP_MAC_CSR_LL_RDWR_WR_DATA(x)                     (((x) & 0xffff) << 0)
#define   NFP_MAC_CSR_LL_RDWR_WR_DATA_of(x)                  (((x) >> 0) & 0xffff)
#define     NFP_MAC_CSR_LL_RDWR_WR_DATA_bf                   0, 15, 0
#define     NFP_MAC_CSR_LL_RDWR_WR_DATA_msk                  (0xffff)
#define     NFP_MAC_CSR_LL_RDWR_WR_DATA_shf                  (0)


/*
 * Register: SerDes4RdWr
 *   [31:29]   SerDesPageAddr
 *   [28]      SerDesPcsPmaSel
 *   [27:16]   SerDesOffsetAddr
 *   [9]       SerDesRdBusy
 *   [8]       SerDesWrBusy
 *   [7:0]     SerDesWrData
 *
 * Name(s):
 * <base>.SerDes4RdWr03To00 <base>.SerDes4RdWr07To04 <base>.SerDes4RdWr11To08
 * <base>.SerDes4RdWr15To12 <base>.SerDes4RdWr19To16 <base>.SerDes4RdWr23To20
 */
#define NFP_MAC_CSR_SERDES4_RDWR_03_00                     0x07c0
#define NFP_MAC_CSR_SERDES4_RDWR_07_04                     0x07c4
#define NFP_MAC_CSR_SERDES4_RDWR_11_08                     0x07c8
#define NFP_MAC_CSR_SERDES4_RDWR_15_12                     0x07cc
#define NFP_MAC_CSR_SERDES4_RDWR_19_16                     0x07d0
#define NFP_MAC_CSR_SERDES4_RDWR_23_20                     0x07d4
#define   NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR(x)              (((x) & 7) << 29)
#define   NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_of(x)           (((x) >> 29) & 7)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_LANE0         (0)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_LANE1         (1)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_LANE2         (2)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_LANE3         (3)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_COMMON        (4)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_ALL           (7)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_bf            0, 31, 29
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_msk           (0x7)
#define     NFP_MAC_CSR_SERDES4_RDWR_PAGE_ADDR_shf           (29)
#define   NFP_MAC_CSR_SERDES4_RDWR_PCS_PMA                   (1 << 28)
#define     NFP_MAC_CSR_SERDES4_RDWR_PCS_PMA_bf              0, 28, 28
#define     NFP_MAC_CSR_SERDES4_RDWR_PCS_PMA_msk             (0x1)
#define     NFP_MAC_CSR_SERDES4_RDWR_PCS_PMA_bit             (28)
#define   NFP_MAC_CSR_SERDES4_RDWR_OFF(x)                    (((x) & 0xfff) << 16)
#define   NFP_MAC_CSR_SERDES4_RDWR_OFF_of(x)                 (((x) >> 16) & 0xfff)
#define     NFP_MAC_CSR_SERDES4_RDWR_OFF_bf                  0, 27, 16
#define     NFP_MAC_CSR_SERDES4_RDWR_OFF_msk                 (0xfff)
#define     NFP_MAC_CSR_SERDES4_RDWR_OFF_shf                 (16)
#define   NFP_MAC_CSR_SERDES4_RDWR_RD_BUSY                   (1 << 9)
#define     NFP_MAC_CSR_SERDES4_RDWR_RD_BUSY_bf              0, 9, 9
#define     NFP_MAC_CSR_SERDES4_RDWR_RD_BUSY_msk             (0x1)
#define     NFP_MAC_CSR_SERDES4_RDWR_RD_BUSY_bit             (9)
#define   NFP_MAC_CSR_SERDES4_RDWR_WR_BUSY                   (1 << 8)
#define     NFP_MAC_CSR_SERDES4_RDWR_WR_BUSY_bf              0, 8, 8
#define     NFP_MAC_CSR_SERDES4_RDWR_WR_BUSY_msk             (0x1)
#define     NFP_MAC_CSR_SERDES4_RDWR_WR_BUSY_bit             (8)
#define   NFP_MAC_CSR_SERDES4_RDWR_WR_DATA(x)                (((x) & 0xff) << 0)
#define   NFP_MAC_CSR_SERDES4_RDWR_WR_DATA_of(x)             (((x) >> 0) & 0xff)
#define     NFP_MAC_CSR_SERDES4_RDWR_WR_DATA_bf              0, 7, 0
#define     NFP_MAC_CSR_SERDES4_RDWR_WR_DATA_msk             (0xff)
#define     NFP_MAC_CSR_SERDES4_RDWR_WR_DATA_shf             (0)


/*
 * Register: TdmMemRdWr
 *   [29:24]   TdmMemRdWrAddr
 *   [21]      TdmMemRdBusy
 *   [20]      TdmMemWrBusy
 *   [15]      TdmPortArbEnable
 *   [14:0]    TdmPortWeightWrData
 *
 * Name(s):
 * <base>.IgDqTdmMemoryRW
 */
#define NFP_MAC_CSR_TDM_MEM_RDWR                           0x07d8
#define   NFP_MAC_CSR_TDM_MEM_RDWR_ADDR(x)                   (((x) & 0x3f) << 24)
#define   NFP_MAC_CSR_TDM_MEM_RDWR_ADDR_of(x)                (((x) >> 24) & 0x3f)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_ADDR_bf                 0, 29, 24
#define     NFP_MAC_CSR_TDM_MEM_RDWR_ADDR_msk                (0x3f)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_ADDR_shf                (24)
#define   NFP_MAC_CSR_TDM_MEM_RDWR_RD_BUSY                   (1 << 21)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_RD_BUSY_bf              0, 21, 21
#define     NFP_MAC_CSR_TDM_MEM_RDWR_RD_BUSY_msk             (0x1)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_RD_BUSY_bit             (21)
#define   NFP_MAC_CSR_TDM_MEM_RDWR_WR_BUSY                   (1 << 20)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_WR_BUSY_bf              0, 20, 20
#define     NFP_MAC_CSR_TDM_MEM_RDWR_WR_BUSY_msk             (0x1)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_WR_BUSY_bit             (20)
#define   NFP_MAC_CSR_TDM_MEM_RDWR_PORT_ARB_ENABLE           (1 << 15)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_PORT_ARB_ENABLE_bf      0, 15, 15
#define     NFP_MAC_CSR_TDM_MEM_RDWR_PORT_ARB_ENABLE_msk     (0x1)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_PORT_ARB_ENABLE_bit     (15)
#define   NFP_MAC_CSR_TDM_MEM_RDWR_PORT_WEIGHTWR(x)          (((x) & 0x7fff) << 0)
#define   NFP_MAC_CSR_TDM_MEM_RDWR_PORT_WEIGHTWR_of(x)       (((x) >> 0) & 0x7fff)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_PORT_WEIGHTWR_bf        0, 14, 0
#define     NFP_MAC_CSR_TDM_MEM_RDWR_PORT_WEIGHTWR_msk       (0x7fff)
#define     NFP_MAC_CSR_TDM_MEM_RDWR_PORT_WEIGHTWR_shf       (0)


/*
 * Register: SerDes4RdData
 *   [31:29]   SerDesRdPageAddr
 *   [28:16]   SerDesRdOffsetAddr
 *   [9]       SerDesRdDataValid
 *   [7:0]     SerDesRdData
 *
 * Name(s):
 * <base>.SerDes4RdData03To00 <base>.SerDes4RdData07To04
 * <base>.SerDes4RdData11To08 <base>.SerDes4RdData15To12
 * <base>.SerDes4RdData19To16 <base>.SerDes4RdData23To20
 */
#define NFP_MAC_CSR_SERDES4_RDDATA_03_00                   0x07e0
#define NFP_MAC_CSR_SERDES4_RDDATA_07_04                   0x07e4
#define NFP_MAC_CSR_SERDES4_RDDATA_11_08                   0x07e8
#define NFP_MAC_CSR_SERDES4_RDDATA_15_12                   0x07ec
#define NFP_MAC_CSR_SERDES4_RDDATA_19_16                   0x07f0
#define NFP_MAC_CSR_SERDES4_RDDATA_23_20                   0x07f4
#define   NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR(x)            (((x) & 7) << 29)
#define   NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_of(x)         (((x) >> 29) & 7)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_LANE0       (0)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_LANE1       (1)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_LANE2       (2)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_LANE3       (3)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_COMMON      (4)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_ALL         (7)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_bf          0, 31, 29
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_msk         (0x7)
#define     NFP_MAC_CSR_SERDES4_RDDATA_PAGE_ADDR_shf         (29)
#define   NFP_MAC_CSR_SERDES4_RDDATA_OFF(x)                  (((x) & 0x1fff) << 16)
#define   NFP_MAC_CSR_SERDES4_RDDATA_OFF_of(x)               (((x) >> 16) & 0x1fff)
#define     NFP_MAC_CSR_SERDES4_RDDATA_OFF_bf                0, 28, 16
#define     NFP_MAC_CSR_SERDES4_RDDATA_OFF_msk               (0x1fff)
#define     NFP_MAC_CSR_SERDES4_RDDATA_OFF_shf               (16)
#define   NFP_MAC_CSR_SERDES4_RDDATA_VALID                   (1 << 9)
#define     NFP_MAC_CSR_SERDES4_RDDATA_VALID_bf              0, 9, 9
#define     NFP_MAC_CSR_SERDES4_RDDATA_VALID_msk             (0x1)
#define     NFP_MAC_CSR_SERDES4_RDDATA_VALID_bit             (9)
#define   NFP_MAC_CSR_SERDES4_RDDATA_DATA(x)                 (((x) & 0xff) << 0)
#define   NFP_MAC_CSR_SERDES4_RDDATA_DATA_of(x)              (((x) >> 0) & 0xff)
#define     NFP_MAC_CSR_SERDES4_RDDATA_DATA_bf               0, 7, 0
#define     NFP_MAC_CSR_SERDES4_RDDATA_DATA_msk              (0xff)
#define     NFP_MAC_CSR_SERDES4_RDDATA_DATA_shf              (0)


/*
 * Register: TdmMemRdData
 *   [29:24]   TdmMemRdAddr
 *   [21]      TdmMemRdDataValid
 *   [15]      TdmPortArbEnable
 *   [14:0]    TdmPortWeightRdData
 *
 * Name(s):
 * <base>.IgDqTdmMemoryRdData
 */
#define NFP_MAC_CSR_TDM_MEM_RDDATA                         0x07f8
#define   NFP_MAC_CSR_TDM_MEM_RDDATA_ADDR(x)                 (((x) & 0x3f) << 24)
#define   NFP_MAC_CSR_TDM_MEM_RDDATA_ADDR_of(x)              (((x) >> 24) & 0x3f)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_ADDR_bf               0, 29, 24
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_ADDR_msk              (0x3f)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_ADDR_shf              (24)
#define   NFP_MAC_CSR_TDM_MEM_RDDATA_VALID                   (1 << 21)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_VALID_bf              0, 21, 21
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_VALID_msk             (0x1)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_VALID_bit             (21)
#define   NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_ARB_ENABLE         (1 << 15)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_ARB_ENABLE_bf    0, 15, 15
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_ARB_ENABLE_msk   (0x1)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_ARB_ENABLE_bit   (15)
#define   NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_WEIGHTRD(x)        (((x) & 0x7fff) << 0)
#define   NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_WEIGHTRD_of(x)     (((x) >> 0) & 0x7fff)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_WEIGHTRD_bf      0, 14, 0
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_WEIGHTRD_msk     (0x7fff)
#define     NFP_MAC_CSR_TDM_MEM_RDDATA_PORT_WEIGHTRD_shf     (0)


/*
 * Register: SerDesPdLn
 *   [23:0]    SerDesLanePowerDown
 *
 * Name(s):
 * <base>.SerDesPdRx <base>.SerDesPdTx
 */
#define NFP_MAC_CSR_SERDES_PD_RX                           0x0800
#define NFP_MAC_CSR_SERDES_PD_TX                           0x0804
#define   NFP_MAC_CSR_SERDES_PD_VAL(x)                       (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_SERDES_PD_VAL_of(x)                    (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_SERDES_PD_VAL_bf                     0, 23, 0
#define     NFP_MAC_CSR_SERDES_PD_VAL_msk                    (0xffffff)
#define     NFP_MAC_CSR_SERDES_PD_VAL_shf                    (0)


/*
 * Register: SerDesPdSy
 *   [5:0]     SerDesSynthPowerDown
 *
 * Name(s):
 * <base>.SerDesPdSy
 */
#define NFP_MAC_CSR_SERDES_PD_SY                           0x0808
#define   NFP_MAC_CSR_SERDES_PD_SY_VAL(x)                    (((x) & 0x3f) << 0)
#define   NFP_MAC_CSR_SERDES_PD_SY_VAL_of(x)                 (((x) >> 0) & 0x3f)
#define     NFP_MAC_CSR_SERDES_PD_SY_VAL_bf                  0, 5, 0
#define     NFP_MAC_CSR_SERDES_PD_SY_VAL_msk                 (0x3f)
#define     NFP_MAC_CSR_SERDES_PD_SY_VAL_shf                 (0)


/*
 * Register: SerDesCkMuxSel
 *   [31:30]   SerDesCkMuxSel_RFU
 *   [29]      SerDesCkMuxSelGang2320
 *   [28]      SerDesCkMuxSelL23
 *   [27]      SerDesCkMuxSelL22
 *   [26]      SerDesCkMuxSelL21
 *   [25]      SerDesCkMuxSelL20
 *   [24]      SerDesCkMuxSelGang1916
 *   [23]      SerDesCkMuxSelL19
 *   [22]      SerDesCkMuxSelL18
 *   [21]      SerDesCkMuxSelL17
 *   [20]      SerDesCkMuxSelL16
 *   [19]      SerDesCkMuxSelGang1512
 *   [18]      SerDesCkMuxSelL15
 *   [17]      SerDesCkMuxSelL14
 *   [16]      SerDesCkMuxSelL13
 *   [15]      SerDesCkMuxSelL12
 *   [14]      SerDesCkMuxSelGang1108
 *   [13]      SerDesCkMuxSelL11
 *   [12]      SerDesCkMuxSelL10
 *   [11]      SerDesCkMuxSelL09
 *   [10]      SerDesCkMuxSelL08
 *   [9]       SerDesCkMuxSelGang0704
 *   [8]       SerDesCkMuxSelL07
 *   [7]       SerDesCkMuxSelL06
 *   [6]       SerDesCkMuxSelL05
 *   [5]       SerDesCkMuxSelL04
 *   [4]       SerDesCkMuxSelGang0300
 *   [3]       SerDesCkMuxSelL03
 *   [2]       SerDesCkMuxSelL02
 *   [1]       SerDesCkMuxSelL01
 *   [0]       SerDesCkMuxSelL00
 *
 * Name(s):
 * <base>.SerDesCkMuxSel
 */
#define NFP_MAC_CSR_SERDES_CK_MUX_SEL                      0x080c
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_RFU(x)               (((x) & 3) << 30)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_RFU_of(x)            (((x) >> 30) & 3)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_RFU_bf             0, 31, 30
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_RFU_msk            (0x3)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_RFU_shf            (30)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_2320            (1 << 29)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_2320_bf       0, 29, 29
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_2320_msk      (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_2320_bit      (29)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE23               (1 << 28)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE23_bf          0, 28, 28
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE23_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE23_bit         (28)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE22               (1 << 27)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE22_bf          0, 27, 27
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE22_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE22_bit         (27)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE21               (1 << 26)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE21_bf          0, 26, 26
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE21_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE21_bit         (26)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE20               (1 << 25)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE20_bf          0, 25, 25
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE20_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE20_bit         (25)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_1916                 (1 << 24)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_1916_bf            0, 24, 24
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_1916_msk           (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_1916_bit           (24)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE19               (1 << 23)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE19_bf          0, 23, 23
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE19_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE19_bit         (23)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE18               (1 << 22)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE18_bf          0, 22, 22
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE18_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE18_bit         (22)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE17               (1 << 21)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE17_bf          0, 21, 21
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE17_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE17_bit         (21)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE16               (1 << 20)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE16_bf          0, 20, 20
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE16_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE16_bit         (20)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1512            (1 << 19)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1512_bf       0, 19, 19
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1512_msk      (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1512_bit      (19)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE15               (1 << 18)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE15_bf          0, 18, 18
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE15_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE15_bit         (18)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE14               (1 << 17)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE14_bf          0, 17, 17
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE14_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE14_bit         (17)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE13               (1 << 16)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE13_bf          0, 16, 16
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE13_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE13_bit         (16)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE12               (1 << 15)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE12_bf          0, 15, 15
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE12_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE12_bit         (15)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1108            (1 << 14)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1108_bf       0, 14, 14
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1108_msk      (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_1108_bit      (14)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE11               (1 << 13)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE11_bf          0, 13, 13
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE11_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE11_bit         (13)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE10               (1 << 12)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE10_bf          0, 12, 12
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE10_msk         (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE10_bit         (12)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE9                (1 << 11)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE9_bf           0, 11, 11
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE9_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE9_bit          (11)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE8                (1 << 10)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE8_bf           0, 10, 10
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE8_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE8_bit          (10)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0704            (1 << 9)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0704_bf       0, 9, 9
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0704_msk      (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0704_bit      (9)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE7                (1 << 8)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE7_bf           0, 8, 8
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE7_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE7_bit          (8)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE6                (1 << 7)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE6_bf           0, 7, 7
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE6_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE6_bit          (7)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE5                (1 << 6)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE5_bf           0, 6, 6
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE5_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE5_bit          (6)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE4                (1 << 5)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE4_bf           0, 5, 5
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE4_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE4_bit          (5)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0300            (1 << 4)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0300_bf       0, 4, 4
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0300_msk      (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_GANG_0300_bit      (4)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE3                (1 << 3)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE3_bf           0, 3, 3
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE3_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE3_bit          (3)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE2                (1 << 2)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE2_bf           0, 2, 2
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE2_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE2_bit          (2)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE1                (1 << 1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE1_bf           0, 1, 1
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE1_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE1_bit          (1)
#define   NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE0                (1 << 0)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE0_bf           0, 0, 0
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE0_msk          (0x1)
#define     NFP_MAC_CSR_SERDES_CK_MUX_SEL_LANE0_bit          (0)


/*
 * Register: SerDesSigDetect
 *   [23:0]    SerDesLaneSigDetect
 *
 * Name(s):
 * <base>.SerDesSigDetect
 */
#define NFP_MAC_CSR_SERDES_SIG_DETECT                      0x0810
#define   NFP_MAC_CSR_SERDES_SIG_DETECT_VAL(x)               (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_SERDES_SIG_DETECT_VAL_of(x)            (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_SERDES_SIG_DETECT_VAL_bf             0, 23, 0
#define     NFP_MAC_CSR_SERDES_SIG_DETECT_VAL_msk            (0xffffff)
#define     NFP_MAC_CSR_SERDES_SIG_DETECT_VAL_shf            (0)


/*
 * Register: SerDesSigDetectOvr
 *   [23:0]    SerDesLaneSigDetectOvr
 *
 * Name(s):
 * <base>.SerDesSigDetectOvr
 */
#define NFP_MAC_CSR_SERDES_SIG_DETECT_OVR                  0x0814
#define   NFP_MAC_CSR_SERDES_SIG_DETECT_OVR_VAL(x)           (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_SERDES_SIG_DETECT_OVR_VAL_of(x)        (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_SERDES_SIG_DETECT_OVR_VAL_bf         0, 23, 0
#define     NFP_MAC_CSR_SERDES_SIG_DETECT_OVR_VAL_msk        (0xffffff)
#define     NFP_MAC_CSR_SERDES_SIG_DETECT_OVR_VAL_shf        (0)


/*
 * Register: SerDesActDetect
 *   [23:0]    SerDesPortActDetect
 *
 * Name(s):
 * <base>.SerDesEthRxActDetect <base>.SerDesEthTxActDetect
 */
#define NFP_MAC_CSR_SERDES_ETH_RX_ACT_DETECT               0x0818
#define NFP_MAC_CSR_SERDES_ETH_TX_ACT_DETECT               0x081c
#define   NFP_MAC_CSR_SERDES_ETH_RX_ACT_DETECT_VAL(x)        (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_SERDES_ETH_RX_ACT_DETECT_VAL_of(x)     (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_SERDES_ETH_RX_ACT_DETECT_VAL_bf      0, 23, 0
#define     NFP_MAC_CSR_SERDES_ETH_RX_ACT_DETECT_VAL_msk     (0xffffff)
#define     NFP_MAC_CSR_SERDES_ETH_RX_ACT_DETECT_VAL_shf     (0)


/*
 * Register: SerDesLinkUp
 *   [23:0]    SerDesLInkUp
 *
 * Name(s):
 * <base>.SerDesLinkUp
 */
#define NFP_MAC_CSR_SERDES_LINK_UP                         0x0820
#define   NFP_MAC_CSR_SERDES_LINK_UP_VAL(x)                  (((x) & 0xffffff) << 0)
#define   NFP_MAC_CSR_SERDES_LINK_UP_VAL_of(x)               (((x) >> 0) & 0xffffff)
#define     NFP_MAC_CSR_SERDES_LINK_UP_VAL_bf                0, 23, 0
#define     NFP_MAC_CSR_SERDES_LINK_UP_VAL_msk               (0xffffff)
#define     NFP_MAC_CSR_SERDES_LINK_UP_VAL_shf               (0)


/*
 * Register: ParityErrInject
 *   [5]       InjectIgParErrDescMem
 *   [4]       InjectIgParErrTsmpMem
 *   [3]       InjectIgParErrRsltMem
 *   [2]       InjectEgParErrDescMem
 *   [1]       InjectEgParErrRslt1Mem
 *   [0]       InjectEgParErrRslt0Mem
 *
 * Name(s):
 * <base>.ParityErrInject
 */
#define NFP_MAC_CSR_PARITY_ERR_INJECT                      0x0824
#define   NFP_MAC_CSR_PARITY_ERR_INJECT_IG_DESC_MEM          (1 << 5)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_DESC_MEM_bf     0, 5, 5
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_DESC_MEM_msk    (0x1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_DESC_MEM_bit    (5)
#define   NFP_MAC_CSR_PARITY_ERR_INJECT_IG_TSMP_MEM          (1 << 4)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_TSMP_MEM_bf     0, 4, 4
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_TSMP_MEM_msk    (0x1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_TSMP_MEM_bit    (4)
#define   NFP_MAC_CSR_PARITY_ERR_INJECT_IG_RSLT_MEM          (1 << 3)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_RSLT_MEM_bf     0, 3, 3
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_RSLT_MEM_msk    (0x1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_IG_RSLT_MEM_bit    (3)
#define   NFP_MAC_CSR_PARITY_ERR_INJECT_EG_DESC_MEM          (1 << 2)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_DESC_MEM_bf     0, 2, 2
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_DESC_MEM_msk    (0x1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_DESC_MEM_bit    (2)
#define   NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT1_MEM         (1 << 1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT1_MEM_bf    0, 1, 1
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT1_MEM_msk   (0x1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT1_MEM_bit   (1)
#define   NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT0_MEM         (1 << 0)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT0_MEM_bf    0, 0, 0
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT0_MEM_msk   (0x1)
#define     NFP_MAC_CSR_PARITY_ERR_INJECT_EG_RSLT0_MEM_bit   (0)


/*
 * Register: IgParityErrStatus
 *   [14:12]   IgParErrType
 *   [10:0]    IgParErrAddr
 *
 * Name(s):
 * <base>.IgParityErrStatus
 */
#define NFP_MAC_CSR_IG_PARITY_ERR_STATUS                   0x0840
#define   NFP_MAC_CSR_IG_PARITY_ERR_STATUS_TYPE(x)           (((x) & 7) << 12)
#define   NFP_MAC_CSR_IG_PARITY_ERR_STATUS_TYPE_of(x)        (((x) >> 12) & 7)
#define     NFP_MAC_CSR_IG_PARITY_ERR_STATUS_TYPE_bf         0, 14, 12
#define     NFP_MAC_CSR_IG_PARITY_ERR_STATUS_TYPE_msk        (0x7)
#define     NFP_MAC_CSR_IG_PARITY_ERR_STATUS_TYPE_shf        (12)
#define   NFP_MAC_CSR_IG_PARITY_ERR_STATUS_ADDR(x)           (((x) & 0x7ff) << 0)
#define   NFP_MAC_CSR_IG_PARITY_ERR_STATUS_ADDR_of(x)        (((x) >> 0) & 0x7ff)
#define     NFP_MAC_CSR_IG_PARITY_ERR_STATUS_ADDR_bf         0, 10, 0
#define     NFP_MAC_CSR_IG_PARITY_ERR_STATUS_ADDR_msk        (0x7ff)
#define     NFP_MAC_CSR_IG_PARITY_ERR_STATUS_ADDR_shf        (0)


/*
 * Register: EgParityErrStatus
 *   [30:28]   EgParErrType1
 *   [25:16]   EgParErrAddr1
 *   [14:12]   EgParErrType0
 *   [9:0]     EgParErrAddr0
 *
 * Name(s):
 * <base>.EgParityErrStatus
 */
#define NFP_MAC_CSR_EG_PARITY_ERR_STATUS                   0x0844
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE1(x)          (((x) & 7) << 28)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE1_of(x)       (((x) >> 28) & 7)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE1_bf        0, 30, 28
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE1_msk       (0x7)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE1_shf       (28)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR1(x)          (((x) & 0x3ff) << 16)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR1_of(x)       (((x) >> 16) & 0x3ff)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR1_bf        0, 25, 16
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR1_msk       (0x3ff)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR1_shf       (16)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE0(x)          (((x) & 7) << 12)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE0_of(x)       (((x) >> 12) & 7)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE0_bf        0, 14, 12
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE0_msk       (0x7)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_TYPE0_shf       (12)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR0(x)          (((x) & 0x3ff) << 0)
#define   NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR0_of(x)       (((x) >> 0) & 0x3ff)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR0_bf        0, 9, 0
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR0_msk       (0x3ff)
#define     NFP_MAC_CSR_EG_PARITY_ERR_STATUS_ADDR0_shf       (0)


/*
 * Register: MemErrDropCounts
 *   [31:24]   IgMemErrDrop1
 *   [23:16]   IgMemErrDrop0
 *   [15:8]    EgMemErrDrop1
 *   [7:0]     EgMemErrDrop0
 *
 * Name(s):
 * <base>.MemErrDropCounts
 */
#define NFP_MAC_CSR_MEM_ERR_DROP_COUNTS                    0x0848
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_1(x)            (((x) & 0xff) << 24)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_1_of(x)         (((x) >> 24) & 0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_1_bf          0, 31, 24
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_1_msk         (0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_1_shf         (24)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_0(x)            (((x) & 0xff) << 16)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_0_of(x)         (((x) >> 16) & 0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_0_bf          0, 23, 16
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_0_msk         (0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_IG_0_shf         (16)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_1(x)            (((x) & 0xff) << 8)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_1_of(x)         (((x) >> 8) & 0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_1_bf          0, 15, 8
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_1_msk         (0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_1_shf         (8)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_0(x)            (((x) & 0xff) << 0)
#define   NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_0_of(x)         (((x) >> 0) & 0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_0_bf          0, 7, 0
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_0_msk         (0xff)
#define     NFP_MAC_CSR_MEM_ERR_DROP_COUNTS_EG_0_shf         (0)


/*
 * Register: AssertConfig0
 *
 * Name(s):
 * <base>.AssertConfig0
 */
#define NFP_MAC_CSR_ASSERT_CONFIG_CSR0                     0x084c

/*
 * Register: AssertConfig1
 *   [31:18]   AssertConfigCsr1Disable
 *   [17]      AssertConfigCsr1EnaFsm1
 *   [16]      AssertConfigCsr1EnaFsm0
 *   [15:8]    AssertConfigCsr1FsmCfg1
 *   [7:0]     AssertConfigCsr1FsmCfg0
 *
 * Name(s):
 * <base>.AssertConfig1
 */
#define NFP_MAC_CSR_ASSERT_CONFIG_CSR1                     0x0850
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_DISABLE(x)          (((x) & 0x3fff) << 18)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_DISABLE_of(x)       (((x) >> 18) & 0x3fff)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_DISABLE_bf        0, 31, 18
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_DISABLE_msk       (0x3fff)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_DISABLE_shf       (18)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM1                (1 << 17)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM1_bf           0, 17, 17
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM1_msk          (0x1)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM1_bit          (17)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM0                (1 << 16)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM0_bf           0, 16, 16
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM0_msk          (0x1)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM0_bit          (16)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG1(x)         (((x) & 0xff) << 8)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG1_of(x)      (((x) >> 8) & 0xff)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG1_bf       0, 15, 8
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG1_msk      (0xff)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG1_shf      (8)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG0(x)         (((x) & 0xff) << 0)
#define   NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG0_of(x)      (((x) >> 0) & 0xff)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG0_bf       0, 7, 0
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG0_msk      (0xff)
#define     NFP_MAC_CSR_ASSERT_CONFIG_CSR1_FSM_CFG0_shf      (0)


/*
 * Macros for Mac statistics
 */

/*
 * Register: StatCounter32
 *
 * Name(s):
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPIfInOctetsLo...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxFrameTooLongErrors...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxInRangeLengthErrors...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxVlanReceivedOK...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPIfInErrors...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPIfInBroadCastPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsDropEvents...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxAlignmentErrors...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPauseMacCtlFramesReceived...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxFramesReceivedOK...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxFrameCheckSequenceErrors...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPIfInUniCastPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPIfInMultiCastPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsUndersizePkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts64octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts65to127octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts512to1023octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts1024to1518octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsJabbers...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsFragments...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived2...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived3...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts128to255octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts256to511octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsPkts1519toMaxoctets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPStatsOversizePkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived0...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived1...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived4...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived5...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived6...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxCBFCPauseFramesReceived7...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxMacCtlFramesReceived...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPIfOutOctetsLo...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxVlanTransmittedOK...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPIfOutErrors...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPIfOutBroadCastPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts64octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts256to511octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts512to1023octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPauseMacCtlFramesTransmitted...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxFramesTransmittedOK...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPIfOutUniCastPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPIfOutMultiCastPkts...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts65to127octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts128to255octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts1024to1518octets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPStatsPkts1518toMAXoctets...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted0...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted1...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted4...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted5...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted2...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted3...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted6...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxCBFCPauseFramesTransmitted7...
 * <base>.MacChannelStats0.RxCIfInOctetsLo...
 * <base>.MacChannelStats0.RxCStatsOctetsLo...
 * <base>.MacChannelStats0.RxCIfInErrors...
 * <base>.MacChannelStats0.RxCIfInUniCastPkts...
 * <base>.MacChannelStats0.RxCIfInMultiCastPkts...
 * <base>.MacChannelStats0.RxCIfInBroadCastPkts...
 * <base>.MacChannelStats0.RxCStatsPkts...
 * <base>.MacChannelStats0.RxCStatsPkts64octets...
 * <base>.MacChannelStats0.RxCStatsPkts65to127octets...
 * <base>.MacChannelStats0.RxCStatsPkts128to255octets...
 * <base>.MacChannelStats0.RxCStatsPkts256to511octets...
 * <base>.MacChannelStats0.RxCStatsPkts512to1023octets...
 * <base>.MacChannelStats0.RxCStatsPkts1024to1518octets...
 * <base>.MacChannelStats0.RxCStatsPkts1519toMaxoctets...
 * <base>.MacChannelStats0.RxChanFramesReceivedOK...
 * <base>.MacChannelStats0.RxChanVlanReceivedOK...
 * <base>.MacChannelStats0.TxCIfOutOctetsLo...
 * <base>.MacChannelStats0.TxCIfOutErrors...
 * <base>.MacChannelStats0.TxCIfOutUniCastPkts...
 * <base>.MacChannelStats0.TxChanFramesTransmittedOK...
 * <base>.MacChannelStats0.TxChanVlanTransmittedOK...
 * <base>.MacChannelStats0.TxCIfOutMultiCastPkts...
 * <base>.MacChannelStats0.TxCIfOutBroadCastPkts...
 */
#define NFP_MAC_STATS_PORT_SEG_RX_OCTS_LO(x)               (0x0000 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_TOO_LONG_ERRS(x)  (0x0008 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_IN_RANGE_LEN_ERRS(x)     (0x000c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_VLAN_FRAMES_OK(x)        (0x0010 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAME_ERRS(x)            (0x0014 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_BROADCAST_FRAMES(x)      (0x0018 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAME_DROP_ERRS(x)       (0x001c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAME_ALIGN_ERRS(x)      (0x0020 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_PAUSE_FRAMES(x)          (0x0024 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_OK(x)             (0x0028 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAME_CRC_ERRS(x)        (0x002c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_UNICAST_FRAMES(x)        (0x0030 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_MULTICAST_FRAMES(x)      (0x0034 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES(x)                (0x0038 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_UNDERSIZE_FRAMES(x)      (0x003c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_64_OCTS(x)        (0x0040 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_65_127_OCTS(x)    (0x0044 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_512_1023_OCTS(x)  (0x0048 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_1024_1518_OCTS(x) (0x004c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_JABBER_FRAMES(x)         (0x0050 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAME_FRAGMENTS(x)       (0x0054 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_2(x)   (0x0058 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_3(x)   (0x005c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_128_255_OCTS(x)   (0x0060 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_256_511_OCTS(x)   (0x0064 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_FRAMES_1519_MAX_OCTS(x)  (0x0068 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_OVERSIZE_FRAMES(x)       (0x006c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_0(x)   (0x0070 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_1(x)   (0x0074 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_4(x)   (0x0078 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_5(x)   (0x007c + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_6(x)   (0x0080 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CBFC_PAUSE_FRAMES_7(x)   (0x0084 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_RX_CTL_FRAMES(x)            (0x0088 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_OCTS_LO(x)               (0x00a0 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_VLAN_FRAMES_OK(x)        (0x00a8 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAME_ERRS(x)            (0x00ac + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_BROADCAST_FRAMES(x)      (0x00b0 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_64_OCTS(x)        (0x00b4 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_256_511_OCTS(x)   (0x00b8 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_512_1023_OCTS(x)  (0x00bc + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_PAUSE_FRAMES(x)          (0x00c0 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_OK(x)             (0x00c4 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_UNICAST_FRAMES(x)        (0x00c8 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_MULTICAST_FRAMES(x)      (0x00cc + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_65_127_OCTS(x)    (0x00d0 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_128_255_OCTS(x)   (0x00d4 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_1024_1518_OCTS(x) (0x00d8 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_FRAMES_1519_MAX_OCTS(x)  (0x00dc + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_0(x)   (0x00e0 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_1(x)   (0x00e4 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_4(x)   (0x00e8 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_5(x)   (0x00ec + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_2(x)   (0x00f0 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_3(x)   (0x00f4 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_6(x)   (0x00f8 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_CBFC_PAUSE_FRAMES_7(x)   (0x00fc + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_OCTS_LO(x)                   (0x10000 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_OCTS_OK_LO(x)                (0x10008 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAME_ERRS(x)                (0x10010 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_UNICAST_FRAMES(x)            (0x10014 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_MULTICAST_FRAMES(x)          (0x10018 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_BROADCAST_FRAMES(x)          (0x1001c + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES(x)                    (0x10020 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_64_OCTS(x)            (0x10024 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_65_127_OCTS(x)        (0x10028 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_128_255_OCTS(x)       (0x1002c + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_256_511_OCTS(x)       (0x10030 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_512_1023_OCTS(x)      (0x10034 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_1024_1518_OCTS(x)     (0x10038 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_FRAMES_1519_MAX_OCTS(x)      (0x1003c + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_CHAN_FRAMES_RECEIVED_OK(x)   (0x10040 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_VLAN_FRAMES_OK(x)            (0x10044 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_OCTS_LO(x)                   (0x10060 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_FRAME_ERRS(x)                (0x10068 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_UNICAST_FRAMES(x)            (0x1006c + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_FRAMES_OK(x)                 (0x10070 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_VLAN_FRAMES_OK(x)            (0x10074 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_MULTICAST_FRAMES(x)          (0x10078 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_BROADCAST_FRAMES(x)          (0x1007c + ((x) * 0x100))

/*
 * Register: StatCounter8
 *   [7:0]     StatCounter8
 *
 * Name(s):
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.RxPIfInOctetsHi...
 * <base>.MacEthPortStatsHyd0.MacEthPortStatsSeg0.TxPIfOutOctetsHi...
 * <base>.MacChannelStats0.RxCIfInOctetsHi...
 * <base>.MacChannelStats0.RxCStatsOctetsHi...
 * <base>.MacChannelStats0.TxCIfOutOctetsHi...
 */
#define NFP_MAC_STATS_PORT_SEG_RX_OCTS_HI(x)               (0x0004 + ((x) * 0x100))
#define NFP_MAC_STATS_PORT_SEG_TX_OCTS_HI(x)               (0x00a4 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_OCTS_HI(x)                   (0x10004 + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_RX_OCTS_OK_HI(x)                (0x1000c + ((x) * 0x100))
#define NFP_MAC_STATS_CHNL_TX_OCTS_HI(x)                   (0x10064 + ((x) * 0x100))
#define   NFP_MAC_STATS_PORT_SEG_COUNTER8_VAL(x)             (((x) & 0xff) << 0)
#define   NFP_MAC_STATS_PORT_SEG_COUNTER8_VAL_of(x)          (((x) >> 0) & 0xff)
#define     NFP_MAC_STATS_PORT_SEG_COUNTER8_VAL_bf           0, 7, 0
#define     NFP_MAC_STATS_PORT_SEG_COUNTER8_VAL_msk          (0xff)
#define     NFP_MAC_STATS_PORT_SEG_COUNTER8_VAL_shf          (0)



#endif /* !_NFP6000__NFP_MAC_H_ */
