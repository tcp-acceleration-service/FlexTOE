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
 * @file          nfp6000/nfp_cls.h
 * @brief         NFP6000 CLS CSR definitions
 *
 * This files is auto-generated. Do not edit!
 */

#ifndef _NFP6000__NFP_CLS_H_
#define _NFP6000__NFP_CLS_H_


/*
 * CLS CSRs
 */

/*
 * Register: ClsRingBase
 *   [31]      Full
 *   [30]      NotEmpty
 *   [28]      ContainsWork
 *   [27:24]   Report
 *   [18:16]   Size
 *   [8:0]     Base
 *
 * Name(s):
 * <base>.Rings.RingBase0...
 */
#define NFP_CLS_RINGS_RING_BASE(x)                         (0x10000 + ((x) * 0x8))
#define   NFP_CLS_RINGS_RING_BASE_FULL                       (1 << 31)
#define     NFP_CLS_RINGS_RING_BASE_FULL_bf                  0, 31, 31
#define     NFP_CLS_RINGS_RING_BASE_FULL_msk                 (0x1)
#define     NFP_CLS_RINGS_RING_BASE_FULL_bit                 (31)
#define   NFP_CLS_RINGS_RING_BASE_NOT_EMPTY                  (1 << 30)
#define     NFP_CLS_RINGS_RING_BASE_NOT_EMPTY_bf             0, 30, 30
#define     NFP_CLS_RINGS_RING_BASE_NOT_EMPTY_msk            (0x1)
#define     NFP_CLS_RINGS_RING_BASE_NOT_EMPTY_bit            (30)
#define   NFP_CLS_RINGS_RING_BASE_WORK                       (1 << 28)
#define     NFP_CLS_RINGS_RING_BASE_WORK_bf                  0, 28, 28
#define     NFP_CLS_RINGS_RING_BASE_WORK_msk                 (0x1)
#define     NFP_CLS_RINGS_RING_BASE_WORK_bit                 (28)
#define   NFP_CLS_RINGS_RING_BASE_REPORT(x)                  (((x) & 0xf) << 24)
#define   NFP_CLS_RINGS_RING_BASE_REPORT_of(x)               (((x) >> 24) & 0xf)
#define     NFP_CLS_RINGS_RING_BASE_REPORT_NOT_FULL          (8)
#define     NFP_CLS_RINGS_RING_BASE_REPORT_UNDERFLOW         (1)
#define     NFP_CLS_RINGS_RING_BASE_REPORT_OVERFLOW          (2)
#define     NFP_CLS_RINGS_RING_BASE_REPORT_NOT_EMPTY         (4)
#define     NFP_CLS_RINGS_RING_BASE_REPORT_bf                0, 27, 24
#define     NFP_CLS_RINGS_RING_BASE_REPORT_msk               (0xf)
#define     NFP_CLS_RINGS_RING_BASE_REPORT_shf               (24)
#define   NFP_CLS_RINGS_RING_BASE_SIZE(x)                    (((x) & 7) << 16)
#define   NFP_CLS_RINGS_RING_BASE_SIZE_of(x)                 (((x) >> 16) & 7)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_32                  (0)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_64                  (1)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_128                 (2)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_256                 (3)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_512                 (4)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_1024                (5)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_bf                  0, 18, 16
#define     NFP_CLS_RINGS_RING_BASE_SIZE_msk                 (0x7)
#define     NFP_CLS_RINGS_RING_BASE_SIZE_shf                 (16)
#define   NFP_CLS_RINGS_RING_BASE_BASE(x)                    (((x) & 0x1ff) << 0)
#define   NFP_CLS_RINGS_RING_BASE_BASE_of(x)                 (((x) >> 0) & 0x1ff)
#define     NFP_CLS_RINGS_RING_BASE_BASE_bf                  0, 8, 0
#define     NFP_CLS_RINGS_RING_BASE_BASE_msk                 (0x1ff)
#define     NFP_CLS_RINGS_RING_BASE_BASE_shf                 (0)


/*
 * Register: ClsRingPointers
 *   [28:16]   TailPointer
 *   [12:0]    HeadPointer
 *
 * Name(s):
 * <base>.Rings.RingPtrs0...
 */
#define NFP_CLS_RINGS_RING_PTRS(x)                         (0x10080 + ((x) * 0x8))
#define   NFP_CLS_RINGS_RING_PTRS_TAIL_POINTER(x)            (((x) & 0x1fff) << 16)
#define   NFP_CLS_RINGS_RING_PTRS_TAIL_POINTER_of(x)         (((x) >> 16) & 0x1fff)
#define     NFP_CLS_RINGS_RING_PTRS_TAIL_POINTER_bf          0, 28, 16
#define     NFP_CLS_RINGS_RING_PTRS_TAIL_POINTER_msk         (0x1fff)
#define     NFP_CLS_RINGS_RING_PTRS_TAIL_POINTER_shf         (16)
#define   NFP_CLS_RINGS_RING_PTRS_HEAD_POINTER(x)            (((x) & 0x1fff) << 0)
#define   NFP_CLS_RINGS_RING_PTRS_HEAD_POINTER_of(x)         (((x) >> 0) & 0x1fff)
#define     NFP_CLS_RINGS_RING_PTRS_HEAD_POINTER_bf          0, 12, 0
#define     NFP_CLS_RINGS_RING_PTRS_HEAD_POINTER_msk         (0x1fff)
#define     NFP_CLS_RINGS_RING_PTRS_HEAD_POINTER_shf         (0)

/* EventManagerMap: <base>.EventManager */
#define NFP_CLS_EM                                         0x20000

/*
 * Register: ClsAutoFilterStatusMonitor
 *   [17]      LastStatus
 *   [16]      EdgeDetected
 *   [9:8]     Monitor
 *   [3:0]     Autopush
 *
 * Name(s):
 * <base>.Autopush.FilterStatusMonitor0...
 */
#define NFP_CLS_AUTOPUSH_STATUS(x)                         (0x30000 + ((x) * 0x8))
#define   NFP_CLS_AUTOPUSH_STATUS_LAST_STATUS                (1 << 17)
#define     NFP_CLS_AUTOPUSH_STATUS_LAST_STATUS_bf           0, 17, 17
#define     NFP_CLS_AUTOPUSH_STATUS_LAST_STATUS_msk          (0x1)
#define     NFP_CLS_AUTOPUSH_STATUS_LAST_STATUS_bit          (17)
#define   NFP_CLS_AUTOPUSH_STATUS_EDGE_DETECTED              (1 << 16)
#define     NFP_CLS_AUTOPUSH_STATUS_EDGE_DETECTED_bf         0, 16, 16
#define     NFP_CLS_AUTOPUSH_STATUS_EDGE_DETECTED_msk        (0x1)
#define     NFP_CLS_AUTOPUSH_STATUS_EDGE_DETECTED_bit        (16)
#define   NFP_CLS_AUTOPUSH_STATUS_MONITOR(x)                 (((x) & 3) << 8)
#define   NFP_CLS_AUTOPUSH_STATUS_MONITOR_of(x)              (((x) >> 8) & 3)
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_OFF              (0)
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_STATUS           (1)
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_ONE_SHOT_STATUS  (2)
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_ONE_SHOT_ACK     (3)
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_bf               0, 9, 8
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_msk              (0x3)
#define     NFP_CLS_AUTOPUSH_STATUS_MONITOR_shf              (8)
#define   NFP_CLS_AUTOPUSH_STATUS_AUTOPUSH(x)                (((x) & 0xf) << 0)
#define   NFP_CLS_AUTOPUSH_STATUS_AUTOPUSH_of(x)             (((x) >> 0) & 0xf)
#define     NFP_CLS_AUTOPUSH_STATUS_AUTOPUSH_bf              0, 3, 0
#define     NFP_CLS_AUTOPUSH_STATUS_AUTOPUSH_msk             (0xf)
#define     NFP_CLS_AUTOPUSH_STATUS_AUTOPUSH_shf             (0)


/*
 * Register: ClsAutoSignal
 *   [31:28]   MasterIslandLow
 *   [27:24]   Master
 *   [22:16]   SignalRef
 *   [15:14]   MasterIslandHigh
 *   [13:0]    DataRef
 *
 * Name(s):
 * <base>.Autopush.AutopushSignal0...
 */
#define NFP_CLS_AUTOPUSH_SIGNAL(x)                         (0x30200 + ((x) * 0x8))
#define   NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_LO(x)           (((x) & 0xf) << 28)
#define   NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_LO_of(x)        (((x) >> 28) & 0xf)
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_LO_bf         0, 31, 28
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_LO_msk        (0xf)
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_LO_shf        (28)
#define   NFP_CLS_AUTOPUSH_SIGNAL_MASTER(x)                  (((x) & 0xf) << 24)
#define   NFP_CLS_AUTOPUSH_SIGNAL_MASTER_of(x)               (((x) >> 24) & 0xf)
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_bf                0, 27, 24
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_msk               (0xf)
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_shf               (24)
#define   NFP_CLS_AUTOPUSH_SIGNAL_SIGNAL_REF(x)              (((x) & 0x7f) << 16)
#define   NFP_CLS_AUTOPUSH_SIGNAL_SIGNAL_REF_of(x)           (((x) >> 16) & 0x7f)
#define     NFP_CLS_AUTOPUSH_SIGNAL_SIGNAL_REF_bf            0, 22, 16
#define     NFP_CLS_AUTOPUSH_SIGNAL_SIGNAL_REF_msk           (0x7f)
#define     NFP_CLS_AUTOPUSH_SIGNAL_SIGNAL_REF_shf           (16)
#define   NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_HI(x)           (((x) & 3) << 14)
#define   NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_HI_of(x)        (((x) >> 14) & 3)
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_HI_bf         0, 15, 14
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_HI_msk        (0x3)
#define     NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_HI_shf        (14)
#define   NFP_CLS_AUTOPUSH_SIGNAL_DATA_REF(x)                (((x) & 0x3fff) << 0)
#define   NFP_CLS_AUTOPUSH_SIGNAL_DATA_REF_of(x)             (((x) >> 0) & 0x3fff)
#define     NFP_CLS_AUTOPUSH_SIGNAL_DATA_REF_bf              0, 13, 0
#define     NFP_CLS_AUTOPUSH_SIGNAL_DATA_REF_msk             (0x3fff)
#define     NFP_CLS_AUTOPUSH_SIGNAL_DATA_REF_shf             (0)

#define NFP_CLS_AUTOPUSH_USER_EVENT                        0x30400
#define   NFP_CLS_AUTOPUSH_USER_EVENT_SOURCE(x)              (((x) & 0xfff) << 4)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_SOURCE_of(x)           (((x) >> 4) & 0xfff)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_SOURCE_bf            0, 15, 4
#define     NFP_CLS_AUTOPUSH_USER_EVENT_SOURCE_msk           (0xfff)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_SOURCE_shf           (4)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_TYPE(x)                (((x) & 0xf) << 0)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_TYPE_of(x)             (((x) >> 0) & 0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_TYPE_bf              0, 3, 0
#define     NFP_CLS_AUTOPUSH_USER_EVENT_TYPE_msk             (0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_TYPE_shf             (0)

#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_OVERFLOW        (1 << 20)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_OVERFLOW_bf   0, 20, 20
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_OVERFLOW_msk  (0x1)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_OVERFLOW_bit  (20)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_HWM(x)          (((x) & 0xf) << 16)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_HWM_of(x)       (((x) >> 16) & 0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_HWM_bf        0, 19, 16
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_HWM_msk       (0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_HWM_shf       (16)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_READ(x)         (((x) & 0xf) << 8)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_READ_of(x)      (((x) >> 8) & 0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_READ_bf       0, 11, 8
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_READ_msk      (0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_READ_shf      (8)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_WRITE(x)        (((x) & 0xf) << 0)
#define   NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_WRITE_of(x)     (((x) >> 0) & 0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_WRITE_bf      0, 3, 0
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_WRITE_msk     (0xf)
#define     NFP_CLS_AUTOPUSH_USER_EVENT_STATUS_WRITE_shf     (0)


/*
 * Register: ClsAutoWriteAlertConfig
 *   [23:16]   AddressMask
 *   [15:8]    AddressMatch
 *   [6:4]     Region
 *   [2]       GenerateEvents
 *   [1]       RequireData63
 *   [0]       Enable
 *
 * Name(s):
 * <base>.Autopush.WriteAlertConfig
 */
#define NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG                   0x30600
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MASK(x)      (((x) & 0xff) << 16)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MASK_of(x)   (((x) >> 16) & 0xff)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MASK_bf    0, 23, 16
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MASK_msk   (0xff)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MASK_shf   (16)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MATCH(x)     (((x) & 0xff) << 8)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MATCH_of(x)  (((x) >> 8) & 0xff)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MATCH_bf   0, 15, 8
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MATCH_msk  (0xff)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MATCH_shf  (8)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_REGION(x)         (((x) & 7) << 4)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_REGION_of(x)      (((x) >> 4) & 7)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_REGION_bf       0, 6, 4
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_REGION_msk      (0x7)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_REGION_shf      (4)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_EVENTS            (1 << 2)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_EVENTS_bf       0, 2, 2
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_EVENTS_msk      (0x1)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_EVENTS_bit      (2)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_DATA63            (1 << 1)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_DATA63_bf       0, 1, 1
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_DATA63_msk      (0x1)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_DATA63_bit      (1)
#define   NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ENABLE            (1 << 0)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ENABLE_bf       0, 0, 0
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ENABLE_msk      (0x1)
#define     NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ENABLE_bit      (0)


/*
 * Register: ClsAutoWriteAlertsPending
 *
 * Name(s):
 * <base>.Autopush.WriteAlertsPending
 */
#define NFP_CLS_AUTOPUSH_WRITE_ALERT_PEND                  0x30608

/*
 * Register: ClsAutoVFTranslation
 *   [25:16]   BaseAddress
 *   [11:10]   OffsetSize
 *   [9:8]     VFOffset
 *   [7:6]     VFAlert
 *   [5:4]     Alignment
 *   [1]       ProtectVfs
 *   [0]       Enable
 *
 * Name(s):
 * <base>.Autopush.VFTranslation
 */
#define NFP_CLS_AUTOPUSH_VF_TRANSLATION                    0x30620
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_BASE(x)            (((x) & 0x3ff) << 16)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_BASE_of(x)         (((x) >> 16) & 0x3ff)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_BASE_bf          0, 25, 16
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_BASE_msk         (0x3ff)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_BASE_shf         (16)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE(x)     (((x) & 3) << 10)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_of(x)  (((x) >> 10) & 3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_OS64B (0)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_OS1kB (1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_OS4kB (2)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_OS16kB (3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_bf   0, 11, 10
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_msk  (0x3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE_shf  (10)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS(x)          (((x) & 3) << 8)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_of(x)       (((x) >> 8) & 3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_VF4kB     (0)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_VF32kB    (1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_VF256kB   (2)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_VF2MB     (3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_bf        0, 9, 8
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_msk       (0x3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS_shf       (8)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT(x)        (((x) & 3) << 6)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT_of(x)     (((x) >> 6) & 3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT_ALERT11 (0)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT_ALERT20 (1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT_bf      0, 7, 6
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT_msk     (0x3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT_shf     (6)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN(x)           (((x) & 3) << 4)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_of(x)        (((x) >> 4) & 3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_NONE       (0)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_RESVD      (1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_WORD       (2)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_WORD64     (3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_bf         0, 5, 4
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_msk        (0x3)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN_shf        (4)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_PROT_VFS           (1 << 1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_PROT_VFS_bf      0, 1, 1
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_PROT_VFS_msk     (0x1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_PROT_VFS_bit     (1)
#define   NFP_CLS_AUTOPUSH_VF_TRANSLATION_ENABLE             (1 << 0)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ENABLE_bf        0, 0, 0
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ENABLE_msk       (0x1)
#define     NFP_CLS_AUTOPUSH_VF_TRANSLATION_ENABLE_bit       (0)


/*
 * Register: ClsHashMultiply
 *   [8]       SboxEnable
 *   [7:4]     NumSboxes
 *   [3]       Mult63
 *   [2]       Mult53
 *   [1]       Mult36
 *   [0]       Mult4
 *
 * Name(s):
 * <base>.Hash.HashMultiply
 */
#define NFP_CLS_HASH_MULT                                  0x40000
#define   NFP_CLS_HASH_MULT_SBOXEN                           (1 << 8)
#define     NFP_CLS_HASH_MULT_SBOXEN_bf                      0, 8, 8
#define     NFP_CLS_HASH_MULT_SBOXEN_msk                     (0x1)
#define     NFP_CLS_HASH_MULT_SBOXEN_bit                     (8)
#define   NFP_CLS_HASH_MULT_NUMSBOXES(x)                     (((x) & 0xf) << 4)
#define   NFP_CLS_HASH_MULT_NUMSBOXES_of(x)                  (((x) >> 4) & 0xf)
#define     NFP_CLS_HASH_MULT_NUMSBOXES_bf                   0, 7, 4
#define     NFP_CLS_HASH_MULT_NUMSBOXES_msk                  (0xf)
#define     NFP_CLS_HASH_MULT_NUMSBOXES_shf                  (4)
#define   NFP_CLS_HASH_MULT_M63                              (1 << 3)
#define     NFP_CLS_HASH_MULT_M63_bf                         0, 3, 3
#define     NFP_CLS_HASH_MULT_M63_msk                        (0x1)
#define     NFP_CLS_HASH_MULT_M63_bit                        (3)
#define   NFP_CLS_HASH_MULT_M53                              (1 << 2)
#define     NFP_CLS_HASH_MULT_M53_bf                         0, 2, 2
#define     NFP_CLS_HASH_MULT_M53_msk                        (0x1)
#define     NFP_CLS_HASH_MULT_M53_bit                        (2)
#define   NFP_CLS_HASH_MULT_M36                              (1 << 1)
#define     NFP_CLS_HASH_MULT_M36_bf                         0, 1, 1
#define     NFP_CLS_HASH_MULT_M36_msk                        (0x1)
#define     NFP_CLS_HASH_MULT_M36_bit                        (1)
#define   NFP_CLS_HASH_MULT_M4                               (1 << 0)
#define     NFP_CLS_HASH_MULT_M4_bf                          0, 0, 0
#define     NFP_CLS_HASH_MULT_M4_msk                         (0x1)
#define     NFP_CLS_HASH_MULT_M4_bit                         (0)


/*
 * Register: ClsHashIndexLow
 *   [31:0]    IndexLow
 *
 * Name(s):
 * <base>.Hash.HashIndexLow0...
 */
#define NFP_CLS_HASH_IDX_LO(x)                             (0x40010 + ((x) * 0x100))
#define   NFP_CLS_HASH_IDX_LO_VAL(x)                         (((x) & 0xffffffff) << 0)
#define   NFP_CLS_HASH_IDX_LO_VAL_of(x)                      (((x) >> 0) & 0xffffffff)
#define     NFP_CLS_HASH_IDX_LO_VAL_bf                       0, 31, 0
#define     NFP_CLS_HASH_IDX_LO_VAL_msk                      (0xffffffff)
#define     NFP_CLS_HASH_IDX_LO_VAL_shf                      (0)


/*
 * Register: ClsHashIndexHigh
 *   [31:0]    IndexHigh
 *
 * Name(s):
 * <base>.Hash.HashIndexHigh0...
 */
#define NFP_CLS_HASH_IDX_HI(x)                             (0x40018 + ((x) * 0x100))
#define   NFP_CLS_HASH_IDX_HI_VAL(x)                         (((x) & 0xffffffff) << 0)
#define   NFP_CLS_HASH_IDX_HI_VAL_of(x)                      (((x) >> 0) & 0xffffffff)
#define     NFP_CLS_HASH_IDX_HI_VAL_bf                       0, 31, 0
#define     NFP_CLS_HASH_IDX_HI_VAL_msk                      (0xffffffff)
#define     NFP_CLS_HASH_IDX_HI_VAL_shf                      (0)


/*
 * Register: ClsHashIndex64
 *
 * Name(s):
 * <base>.Hash.HashIndex0...
 */
#define NFP_CLS_HASH_IDX64(x)                              (0x40800 + ((x) * 0x8))
/* InterruptManagerMap: <base>.InterruptManager */
#define NFP_CLS_IM                                         0x50000
/* PeripheralTrngDataMap: <base>.PeripheralTrngData */
#define NFP_CLS_TRNG                                       0x60000
/* ClsNfaMap: <base>.Nfa */
#define NFP_CLS_NFA                                        0x70000


/*
 * Event manager CSRs are accessed at 8-byte offsets in CLS
 */
#define NFP_CLS_EM_CSR_SPACING                             8

/*
 * CLS register structures
 */
#if defined(__NFP_LANG_MICROC)

struct nfp_cls_rings_ring_base {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int full:1;
            unsigned int not_empty:1;
            unsigned int __reserved_29:1;
            unsigned int work:1;
            unsigned int report:4;
            unsigned int __reserved_19:5;
            unsigned int size:3;
            unsigned int __reserved_9:7;
            unsigned int base:9;
#           else
            unsigned int base:9;
            unsigned int __reserved_9:7;
            unsigned int size:3;
            unsigned int __reserved_19:5;
            unsigned int report:4;
            unsigned int work:1;
            unsigned int __reserved_29:1;
            unsigned int not_empty:1;
            unsigned int full:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_cls_rings_ring_ptrs {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_29:3;
            unsigned int tail_pointer:13;
            unsigned int __reserved_13:3;
            unsigned int head_pointer:13;
#           else
            unsigned int head_pointer:13;
            unsigned int __reserved_13:3;
            unsigned int tail_pointer:13;
            unsigned int __reserved_29:3;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_cls_autopush_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_18:14;
            unsigned int last_status:1;
            unsigned int edge_detected:1;
            unsigned int __reserved_10:6;
            unsigned int monitor:2;
            unsigned int __reserved_4:4;
            unsigned int autopush:4;
#           else
            unsigned int autopush:4;
            unsigned int __reserved_4:4;
            unsigned int monitor:2;
            unsigned int __reserved_10:6;
            unsigned int edge_detected:1;
            unsigned int last_status:1;
            unsigned int __reserved_18:14;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_cls_autopush_signal {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int master_isl_lo:4;
            unsigned int master:4;
            unsigned int __reserved_23:1;
            unsigned int signal_ref:7;
            unsigned int master_isl_hi:2;
            unsigned int data_ref:14;
#           else
            unsigned int data_ref:14;
            unsigned int master_isl_hi:2;
            unsigned int signal_ref:7;
            unsigned int __reserved_23:1;
            unsigned int master:4;
            unsigned int master_isl_lo:4;
#           endif
        };
        unsigned int __raw;
    };
};

        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int source:12;
            unsigned int type:4;
#           else
            unsigned int type:4;
            unsigned int source:12;
            unsigned int __reserved_16:16;
#           endif
        } nfp_cls_autopush_user_event;
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_21:11;
            unsigned int overflow:1;
            unsigned int hwm:4;
            unsigned int __reserved_12:4;
            unsigned int read:4;
            unsigned int __reserved_4:4;
            unsigned int write:4;
#           else
            unsigned int write:4;
            unsigned int __reserved_4:4;
            unsigned int read:4;
            unsigned int __reserved_12:4;
            unsigned int hwm:4;
            unsigned int overflow:1;
            unsigned int __reserved_21:11;
#           endif
        } nfp_cls_autopush_user_event_status;

struct nfp_cls_autopush_write_alert_cfg {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_24:8;
            unsigned int addr_mask:8;
            unsigned int addr_match:8;
            unsigned int __reserved_7:1;
            unsigned int region:3;
            unsigned int __reserved_3:1;
            unsigned int events:1;
            unsigned int data63:1;
            unsigned int enable:1;
#           else
            unsigned int enable:1;
            unsigned int data63:1;
            unsigned int events:1;
            unsigned int __reserved_3:1;
            unsigned int region:3;
            unsigned int __reserved_7:1;
            unsigned int addr_match:8;
            unsigned int addr_mask:8;
            unsigned int __reserved_24:8;
#           endif
        };
        unsigned int __raw;
    };
};


struct nfp_cls_autopush_vf_translation {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_26:6;
            unsigned int base:10;
            unsigned int __reserved_12:4;
            unsigned int offset_size:2;
            unsigned int vf_ofs:2;
            unsigned int vf_alert:2;
            unsigned int align:2;
            unsigned int __reserved_2:2;
            unsigned int prot_vfs:1;
            unsigned int enable:1;
#           else
            unsigned int enable:1;
            unsigned int prot_vfs:1;
            unsigned int __reserved_2:2;
            unsigned int align:2;
            unsigned int vf_alert:2;
            unsigned int vf_ofs:2;
            unsigned int offset_size:2;
            unsigned int __reserved_12:4;
            unsigned int base:10;
            unsigned int __reserved_26:6;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_cls_hash_mult {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_9:23;
            unsigned int sboxen:1;
            unsigned int numsboxes:4;
            unsigned int m63:1;
            unsigned int m53:1;
            unsigned int m36:1;
            unsigned int m4:1;
#           else
            unsigned int m4:1;
            unsigned int m36:1;
            unsigned int m53:1;
            unsigned int m63:1;
            unsigned int numsboxes:4;
            unsigned int sboxen:1;
            unsigned int __reserved_9:23;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_cls_hash_idx_lo {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int val:32;
#           else
            unsigned int val:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_cls_hash_idx_hi {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int val:32;
#           else
            unsigned int val:32;
#           endif
        };
        unsigned int __raw;
    };
};




#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_CLS_H_ */
