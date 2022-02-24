/*
 * Copyright (C) 2013-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          nfp6000/nfp_me.h
 * @brief         NFP6000 Microengine CSR definitions
 *
 * This files is auto-generated. Do not edit!
 */

#ifndef _NFP6000__NFP_ME_H_
#define _NFP6000__NFP_ME_H_



/*
 * Register: UstorAddr
 *   [31]      EnableCS
 *   [30]      UstoreWrStrb
 *   [29]      UstoreDataInvert
 *   [12:0]    Uaddr
 *
 * Name(s):
 * <base>.UstorAddr
 */
#define NFP_MECSR_USTORE_ADDRESS                           0x0000
#define   NFP_MECSR_USTORE_ADDRESS_ECS                       (1 << 31)
#define     NFP_MECSR_USTORE_ADDRESS_ECS_bf                  0, 31, 31
#define     NFP_MECSR_USTORE_ADDRESS_ECS_msk                 (0x1)
#define     NFP_MECSR_USTORE_ADDRESS_ECS_bit                 (31)
#define   NFP_MECSR_USTORE_ADDRESS_WRITE_STROBE              (1 << 30)
#define     NFP_MECSR_USTORE_ADDRESS_WRITE_STROBE_bf         0, 30, 30
#define     NFP_MECSR_USTORE_ADDRESS_WRITE_STROBE_msk        (0x1)
#define     NFP_MECSR_USTORE_ADDRESS_WRITE_STROBE_bit        (30)
#define   NFP_MECSR_USTORE_ADDRESS_DATA_INVERT               (1 << 29)
#define     NFP_MECSR_USTORE_ADDRESS_DATA_INVERT_bf          0, 29, 29
#define     NFP_MECSR_USTORE_ADDRESS_DATA_INVERT_msk         (0x1)
#define     NFP_MECSR_USTORE_ADDRESS_DATA_INVERT_bit         (29)
#define   NFP_MECSR_USTORE_ADDRESS_ADDR(x)                   (((x) & 0x1fff) << 0)
#define   NFP_MECSR_USTORE_ADDRESS_ADDR_of(x)                (((x) >> 0) & 0x1fff)
#define     NFP_MECSR_USTORE_ADDRESS_ADDR_bf                 0, 12, 0
#define     NFP_MECSR_USTORE_ADDRESS_ADDR_msk                (0x1fff)
#define     NFP_MECSR_USTORE_ADDRESS_ADDR_shf                (0)


/*
 * Register: UstorDataLwr
 *
 * Name(s):
 * <base>.UstorDataLwr
 */
#define NFP_MECSR_USTORE_DATA_LOWER                        0x0004


/*
 * Register: UstorDataUpr
 *   [19:13]   ECC
 *   [12:0]    UdataUpper
 *
 * Name(s):
 * <base>.UstorDataUpr
 */
#define NFP_MECSR_USTORE_DATA_UPPER                        0x0008
#define   MECSR_USTORE_DATA_ECC(x)                           (((x) & 0x7f) << 13)
#define   MECSR_USTORE_DATA_ECC_of(x)                        (((x) >> 13) & 0x7f)
#define     MECSR_USTORE_DATA_ECC_bf                         0, 19, 13
#define     MECSR_USTORE_DATA_ECC_msk                        (0x7f)
#define     MECSR_USTORE_DATA_ECC_shf                        (13)
#define   MECSR_USTORE_DATA_HIVAL(x)                         (((x) & 0x1fff) << 0)
#define   MECSR_USTORE_DATA_HIVAL_of(x)                      (((x) >> 0) & 0x1fff)
#define     MECSR_USTORE_DATA_HIVAL_bf                       0, 12, 0
#define     MECSR_USTORE_DATA_HIVAL_msk                      (0x1fff)
#define     MECSR_USTORE_DATA_HIVAL_shf                      (0)


/*
 * Register: UstorErrStat
 *   [31]      UncorrectableErr
 *   [26:20]   Syndrome
 *   [18:16]   Context
 *   [13:0]    Uaddr
 *
 * Name(s):
 * <base>.UstorErrStat
 */
#define NFP_MECSR_USTORE_ERROR_STATUS                      0x000c
#define   NFP_MECSR_USTORE_ERROR_STATUS_UNCORRECTABLE        (1 << 31)
#define     NFP_MECSR_USTORE_ERROR_STATUS_UNCORRECTABLE_bf   0, 31, 31
#define     NFP_MECSR_USTORE_ERROR_STATUS_UNCORRECTABLE_msk  (0x1)
#define     NFP_MECSR_USTORE_ERROR_STATUS_UNCORRECTABLE_bit  (31)
#define   NFP_MECSR_USTORE_ERROR_STATUS_SYNDROME(x)          (((x) & 0x7f) << 20)
#define   NFP_MECSR_USTORE_ERROR_STATUS_SYNDROME_of(x)       (((x) >> 20) & 0x7f)
#define     NFP_MECSR_USTORE_ERROR_STATUS_SYNDROME_bf        0, 26, 20
#define     NFP_MECSR_USTORE_ERROR_STATUS_SYNDROME_msk       (0x7f)
#define     NFP_MECSR_USTORE_ERROR_STATUS_SYNDROME_shf       (20)
#define   NFP_MECSR_USTORE_ERROR_STATUS_CTX(x)               (((x) & 7) << 16)
#define   NFP_MECSR_USTORE_ERROR_STATUS_CTX_of(x)            (((x) >> 16) & 7)
#define     NFP_MECSR_USTORE_ERROR_STATUS_CTX_bf             0, 18, 16
#define     NFP_MECSR_USTORE_ERROR_STATUS_CTX_msk            (0x7)
#define     NFP_MECSR_USTORE_ERROR_STATUS_CTX_shf            (16)
#define   NFP_MECSR_USTORE_ERROR_STATUS_UADDR(x)             (((x) & 0x3fff) << 0)
#define   NFP_MECSR_USTORE_ERROR_STATUS_UADDR_of(x)          (((x) >> 0) & 0x3fff)
#define     NFP_MECSR_USTORE_ERROR_STATUS_UADDR_bf           0, 13, 0
#define     NFP_MECSR_USTORE_ERROR_STATUS_UADDR_msk          (0x3fff)
#define     NFP_MECSR_USTORE_ERROR_STATUS_UADDR_shf          (0)


/*
 * Register: ALUOut
 *
 * Name(s):
 * <base>.ALUOut
 */
#define NFP_MECSR_ALU_OUT                                  0x0010


/*
 * Register: CtxArbCtrl
 *   [6:4]     PreviousCtx
 *   [2:0]     NextCtx
 *
 * Name(s):
 * <base>.CtxArbCtrl
 */
#define NFP_MECSR_CTX_ARB_CNTL                             0x0014
#define   NFP_MECSR_CTX_ARB_CNTL_PCTX(x)                     (((x) & 7) << 4)
#define   NFP_MECSR_CTX_ARB_CNTL_PCTX_of(x)                  (((x) >> 4) & 7)
#define     NFP_MECSR_CTX_ARB_CNTL_PCTX_bf                   0, 6, 4
#define     NFP_MECSR_CTX_ARB_CNTL_PCTX_msk                  (0x7)
#define     NFP_MECSR_CTX_ARB_CNTL_PCTX_shf                  (4)
#define   NFP_MECSR_CTX_ARB_CNTL_NCTX(x)                     (((x) & 7) << 0)
#define   NFP_MECSR_CTX_ARB_CNTL_NCTX_of(x)                  (((x) >> 0) & 7)
#define     NFP_MECSR_CTX_ARB_CNTL_NCTX_bf                   0, 2, 0
#define     NFP_MECSR_CTX_ARB_CNTL_NCTX_msk                  (0x7)
#define     NFP_MECSR_CTX_ARB_CNTL_NCTX_shf                  (0)


/*
 * Register: CtxEnables
 *   [31]      InUseContexts
 *   [30]      PseudoRandNum
 *   [29]      CSEccError
 *   [28]      CSEccEnable
 *   [27]      Breakpoint
 *   [25]      RegisterParityErr
 *   [23]      LMEccEnable
 *   [22]      LMAddr3Global
 *   [21]      LMAddr2Global
 *   [20]      NNsendConfig
 *   [19:18]   NextNeighborEmptyAssert
 *   [17]      LMAddr1Global
 *   [16]      LMAddr0Global
 *   [15:8]    CtxEnables
 *   [2:0]     NNreceiveConfig
 *
 * Name(s):
 * <base>.CtxEnables
 */
#define NFP_MECSR_CTX_ENABLES                              0x0018
#define   NFP_MECSR_CTX_ENABLES_IN_USE                       (1 << 31)
#define     NFP_MECSR_CTX_ENABLES_IN_USE_4CTX                (0x80000000)
#define     NFP_MECSR_CTX_ENABLES_IN_USE_bf                  0, 31, 31
#define     NFP_MECSR_CTX_ENABLES_IN_USE_msk                 (0x1)
#define     NFP_MECSR_CTX_ENABLES_IN_USE_bit                 (31)
#define   NFP_MECSR_CTX_ENABLES_PRN_MODE                     (1 << 30)
#define     NFP_MECSR_CTX_ENABLES_PRN_MODE_ME_INSTR          (0)
#define     NFP_MECSR_CTX_ENABLES_PRN_MODE_EVERY_CYCLE       (0x40000000)
#define     NFP_MECSR_CTX_ENABLES_PRN_MODE_bf                0, 30, 30
#define     NFP_MECSR_CTX_ENABLES_PRN_MODE_msk               (0x1)
#define     NFP_MECSR_CTX_ENABLES_PRN_MODE_bit               (30)
#define   NFP_MECSR_CTX_ENABLES_USTORE_ECC_ERR               (1 << 29)
#define     NFP_MECSR_CTX_ENABLES_USTORE_ECC_ERR_bf          0, 29, 29
#define     NFP_MECSR_CTX_ENABLES_USTORE_ECC_ERR_msk         (0x1)
#define     NFP_MECSR_CTX_ENABLES_USTORE_ECC_ERR_bit         (29)
#define   NFP_MECSR_CTX_ENABLES_USTORE_ECC_ENABLE            (1 << 28)
#define     NFP_MECSR_CTX_ENABLES_USTORE_ECC_ENABLE_bf       0, 28, 28
#define     NFP_MECSR_CTX_ENABLES_USTORE_ECC_ENABLE_msk      (0x1)
#define     NFP_MECSR_CTX_ENABLES_USTORE_ECC_ENABLE_bit      (28)
#define   NFP_MECSR_CTX_ENABLES_BREAKPOINT                   (1 << 27)
#define     NFP_MECSR_CTX_ENABLES_BREAKPOINT_bf              0, 27, 27
#define     NFP_MECSR_CTX_ENABLES_BREAKPOINT_msk             (0x1)
#define     NFP_MECSR_CTX_ENABLES_BREAKPOINT_bit             (27)
#define   NFP_MECSR_CTX_ENABLES_REG_PAR_ERR                  (1 << 25)
#define     NFP_MECSR_CTX_ENABLES_REG_PAR_ERR_bf             0, 25, 25
#define     NFP_MECSR_CTX_ENABLES_REG_PAR_ERR_msk            (0x1)
#define     NFP_MECSR_CTX_ENABLES_REG_PAR_ERR_bit            (25)
#define   NFP_MECSR_CTX_ENABLES_LOCAL_MEMORY_ECC_ENABLE      (1 << 23)
#define     NFP_MECSR_CTX_ENABLES_LOCAL_MEMORY_ECC_ENABLE_bf 0, 23, 23
#define     NFP_MECSR_CTX_ENABLES_LOCAL_MEMORY_ECC_ENABLE_msk (0x1)
#define     NFP_MECSR_CTX_ENABLES_LOCAL_MEMORY_ECC_ENABLE_bit (23)
#define   NFP_MECSR_CTX_ENABLES_LM_ADDR_3_GLOB               (1 << 22)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_3_GLOB_bf          0, 22, 22
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_3_GLOB_msk         (0x1)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_3_GLOB_bit         (22)
#define   NFP_MECSR_CTX_ENABLES_LM_ADDR_2_GLOB               (1 << 21)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_2_GLOB_bf          0, 21, 21
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_2_GLOB_msk         (0x1)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_2_GLOB_bit         (21)
#define   NFP_MECSR_CTX_ENABLES_NN_SEND_CONFIGURATION        (1 << 20)
#define     NFP_MECSR_CTX_ENABLES_NN_SEND_CONFIGURATION_NEXT (0)
#define     NFP_MECSR_CTX_ENABLES_NN_SEND_CONFIGURATION_SELF (0x100000)
#define     NFP_MECSR_CTX_ENABLES_NN_SEND_CONFIGURATION_bf   0, 20, 20
#define     NFP_MECSR_CTX_ENABLES_NN_SEND_CONFIGURATION_msk  (0x1)
#define     NFP_MECSR_CTX_ENABLES_NN_SEND_CONFIGURATION_bit  (20)
#define   NFP_MECSR_CTX_ENABLES_NN_RING_EMPTY(x)             (((x) & 3) << 18)
#define   NFP_MECSR_CTX_ENABLES_NN_RING_EMPTY_of(x)          (((x) >> 18) & 3)
#define     NFP_MECSR_CTX_ENABLES_NN_RING_EMPTY_bf           0, 19, 18
#define     NFP_MECSR_CTX_ENABLES_NN_RING_EMPTY_msk          (0x3)
#define     NFP_MECSR_CTX_ENABLES_NN_RING_EMPTY_shf          (18)
#define   NFP_MECSR_CTX_ENABLES_LM_ADDR_1_GLOB               (1 << 17)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_1_GLOB_bf          0, 17, 17
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_1_GLOB_msk         (0x1)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_1_GLOB_bit         (17)
#define   NFP_MECSR_CTX_ENABLES_LM_ADDR_0_GLOB               (1 << 16)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_0_GLOB_bf          0, 16, 16
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_0_GLOB_msk         (0x1)
#define     NFP_MECSR_CTX_ENABLES_LM_ADDR_0_GLOB_bit         (16)
#define   NFP_MECSR_CTX_ENABLES_CONTEXTS(x)                  (((x) & 0xff) << 8)
#define   NFP_MECSR_CTX_ENABLES_CONTEXTS_of(x)               (((x) >> 8) & 0xff)
#define     NFP_MECSR_CTX_ENABLES_CONTEXTS_bf                0, 15, 8
#define     NFP_MECSR_CTX_ENABLES_CONTEXTS_msk               (0xff)
#define     NFP_MECSR_CTX_ENABLES_CONTEXTS_shf               (8)
#define   NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION(x)  (((x) & 7) << 0)
#define   NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_of(x) (((x) >> 0) & 7)
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_NN (0)
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_SELF (1)
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_CTnn (2)
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_PushBus (3)
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_bf 0, 2, 0
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_msk (0x7)
#define     NFP_MECSR_CTX_ENABLES_NN_RECEIVE_CONFIGURATION_shf (0)


/*
 * Register: CondCodeEn
 *   [13]      CurCndtnCodeEn
 *
 * Name(s):
 * <base>.CondCodeEn
 */
#define NFP_MECSR_CC_ENABLE                                0x001c
#define   NFP_MECSR_CC_ENABLE_CCCE                           (1 << 13)
#define     NFP_MECSR_CC_ENABLE_CCCE_bf                      0, 13, 13
#define     NFP_MECSR_CC_ENABLE_CCCE_msk                     (0x1)
#define     NFP_MECSR_CC_ENABLE_CCCE_bit                     (13)


/*
 * Register: CSRCtxPtr
 *   [2:0]     CtxSelect
 *
 * Name(s):
 * <base>.CSRCtxPtr
 */
#define NFP_MECSR_CSR_CTX_POINTER                          0x0020
#define   NFP_MECSR_CSR_CTX_POINTER_CTX(x)                   (((x) & 7) << 0)
#define   NFP_MECSR_CSR_CTX_POINTER_CTX_of(x)                (((x) >> 0) & 7)
#define     NFP_MECSR_CSR_CTX_POINTER_CTX_bf                 0, 2, 0
#define     NFP_MECSR_CSR_CTX_POINTER_CTX_msk                (0x7)
#define     NFP_MECSR_CSR_CTX_POINTER_CTX_shf                (0)


/*
 * Register: PcBreakpoint
 *   [31:24]   PcBrkpntCntl
 *   [13:0]    PcBrkpntAddr
 *
 * Name(s):
 * <base>.PcBreakpoint0 <base>.PcBreakpoint1
 */
#define NFP_MECSR_PC_BREAKPOINT_0                          0x0024
#define NFP_MECSR_PC_BREAKPOINT_1                          0x0028
#define   MECSR_PC_BREAKPOINT_CNTL(x)                        (((x) & 0xff) << 24)
#define   MECSR_PC_BREAKPOINT_CNTL_of(x)                     (((x) >> 24) & 0xff)
#define     MECSR_PC_BREAKPOINT_CNTL_bf                      0, 31, 24
#define     MECSR_PC_BREAKPOINT_CNTL_msk                     (0xff)
#define     MECSR_PC_BREAKPOINT_CNTL_shf                     (24)
#define   MECSR_PC_BREAKPOINT_ADDR(x)                        (((x) & 0x3fff) << 0)
#define   MECSR_PC_BREAKPOINT_ADDR_of(x)                     (((x) >> 0) & 0x3fff)
#define     MECSR_PC_BREAKPOINT_ADDR_bf                      0, 13, 0
#define     MECSR_PC_BREAKPOINT_ADDR_msk                     (0x3fff)
#define     MECSR_PC_BREAKPOINT_ADDR_shf                     (0)


/*
 * Register: PcBreakpointStatus
 *   [9:2]     PcBrkpntedCtx
 *   [1]       PcBrkpnt1Status
 *   [0]       PcBrkpnt0Status
 *
 * Name(s):
 * <base>.PcBreakpointStatus
 */
#define NFP_MECSR_PC_BREAKPOINT_STATUS                     0x002c
#define   NFP_MECSR_PC_BREAKPOINT_STATUS_CTX(x)              (((x) & 0xff) << 2)
#define   NFP_MECSR_PC_BREAKPOINT_STATUS_CTX_of(x)           (((x) >> 2) & 0xff)
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_CTX_bf            0, 9, 2
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_CTX_msk           (0xff)
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_CTX_shf           (2)
#define   NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT1             (1 << 1)
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT1_bf        0, 1, 1
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT1_msk       (0x1)
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT1_bit       (1)
#define   NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT0             (1 << 0)
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT0_bf        0, 0, 0
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT0_msk       (0x1)
#define     NFP_MECSR_PC_BREAKPOINT_STATUS_BRKPNT0_bit       (0)


/*
 * Register: RegErrStatus
 *   [17:16]   Type
 *   [15:0]    RegisterNumber
 *
 * Name(s):
 * <base>.RegErrStatus
 */
#define NFP_MECSR_REGISTER_ERROR_STATUS                    0x0030
#define   NFP_MECSR_REGISTER_ERROR_STATUS_TYPE(x)            (((x) & 3) << 16)
#define   NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_of(x)         (((x) >> 16) & 3)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_GPR         (0)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_XFER        (1)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_NN          (2)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_bf          0, 17, 16
#define     NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_msk         (0x3)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_TYPE_shf         (16)
#define   NFP_MECSR_REGISTER_ERROR_STATUS_REG(x)             (((x) & 0xffff) << 0)
#define   NFP_MECSR_REGISTER_ERROR_STATUS_REG_of(x)          (((x) >> 0) & 0xffff)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_REG_bf           0, 15, 0
#define     NFP_MECSR_REGISTER_ERROR_STATUS_REG_msk          (0xffff)
#define     NFP_MECSR_REGISTER_ERROR_STATUS_REG_shf          (0)


/*
 * Register: LMErrStatus
 *   [31]      UncorrectableErr
 *   [26:20]   Syndrome
 *   [18:16]   Context
 *   [9:0]     LMaddress
 *
 * Name(s):
 * <base>.LMErrStatus
 */
#define NFP_MECSR_LM_ERROR_STATUS                          0x0034
#define   NFP_MECSR_LM_ERROR_STATUS_UNCORRECTABLE            (1 << 31)
#define     NFP_MECSR_LM_ERROR_STATUS_UNCORRECTABLE_bf       0, 31, 31
#define     NFP_MECSR_LM_ERROR_STATUS_UNCORRECTABLE_msk      (0x1)
#define     NFP_MECSR_LM_ERROR_STATUS_UNCORRECTABLE_bit      (31)
#define   NFP_MECSR_LM_ERROR_STATUS_SYNDROME(x)              (((x) & 0x7f) << 20)
#define   NFP_MECSR_LM_ERROR_STATUS_SYNDROME_of(x)           (((x) >> 20) & 0x7f)
#define     NFP_MECSR_LM_ERROR_STATUS_SYNDROME_bf            0, 26, 20
#define     NFP_MECSR_LM_ERROR_STATUS_SYNDROME_msk           (0x7f)
#define     NFP_MECSR_LM_ERROR_STATUS_SYNDROME_shf           (20)
#define   NFP_MECSR_LM_ERROR_STATUS_CTX(x)                   (((x) & 7) << 16)
#define   NFP_MECSR_LM_ERROR_STATUS_CTX_of(x)                (((x) >> 16) & 7)
#define     NFP_MECSR_LM_ERROR_STATUS_CTX_bf                 0, 18, 16
#define     NFP_MECSR_LM_ERROR_STATUS_CTX_msk                (0x7)
#define     NFP_MECSR_LM_ERROR_STATUS_CTX_shf                (16)
#define   NFP_MECSR_LM_ERROR_STATUS_LM_ADDRESS(x)            (((x) & 0x3ff) << 0)
#define   NFP_MECSR_LM_ERROR_STATUS_LM_ADDRESS_of(x)         (((x) >> 0) & 0x3ff)
#define     NFP_MECSR_LM_ERROR_STATUS_LM_ADDRESS_bf          0, 9, 0
#define     NFP_MECSR_LM_ERROR_STATUS_LM_ADDRESS_msk         (0x3ff)
#define     NFP_MECSR_LM_ERROR_STATUS_LM_ADDRESS_shf         (0)


/*
 * Register: LMeccErrorMask
 *   [31:0]    ErrorInjectionMask
 *
 * Name(s):
 * <base>.LMeccErrorMask
 */
#define NFP_MECSR_FORCE_BAD_LM_ECC                         0x0038
#define   NFP_MECSR_FORCE_BAD_LM_ECC_ERROR_INJECTION_MASK(x) (((x) & 0xffffffff) << 0)
#define   NFP_MECSR_FORCE_BAD_LM_ECC_ERROR_INJECTION_MASK_of(x) (((x) >> 0) & 0xffffffff)
#define     NFP_MECSR_FORCE_BAD_LM_ECC_ERROR_INJECTION_MASK_bf 0, 31, 0
#define     NFP_MECSR_FORCE_BAD_LM_ECC_ERROR_INJECTION_MASK_msk (0xffffffff)
#define     NFP_MECSR_FORCE_BAD_LM_ECC_ERROR_INJECTION_MASK_shf (0)


/*
 * Register: IndCtxStatus
 *   [31]      ReadyToRun
 *   [13:0]    ContextPC
 *
 * Name(s):
 * <base>.IndCtxStatus
 */
#define NFP_MECSR_INDIRECT_CTX_STS                         0x0040
#define   NFP_MECSR_INDIRECT_CTX_STS_RR                      (1 << 31)
#define     NFP_MECSR_INDIRECT_CTX_STS_RR_bf                 0, 31, 31
#define     NFP_MECSR_INDIRECT_CTX_STS_RR_msk                (0x1)
#define     NFP_MECSR_INDIRECT_CTX_STS_RR_bit                (31)
#define   NFP_MECSR_INDIRECT_CTX_STS_CTX_PC(x)               (((x) & 0x3fff) << 0)
#define   NFP_MECSR_INDIRECT_CTX_STS_CTX_PC_of(x)            (((x) >> 0) & 0x3fff)
#define     NFP_MECSR_INDIRECT_CTX_STS_CTX_PC_bf             0, 13, 0
#define     NFP_MECSR_INDIRECT_CTX_STS_CTX_PC_msk            (0x3fff)
#define     NFP_MECSR_INDIRECT_CTX_STS_CTX_PC_shf            (0)


/*
 * Register: ActCtxStatus
 *   [31]      AB0
 *   [30:25]   IslandId
 *   [24:8]    ActiveContextPC
 *   [6:3]     MENumber
 *   [2:0]     ActiveContextNo
 *
 * Name(s):
 * <base>.ActCtxStatus
 */
#define NFP_MECSR_ACTIVE_CTX_STS                           0x0044
#define   NFP_MECSR_ACTIVE_CTX_STS_AB0                       (1 << 31)
#define     NFP_MECSR_ACTIVE_CTX_STS_AB0_bf                  0, 31, 31
#define     NFP_MECSR_ACTIVE_CTX_STS_AB0_msk                 (0x1)
#define     NFP_MECSR_ACTIVE_CTX_STS_AB0_bit                 (31)
#define   NFP_MECSR_ACTIVE_CTX_STS_IL_ID(x)                  (((x) & 0x3f) << 25)
#define   NFP_MECSR_ACTIVE_CTX_STS_IL_ID_of(x)               (((x) >> 25) & 0x3f)
#define     NFP_MECSR_ACTIVE_CTX_STS_IL_ID_bf                0, 30, 25
#define     NFP_MECSR_ACTIVE_CTX_STS_IL_ID_msk               (0x3f)
#define     NFP_MECSR_ACTIVE_CTX_STS_IL_ID_shf               (25)
#define   NFP_MECSR_ACTIVE_CTX_STS_ACTXPC(x)                 (((x) & 0x1ffff) << 8)
#define   NFP_MECSR_ACTIVE_CTX_STS_ACTXPC_of(x)              (((x) >> 8) & 0x1ffff)
#define     NFP_MECSR_ACTIVE_CTX_STS_ACTXPC_bf               0, 24, 8
#define     NFP_MECSR_ACTIVE_CTX_STS_ACTXPC_msk              (0x1ffff)
#define     NFP_MECSR_ACTIVE_CTX_STS_ACTXPC_shf              (8)
#define   NFP_MECSR_ACTIVE_CTX_STS_ME_ID(x)                  (((x) & 0xf) << 3)
#define   NFP_MECSR_ACTIVE_CTX_STS_ME_ID_of(x)               (((x) >> 3) & 0xf)
#define     NFP_MECSR_ACTIVE_CTX_STS_ME_ID_bf                0, 6, 3
#define     NFP_MECSR_ACTIVE_CTX_STS_ME_ID_msk               (0xf)
#define     NFP_MECSR_ACTIVE_CTX_STS_ME_ID_shf               (3)
#define   NFP_MECSR_ACTIVE_CTX_STS_ACNO(x)                   (((x) & 7) << 0)
#define   NFP_MECSR_ACTIVE_CTX_STS_ACNO_of(x)                (((x) >> 0) & 7)
#define     NFP_MECSR_ACTIVE_CTX_STS_ACNO_bf                 0, 2, 0
#define     NFP_MECSR_ACTIVE_CTX_STS_ACNO_msk                (0x7)
#define     NFP_MECSR_ACTIVE_CTX_STS_ACNO_shf                (0)


/*
 * Register: CtxSglEvt
 *   [15:1]    Signals
 *   [0]       VoluntaryWakeup
 *
 * Name(s):
 * <base>.IndCtxSglEvt <base>.ActCtxSglEvt
 */
#define NFP_MECSR_INDIRECT_CTX_SIG_EVENTS                  0x0048
#define NFP_MECSR_ACTIVE_CTX_SIG_EVENTS                    0x004c
#define   MECSR_SIG_EVENTS_SIGNALS(x)                        (((x) & 0x7fff) << 1)
#define   MECSR_SIG_EVENTS_SIGNALS_of(x)                     (((x) >> 1) & 0x7fff)
#define     MECSR_SIG_EVENTS_SIGNALS_bf                      0, 15, 1
#define     MECSR_SIG_EVENTS_SIGNALS_msk                     (0x7fff)
#define     MECSR_SIG_EVENTS_SIGNALS_shf                     (1)
#define   MECSR_SIG_EVENTS_VOLUNTARY                         (1 << 0)
#define     MECSR_SIG_EVENTS_VOLUNTARY_bf                    0, 0, 0
#define     MECSR_SIG_EVENTS_VOLUNTARY_msk                   (0x1)
#define     MECSR_SIG_EVENTS_VOLUNTARY_bit                   (0)


/*
 * Register: CtxWkpEvt
 *   [16]      AnyWakeEvnts
 *   [15:1]    WakeupEvents
 *   [0]       Voluntary
 *
 * Name(s):
 * <base>.IndCtxWkpEvt <base>.ActCtxWkpEvt
 */
#define NFP_MECSR_INDIRECT_CTX_WAKEUP_EVENTS               0x0050
#define NFP_MECSR_ACTIVE_CTX_WAKEUP_EVENTS                 0x0054
#define   MECSR_WAKEUP_ANY                                   (1 << 16)
#define     MECSR_WAKEUP_ANY_bf                              0, 16, 16
#define     MECSR_WAKEUP_ANY_msk                             (0x1)
#define     MECSR_WAKEUP_ANY_bit                             (16)
#define   MECSR_WAKEUP_EVENTS(x)                             (((x) & 0x7fff) << 1)
#define   MECSR_WAKEUP_EVENTS_of(x)                          (((x) >> 1) & 0x7fff)
#define     MECSR_WAKEUP_EVENTS_bf                           0, 15, 1
#define     MECSR_WAKEUP_EVENTS_msk                          (0x7fff)
#define     MECSR_WAKEUP_EVENTS_shf                          (1)
#define   MECSR_WAKEUP_VOLUNTARY                             (1 << 0)
#define     MECSR_WAKEUP_VOLUNTARY_bf                        0, 0, 0
#define     MECSR_WAKEUP_VOLUNTARY_msk                       (0x1)
#define     MECSR_WAKEUP_VOLUNTARY_bit                       (0)


/*
 * Register: CtxFtrCnt
 *   [15:0]    FutureCount
 *
 * Name(s):
 * <base>.IndCtxFtrCnt <base>.ActCtxFtrCnt
 */
#define NFP_MECSR_INDIRECT_CTX_FUTURE_COUNT                0x0058
#define NFP_MECSR_ACTIVE_CTX_FUTURE_COUNT                  0x005c
#define   MECSR_CTX_CNT(x)                                   (((x) & 0xffff) << 0)
#define   MECSR_CTX_CNT_of(x)                                (((x) >> 0) & 0xffff)
#define     MECSR_CTX_CNT_bf                                 0, 15, 0
#define     MECSR_CTX_CNT_msk                                (0xffff)
#define     MECSR_CTX_CNT_shf                                (0)


/*
 * Register: LMAddr
 *   [11:2]    LocalMemoryAddr
 *
 * Name(s):
 * <base>.IndLMAddr0 <base>.ActLMAddr0 <base>.IndLMAddr1 <base>.ActLMAddr1
 * <base>.IndLMAddr2 <base>.ActLMAddr2 <base>.IndLMAddr3 <base>.ActLMAddr3
 */
#define NFP_MECSR_INDIRECT_LM_ADDR_0                       0x0060
#define NFP_MECSR_ACTIVE_LM_ADDR_0                         0x0064
#define NFP_MECSR_INDIRECT_LM_ADDR_1                       0x0068
#define NFP_MECSR_ACTIVE_LM_ADDR_1                         0x006c
#define NFP_MECSR_INDIRECT_LM_ADDR_2                       0x0090
#define NFP_MECSR_ACTIVE_LM_ADDR_2                         0x0094
#define NFP_MECSR_INDIRECT_LM_ADDR_3                       0x0098
#define NFP_MECSR_ACTIVE_LM_ADDR_3                         0x009c
#define   MECSR_LM_ADDR(x)                                   (((x) & 0x3ff) << 2)
#define   MECSR_LM_ADDR_of(x)                                (((x) >> 2) & 0x3ff)
#define     MECSR_LM_ADDR_bf                                 0, 11, 2
#define     MECSR_LM_ADDR_msk                                (0x3ff)
#define     MECSR_LM_ADDR_shf                                (2)


/*
 * Register: ByteIndex
 *   [1:0]     ByteNumber
 *
 * Name(s):
 * <base>.ByteIndex
 */
#define NFP_MECSR_BYTE_INDEX                               0x0070
#define   MECSR_BYTE_N0(x)                                   (((x) & 3) << 0)
#define   MECSR_BYTE_N0_of(x)                                (((x) >> 0) & 3)
#define     MECSR_BYTE_N0_bf                                 0, 1, 0
#define     MECSR_BYTE_N0_msk                                (0x3)
#define     MECSR_BYTE_N0_shf                                (0)


/*
 * Register: XferIndex
 *   [9:2]     XferIndex
 *
 * Name(s):
 * <base>.XferIndex
 */
#define NFP_MECSR_T_INDEX                                  0x0074
#define   MECSR_XFER_INDEX(x)                                (((x) & 0xff) << 2)
#define   MECSR_XFER_INDEX_of(x)                             (((x) >> 2) & 0xff)
#define     MECSR_XFER_INDEX_bf                              0, 9, 2
#define     MECSR_XFER_INDEX_msk                             (0xff)
#define     MECSR_XFER_INDEX_shf                             (2)


/*
 * Register: FtrCntSgl
 *   [3:0]     SignalNo
 *
 * Name(s):
 * <base>.IndFtrCntSgl <base>.ActFtrCntSgl
 */
#define NFP_MECSR_INDIRECT_FUTURE_COUNT_SIGNAL             0x0078
#define NFP_MECSR_ACTIVE_FUTURE_COUNT_SIGNAL               0x007c
#define   MECSR_CTX_CNT_SIGNAL_NO(x)                         (((x) & 0xf) << 0)
#define   MECSR_CTX_CNT_SIGNAL_NO_of(x)                      (((x) >> 0) & 0xf)
#define     MECSR_CTX_CNT_SIGNAL_NO_bf                       0, 3, 0
#define     MECSR_CTX_CNT_SIGNAL_NO_msk                      (0xf)
#define     MECSR_CTX_CNT_SIGNAL_NO_shf                      (0)


/*
 * Register: NNPutGet
 *   [6:0]     NNRegIndex
 *
 * Name(s):
 * <base>.NNPut <base>.NNGet
 */
#define NFP_MECSR_NN_PUT                                   0x0080
#define NFP_MECSR_NN_GET                                   0x0084
#define   MECSR_NN_REG_INDEX(x)                              (((x) & 0x7f) << 0)
#define   MECSR_NN_REG_INDEX_of(x)                           (((x) >> 0) & 0x7f)
#define     MECSR_NN_REG_INDEX_bf                            0, 6, 0
#define     MECSR_NN_REG_INDEX_msk                           (0x7f)
#define     MECSR_NN_REG_INDEX_shf                           (0)


/*
 * Register: LMAddrBytIdx
 *
 * Name(s):
 * <base>.IndLMAddr2BytIdx <base>.ActLMAddr2BytIdx <base>.IndLMAddr3BytIdx
 * <base>.ActLMAddr3BytIdx <base>.IndLMAddr0BytIdx <base>.ActLMAddr0BytIdx
 * <base>.IndLMAddr1BytIdx <base>.ActLMAddr1BytIdx
 */
#define NFP_MECSR_INDIRECT_LM_ADDR_2_BYTE_INDEX            0x00a0
#define NFP_MECSR_ACTIVE_LM_ADDR_2_BYTE_INDEX              0x00a4
#define NFP_MECSR_INDIRECT_LM_ADDR_3_BYTE_INDEX            0x00a8
#define NFP_MECSR_ACTIVE_LM_ADDR_3_BYTE_INDEX              0x00ac
#define NFP_MECSR_INDIRECT_LM_ADDR_0_BYTE_INDEX            0x00e0
#define NFP_MECSR_ACTIVE_LM_ADDR_0_BYTE_INDEX              0x00e4
#define NFP_MECSR_INDIRECT_LM_ADDR_1_BYTE_INDEX            0x00e8
#define NFP_MECSR_ACTIVE_LM_ADDR_1_BYTE_INDEX              0x00ec


/*
 * Register: IndPredCC
 *   [3:0]     PredCCSel
 *
 * Name(s):
 * <base>.IndPredCC
 */
#define NFP_MECSR_INDIRECT_PREDICATE_CC                    0x00b0
#define   NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT(x)          (((x) & 0xf) << 0)
#define   NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_of(x)       (((x) >> 0) & 0xf)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PEQ       (0)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PNE       (1)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PMI       (2)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PPL       (3)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PCS       (4)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PCC       (5)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PVS       (6)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PVC       (7)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PGE       (8)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PLT       (9)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PLE       (0xa)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_PGT       (0xb)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_bf        0, 3, 0
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_msk       (0xf)
#define     NFP_MECSR_INDIRECT_PREDICATE_CC_SELECT_shf       (0)


/*
 * Register: TimestampLow
 *
 * Name(s):
 * <base>.TimestampLow
 */
#define NFP_MECSR_TIMESTAMP_LOW                            0x00c0


/*
 * Register: TimestampHgh
 *
 * Name(s):
 * <base>.TimestampHgh
 */
#define NFP_MECSR_TIMESTAMP_HIGH                           0x00c4


/*
 * Register: XfrAndBytIdx
 *
 * Name(s):
 * <base>.XfrAndBytIdx
 */
#define NFP_MECSR_T_INDEX_BYTE_INDEX                       0x00f4


/*
 * Register: NxtNghbrSgl
 *   [7]       ThisContext
 *   [6:3]     SignalNo
 *   [2:0]     Context
 *
 * Name(s):
 * <base>.NxtNghbrSgl
 */
#define NFP_MECSR_NEXT_NEIGHBOR_SIGNAL                     0x0100
#define   NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_THIS_CTX            (1 << 7)
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_THIS_CTX_bf       0, 7, 7
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_THIS_CTX_msk      (0x1)
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_THIS_CTX_bit      (7)
#define   NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_SIG_NO(x)           (((x) & 0xf) << 3)
#define   NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_SIG_NO_of(x)        (((x) >> 3) & 0xf)
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_SIG_NO_bf         0, 6, 3
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_SIG_NO_msk        (0xf)
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_SIG_NO_shf        (3)
#define   NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_CTX(x)              (((x) & 7) << 0)
#define   NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_CTX_of(x)           (((x) >> 0) & 7)
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_CTX_bf            0, 2, 0
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_CTX_msk           (0x7)
#define     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_CTX_shf           (0)


/*
 * Register: PrvNghbrSgl
 *   [7]       ThisContext
 *   [6:3]     SignalNo
 *   [2:0]     Context
 *
 * Name(s):
 * <base>.PrvNghbrSgl
 */
#define NFP_MECSR_PREV_NEIGHBOR_SIGNAL                     0x0104
#define   NFP_MECSR_PREV_NEIGHBOR_SIGNAL_THIS_CTX            (1 << 7)
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_THIS_CTX_bf       0, 7, 7
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_THIS_CTX_msk      (0x1)
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_THIS_CTX_bit      (7)
#define   NFP_MECSR_PREV_NEIGHBOR_SIGNAL_SIG_NO(x)           (((x) & 0xf) << 3)
#define   NFP_MECSR_PREV_NEIGHBOR_SIGNAL_SIG_NO_of(x)        (((x) >> 3) & 0xf)
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_SIG_NO_bf         0, 6, 3
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_SIG_NO_msk        (0xf)
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_SIG_NO_shf        (3)
#define   NFP_MECSR_PREV_NEIGHBOR_SIGNAL_CTX(x)              (((x) & 7) << 0)
#define   NFP_MECSR_PREV_NEIGHBOR_SIGNAL_CTX_of(x)           (((x) >> 0) & 7)
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_CTX_bf            0, 2, 0
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_CTX_msk           (0x7)
#define     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_CTX_shf           (0)


/*
 * Register: SameMESignal
 *   [7]       NextContext
 *   [6:3]     SignalNo
 *   [2:0]     Context
 *
 * Name(s):
 * <base>.SameMESignal
 */
#define NFP_MECSR_SAME_ME_SIGNAL                           0x0108
#define   NFP_MECSR_SAME_ME_SIGNAL_NEXT_CTX                  (1 << 7)
#define     NFP_MECSR_SAME_ME_SIGNAL_NEXT_CTX_bf             0, 7, 7
#define     NFP_MECSR_SAME_ME_SIGNAL_NEXT_CTX_msk            (0x1)
#define     NFP_MECSR_SAME_ME_SIGNAL_NEXT_CTX_bit            (7)
#define   NFP_MECSR_SAME_ME_SIGNAL_SIG_NO(x)                 (((x) & 0xf) << 3)
#define   NFP_MECSR_SAME_ME_SIGNAL_SIG_NO_of(x)              (((x) >> 3) & 0xf)
#define     NFP_MECSR_SAME_ME_SIGNAL_SIG_NO_bf               0, 6, 3
#define     NFP_MECSR_SAME_ME_SIGNAL_SIG_NO_msk              (0xf)
#define     NFP_MECSR_SAME_ME_SIGNAL_SIG_NO_shf              (3)
#define   NFP_MECSR_SAME_ME_SIGNAL_CTX(x)                    (((x) & 7) << 0)
#define   NFP_MECSR_SAME_ME_SIGNAL_CTX_of(x)                 (((x) >> 0) & 7)
#define     NFP_MECSR_SAME_ME_SIGNAL_CTX_bf                  0, 2, 0
#define     NFP_MECSR_SAME_ME_SIGNAL_CTX_msk                 (0x7)
#define     NFP_MECSR_SAME_ME_SIGNAL_CTX_shf                 (0)


/*
 * Register: CRCRemainder
 *
 * Name(s):
 * <base>.CRCRemainder
 */
#define NFP_MECSR_CRC_REMAINDER                            0x0140


/*
 * Register: ProfileCnt
 *
 * Name(s):
 * <base>.ProfileCnt
 */
#define NFP_MECSR_PROFILE_COUNT                            0x0144


/*
 * Register: PseudoRndNum
 *
 * Name(s):
 * <base>.PseudoRndNum
 */
#define NFP_MECSR_PSEUDO_RANDOM_NUMBER                     0x0148


/*
 * Register: MiscControl
 *   [30]      ReqOnCsEccError
 *   [29]      ReqOnLmEccError
 *   [26]      Interrupt
 *   [25]      ECCCorrGate
 *   [24]      ParityEnable
 *   [23]      ForceBadParity
 *   [12]      UstorECCCrctEn
 *   [5]       NNPshAdrMode
 *   [4]       ThrdPrtyAdrMode
 *   [3]       LegacyIndRef
 *   [2]       ShareUstore
 *   [1]       LMRegionPull
 *   [0]       LMRegionPush
 *
 * Name(s):
 * <base>.MiscControl
 */
#define NFP_MECSR_MISC_CONTROL                             0x0160
#define   NFP_MECSR_MISC_CONTROL_REQUEST_ON_CS_ECC_ERROR     (1 << 30)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_CS_ECC_ERROR_NO REQUEST (0)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_CS_ECC_ERROR_REQUEST (0x40000000)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_CS_ECC_ERROR_bf 0, 30, 30
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_CS_ECC_ERROR_msk (0x1)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_CS_ECC_ERROR_bit (30)
#define   NFP_MECSR_MISC_CONTROL_REQUEST_ON_LM_ECC_ERROR     (1 << 29)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_LM_ECC_ERROR_NO REQUEST (0)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_LM_ECC_ERROR_REQUEST (0x20000000)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_LM_ECC_ERROR_bf 0, 29, 29
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_LM_ECC_ERROR_msk (0x1)
#define     NFP_MECSR_MISC_CONTROL_REQUEST_ON_LM_ECC_ERROR_bit (29)
#define   NFP_MECSR_MISC_CONTROL_INTERRUPT                   (1 << 26)
#define     NFP_MECSR_MISC_CONTROL_INTERRUPT_NOT_INTERRUPTED (0)
#define     NFP_MECSR_MISC_CONTROL_INTERRUPT_INTERRUPTED     (0x4000000)
#define     NFP_MECSR_MISC_CONTROL_INTERRUPT_bf              0, 26, 26
#define     NFP_MECSR_MISC_CONTROL_INTERRUPT_msk             (0x1)
#define     NFP_MECSR_MISC_CONTROL_INTERRUPT_bit             (26)
#define   NFP_MECSR_MISC_CONTROL_ECC_CORR_GATE               (1 << 25)
#define     NFP_MECSR_MISC_CONTROL_ECC_CORR_GATE_DISABLE     (0)
#define     NFP_MECSR_MISC_CONTROL_ECC_CORR_GATE_ENABLE      (0x2000000)
#define     NFP_MECSR_MISC_CONTROL_ECC_CORR_GATE_bf          0, 25, 25
#define     NFP_MECSR_MISC_CONTROL_ECC_CORR_GATE_msk         (0x1)
#define     NFP_MECSR_MISC_CONTROL_ECC_CORR_GATE_bit         (25)
#define   NFP_MECSR_MISC_CONTROL_PARITY_ENBLE                (1 << 24)
#define     NFP_MECSR_MISC_CONTROL_PARITY_ENBLE_bf           0, 24, 24
#define     NFP_MECSR_MISC_CONTROL_PARITY_ENBLE_msk          (0x1)
#define     NFP_MECSR_MISC_CONTROL_PARITY_ENBLE_bit          (24)
#define   NFP_MECSR_MISC_CONTROL_FORCE_BAR_PARITY            (1 << 23)
#define     NFP_MECSR_MISC_CONTROL_FORCE_BAR_PARITY_bf       0, 23, 23
#define     NFP_MECSR_MISC_CONTROL_FORCE_BAR_PARITY_msk      (0x1)
#define     NFP_MECSR_MISC_CONTROL_FORCE_BAR_PARITY_bit      (23)
#define   NFP_MECSR_MISC_CONTROL_USTORE_ECC_CORRECT          (1 << 12)
#define     NFP_MECSR_MISC_CONTROL_USTORE_ECC_CORRECT_bf     0, 12, 12
#define     NFP_MECSR_MISC_CONTROL_USTORE_ECC_CORRECT_msk    (0x1)
#define     NFP_MECSR_MISC_CONTROL_USTORE_ECC_CORRECT_bit    (12)
#define   NFP_MECSR_MISC_CONTROL_NN_PUSH_ADDRESS_MODE        (1 << 5)
#define     NFP_MECSR_MISC_CONTROL_NN_PUSH_ADDRESS_MODE_IND  (0)
#define     NFP_MECSR_MISC_CONTROL_NN_PUSH_ADDRESS_MODE_ABS  (0x20)
#define     NFP_MECSR_MISC_CONTROL_NN_PUSH_ADDRESS_MODE_bf   0, 5, 5
#define     NFP_MECSR_MISC_CONTROL_NN_PUSH_ADDRESS_MODE_msk  (0x1)
#define     NFP_MECSR_MISC_CONTROL_NN_PUSH_ADDRESS_MODE_bit  (5)
#define   NFP_MECSR_MISC_CONTROL_THIRD_PARTY_ADDRESS_MODE    (1 << 4)
#define     NFP_MECSR_MISC_CONTROL_THIRD_PARTY_ADDRESS_MODE_40BIT (0)
#define     NFP_MECSR_MISC_CONTROL_THIRD_PARTY_ADDRESS_MODE_32BIT (0x10)
#define     NFP_MECSR_MISC_CONTROL_THIRD_PARTY_ADDRESS_MODE_bf 0, 4, 4
#define     NFP_MECSR_MISC_CONTROL_THIRD_PARTY_ADDRESS_MODE_msk (0x1)
#define     NFP_MECSR_MISC_CONTROL_THIRD_PARTY_ADDRESS_MODE_bit (4)
#define   NFP_MECSR_MISC_CONTROL_LEGACY_INDIRECT_REF_MODE    (1 << 3)
#define     NFP_MECSR_MISC_CONTROL_LEGACY_INDIRECT_REF_MODE_NORMAL (0)
#define     NFP_MECSR_MISC_CONTROL_LEGACY_INDIRECT_REF_MODE_LEGACY (8)
#define     NFP_MECSR_MISC_CONTROL_LEGACY_INDIRECT_REF_MODE_bf 0, 3, 3
#define     NFP_MECSR_MISC_CONTROL_LEGACY_INDIRECT_REF_MODE_msk (0x1)
#define     NFP_MECSR_MISC_CONTROL_LEGACY_INDIRECT_REF_MODE_bit (3)
#define   NFP_MECSR_MISC_CONTROL_SHARE_USTORE                (1 << 2)
#define     NFP_MECSR_MISC_CONTROL_SHARE_USTORE_NOT_SHARED   (0)
#define     NFP_MECSR_MISC_CONTROL_SHARE_USTORE_SHARED       (4)
#define     NFP_MECSR_MISC_CONTROL_SHARE_USTORE_bf           0, 2, 2
#define     NFP_MECSR_MISC_CONTROL_SHARE_USTORE_msk          (0x1)
#define     NFP_MECSR_MISC_CONTROL_SHARE_USTORE_bit          (2)
#define   NFP_MECSR_MISC_CONTROL_LM_REGION_PULL              (1 << 1)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PULL_LOWER_HALF (0)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PULL_UPPER_HALF (2)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PULL_bf         0, 1, 1
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PULL_msk        (0x1)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PULL_bit        (1)
#define   NFP_MECSR_MISC_CONTROL_LM_REGION_PUSH              (1 << 0)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PUSH_LOWER_HALF (0)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PUSH_UPPER_HALF (1)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PUSH_bf         0, 0, 0
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PUSH_msk        (0x1)
#define     NFP_MECSR_MISC_CONTROL_LM_REGION_PUSH_bit        (0)


/*
 * Register: PcBreakpointMask
 *   [13:0]    PcMask
 *
 * Name(s):
 * <base>.PcBreakpoint0Mask <base>.PcBreakpoint1Mask
 */
#define NFP_MECSR_PC_BREAKPOINT_0_MASK                     0x0164
#define NFP_MECSR_PC_BREAKPOINT_1_MASK                     0x0168
#define   MECSR_PC_BREAKPOINT_MASK_MASK(x)                   (((x) & 0x3fff) << 0)
#define   MECSR_PC_BREAKPOINT_MASK_MASK_of(x)                (((x) >> 0) & 0x3fff)
#define     MECSR_PC_BREAKPOINT_MASK_MASK_bf                 0, 13, 0
#define     MECSR_PC_BREAKPOINT_MASK_MASK_msk                (0x3fff)
#define     MECSR_PC_BREAKPOINT_MASK_MASK_shf                (0)


/*
 * Register: Mailbox
 *
 * Name(s):
 * <base>.Mailbox0 <base>.Mailbox1 <base>.Mailbox2 <base>.Mailbox3
 */
#define NFP_MECSR_MAILBOX_0                                0x0170
#define NFP_MECSR_MAILBOX_1                                0x0174
#define NFP_MECSR_MAILBOX_2                                0x0178
#define NFP_MECSR_MAILBOX_3                                0x017c

/*
 * Register: CmdIndirectRef0
 *
 * Name(s):
 * <base>.CmdIndirectRef0
 */
#define NFP_MECSR_CMD_INDIRECT_REF_0                       0x0190


/*
 * Register: CmdIndirectRef1
 *
 * Name(s):
 * <base>.CmdIndirectRef1
 */
#define NFP_MECSR_CMD_INDIRECT_REF_1                       0x0194



/*
 * ME CSRs
 */
/*
 * Register: PrevAlu (Note: This is not strictly a CSR)
 *   [31:16]   Data16
 *   [15]      Reserved (0)
 *   [14]      OvSigCtx
 *   [13]      OvSigNum
 *   [12:8]    Length
 *   [7]       OvLen
 *   [6]       OvBmCSR
 *   [5:3]     OveData
 *   [2:1]     OveMaster
 *   [0]       OvSM
 */
#define NFP_MECSR_PREV_ALU_DATA16(x)                       (((x) & 0xffff) << 16)
#define NFP_MECSR_PREV_ALU_DATA16_of(x)                    (((x) >> 16) & 0xffff)
#define   NFP_MECSR_PREV_ALU_DATA16_bf                     0, 31, 16
#define   NFP_MECSR_PREV_ALU_DATA16_mask                   (0xffff)
#define   NFP_MECSR_PREV_ALU_DATA16_shift                  (16)
#define NFP_MECSR_PREV_ALU_OV_SIG_CTX                      (1 << 14)
#define   NFP_MECSR_PREV_ALU_OV_SIG_CTX_bf                 0, 14, 14
#define   NFP_MECSR_PREV_ALU_OV_SIG_CTX_mask               (0x1)
#define   NFP_MECSR_PREV_ALU_OV_SIG_CTX_bit                (14)
#define NFP_MECSR_PREV_ALU_OV_SIG_NUM                      (1 << 13)
#define   NFP_MECSR_PREV_ALU_OV_SIG_NUM_bf                 0, 13, 13
#define   NFP_MECSR_PREV_ALU_OV_SIG_NUM_mask               (0x1)
#define   NFP_MECSR_PREV_ALU_OV_SIG_NUM_bit                (13)
#define NFP_MECSR_PREV_ALU_LENGTH(x)                       (((x) & 0x1f) << 8)
#define NFP_MECSR_PREV_ALU_LENGTH_of(x)                    (((x) >> 8) & 0x1f)
#define   NFP_MECSR_PREV_ALU_LENGTH_bf                     0, 12, 8
#define   NFP_MECSR_PREV_ALU_LENGTH_mask                   (0x1f)
#define   NFP_MECSR_PREV_ALU_LENGTH_shift                  (8)
#define NFP_MECSR_PREV_ALU_OV_LEN                          (1 << 7)
#define   NFP_MECSR_PREV_ALU_OV_LEN_bf                     0, 7, 7
#define   NFP_MECSR_PREV_ALU_OV_LEN_mask                   (0x1)
#define   NFP_MECSR_PREV_ALU_OV_LEN_bit                    (7)
#define NFP_MECSR_PREV_ALU_OV_BM_CSR                       (1 << 6)
#define   NFP_MECSR_PREV_ALU_OV_BM_CSR_bf                  0, 6, 6
#define   NFP_MECSR_PREV_ALU_OV_BM_CSR_mask                (0x1)
#define   NFP_MECSR_PREV_ALU_OV_BM_CSR_bit                 (6)
#define NFP_MECSR_PREV_ALU_OVE_DATA(x)                     (((x) & 7) << 3)
#define NFP_MECSR_PREV_ALU_OVE_DATA_of(x)                  (((x) >> 3) & 7)
#define   NFP_MECSR_PREV_ALU_OVE_DATA_bf                   0, 5, 3
#define   NFP_MECSR_PREV_ALU_OVE_DATA_mask                 (7)
#define   NFP_MECSR_PREV_ALU_OVE_DATA_shift                (3)
#define NFP_MECSR_PREV_ALU_OVE_MASTER(x)                   (((x) & 3) << 1)
#define NFP_MECSR_PREV_ALU_OVE_MASTER_of(x)                (((x) >> 1) & 3)
#define   NFP_MECSR_PREV_ALU_OVE_MASTER_bf                 0, 2, 1
#define   NFP_MECSR_PREV_ALU_OVE_MASTER_mask               (3)
#define   NFP_MECSR_PREV_ALU_OVE_MASTER_shift              (1)
#define NFP_MECSR_PREV_ALU_OV_SM                           (1 << 0)
#define   NFP_MECSR_PREV_ALU_OV_SM_bf                      0, 0, 0
#define   NFP_MECSR_PREV_ALU_OV_SM_mask                    (0x1)
#define   NFP_MECSR_PREV_ALU_OV_SM_bit                     (0)

/*
 * ME CSR register structures
 */
#if defined(__NFP_LANG_MICROC)

struct nfp_mecsr_ustore_address {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int ecs:1;
            unsigned int write_strobe:1;
            unsigned int data_invert:1;
            unsigned int __reserved_13:16;
            unsigned int addr:13;
#           else
            unsigned int addr:13;
            unsigned int __reserved_13:16;
            unsigned int data_invert:1;
            unsigned int write_strobe:1;
            unsigned int ecs:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ustore_data_lower {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int data:32;
#           else
            unsigned int data:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ustore_data {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_20:12;
            unsigned int ecc:7;
            unsigned int hival:13;
#           else
            unsigned int hival:13;
            unsigned int ecc:7;
            unsigned int __reserved_20:12;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ustore_error_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int uncorrectable:1;
            unsigned int __reserved_27:4;
            unsigned int syndrome:7;
            unsigned int __reserved_19:1;
            unsigned int ctx:3;
            unsigned int __reserved_14:2;
            unsigned int uaddr:14;
#           else
            unsigned int uaddr:14;
            unsigned int __reserved_14:2;
            unsigned int ctx:3;
            unsigned int __reserved_19:1;
            unsigned int syndrome:7;
            unsigned int __reserved_27:4;
            unsigned int uncorrectable:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_alu_out {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int value:32;
#           else
            unsigned int value:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ctx_arb_cntl {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_7:25;
            unsigned int pctx:3;
            unsigned int __reserved_3:1;
            unsigned int nctx:3;
#           else
            unsigned int nctx:3;
            unsigned int __reserved_3:1;
            unsigned int pctx:3;
            unsigned int __reserved_7:25;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ctx_enables {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int in_use:1;
            unsigned int prn_mode:1;
            unsigned int ustore_ecc_err:1;
            unsigned int ustore_ecc_enable:1;
            unsigned int breakpoint:1;
            unsigned int __reserved_26:1;
            unsigned int reg_par_err:1;
            unsigned int __reserved_24:1;
            unsigned int local_memory_ecc_enable:1;
            unsigned int lm_addr_3_glob:1;
            unsigned int lm_addr_2_glob:1;
            unsigned int nn_send_configuration:1;
            unsigned int nn_ring_empty:2;
            unsigned int lm_addr_1_glob:1;
            unsigned int lm_addr_0_glob:1;
            unsigned int contexts:8;
            unsigned int __reserved_3:5;
            unsigned int nn_receive_configuration:3;
#           else
            unsigned int nn_receive_configuration:3;
            unsigned int __reserved_3:5;
            unsigned int contexts:8;
            unsigned int lm_addr_0_glob:1;
            unsigned int lm_addr_1_glob:1;
            unsigned int nn_ring_empty:2;
            unsigned int nn_send_configuration:1;
            unsigned int lm_addr_2_glob:1;
            unsigned int lm_addr_3_glob:1;
            unsigned int local_memory_ecc_enable:1;
            unsigned int __reserved_24:1;
            unsigned int reg_par_err:1;
            unsigned int __reserved_26:1;
            unsigned int breakpoint:1;
            unsigned int ustore_ecc_enable:1;
            unsigned int ustore_ecc_err:1;
            unsigned int prn_mode:1;
            unsigned int in_use:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_cc_enable {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_14:18;
            unsigned int ccce:1;
            unsigned int __reserved_0:13;
#           else
            unsigned int __reserved_0:13;
            unsigned int ccce:1;
            unsigned int __reserved_14:18;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_csr_ctx_pointer {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_3:29;
            unsigned int ctx:3;
#           else
            unsigned int ctx:3;
            unsigned int __reserved_3:29;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_pc_breakpoint {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int cntl:8;
            unsigned int __reserved_14:10;
            unsigned int addr:14;
#           else
            unsigned int addr:14;
            unsigned int __reserved_14:10;
            unsigned int cntl:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_pc_breakpoint_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_10:22;
            unsigned int ctx:8;
            unsigned int brkpnt1:1;
            unsigned int brkpnt0:1;
#           else
            unsigned int brkpnt0:1;
            unsigned int brkpnt1:1;
            unsigned int ctx:8;
            unsigned int __reserved_10:22;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_register_error_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_18:14;
            unsigned int type:2;
            unsigned int reg:16;
#           else
            unsigned int reg:16;
            unsigned int type:2;
            unsigned int __reserved_18:14;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_lm_error_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int uncorrectable:1;
            unsigned int __reserved_27:4;
            unsigned int syndrome:7;
            unsigned int __reserved_19:1;
            unsigned int ctx:3;
            unsigned int __reserved_10:6;
            unsigned int lm_address:10;
#           else
            unsigned int lm_address:10;
            unsigned int __reserved_10:6;
            unsigned int ctx:3;
            unsigned int __reserved_19:1;
            unsigned int syndrome:7;
            unsigned int __reserved_27:4;
            unsigned int uncorrectable:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_force_bad_lm_ecc {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int error_injection_mask:32;
#           else
            unsigned int error_injection_mask:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_indirect_ctx_sts {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int rr:1;
            unsigned int __reserved_14:17;
            unsigned int ctx_pc:14;
#           else
            unsigned int ctx_pc:14;
            unsigned int __reserved_14:17;
            unsigned int rr:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_active_ctx_sts {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int ab0:1;
            unsigned int il_id:6;
            unsigned int actxpc:17;
            unsigned int __reserved_7:1;
            unsigned int me_id:4;
            unsigned int acno:3;
#           else
            unsigned int acno:3;
            unsigned int me_id:4;
            unsigned int __reserved_7:1;
            unsigned int actxpc:17;
            unsigned int il_id:6;
            unsigned int ab0:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_sig_events {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int signals:15;
            unsigned int voluntary:1;
#           else
            unsigned int voluntary:1;
            unsigned int signals:15;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_wakeup {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_17:15;
            unsigned int any:1;
            unsigned int events:15;
            unsigned int voluntary:1;
#           else
            unsigned int voluntary:1;
            unsigned int events:15;
            unsigned int any:1;
            unsigned int __reserved_17:15;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ctx {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int cnt:16;
#           else
            unsigned int cnt:16;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_lm {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_12:20;
            unsigned int addr:10;
            unsigned int __reserved_0:2;
#           else
            unsigned int __reserved_0:2;
            unsigned int addr:10;
            unsigned int __reserved_12:20;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_byte {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_2:30;
            unsigned int n0:2;
#           else
            unsigned int n0:2;
            unsigned int __reserved_2:30;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_xfer {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_10:22;
            unsigned int index:8;
            unsigned int __reserved_0:2;
#           else
            unsigned int __reserved_0:2;
            unsigned int index:8;
            unsigned int __reserved_10:22;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_ctx_cnt_signal {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_4:28;
            unsigned int no:4;
#           else
            unsigned int no:4;
            unsigned int __reserved_4:28;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_nn {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_7:25;
            unsigned int reg_index:7;
#           else
            unsigned int reg_index:7;
            unsigned int __reserved_7:25;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_lmb {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_12:20;
            unsigned int lm_addr:10;
            unsigned int byte_no:2;
#           else
            unsigned int byte_no:2;
            unsigned int lm_addr:10;
            unsigned int __reserved_12:20;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_indirect_predicate_cc {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_4:28;
            unsigned int select:4;
#           else
            unsigned int select:4;
            unsigned int __reserved_4:28;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_timestamp_low {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int count:32;
#           else
            unsigned int count:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_timestamp_high {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int count:32;
#           else
            unsigned int count:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_tb {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_10:22;
            unsigned int xfer_index:8;
            unsigned int byte_no:2;
#           else
            unsigned int byte_no:2;
            unsigned int xfer_index:8;
            unsigned int __reserved_10:22;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_next_neighbor_signal {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_8:24;
            unsigned int this_ctx:1;
            unsigned int sig_no:4;
            unsigned int ctx:3;
#           else
            unsigned int ctx:3;
            unsigned int sig_no:4;
            unsigned int this_ctx:1;
            unsigned int __reserved_8:24;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_prev_neighbor_signal {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_8:24;
            unsigned int this_ctx:1;
            unsigned int sig_no:4;
            unsigned int ctx:3;
#           else
            unsigned int ctx:3;
            unsigned int sig_no:4;
            unsigned int this_ctx:1;
            unsigned int __reserved_8:24;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_same_me_signal {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_8:24;
            unsigned int next_ctx:1;
            unsigned int sig_no:4;
            unsigned int ctx:3;
#           else
            unsigned int ctx:3;
            unsigned int sig_no:4;
            unsigned int next_ctx:1;
            unsigned int __reserved_8:24;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_crc_remainder {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int crc_remainder:32;
#           else
            unsigned int crc_remainder:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_profile_count {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int count:16;
#           else
            unsigned int count:16;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_pseudo_random_number {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int number:32;
#           else
            unsigned int number:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_misc_control {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_31:1;
            unsigned int request_on_cs_ecc_error:1;
            unsigned int request_on_lm_ecc_error:1;
            unsigned int __reserved_27:2;
            unsigned int interrupt:1;
            unsigned int ecc_corr_gate:1;
            unsigned int parity_enble:1;
            unsigned int force_bar_parity:1;
            unsigned int __reserved_13:10;
            unsigned int ustore_ecc_correct:1;
            unsigned int __reserved_6:6;
            unsigned int nn_push_address_mode:1;
            unsigned int third_party_address_mode:1;
            unsigned int legacy_indirect_ref_mode:1;
            unsigned int share_ustore:1;
            unsigned int lm_region_pull:1;
            unsigned int lm_region_push:1;
#           else
            unsigned int lm_region_push:1;
            unsigned int lm_region_pull:1;
            unsigned int share_ustore:1;
            unsigned int legacy_indirect_ref_mode:1;
            unsigned int third_party_address_mode:1;
            unsigned int nn_push_address_mode:1;
            unsigned int __reserved_6:6;
            unsigned int ustore_ecc_correct:1;
            unsigned int __reserved_13:10;
            unsigned int force_bar_parity:1;
            unsigned int parity_enble:1;
            unsigned int ecc_corr_gate:1;
            unsigned int interrupt:1;
            unsigned int __reserved_27:2;
            unsigned int request_on_lm_ecc_error:1;
            unsigned int request_on_cs_ecc_error:1;
            unsigned int __reserved_31:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_pc_breakpoint_mask {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_14:18;
            unsigned int mask:14;
#           else
            unsigned int mask:14;
            unsigned int __reserved_14:18;
#           endif
        };
        unsigned int __raw;
    };
};


struct nfp_mecsr_cmd_indirect_ref_0 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_30:2;
            unsigned int island:6;
            unsigned int master:4;
            unsigned int signal_master:4;
            unsigned int signal_ctx:3;
            unsigned int signal_num:4;
            unsigned int __reserved_8:1;
            unsigned int byte_mask:8;
#           else
            unsigned int byte_mask:8;
            unsigned int __reserved_8:1;
            unsigned int signal_num:4;
            unsigned int signal_ctx:3;
            unsigned int signal_master:4;
            unsigned int master:4;
            unsigned int island:6;
            unsigned int __reserved_30:2;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_mecsr_cmd_indirect_ref_1 {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int target:4;
            unsigned int action:5;
            unsigned int token:2;
            unsigned int __reserved_3:2;
            unsigned int ov_target:1;
            unsigned int ov_action:1;
            unsigned int ov_token:1;
#           else
            unsigned int ov_token:1;
            unsigned int ov_action:1;
            unsigned int ov_target:1;
            unsigned int __reserved_3:2;
            unsigned int token:2;
            unsigned int action:5;
            unsigned int target:4;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};



struct nfp_mecsr_prev_alu {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int data16:16;
            unsigned int res:1;
            unsigned int ov_sig_ctx:1;
            unsigned int ov_sig_num:1;
            unsigned int length:5;
            unsigned int ov_len:1;
            unsigned int ov_bm_csr:1;
            unsigned int ove_data:3;
            unsigned int ove_master:2;
            unsigned int ov_sm:1;
#           else
            unsigned int ov_sm:1;
            unsigned int ove_master:2;
            unsigned int ove_data:3;
            unsigned int ov_bm_csr:1;
            unsigned int ov_len:1;
            unsigned int length:5;
            unsigned int ov_sig_num:1;
            unsigned int ov_sig_ctx:1;
            unsigned int res:1;
            unsigned int data16:16;
#           endif
        };
        unsigned int __raw;
    };
};

#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_ME_H_ */
