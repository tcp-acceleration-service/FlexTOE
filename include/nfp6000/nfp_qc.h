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
 * @file          nfp6000/nfp_qc.h
 * @brief         NFP6000 Queue Controller CSR definitions
 *
 * This files is auto-generated. Do not edit!
 */

#ifndef _NFP6000__NFP_QC_H_
#define _NFP6000__NFP_QC_H_


/*
 * QC CSRs
 */

/*
 * Register: QueueAddToReadPtr
 *   [17:0]    Operand
 *
 * Name(s):
 * <base>.AddToReadPtr
 */
#define NFP_QC_ADD_RPTR                                    0x0000
#define   NFP_QC_ADD_RPTR_VAL(x)                             (((x) & 0x3ffff) << 0)
#define   NFP_QC_ADD_RPTR_VAL_of(x)                          (((x) >> 0) & 0x3ffff)
#define     NFP_QC_ADD_RPTR_VAL_bf                           0, 17, 0
#define     NFP_QC_ADD_RPTR_VAL_msk                          (0x3ffff)
#define     NFP_QC_ADD_RPTR_VAL_shf                          (0)


/*
 * Register: QueueAddToWritePtr
 *   [17:0]    Operand
 *
 * Name(s):
 * <base>.AddToWritePtr
 */
#define NFP_QC_ADD_WPTR                                    0x0004
#define   NFP_QC_ADD_WPTR_VAL(x)                             (((x) & 0x3ffff) << 0)
#define   NFP_QC_ADD_WPTR_VAL_of(x)                          (((x) >> 0) & 0x3ffff)
#define     NFP_QC_ADD_WPTR_VAL_bf                           0, 17, 0
#define     NFP_QC_ADD_WPTR_VAL_msk                          (0x3ffff)
#define     NFP_QC_ADD_WPTR_VAL_shf                          (0)


/*
 * Register: QueueConfigStatusLow
 *   [31]      ReadPtrEnable
 *   [30]      Overflowed
 *   [29]      Underflowed
 *   [28]      WMReached
 *   [27]      Full
 *   [26]      Empty
 *   [25:20]   EventData
 *   [19:18]   EventType
 *   [17:0]    ReadPtr
 *
 * Name(s):
 * <base>.ConfigStatusLow
 */
#define NFP_QC_STS_LO                                      0x0008
#define   NFP_QC_STS_LO_RPTR_ENABLE                          (1 << 31)
#define     NFP_QC_STS_LO_RPTR_ENABLE_bf                     0, 31, 31
#define     NFP_QC_STS_LO_RPTR_ENABLE_msk                    (0x1)
#define     NFP_QC_STS_LO_RPTR_ENABLE_bit                    (31)
#define   NFP_QC_STS_LO_OVERFLOWED                           (1 << 30)
#define     NFP_QC_STS_LO_OVERFLOWED_bf                      0, 30, 30
#define     NFP_QC_STS_LO_OVERFLOWED_msk                     (0x1)
#define     NFP_QC_STS_LO_OVERFLOWED_bit                     (30)
#define   NFP_QC_STS_LO_UNDERFLOWED                          (1 << 29)
#define     NFP_QC_STS_LO_UNDERFLOWED_bf                     0, 29, 29
#define     NFP_QC_STS_LO_UNDERFLOWED_msk                    (0x1)
#define     NFP_QC_STS_LO_UNDERFLOWED_bit                    (29)
#define   NFP_QC_STS_LO_WMREACHED                            (1 << 28)
#define     NFP_QC_STS_LO_WMREACHED_bf                       0, 28, 28
#define     NFP_QC_STS_LO_WMREACHED_msk                      (0x1)
#define     NFP_QC_STS_LO_WMREACHED_bit                      (28)
#define   NFP_QC_STS_LO_FULL                                 (1 << 27)
#define     NFP_QC_STS_LO_FULL_bf                            0, 27, 27
#define     NFP_QC_STS_LO_FULL_msk                           (0x1)
#define     NFP_QC_STS_LO_FULL_bit                           (27)
#define   NFP_QC_STS_LO_EMPTY                                (1 << 26)
#define     NFP_QC_STS_LO_EMPTY_bf                           0, 26, 26
#define     NFP_QC_STS_LO_EMPTY_msk                          (0x1)
#define     NFP_QC_STS_LO_EMPTY_bit                          (26)
#define   NFP_QC_STS_LO_EVENT_DATA(x)                        (((x) & 0x3f) << 20)
#define   NFP_QC_STS_LO_EVENT_DATA_of(x)                     (((x) >> 20) & 0x3f)
#define     NFP_QC_STS_LO_EVENT_DATA_bf                      0, 25, 20
#define     NFP_QC_STS_LO_EVENT_DATA_msk                     (0x3f)
#define     NFP_QC_STS_LO_EVENT_DATA_shf                     (20)
#define   NFP_QC_STS_LO_EVENT_TYPE(x)                        (((x) & 3) << 18)
#define   NFP_QC_STS_LO_EVENT_TYPE_of(x)                     (((x) >> 18) & 3)
#define     NFP_QC_STS_LO_EVENT_TYPE_NEVER                   (0)
#define     NFP_QC_STS_LO_EVENT_TYPE_NOT_EMPTY               (1)
#define     NFP_QC_STS_LO_EVENT_TYPE_LO_WATERMARK            (2)
#define     NFP_QC_STS_LO_EVENT_TYPE_HI_WATERMARK            (3)
#define     NFP_QC_STS_LO_EVENT_TYPE_bf                      0, 19, 18
#define     NFP_QC_STS_LO_EVENT_TYPE_msk                     (0x3)
#define     NFP_QC_STS_LO_EVENT_TYPE_shf                     (18)
#define   NFP_QC_STS_LO_READPTR(x)                           (((x) & 0x3ffff) << 0)
#define   NFP_QC_STS_LO_READPTR_of(x)                        (((x) >> 0) & 0x3ffff)
#define     NFP_QC_STS_LO_READPTR_bf                         0, 17, 0
#define     NFP_QC_STS_LO_READPTR_msk                        (0x3ffff)
#define     NFP_QC_STS_LO_READPTR_shf                        (0)


/*
 * Register: QueueConfigStatusHigh
 *   [30]      Overflowed
 *   [29]      Underflowed
 *   [28]      WMReached
 *   [27]      Full
 *   [26]      Empty
 *   [24:22]   Watermark
 *   [21:18]   Size
 *   [17:0]    WritePtr
 *
 * Name(s):
 * <base>.ConfigStatusHigh
 */
#define NFP_QC_STS_HI                                      0x000c
#define   NFP_QC_STS_HI_OVERFLOWED                           (1 << 30)
#define     NFP_QC_STS_HI_OVERFLOWED_bf                      0, 30, 30
#define     NFP_QC_STS_HI_OVERFLOWED_msk                     (0x1)
#define     NFP_QC_STS_HI_OVERFLOWED_bit                     (30)
#define   NFP_QC_STS_HI_UNDERFLOWED                          (1 << 29)
#define     NFP_QC_STS_HI_UNDERFLOWED_bf                     0, 29, 29
#define     NFP_QC_STS_HI_UNDERFLOWED_msk                    (0x1)
#define     NFP_QC_STS_HI_UNDERFLOWED_bit                    (29)
#define   NFP_QC_STS_HI_WMREACHED                            (1 << 28)
#define     NFP_QC_STS_HI_WMREACHED_bf                       0, 28, 28
#define     NFP_QC_STS_HI_WMREACHED_msk                      (0x1)
#define     NFP_QC_STS_HI_WMREACHED_bit                      (28)
#define   NFP_QC_STS_HI_FULL                                 (1 << 27)
#define     NFP_QC_STS_HI_FULL_bf                            0, 27, 27
#define     NFP_QC_STS_HI_FULL_msk                           (0x1)
#define     NFP_QC_STS_HI_FULL_bit                           (27)
#define   NFP_QC_STS_HI_EMPTY                                (1 << 26)
#define     NFP_QC_STS_HI_EMPTY_bf                           0, 26, 26
#define     NFP_QC_STS_HI_EMPTY_msk                          (0x1)
#define     NFP_QC_STS_HI_EMPTY_bit                          (26)
#define   NFP_QC_STS_HI_WATERMARK(x)                         (((x) & 7) << 22)
#define   NFP_QC_STS_HI_WATERMARK_of(x)                      (((x) >> 22) & 7)
#define     NFP_QC_STS_HI_WATERMARK_4                        (0)
#define     NFP_QC_STS_HI_WATERMARK_8                        (1)
#define     NFP_QC_STS_HI_WATERMARK_16                       (2)
#define     NFP_QC_STS_HI_WATERMARK_32                       (3)
#define     NFP_QC_STS_HI_WATERMARK_64                       (4)
#define     NFP_QC_STS_HI_WATERMARK_128                      (5)
#define     NFP_QC_STS_HI_WATERMARK_256                      (6)
#define     NFP_QC_STS_HI_WATERMARK_512                      (7)
#define     NFP_QC_STS_HI_WATERMARK_bf                       0, 24, 22
#define     NFP_QC_STS_HI_WATERMARK_msk                      (0x7)
#define     NFP_QC_STS_HI_WATERMARK_shf                      (22)
#define   NFP_QC_STS_HI_SIZE(x)                              (((x) & 0xf) << 18)
#define   NFP_QC_STS_HI_SIZE_of(x)                           (((x) >> 18) & 0xf)
#define     NFP_QC_STS_HI_SIZE_256                           (0)
#define     NFP_QC_STS_HI_SIZE_512                           (1)
#define     NFP_QC_STS_HI_SIZE_1K                            (2)
#define     NFP_QC_STS_HI_SIZE_2K                            (3)
#define     NFP_QC_STS_HI_SIZE_4K                            (4)
#define     NFP_QC_STS_HI_SIZE_8K                            (5)
#define     NFP_QC_STS_HI_SIZE_16K                           (6)
#define     NFP_QC_STS_HI_SIZE_32K                           (7)
#define     NFP_QC_STS_HI_SIZE_64K                           (8)
#define     NFP_QC_STS_HI_SIZE_128K                          (9)
#define     NFP_QC_STS_HI_SIZE_256K                          (0xa)
#define     NFP_QC_STS_HI_SIZE_bf                            0, 21, 18
#define     NFP_QC_STS_HI_SIZE_msk                           (0xf)
#define     NFP_QC_STS_HI_SIZE_shf                           (18)
#define   NFP_QC_STS_HI_WRITEPTR(x)                          (((x) & 0x3ffff) << 0)
#define   NFP_QC_STS_HI_WRITEPTR_of(x)                       (((x) >> 0) & 0x3ffff)
#define     NFP_QC_STS_HI_WRITEPTR_bf                        0, 17, 0
#define     NFP_QC_STS_HI_WRITEPTR_msk                       (0x3ffff)
#define     NFP_QC_STS_HI_WRITEPTR_shf                       (0)



/*
 * QC register structures
 */
#if defined(__NFP_LANG_MICROC)

enum pcie_qc_event {
    PCIE_QC_EVENT_NO_EVENT = 0,     /**  Never generate events */
    PCIE_QC_EVENT_NOT_EMPTY = 1,    /**  Event on not empty */
    PCIE_QC_EVENT_GE_MARK = 2,      /**  Event on >= watermark */
    PCIE_QC_EVENT_LT_MARK = 3       /**  Event on < watermark */
};

enum pcie_qc_watermark {
    PCIE_QC_WM_4 = 0,
    PCIE_QC_WM_8 = 1,
    PCIE_QC_WM_16 = 2,
    PCIE_QC_WM_32 = 3,
    PCIE_QC_WM_64 = 4,
    PCIE_QC_WM_128 = 5,
    PCIE_QC_WM_256 = 6,
    PCIE_QC_WM_512 = 7,
};

enum pcie_qc_q_size {
    PCIE_QC_SZ_256 = 0,
    PCIE_QC_SZ_512 = 1,
    PCIE_QC_SZ_1k = 2,
    PCIE_QC_SZ_2k = 3,
    PCIE_QC_SZ_4k = 4,
    PCIE_QC_SZ_8k = 5,
    PCIE_QC_SZ_16k = 6,
    PCIE_QC_SZ_32k = 7,
    PCIE_QC_SZ_64k = 8,
    PCIE_QC_SZ_128k = 9,
    PCIE_QC_SZ_256k = 10,
};

struct nfp_qc_add_rptr {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_18:14;
            unsigned int val:18;
#           else
            unsigned int val:18;
            unsigned int __reserved_18:14;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_qc_add_wptr {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_18:14;
            unsigned int val:18;
#           else
            unsigned int val:18;
            unsigned int __reserved_18:14;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_qc_sts_lo {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int rptr_enable:1;
            unsigned int overflowed:1;
            unsigned int underflowed:1;
            unsigned int wmreached:1;
            unsigned int full:1;
            unsigned int empty:1;
            unsigned int event_data:6;
            unsigned int event_type:2;
            unsigned int readptr:18;
#           else
            unsigned int readptr:18;
            unsigned int event_type:2;
            unsigned int event_data:6;
            unsigned int empty:1;
            unsigned int full:1;
            unsigned int wmreached:1;
            unsigned int underflowed:1;
            unsigned int overflowed:1;
            unsigned int rptr_enable:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_qc_sts_hi {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_31:1;
            unsigned int overflowed:1;
            unsigned int underflowed:1;
            unsigned int wmreached:1;
            unsigned int full:1;
            unsigned int empty:1;
            unsigned int __reserved_25:1;
            unsigned int watermark:3;
            unsigned int size:4;
            unsigned int writeptr:18;
#           else
            unsigned int writeptr:18;
            unsigned int size:4;
            unsigned int watermark:3;
            unsigned int __reserved_25:1;
            unsigned int empty:1;
            unsigned int full:1;
            unsigned int wmreached:1;
            unsigned int underflowed:1;
            unsigned int overflowed:1;
            unsigned int __reserved_31:1;
#           endif
        };
        unsigned int __raw;
    };
};



#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_QC_H_ */
