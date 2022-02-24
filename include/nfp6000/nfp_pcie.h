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
 * @file          nfp6000/nfp_pcie.h
 * @brief         NFP6000 PCIe CSR definitions
 *
 * This files is auto-generated. Do not edit!
 */

#ifndef _NFP6000__NFP_PCIE_H_
#define _NFP6000__NFP_PCIE_H_


/*
 * PCIe CSRs
 */
#define NFP_PCIE_SRAM_BASE                                 0x0000
#define NFP_PCIE_SRAM_SIZE                                 0x10000
/* InterruptManagerMap: <base>.InterruptManager */
#define NFP_PCIE_IM                                        0x10000
/* EventManagerMap: <base>.EventManager */
#define NFP_PCIE_EM                                        0x20000

/*
 * Register: PcieToCppExpansionBar
 *   [31:29]   MapType
 *   [28:27]   LengthSelect
 *   [26:23]   Target_BaseAddress
 *   [22:21]   Token_BaseAddress
 *   [20:16]   Action_BaseAddress
 *   [15:0]    BaseAddress
 *
 * Name(s):
 * <base>.BARConfiguration.PCIeToCppExpansionBAR0_0...
 */
#define NFP_PCIE_BARCFG_P2C(_bar, _sub)                    (0x30000 + ((_bar) * 0x20) + ((_sub) * 0x4))
#define   NFP_PCIE_BARCFG_P2C_MAP_TYPE(x)                    (((x) & 7) << 29)
#define   NFP_PCIE_BARCFG_P2C_MAP_TYPE_of(x)                 (((x) >> 29) & 7)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_FIXED               (0)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_BULK                (1)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_TARGET              (2)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_GENERAL             (3)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_EXPLICIT0           (4)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_EXPLICIT1           (5)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_EXPLICIT2           (6)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_EXPLICIT3           (7)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_bf                  0, 31, 29
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_msk                 (0x7)
#define     NFP_PCIE_BARCFG_P2C_MAP_TYPE_shf                 (29)
#define   NFP_PCIE_BARCFG_P2C_LEN(x)                         (((x) & 3) << 27)
#define   NFP_PCIE_BARCFG_P2C_LEN_of(x)                      (((x) >> 27) & 3)
#define     NFP_PCIE_BARCFG_P2C_LEN_32BIT                    (0)
#define     NFP_PCIE_BARCFG_P2C_LEN_64BIT                    (1)
#define     NFP_PCIE_BARCFG_P2C_LEN_RSVD                     (2)
#define     NFP_PCIE_BARCFG_P2C_LEN_0BYTE                    (3)
#define     NFP_PCIE_BARCFG_P2C_LEN_bf                       0, 28, 27
#define     NFP_PCIE_BARCFG_P2C_LEN_msk                      (0x3)
#define     NFP_PCIE_BARCFG_P2C_LEN_shf                      (27)
#define   NFP_PCIE_BARCFG_P2C_TARGET(x)                      (((x) & 0xf) << 23)
#define   NFP_PCIE_BARCFG_P2C_TARGET_of(x)                   (((x) >> 23) & 0xf)
#define     NFP_PCIE_BARCFG_P2C_TARGET_bf                    0, 26, 23
#define     NFP_PCIE_BARCFG_P2C_TARGET_msk                   (0xf)
#define     NFP_PCIE_BARCFG_P2C_TARGET_shf                   (23)
#define   NFP_PCIE_BARCFG_P2C_TOKEN(x)                       (((x) & 3) << 21)
#define   NFP_PCIE_BARCFG_P2C_TOKEN_of(x)                    (((x) >> 21) & 3)
#define     NFP_PCIE_BARCFG_P2C_TOKEN_bf                     0, 22, 21
#define     NFP_PCIE_BARCFG_P2C_TOKEN_msk                    (0x3)
#define     NFP_PCIE_BARCFG_P2C_TOKEN_shf                    (21)
#define   NFP_PCIE_BARCFG_P2C_ACTADDR(x)                     (((x) & 0x1f) << 16)
#define   NFP_PCIE_BARCFG_P2C_ACTADDR_of(x)                  (((x) >> 16) & 0x1f)
#define     NFP_PCIE_BARCFG_P2C_ACTADDR_bf                   0, 20, 16
#define     NFP_PCIE_BARCFG_P2C_ACTADDR_msk                  (0x1f)
#define     NFP_PCIE_BARCFG_P2C_ACTADDR_shf                  (16)
#define   NFP_PCIE_BARCFG_P2C_BASE(x)                        (((x) & 0xffff) << 0)
#define   NFP_PCIE_BARCFG_P2C_BASE_of(x)                     (((x) >> 0) & 0xffff)
#define     NFP_PCIE_BARCFG_P2C_BASE_bf                      0, 15, 0
#define     NFP_PCIE_BARCFG_P2C_BASE_msk                     (0xffff)
#define     NFP_PCIE_BARCFG_P2C_BASE_shf                     (0)


/*
 * Register: PcieToCppExpansionBarVf
 *   [31:30]   LengthSelect
 *   [29:26]   Target
 *   [25:24]   Token
 *   [18:0]    BaseAddress
 *
 * Name(s):
 * <base>.BARConfiguration.PCIeToCppExpansionBARVf0...
 */
#define NFP_PCIE_BARCFG_VF_P2C(_bar)                       (0x30060 + ((_bar) * 0x4))
#define   NFP_PCIE_BARCFG_VF_P2C_LEN(x)                      (((x) & 3) << 30)
#define   NFP_PCIE_BARCFG_VF_P2C_LEN_of(x)                   (((x) >> 30) & 3)
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_32BIT                 (0)
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_64BIT                 (1)
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_RSVD                  (2)
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_0BYTE                 (3)
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_bf                    0, 31, 30
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_msk                   (0x3)
#define     NFP_PCIE_BARCFG_VF_P2C_LEN_shf                   (30)
#define   NFP_PCIE_BARCFG_VF_P2C_TARGET(x)                   (((x) & 0xf) << 26)
#define   NFP_PCIE_BARCFG_VF_P2C_TARGET_of(x)                (((x) >> 26) & 0xf)
#define     NFP_PCIE_BARCFG_VF_P2C_TARGET_bf                 0, 29, 26
#define     NFP_PCIE_BARCFG_VF_P2C_TARGET_msk                (0xf)
#define     NFP_PCIE_BARCFG_VF_P2C_TARGET_shf                (26)
#define   NFP_PCIE_BARCFG_VF_P2C_TOKEN(x)                    (((x) & 3) << 24)
#define   NFP_PCIE_BARCFG_VF_P2C_TOKEN_of(x)                 (((x) >> 24) & 3)
#define     NFP_PCIE_BARCFG_VF_P2C_TOKEN_bf                  0, 25, 24
#define     NFP_PCIE_BARCFG_VF_P2C_TOKEN_msk                 (0x3)
#define     NFP_PCIE_BARCFG_VF_P2C_TOKEN_shf                 (24)
#define   NFP_PCIE_BARCFG_VF_P2C_BASE(x)                     (((x) & 0x7ffff) << 0)
#define   NFP_PCIE_BARCFG_VF_P2C_BASE_of(x)                  (((x) >> 0) & 0x7ffff)
#define     NFP_PCIE_BARCFG_VF_P2C_BASE_bf                   0, 18, 0
#define     NFP_PCIE_BARCFG_VF_P2C_BASE_msk                  (0x7ffff)
#define     NFP_PCIE_BARCFG_VF_P2C_BASE_shf                  (0)


/*
 * Register: PcieExplicitCmdBar0
 *   [31:30]   SignalType
 *   [29:28]   Token
 *   [23:0]    Address
 *
 * Name(s):
 * <base>.BARConfiguration.PCIeExplicitCmd0_0.PCIeExplicitCmdBar0...
 */
#define NFP_PCIE_BARCFG_P2CX0(_set, _num)                  (0x30080 + ((_set) * 0x40) + ((_num) * 0x10))
#define   NFP_PCIE_BARCFG_P2CX0_SIGTYPE(x)                   (((x) & 3) << 30)
#define   NFP_PCIE_BARCFG_P2CX0_SIGTYPE_of(x)                (((x) >> 30) & 3)
#define     NFP_PCIE_BARCFG_P2CX0_SIGTYPE_bf                 0, 31, 30
#define     NFP_PCIE_BARCFG_P2CX0_SIGTYPE_msk                (0x3)
#define     NFP_PCIE_BARCFG_P2CX0_SIGTYPE_shf                (30)
#define   NFP_PCIE_BARCFG_P2CX0_TOK(x)                       (((x) & 3) << 28)
#define   NFP_PCIE_BARCFG_P2CX0_TOK_of(x)                    (((x) >> 28) & 3)
#define     NFP_PCIE_BARCFG_P2CX0_TOK_bf                     0, 29, 28
#define     NFP_PCIE_BARCFG_P2CX0_TOK_msk                    (0x3)
#define     NFP_PCIE_BARCFG_P2CX0_TOK_shf                    (28)
#define   NFP_PCIE_BARCFG_P2CX0_ADDR(x)                      (((x) & 0xffffff) << 0)
#define   NFP_PCIE_BARCFG_P2CX0_ADDR_of(x)                   (((x) >> 0) & 0xffffff)
#define     NFP_PCIE_BARCFG_P2CX0_ADDR_bf                    0, 23, 0
#define     NFP_PCIE_BARCFG_P2CX0_ADDR_msk                   (0xffffff)
#define     NFP_PCIE_BARCFG_P2CX0_ADDR_shf                   (0)


/*
 * Register: PcieExplicitCmdBar1
 *   [30:24]   SignalRef
 *   [23:14]   DataMaster
 *   [13:0]    DataRef
 *
 * Name(s):
 * <base>.BARConfiguration.PCIeExplicitCmd0_0.PCIeExplicitCmdBAR1...
 */
#define NFP_PCIE_BARCFG_P2CX1(_set, _num)                  (0x30084 + ((_set) * 0x40) + ((_num) * 0x10))
#define   NFP_PCIE_BARCFG_P2CX1_SIGNAL_REF(x)                (((x) & 0x7f) << 24)
#define   NFP_PCIE_BARCFG_P2CX1_SIGNAL_REF_of(x)             (((x) >> 24) & 0x7f)
#define     NFP_PCIE_BARCFG_P2CX1_SIGNAL_REF_bf              0, 30, 24
#define     NFP_PCIE_BARCFG_P2CX1_SIGNAL_REF_msk             (0x7f)
#define     NFP_PCIE_BARCFG_P2CX1_SIGNAL_REF_shf             (24)
#define   NFP_PCIE_BARCFG_P2CX1_DATA_MASTER(x)               (((x) & 0x3ff) << 14)
#define   NFP_PCIE_BARCFG_P2CX1_DATA_MASTER_of(x)            (((x) >> 14) & 0x3ff)
#define     NFP_PCIE_BARCFG_P2CX1_DATA_MASTER_bf             0, 23, 14
#define     NFP_PCIE_BARCFG_P2CX1_DATA_MASTER_msk            (0x3ff)
#define     NFP_PCIE_BARCFG_P2CX1_DATA_MASTER_shf            (14)
#define   NFP_PCIE_BARCFG_P2CX1_DATA_REF(x)                  (((x) & 0x3fff) << 0)
#define   NFP_PCIE_BARCFG_P2CX1_DATA_REF_of(x)               (((x) >> 0) & 0x3fff)
#define     NFP_PCIE_BARCFG_P2CX1_DATA_REF_bf                0, 13, 0
#define     NFP_PCIE_BARCFG_P2CX1_DATA_REF_msk               (0x3fff)
#define     NFP_PCIE_BARCFG_P2CX1_DATA_REF_shf               (0)


/*
 * Register: PcieExplicitCmdBar2
 *   [31:28]   Target
 *   [27:23]   Action
 *   [22:18]   Length
 *   [17:10]   ByteMask
 *   [9:0]     SignalMaster
 *
 * Name(s):
 * <base>.BARConfiguration.PCIeExplicitCmd0_0.PCIeExplicitCmdBAR2...
 */
#define NFP_PCIE_BARCFG_P2CX2(_set, _num)                  (0x30088 + ((_set) * 0x40) + ((_num) * 0x10))
#define   NFP_PCIE_BARCFG_P2CX2_TGT(x)                       (((x) & 0xf) << 28)
#define   NFP_PCIE_BARCFG_P2CX2_TGT_of(x)                    (((x) >> 28) & 0xf)
#define     NFP_PCIE_BARCFG_P2CX2_TGT_bf                     0, 31, 28
#define     NFP_PCIE_BARCFG_P2CX2_TGT_msk                    (0xf)
#define     NFP_PCIE_BARCFG_P2CX2_TGT_shf                    (28)
#define   NFP_PCIE_BARCFG_P2CX2_ACT(x)                       (((x) & 0x1f) << 23)
#define   NFP_PCIE_BARCFG_P2CX2_ACT_of(x)                    (((x) >> 23) & 0x1f)
#define     NFP_PCIE_BARCFG_P2CX2_ACT_bf                     0, 27, 23
#define     NFP_PCIE_BARCFG_P2CX2_ACT_msk                    (0x1f)
#define     NFP_PCIE_BARCFG_P2CX2_ACT_shf                    (23)
#define   NFP_PCIE_BARCFG_P2CX2_LEN(x)                       (((x) & 0x1f) << 18)
#define   NFP_PCIE_BARCFG_P2CX2_LEN_of(x)                    (((x) >> 18) & 0x1f)
#define     NFP_PCIE_BARCFG_P2CX2_LEN_bf                     0, 22, 18
#define     NFP_PCIE_BARCFG_P2CX2_LEN_msk                    (0x1f)
#define     NFP_PCIE_BARCFG_P2CX2_LEN_shf                    (18)
#define   NFP_PCIE_BARCFG_P2CX2_BYTE_MASK(x)                 (((x) & 0xff) << 10)
#define   NFP_PCIE_BARCFG_P2CX2_BYTE_MASK_of(x)              (((x) >> 10) & 0xff)
#define     NFP_PCIE_BARCFG_P2CX2_BYTE_MASK_bf               0, 17, 10
#define     NFP_PCIE_BARCFG_P2CX2_BYTE_MASK_msk              (0xff)
#define     NFP_PCIE_BARCFG_P2CX2_BYTE_MASK_shf              (10)
#define   NFP_PCIE_BARCFG_P2CX2_SIGNAL_MASTER(x)             (((x) & 0x3ff) << 0)
#define   NFP_PCIE_BARCFG_P2CX2_SIGNAL_MASTER_of(x)          (((x) >> 0) & 0x3ff)
#define     NFP_PCIE_BARCFG_P2CX2_SIGNAL_MASTER_bf           0, 9, 0
#define     NFP_PCIE_BARCFG_P2CX2_SIGNAL_MASTER_msk          (0x3ff)
#define     NFP_PCIE_BARCFG_P2CX2_SIGNAL_MASTER_shf          (0)


/*
 * Register: CppToPcieBar
 *   [31:30]   MapType
 *   [29]      OverrideRID
 *   [28:21]   RequesterID
 *   [20:0]    Address
 *
 * Name(s):
 * <base>.BARConfiguration.CPPToPCIeBAR0...
 */
#define NFP_PCIE_BARCFG_C2P(_bar)                          (0x30180 + ((_bar) * 0x4))
#define   NFP_PCIE_BARCFG_C2P_TYPE(x)                        (((x) & 3) << 30)
#define   NFP_PCIE_BARCFG_C2P_TYPE_of(x)                     (((x) >> 30) & 3)
#define     NFP_PCIE_BARCFG_C2P_TYPE_MEM                     (0)
#define     NFP_PCIE_BARCFG_C2P_TYPE_IO                      (1)
#define     NFP_PCIE_BARCFG_C2P_TYPE_CFG                     (2)
#define     NFP_PCIE_BARCFG_C2P_TYPE_RSVD                    (3)
#define     NFP_PCIE_BARCFG_C2P_TYPE_bf                      0, 31, 30
#define     NFP_PCIE_BARCFG_C2P_TYPE_msk                     (0x3)
#define     NFP_PCIE_BARCFG_C2P_TYPE_shf                     (30)
#define   NFP_PCIE_BARCFG_C2P_ARI_ENABLE                     (1 << 29)
#define     NFP_PCIE_BARCFG_C2P_ARI_ENABLE_bf                0, 29, 29
#define     NFP_PCIE_BARCFG_C2P_ARI_ENABLE_msk               (0x1)
#define     NFP_PCIE_BARCFG_C2P_ARI_ENABLE_bit               (29)
#define   NFP_PCIE_BARCFG_C2P_ARI(x)                         (((x) & 0xff) << 21)
#define   NFP_PCIE_BARCFG_C2P_ARI_of(x)                      (((x) >> 21) & 0xff)
#define     NFP_PCIE_BARCFG_C2P_ARI_bf                       0, 28, 21
#define     NFP_PCIE_BARCFG_C2P_ARI_msk                      (0xff)
#define     NFP_PCIE_BARCFG_C2P_ARI_shf                      (21)
#define   NFP_PCIE_BARCFG_C2P_ADDR(x)                        (((x) & 0x1fffff) << 0)
#define   NFP_PCIE_BARCFG_C2P_ADDR_of(x)                     (((x) >> 0) & 0x1fffff)
#define     NFP_PCIE_BARCFG_C2P_ADDR_bf                      0, 20, 0
#define     NFP_PCIE_BARCFG_C2P_ADDR_msk                     (0x1fffff)
#define     NFP_PCIE_BARCFG_C2P_ADDR_shf                     (0)


/*
 * Register: DMADescrLegacy
 *   [127:116]   XferLength
 *   [115:108]   RequesterID
 *   [107]     OverrideRID
 *   [106:104]   TransClass
 *   [103:96]   PCIeAddressHi
 *   [95:64]   PCIeAddressLo
 *   [63:62]   ModeSelect
 *   [61:46]   DmaMode
 *   [45:44]   CppToken
 *   [43:40]   DmaConfigRegIndex
 *   [39:32]   CPPAddressHi
 *   [31:0]    CPPAddressLo
 *
 * Name(s):
 * <base>.DMAController.DmaCmdInsertHiToPCI
 * <base>.DMAController.DmaCmdInsertMedToPCI
 * <base>.DMAController.DmaCmdInsertLoToPCI
 * <base>.DMAController.DmaCmdInsertHiFrmPCI
 * <base>.DMAController.DmaCmdInsertMedFrmPCI
 * <base>.DMAController.DmaCmdInsertLoFrmPCI
 */
#define NFP_PCIE_DMA_TOPCI_HI                              0x40000
#define NFP_PCIE_DMA_TOPCI_MED                             0x40020
#define NFP_PCIE_DMA_TOPCI_LO                              0x40040
#define NFP_PCIE_DMA_FROMPCI_HI                            0x40060
#define NFP_PCIE_DMA_FROMPCI_MED                           0x40080
#define NFP_PCIE_DMA_FROMPCI_LO                            0x400a0
#define   NFP_PCIE_DMA_CMD_LW3                               (3)
#define   NFP_PCIE_DMA_CMD_LENGTH(x)                         (((x) & 0xfff) << 20)
#define   NFP_PCIE_DMA_CMD_LENGTH_of(x)                      (((x) >> 20) & 0xfff)
#define     NFP_PCIE_DMA_CMD_LENGTH_bf                       3, 31, 20
#define     NFP_PCIE_DMA_CMD_LENGTH_msk                      (0xfff)
#define     NFP_PCIE_DMA_CMD_LENGTH_shf                      (20)
#define   NFP_PCIE_DMA_CMD_RID(x)                            (((x) & 0xff) << 12)
#define   NFP_PCIE_DMA_CMD_RID_of(x)                         (((x) >> 12) & 0xff)
#define     NFP_PCIE_DMA_CMD_RID_bf                          3, 19, 12
#define     NFP_PCIE_DMA_CMD_RID_msk                         (0xff)
#define     NFP_PCIE_DMA_CMD_RID_shf                         (12)
#define   NFP_PCIE_DMA_CMD_RID_OVERRIDE                      (1 << 11)
#define     NFP_PCIE_DMA_CMD_RID_OVERRIDE_bf                 3, 11, 11
#define     NFP_PCIE_DMA_CMD_RID_OVERRIDE_msk                (0x1)
#define     NFP_PCIE_DMA_CMD_RID_OVERRIDE_bit                (11)
#define   NFP_PCIE_DMA_CMD_TRANS_CLASS(x)                    (((x) & 7) << 8)
#define   NFP_PCIE_DMA_CMD_TRANS_CLASS_of(x)                 (((x) >> 8) & 7)
#define     NFP_PCIE_DMA_CMD_TRANS_CLASS_bf                  3, 10, 8
#define     NFP_PCIE_DMA_CMD_TRANS_CLASS_msk                 (0x7)
#define     NFP_PCIE_DMA_CMD_TRANS_CLASS_shf                 (8)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_HI(x)                   (((x) & 0xff) << 0)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_HI_of(x)                (((x) >> 0) & 0xff)
#define     NFP_PCIE_DMA_CMD_PCIE_ADDR_HI_bf                 3, 7, 0
#define     NFP_PCIE_DMA_CMD_PCIE_ADDR_HI_msk                (0xff)
#define     NFP_PCIE_DMA_CMD_PCIE_ADDR_HI_shf                (0)
#define   NFP_PCIE_DMA_CMD_LW2                               (2)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_LO(x)                   (((x) & 0xffffffff) << 0)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_LO_of(x)                (((x) >> 0) & 0xffffffff)
#define     NFP_PCIE_DMA_CMD_PCIE_ADDR_LO_bf                 2, 31, 0
#define     NFP_PCIE_DMA_CMD_PCIE_ADDR_LO_msk                (0xffffffff)
#define     NFP_PCIE_DMA_CMD_PCIE_ADDR_LO_shf                (0)
#define   NFP_PCIE_DMA_CMD_LW1                               (1)
#define   NFP_PCIE_DMA_CMD_MODE_SEL(x)                       (((x) & 3) << 30)
#define   NFP_PCIE_DMA_CMD_MODE_SEL_of(x)                    (((x) >> 30) & 3)
#define     NFP_PCIE_DMA_CMD_MODE_SEL_0                      (0)
#define     NFP_PCIE_DMA_CMD_MODE_SEL_1                      (1)
#define     NFP_PCIE_DMA_CMD_MODE_SEL_2                      (2)
#define     NFP_PCIE_DMA_CMD_MODE_SEL_3                      (3)
#define     NFP_PCIE_DMA_CMD_MODE_SEL_bf                     1, 31, 30
#define     NFP_PCIE_DMA_CMD_MODE_SEL_msk                    (0x3)
#define     NFP_PCIE_DMA_CMD_MODE_SEL_shf                    (30)
#define   NFP_PCIE_DMA_CMD_DMA_MODE(x)                       (((x) & 0xffff) << 14)
#define   NFP_PCIE_DMA_CMD_DMA_MODE_of(x)                    (((x) >> 14) & 0xffff)
#define     NFP_PCIE_DMA_CMD_DMA_MODE_0                      (0)
#define     NFP_PCIE_DMA_CMD_DMA_MODE_1                      (1)
#define     NFP_PCIE_DMA_CMD_DMA_MODE_2                      (2)
#define     NFP_PCIE_DMA_CMD_DMA_MODE_3                      (3)
#define     NFP_PCIE_DMA_CMD_DMA_MODE_bf                     1, 29, 14
#define     NFP_PCIE_DMA_CMD_DMA_MODE_msk                    (0xffff)
#define     NFP_PCIE_DMA_CMD_DMA_MODE_shf                    (14)
#define   NFP_PCIE_DMA_CMD_CPP_TOKEN(x)                      (((x) & 3) << 12)
#define   NFP_PCIE_DMA_CMD_CPP_TOKEN_of(x)                   (((x) >> 12) & 3)
#define     NFP_PCIE_DMA_CMD_CPP_TOKEN_bf                    1, 13, 12
#define     NFP_PCIE_DMA_CMD_CPP_TOKEN_msk                   (0x3)
#define     NFP_PCIE_DMA_CMD_CPP_TOKEN_shf                   (12)
#define   NFP_PCIE_DMA_CMD_DMA_CFG_INDEX(x)                  (((x) & 0xf) << 8)
#define   NFP_PCIE_DMA_CMD_DMA_CFG_INDEX_of(x)               (((x) >> 8) & 0xf)
#define     NFP_PCIE_DMA_CMD_DMA_CFG_INDEX_bf                1, 11, 8
#define     NFP_PCIE_DMA_CMD_DMA_CFG_INDEX_msk               (0xf)
#define     NFP_PCIE_DMA_CMD_DMA_CFG_INDEX_shf               (8)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_HI(x)                    (((x) & 0xff) << 0)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_HI_of(x)                 (((x) >> 0) & 0xff)
#define     NFP_PCIE_DMA_CMD_CPP_ADDR_HI_bf                  1, 7, 0
#define     NFP_PCIE_DMA_CMD_CPP_ADDR_HI_msk                 (0xff)
#define     NFP_PCIE_DMA_CMD_CPP_ADDR_HI_shf                 (0)
#define   NFP_PCIE_DMA_CMD_LW0                               (0)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_LO(x)                    (((x) & 0xffffffff) << 0)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_LO_of(x)                 (((x) >> 0) & 0xffffffff)
#define     NFP_PCIE_DMA_CMD_CPP_ADDR_LO_bf                  0, 31, 0
#define     NFP_PCIE_DMA_CMD_CPP_ADDR_LO_msk                 (0xffffffff)
#define     NFP_PCIE_DMA_CMD_CPP_ADDR_LO_shf                 (0)


/*
 * Register: DMADescrConfig
 *   [28]      SignalOnlyOdd
 *   [27:26]   EndPaddingOdd
 *   [25:24]   StartPaddingOdd
 *   [23]      IdBasedOrderingOdd
 *   [22]      RelaxedOrderingOdd
 *   [21]      NoSnoopOdd
 *   [20]      Target64bitOdd
 *   [19:16]   CppTargetIDOdd
 *   [12]      SignalOnlyEven
 *   [11:10]   EndPaddingEven
 *   [9:8]     StartPaddingEven
 *   [7]       IdBasedOrderingEven
 *   [6]       RelaxedOrderingEven
 *   [5]       NoSnoopEven
 *   [4]       Target64bitEven
 *   [3:0]     CppTargetIDEven
 *
 * Name(s):
 * <base>.DMAController.DMADescrConfig0 <base>.DMAController.DMADescrConfig1
 * <base>.DMAController.DMADescrConfig2 <base>.DMAController.DMADescrConfig3
 * <base>.DMAController.DMADescrConfig4 <base>.DMAController.DMADescrConfig5
 * <base>.DMAController.DMADescrConfig6 <base>.DMAController.DMADescrConfig7
 */
#define NFP_PCIE_DMA_CFG0                                  0x400c0
#define NFP_PCIE_DMA_CFG1                                  0x400c4
#define NFP_PCIE_DMA_CFG2                                  0x400c8
#define NFP_PCIE_DMA_CFG3                                  0x400cc
#define NFP_PCIE_DMA_CFG4                                  0x400d0
#define NFP_PCIE_DMA_CFG5                                  0x400d4
#define NFP_PCIE_DMA_CFG6                                  0x400d8
#define NFP_PCIE_DMA_CFG7                                  0x400dc
#define   NFP_PCIE_DMA_CFG_SIGNAL_ONLY_ODD                   (1 << 28)
#define     NFP_PCIE_DMA_CFG_SIGNAL_ONLY_ODD_bf              0, 28, 28
#define     NFP_PCIE_DMA_CFG_SIGNAL_ONLY_ODD_msk             (0x1)
#define     NFP_PCIE_DMA_CFG_SIGNAL_ONLY_ODD_bit             (28)
#define   NFP_PCIE_DMA_CFG_END_PAD_ODD(x)                    (((x) & 3) << 26)
#define   NFP_PCIE_DMA_CFG_END_PAD_ODD_of(x)                 (((x) >> 26) & 3)
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_DISABLED            (0)
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_16B                 (1)
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_64B                 (2)
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_128B                (3)
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_bf                  0, 27, 26
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_msk                 (0x3)
#define     NFP_PCIE_DMA_CFG_END_PAD_ODD_shf                 (26)
#define   NFP_PCIE_DMA_CFG_START_PAD_ODD(x)                  (((x) & 3) << 24)
#define   NFP_PCIE_DMA_CFG_START_PAD_ODD_of(x)               (((x) >> 24) & 3)
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_DISABLED          (0)
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_16B               (1)
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_64B               (2)
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_128B              (3)
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_bf                0, 25, 24
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_msk               (0x3)
#define     NFP_PCIE_DMA_CFG_START_PAD_ODD_shf               (24)
#define   NFP_PCIE_DMA_CFG_ID_BASED_ORDER_ODD                (1 << 23)
#define     NFP_PCIE_DMA_CFG_ID_BASED_ORDER_ODD_bf           0, 23, 23
#define     NFP_PCIE_DMA_CFG_ID_BASED_ORDER_ODD_msk          (0x1)
#define     NFP_PCIE_DMA_CFG_ID_BASED_ORDER_ODD_bit          (23)
#define   NFP_PCIE_DMA_CFG_RELAXED_ORDER_ODD                 (1 << 22)
#define     NFP_PCIE_DMA_CFG_RELAXED_ORDER_ODD_bf            0, 22, 22
#define     NFP_PCIE_DMA_CFG_RELAXED_ORDER_ODD_msk           (0x1)
#define     NFP_PCIE_DMA_CFG_RELAXED_ORDER_ODD_bit           (22)
#define   NFP_PCIE_DMA_CFG_NO_SNOOP_ODD                      (1 << 21)
#define     NFP_PCIE_DMA_CFG_NO_SNOOP_ODD_bf                 0, 21, 21
#define     NFP_PCIE_DMA_CFG_NO_SNOOP_ODD_msk                (0x1)
#define     NFP_PCIE_DMA_CFG_NO_SNOOP_ODD_bit                (21)
#define   NFP_PCIE_DMA_CFG_TARGET_64_ODD                     (1 << 20)
#define     NFP_PCIE_DMA_CFG_TARGET_64_ODD_bf                0, 20, 20
#define     NFP_PCIE_DMA_CFG_TARGET_64_ODD_msk               (0x1)
#define     NFP_PCIE_DMA_CFG_TARGET_64_ODD_bit               (20)
#define   NFP_PCIE_DMA_CFG_CPP_TARGET_ODD(x)                 (((x) & 0xf) << 16)
#define   NFP_PCIE_DMA_CFG_CPP_TARGET_ODD_of(x)              (((x) >> 16) & 0xf)
#define     NFP_PCIE_DMA_CFG_CPP_TARGET_ODD_bf               0, 19, 16
#define     NFP_PCIE_DMA_CFG_CPP_TARGET_ODD_msk              (0xf)
#define     NFP_PCIE_DMA_CFG_CPP_TARGET_ODD_shf              (16)
#define   NFP_PCIE_DMA_CFG_SIGNAL_ONLY_EVEN                  (1 << 12)
#define     NFP_PCIE_DMA_CFG_SIGNAL_ONLY_EVEN_bf             0, 12, 12
#define     NFP_PCIE_DMA_CFG_SIGNAL_ONLY_EVEN_msk            (0x1)
#define     NFP_PCIE_DMA_CFG_SIGNAL_ONLY_EVEN_bit            (12)
#define   NFP_PCIE_DMA_CFG_END_PAD_EVEN(x)                   (((x) & 3) << 10)
#define   NFP_PCIE_DMA_CFG_END_PAD_EVEN_of(x)                (((x) >> 10) & 3)
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_DISABLED           (0)
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_16B                (1)
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_64B                (2)
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_128B               (3)
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_bf                 0, 11, 10
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_msk                (0x3)
#define     NFP_PCIE_DMA_CFG_END_PAD_EVEN_shf                (10)
#define   NFP_PCIE_DMA_CFG_START_PAD_EVEN(x)                 (((x) & 3) << 8)
#define   NFP_PCIE_DMA_CFG_START_PAD_EVEN_of(x)              (((x) >> 8) & 3)
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_DISABLED         (0)
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_16B              (1)
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_64B              (2)
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_128B             (3)
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_bf               0, 9, 8
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_msk              (0x3)
#define     NFP_PCIE_DMA_CFG_START_PAD_EVEN_shf              (8)
#define   NFP_PCIE_DMA_CFG_ID_BASED_ORDER_EVEN               (1 << 7)
#define     NFP_PCIE_DMA_CFG_ID_BASED_ORDER_EVEN_bf          0, 7, 7
#define     NFP_PCIE_DMA_CFG_ID_BASED_ORDER_EVEN_msk         (0x1)
#define     NFP_PCIE_DMA_CFG_ID_BASED_ORDER_EVEN_bit         (7)
#define   NFP_PCIE_DMA_CFG_RELAXED_ORDER_EVEN                (1 << 6)
#define     NFP_PCIE_DMA_CFG_RELAXED_ORDER_EVEN_bf           0, 6, 6
#define     NFP_PCIE_DMA_CFG_RELAXED_ORDER_EVEN_msk          (0x1)
#define     NFP_PCIE_DMA_CFG_RELAXED_ORDER_EVEN_bit          (6)
#define   NFP_PCIE_DMA_CFG_NO_SNOOP_EVEN                     (1 << 5)
#define     NFP_PCIE_DMA_CFG_NO_SNOOP_EVEN_bf                0, 5, 5
#define     NFP_PCIE_DMA_CFG_NO_SNOOP_EVEN_msk               (0x1)
#define     NFP_PCIE_DMA_CFG_NO_SNOOP_EVEN_bit               (5)
#define   NFP_PCIE_DMA_CFG_TARGET_64_EVEN                    (1 << 4)
#define     NFP_PCIE_DMA_CFG_TARGET_64_EVEN_bf               0, 4, 4
#define     NFP_PCIE_DMA_CFG_TARGET_64_EVEN_msk              (0x1)
#define     NFP_PCIE_DMA_CFG_TARGET_64_EVEN_bit              (4)
#define   NFP_PCIE_DMA_CFG_CPP_TARGET_EVEN(x)                (((x) & 0xf) << 0)
#define   NFP_PCIE_DMA_CFG_CPP_TARGET_EVEN_of(x)             (((x) >> 0) & 0xf)
#define     NFP_PCIE_DMA_CFG_CPP_TARGET_EVEN_bf              0, 3, 0
#define     NFP_PCIE_DMA_CFG_CPP_TARGET_EVEN_msk             (0xf)
#define     NFP_PCIE_DMA_CFG_CPP_TARGET_EVEN_shf             (0)


/*
 * Register: DMAQueueStatus0
 *   [31:24]   DmaLoQueAvail
 *   [23:16]   DmaLoQueWrPtr
 *   [11]      ParityError
 *   [10]      DisDmaDescParity
 *   [9]       HaltOnPfDmaError
 *   [8]       HaltOnRdParityError
 *   [7:6]     DmaError
 *   [5:4]     QueueOverflow
 *   [3]       HaltOnFull
 *   [2]       HighQueueStop
 *   [1]       MediumQueueStop
 *   [0]       LowQueueStop
 *
 * Name(s):
 * <base>.DMAController.DMAQStatToPCI0 <base>.DMAController.DMAQStatFrmPCI0
 */
#define NFP_PCIE_DMA_QSTS0_TOPCI                           0x400e0
#define NFP_PCIE_DMA_QSTS0_FROMPCI                         0x400e8
#define   NFP_PCIE_DMA_QSTS0_DMA_LO_AVAIL(x)                 (((x) & 0xff) << 24)
#define   NFP_PCIE_DMA_QSTS0_DMA_LO_AVAIL_of(x)              (((x) >> 24) & 0xff)
#define     NFP_PCIE_DMA_QSTS0_DMA_LO_AVAIL_bf               0, 31, 24
#define     NFP_PCIE_DMA_QSTS0_DMA_LO_AVAIL_msk              (0xff)
#define     NFP_PCIE_DMA_QSTS0_DMA_LO_AVAIL_shf              (24)
#define   NFP_PCIE_DMA_QSTS0_DMA_LO_WRPTR(x)                 (((x) & 0xff) << 16)
#define   NFP_PCIE_DMA_QSTS0_DMA_LO_WRPTR_of(x)              (((x) >> 16) & 0xff)
#define     NFP_PCIE_DMA_QSTS0_DMA_LO_WRPTR_bf               0, 23, 16
#define     NFP_PCIE_DMA_QSTS0_DMA_LO_WRPTR_msk              (0xff)
#define     NFP_PCIE_DMA_QSTS0_DMA_LO_WRPTR_shf              (16)
#define   NFP_PCIE_DMA_QSTS0_PARITY_ERROR                    (1 << 11)
#define     NFP_PCIE_DMA_QSTS0_PARITY_ERROR_bf               0, 11, 11
#define     NFP_PCIE_DMA_QSTS0_PARITY_ERROR_msk              (0x1)
#define     NFP_PCIE_DMA_QSTS0_PARITY_ERROR_bit              (11)
#define   NFP_PCIE_DMA_QSTS0_DIS_DMADESC_PARITY              (1 << 10)
#define     NFP_PCIE_DMA_QSTS0_DIS_DMADESC_PARITY_bf         0, 10, 10
#define     NFP_PCIE_DMA_QSTS0_DIS_DMADESC_PARITY_msk        (0x1)
#define     NFP_PCIE_DMA_QSTS0_DIS_DMADESC_PARITY_bit        (10)
#define   NFP_PCIE_DMA_QSTS0_PF_ERROR_HALT                   (1 << 9)
#define     NFP_PCIE_DMA_QSTS0_PF_ERROR_HALT_bf              0, 9, 9
#define     NFP_PCIE_DMA_QSTS0_PF_ERROR_HALT_msk             (0x1)
#define     NFP_PCIE_DMA_QSTS0_PF_ERROR_HALT_bit             (9)
#define   NFP_PCIE_DMA_QSTS0_PARITY_ERROR_HALT               (1 << 8)
#define     NFP_PCIE_DMA_QSTS0_PARITY_ERROR_HALT_bf          0, 8, 8
#define     NFP_PCIE_DMA_QSTS0_PARITY_ERROR_HALT_msk         (0x1)
#define     NFP_PCIE_DMA_QSTS0_PARITY_ERROR_HALT_bit         (8)
#define   NFP_PCIE_DMA_QSTS0_DMA_ERROR(x)                    (((x) & 3) << 6)
#define   NFP_PCIE_DMA_QSTS0_DMA_ERROR_of(x)                 (((x) >> 6) & 3)
#define     NFP_PCIE_DMA_QSTS0_DMA_ERROR_bf                  0, 7, 6
#define     NFP_PCIE_DMA_QSTS0_DMA_ERROR_msk                 (0x3)
#define     NFP_PCIE_DMA_QSTS0_DMA_ERROR_shf                 (6)
#define   NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW(x)               (((x) & 3) << 4)
#define   NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_of(x)            (((x) >> 4) & 3)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_NoOvfl         (0)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_LowOvfl        (1)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_MedOvfl        (2)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_HighOvfl       (3)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_bf             0, 5, 4
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_msk            (0x3)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_OVERFLOW_shf            (4)
#define   NFP_PCIE_DMA_QSTS0_QUEUE_HALT_FULL                 (1 << 3)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_HALT_FULL_bf            0, 3, 3
#define     NFP_PCIE_DMA_QSTS0_QUEUE_HALT_FULL_msk           (0x1)
#define     NFP_PCIE_DMA_QSTS0_QUEUE_HALT_FULL_bit           (3)
#define   NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP                   (1 << 2)
#define     NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP_bf              0, 2, 2
#define     NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP_msk             (0x1)
#define     NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP_bit             (2)
#define   NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP                  (1 << 1)
#define     NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP_bf             0, 1, 1
#define     NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP_msk            (0x1)
#define     NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP_bit            (1)
#define   NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP                   (1 << 0)
#define     NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP_bf              0, 0, 0
#define     NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP_msk             (0x1)
#define     NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP_bit             (0)


/*
 * Register: DMAQueueStatus1
 *   [31:24]   DmaHiQueAvail
 *   [23:16]   DmaHiQueWrPtr
 *   [15:8]    DmaMedQueAvail
 *   [7:0]     DmaMedQueWrPtr
 *
 * Name(s):
 * <base>.DMAController.DMAQStatToPCI1 <base>.DMAController.DMAQStatFrmPCI1
 */
#define NFP_PCIE_DMA_QSTS1_TOPCI                           0x400e4
#define NFP_PCIE_DMA_QSTS1_FROMPCI                         0x400ec
#define   NFP_PCIE_DMA_QSTS1_DMA_HI_AVAIL(x)                 (((x) & 0xff) << 24)
#define   NFP_PCIE_DMA_QSTS1_DMA_HI_AVAIL_of(x)              (((x) >> 24) & 0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_HI_AVAIL_bf               0, 31, 24
#define     NFP_PCIE_DMA_QSTS1_DMA_HI_AVAIL_msk              (0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_HI_AVAIL_shf              (24)
#define   NFP_PCIE_DMA_QSTS1_DMA_HI_WRPTR(x)                 (((x) & 0xff) << 16)
#define   NFP_PCIE_DMA_QSTS1_DMA_HI_WRPTR_of(x)              (((x) >> 16) & 0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_HI_WRPTR_bf               0, 23, 16
#define     NFP_PCIE_DMA_QSTS1_DMA_HI_WRPTR_msk              (0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_HI_WRPTR_shf              (16)
#define   NFP_PCIE_DMA_QSTS1_DMA_MED_AVAIL(x)                (((x) & 0xff) << 8)
#define   NFP_PCIE_DMA_QSTS1_DMA_MED_AVAIL_of(x)             (((x) >> 8) & 0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_MED_AVAIL_bf              0, 15, 8
#define     NFP_PCIE_DMA_QSTS1_DMA_MED_AVAIL_msk             (0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_MED_AVAIL_shf             (8)
#define   NFP_PCIE_DMA_QSTS1_DMA_MED_WRPTR(x)                (((x) & 0xff) << 0)
#define   NFP_PCIE_DMA_QSTS1_DMA_MED_WRPTR_of(x)             (((x) >> 0) & 0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_MED_WRPTR_bf              0, 7, 0
#define     NFP_PCIE_DMA_QSTS1_DMA_MED_WRPTR_msk             (0xff)
#define     NFP_PCIE_DMA_QSTS1_DMA_MED_WRPTR_shf             (0)


/*
 * Register: DMADbgReg0
 *   [31]      DmaByteMaskSwapEna
 *   [25:24]   InvLWrPar
 *   [23:22]   InvMWrPar
 *   [21:20]   InvHWrPar
 *   [19:16]   DataHi
 *   [11]      Direction
 *   [10]      RAMSel
 *   [9:0]     RAMAddr
 *
 * Name(s):
 * <base>.DMAController.DMADbgReg0
 */
#define NFP_PCIE_DMA_DBG_REG0                              0x400f0
#define   NFP_PCIE_DMA_DBG_REG0_DMA_BYTE_MASK_SWAP_ENA       (1 << 31)
#define     NFP_PCIE_DMA_DBG_REG0_DMA_BYTE_MASK_SWAP_ENA_bf  0, 31, 31
#define     NFP_PCIE_DMA_DBG_REG0_DMA_BYTE_MASK_SWAP_ENA_msk (0x1)
#define     NFP_PCIE_DMA_DBG_REG0_DMA_BYTE_MASK_SWAP_ENA_bit (31)
#define   NFP_PCIE_DMA_DBG_REG0_INV_L_WR_PAR(x)              (((x) & 3) << 24)
#define   NFP_PCIE_DMA_DBG_REG0_INV_L_WR_PAR_of(x)           (((x) >> 24) & 3)
#define     NFP_PCIE_DMA_DBG_REG0_INV_L_WR_PAR_bf            0, 25, 24
#define     NFP_PCIE_DMA_DBG_REG0_INV_L_WR_PAR_msk           (0x3)
#define     NFP_PCIE_DMA_DBG_REG0_INV_L_WR_PAR_shf           (24)
#define   NFP_PCIE_DMA_DBG_REG0_INV_M_WR_PAR(x)              (((x) & 3) << 22)
#define   NFP_PCIE_DMA_DBG_REG0_INV_M_WR_PAR_of(x)           (((x) >> 22) & 3)
#define     NFP_PCIE_DMA_DBG_REG0_INV_M_WR_PAR_bf            0, 23, 22
#define     NFP_PCIE_DMA_DBG_REG0_INV_M_WR_PAR_msk           (0x3)
#define     NFP_PCIE_DMA_DBG_REG0_INV_M_WR_PAR_shf           (22)
#define   NFP_PCIE_DMA_DBG_REG0_INV_H_WR_PAR(x)              (((x) & 3) << 20)
#define   NFP_PCIE_DMA_DBG_REG0_INV_H_WR_PAR_of(x)           (((x) >> 20) & 3)
#define     NFP_PCIE_DMA_DBG_REG0_INV_H_WR_PAR_bf            0, 21, 20
#define     NFP_PCIE_DMA_DBG_REG0_INV_H_WR_PAR_msk           (0x3)
#define     NFP_PCIE_DMA_DBG_REG0_INV_H_WR_PAR_shf           (20)
#define   NFP_PCIE_DMA_DBG_REG0_DATA_HI(x)                   (((x) & 0xf) << 16)
#define   NFP_PCIE_DMA_DBG_REG0_DATA_HI_of(x)                (((x) >> 16) & 0xf)
#define     NFP_PCIE_DMA_DBG_REG0_DATA_HI_bf                 0, 19, 16
#define     NFP_PCIE_DMA_DBG_REG0_DATA_HI_msk                (0xf)
#define     NFP_PCIE_DMA_DBG_REG0_DATA_HI_shf                (16)
#define   NFP_PCIE_DMA_DBG_REG0_DIRECTION                    (1 << 11)
#define     NFP_PCIE_DMA_DBG_REG0_DIRECTION_bf               0, 11, 11
#define     NFP_PCIE_DMA_DBG_REG0_DIRECTION_msk              (0x1)
#define     NFP_PCIE_DMA_DBG_REG0_DIRECTION_bit              (11)
#define   NFP_PCIE_DMA_DBG_REG0_RAM_SEL                      (1 << 10)
#define     NFP_PCIE_DMA_DBG_REG0_RAM_SEL_bf                 0, 10, 10
#define     NFP_PCIE_DMA_DBG_REG0_RAM_SEL_msk                (0x1)
#define     NFP_PCIE_DMA_DBG_REG0_RAM_SEL_bit                (10)
#define   NFP_PCIE_DMA_DBG_REG0_RAM_ADDR(x)                  (((x) & 0x3ff) << 0)
#define   NFP_PCIE_DMA_DBG_REG0_RAM_ADDR_of(x)               (((x) >> 0) & 0x3ff)
#define     NFP_PCIE_DMA_DBG_REG0_RAM_ADDR_bf                0, 9, 0
#define     NFP_PCIE_DMA_DBG_REG0_RAM_ADDR_msk               (0x3ff)
#define     NFP_PCIE_DMA_DBG_REG0_RAM_ADDR_shf               (0)


/*
 * Register: DMACntrlCfg1
 *   [31:28]   DmaFrmCppRate
 *   [27:24]   DmaToPcieRate
 *   [23:20]   DmaToCppRate
 *   [19:16]   DmaFrmPcieRate
 *   [15:12]   DmaFrmPcieIssueLoad
 *
 * Name(s):
 * <base>.DMAController.DMACntrlConfig
 */
#define NFP_PCIE_DMA_CTRL_CFG                              0x400f8
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_CPP_RATE(x)          (((x) & 0xf) << 28)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_CPP_RATE_of(x)       (((x) >> 28) & 0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_CPP_RATE_bf        0, 31, 28
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_CPP_RATE_msk       (0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_CPP_RATE_shf       (28)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_TO_PCIE_RATE(x)          (((x) & 0xf) << 24)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_TO_PCIE_RATE_of(x)       (((x) >> 24) & 0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_TO_PCIE_RATE_bf        0, 27, 24
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_TO_PCIE_RATE_msk       (0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_TO_PCIE_RATE_shf       (24)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_TO_CPP_RATE(x)           (((x) & 0xf) << 20)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_TO_CPP_RATE_of(x)        (((x) >> 20) & 0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_TO_CPP_RATE_bf         0, 23, 20
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_TO_CPP_RATE_msk        (0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_TO_CPP_RATE_shf        (20)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_RATE(x)         (((x) & 0xf) << 16)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_RATE_of(x)      (((x) >> 16) & 0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_RATE_bf       0, 19, 16
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_RATE_msk      (0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_RATE_shf      (16)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_ISSUE_LOAD(x)   (((x) & 0xf) << 12)
#define   NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_ISSUE_LOAD_of(x) (((x) >> 12) & 0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_ISSUE_LOAD_bf 0, 15, 12
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_ISSUE_LOAD_msk (0xf)
#define     NFP_PCIE_DMA_CTRL_CFG_DMA_FRM_PCIE_ISSUE_LOAD_shf (12)

#define NFP_PCIE_DMA_MSIX_INSTR_GEN(_f)                    (0x60000 + ((_f) * 0x4))
#define   NFP_PCIE_PCIE_MSI_SW_GEN_MSI_VEC_NUM(x)            (((x) & 0x1f) << 0)
#define   NFP_PCIE_PCIE_MSI_SW_GEN_MSI_VEC_NUM_of(x)         (((x) >> 0) & 0x1f)
#define     NFP_PCIE_PCIE_MSI_SW_GEN_MSI_VEC_NUM_bf          0, 4, 0
#define     NFP_PCIE_PCIE_MSI_SW_GEN_MSI_VEC_NUM_msk         (0x1f)
#define     NFP_PCIE_PCIE_MSI_SW_GEN_MSI_VEC_NUM_shf         (0)

#define   NFP_PCIE_PCIE_MSIX_SW_GEN_MSIX_VEC_NUM(x)          (((x) & 0xff) << 0)
#define   NFP_PCIE_PCIE_MSIX_SW_GEN_MSIX_VEC_NUM_of(x)       (((x) >> 0) & 0xff)
#define     NFP_PCIE_PCIE_MSIX_SW_GEN_MSIX_VEC_NUM_bf        0, 7, 0
#define     NFP_PCIE_PCIE_MSIX_SW_GEN_MSIX_VEC_NUM_msk       (0xff)
#define     NFP_PCIE_PCIE_MSIX_SW_GEN_MSIX_VEC_NUM_shf       (0)

/* QC_Queue_x4SSB: <base>.Queue0... */
#define NFP_PCIE_QUEUE(_q)                                 (0x80000 + ((_q) * 0x800))


/*
 * PCIe register structures
 */
#if defined(__NFP_LANG_MICROC)

struct nfp_pcie_sram {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int word:32;
#           else
            unsigned int word:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_barcfg_p2c {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int map_type:3;
            unsigned int len:2;
            unsigned int target:4;
            unsigned int token:2;
            unsigned int actaddr:5;
            unsigned int base:16;
#           else
            unsigned int base:16;
            unsigned int actaddr:5;
            unsigned int token:2;
            unsigned int target:4;
            unsigned int len:2;
            unsigned int map_type:3;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_barcfg_vf_p2c {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int len:2;
            unsigned int target:4;
            unsigned int token:2;
            unsigned int __reserved_19:5;
            unsigned int base:19;
#           else
            unsigned int base:19;
            unsigned int __reserved_19:5;
            unsigned int token:2;
            unsigned int target:4;
            unsigned int len:2;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_barcfg_p2cx0 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int sigtype:2;
            unsigned int tok:2;
            unsigned int __reserved_24:4;
            unsigned int addr:24;
#           else
            unsigned int addr:24;
            unsigned int __reserved_24:4;
            unsigned int tok:2;
            unsigned int sigtype:2;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_barcfg_p2cx1 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_31:1;
            unsigned int signal_ref:7;
            unsigned int data_master:10;
            unsigned int data_ref:14;
#           else
            unsigned int data_ref:14;
            unsigned int data_master:10;
            unsigned int signal_ref:7;
            unsigned int __reserved_31:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_barcfg_p2cx2 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int tgt:4;
            unsigned int act:5;
            unsigned int len:5;
            unsigned int byte_mask:8;
            unsigned int signal_master:10;
#           else
            unsigned int signal_master:10;
            unsigned int byte_mask:8;
            unsigned int len:5;
            unsigned int act:5;
            unsigned int tgt:4;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_barcfg_c2p {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int type:2;
            unsigned int ari_enable:1;
            unsigned int ari:8;
            unsigned int addr:21;
#           else
            unsigned int addr:21;
            unsigned int ari:8;
            unsigned int ari_enable:1;
            unsigned int type:2;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_dma_cmd {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int cpp_addr_lo:32;
            unsigned int mode_sel:2;
            unsigned int dma_mode:16;
            unsigned int cpp_token:2;
            unsigned int dma_cfg_index:4;
            unsigned int cpp_addr_hi:8;
            unsigned int pcie_addr_lo:32;
            unsigned int length:12;
            unsigned int rid:8;
            unsigned int rid_override:1;
            unsigned int trans_class:3;
            unsigned int pcie_addr_hi:8;
#           else
            unsigned int cpp_addr_lo:32;
            unsigned int cpp_addr_hi:8;
            unsigned int dma_cfg_index:4;
            unsigned int cpp_token:2;
            unsigned int dma_mode:16;
            unsigned int mode_sel:2;
            unsigned int pcie_addr_lo:32;
            unsigned int pcie_addr_hi:8;
            unsigned int trans_class:3;
            unsigned int rid_override:1;
            unsigned int rid:8;
            unsigned int length:12;
#           endif
        };
        unsigned int __raw[4];
    };
};

struct nfp_pcie_dma_cfg {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_29:3;
            unsigned int signal_only_odd:1;
            unsigned int end_pad_odd:2;
            unsigned int start_pad_odd:2;
            unsigned int id_based_order_odd:1;
            unsigned int relaxed_order_odd:1;
            unsigned int no_snoop_odd:1;
            unsigned int target_64_odd:1;
            unsigned int cpp_target_odd:4;
            unsigned int __reserved_13:3;
            unsigned int signal_only_even:1;
            unsigned int end_pad_even:2;
            unsigned int start_pad_even:2;
            unsigned int id_based_order_even:1;
            unsigned int relaxed_order_even:1;
            unsigned int no_snoop_even:1;
            unsigned int target_64_even:1;
            unsigned int cpp_target_even:4;
#           else
            unsigned int cpp_target_even:4;
            unsigned int target_64_even:1;
            unsigned int no_snoop_even:1;
            unsigned int relaxed_order_even:1;
            unsigned int id_based_order_even:1;
            unsigned int start_pad_even:2;
            unsigned int end_pad_even:2;
            unsigned int signal_only_even:1;
            unsigned int __reserved_13:3;
            unsigned int cpp_target_odd:4;
            unsigned int target_64_odd:1;
            unsigned int no_snoop_odd:1;
            unsigned int relaxed_order_odd:1;
            unsigned int id_based_order_odd:1;
            unsigned int start_pad_odd:2;
            unsigned int end_pad_odd:2;
            unsigned int signal_only_odd:1;
            unsigned int __reserved_29:3;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_dma_qsts0 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int dma_lo_avail:8;
            unsigned int dma_lo_wrptr:8;
            unsigned int __reserved_12:4;
            unsigned int parity_error:1;
            unsigned int dis_dmadesc_parity:1;
            unsigned int pf_error_halt:1;
            unsigned int parity_error_halt:1;
            unsigned int dma_error:2;
            unsigned int queue_overflow:2;
            unsigned int queue_halt_full:1;
            unsigned int hi_queue_stop:1;
            unsigned int med_queue_stop:1;
            unsigned int lo_queue_stop:1;
#           else
            unsigned int lo_queue_stop:1;
            unsigned int med_queue_stop:1;
            unsigned int hi_queue_stop:1;
            unsigned int queue_halt_full:1;
            unsigned int queue_overflow:2;
            unsigned int dma_error:2;
            unsigned int parity_error_halt:1;
            unsigned int pf_error_halt:1;
            unsigned int dis_dmadesc_parity:1;
            unsigned int parity_error:1;
            unsigned int __reserved_12:4;
            unsigned int dma_lo_wrptr:8;
            unsigned int dma_lo_avail:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_dma_qsts1 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int dma_hi_avail:8;
            unsigned int dma_hi_wrptr:8;
            unsigned int dma_med_avail:8;
            unsigned int dma_med_wrptr:8;
#           else
            unsigned int dma_med_wrptr:8;
            unsigned int dma_med_avail:8;
            unsigned int dma_hi_wrptr:8;
            unsigned int dma_hi_avail:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_dma_dbg_reg0 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int dma_byte_mask_swap_ena:1;
            unsigned int __reserved_26:5;
            unsigned int inv_l_wr_par:2;
            unsigned int inv_m_wr_par:2;
            unsigned int inv_h_wr_par:2;
            unsigned int data_hi:4;
            unsigned int __reserved_12:4;
            unsigned int direction:1;
            unsigned int ram_sel:1;
            unsigned int ram_addr:10;
#           else
            unsigned int ram_addr:10;
            unsigned int ram_sel:1;
            unsigned int direction:1;
            unsigned int __reserved_12:4;
            unsigned int data_hi:4;
            unsigned int inv_h_wr_par:2;
            unsigned int inv_m_wr_par:2;
            unsigned int inv_l_wr_par:2;
            unsigned int __reserved_26:5;
            unsigned int dma_byte_mask_swap_ena:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_pcie_dma_ctrl_cfg {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int dma_frm_cpp_rate:4;
            unsigned int dma_to_pcie_rate:4;
            unsigned int dma_to_cpp_rate:4;
            unsigned int dma_frm_pcie_rate:4;
            unsigned int dma_frm_pcie_issue_load:4;
            unsigned int __reserved_0:12;
#           else
            unsigned int __reserved_0:12;
            unsigned int dma_frm_pcie_issue_load:4;
            unsigned int dma_frm_pcie_rate:4;
            unsigned int dma_to_cpp_rate:4;
            unsigned int dma_to_pcie_rate:4;
            unsigned int dma_frm_cpp_rate:4;
#           endif
        };
        unsigned int __raw;
    };
};

        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_5:27;
            unsigned int msi_vec_num:5;
#           else
            unsigned int msi_vec_num:5;
            unsigned int __reserved_5:27;
#           endif
        } nfp_pcie_pcie_msi_sw_gen;
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_8:24;
            unsigned int msix_vec_num:8;
#           else
            unsigned int msix_vec_num:8;
            unsigned int __reserved_8:24;
#           endif
        } nfp_pcie_pcie_msix_sw_gen;



#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_PCIE_H_ */
