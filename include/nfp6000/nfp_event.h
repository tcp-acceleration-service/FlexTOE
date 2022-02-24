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
 * @file          nfp6000/nfp_event.h
 * @brief         NFP6000 EventManager CSR definitions
 *
 * This files is auto-generated. Do not edit!
 */

#ifndef _NFP6000__NFP_EVENT_H_
#define _NFP6000__NFP_EVENT_H_


/*
 * Event bus provider numbers
 */
#define NFP_EVENT_PROVIDER_INDEX(_ind)          ((_ind) & 3)
#define NFP_EVENT_PROVIDER_INDEX_of(_pvdr)      ((_pvdr) & 3)
#define NFP_EVENT_PROVIDER_INDEX_bf             0, 1, 0
#define   NFP_EVENT_PROVIDER_INDEX_CLS          0
#define   NFP_EVENT_PROVIDER_INDEX_CTM          1
#define   NFP_EVENT_PROVIDER_INDEX_ARM          2
#define   NFP_EVENT_PROVIDER_INDEX_PCIE         2
#define   NFP_EVENT_PROVIDER_INDEX_CRYPTO       2
#define   NFP_EVENT_PROVIDER_INDEX_ILA          2
#define   NFP_EVENT_PROVIDER_INDEX_ARM_PER      3
#define   NFP_EVENT_PROVIDER_INDEX_NBI          0
#define   NFP_EVENT_PROVIDER_INDEX_MAC          1
#define   NFP_EVENT_PROVIDER_INDEX_EMU          0
#define   NFP_EVENT_PROVIDER_INDEX_IMU          0
#define NFP_EVENT_PROVIDER_ISL(_isl)            (((_isl) & 0x3f) << 2)
#define NFP_EVENT_PROVIDER_ISL_of(_pvdr)        (((_pvdr) >> 2) & 0x3f)
#define NFP_EVENT_PROVIDER_ISL_bf               0, 7, 2
#define NFP_EVENT_PROVIDER_NUM(_isl, _pvdr)     (NFP_EVENT_PROVIDER_ISL(_isl) | \
                                                 NFP_EVENT_PROVIDER_INDEX(_pvdr))

/*
 * Event data as seen on the event bus
 */
#define NFP_EVENT_TYPE(_type)                   ((_type) & 0xf)
#define NFP_EVENT_TYPE_of(_ev)                  ((_ev) & 0xf)
#define NFP_EVENT_TYPE_bf                       0, 3, 0
#define   NFP_EVENT_TYPE_FIFO_NOT_EMPTY         0
#define   NFP_EVENT_TYPE_FIFO_NOT_FULL          1
#define   NFP_EVENT_TYPE_FIFO_ABOVE_WM          2
#define   NFP_EVENT_TYPE_STATS_FULL_WRAP        2
#define   NFP_EVENT_TYPE_FIFO_BELOW_WM          3
#define   NFP_EVENT_TYPE_STATS_HALFWAY          3
#define   NFP_EVENT_TYPE_STATUS_CHANGED         4
#define   NFP_EVENT_TYPE_VALUE_UPDATED          5
#define   NFP_EVENT_TYPE_STATUS_COMPL           6
#define   NFP_EVENT_TYPE_FIFO_UNDERFLOW         8
#define   NFP_EVENT_TYPE_FIFO_OVERFLOW          9
#define   NFP_EVENT_TYPE_ECC_SINGLE_CORRECTION  10
#define   NFP_EVENT_TYPE_ECC_MULTI_ERROR        11
#define   NFP_EVENT_TYPE_ECC_SINGLE_ERROR       12
#define   NFP_EVENT_TYPE_STATS_SYSOP_COMPL      13
#define   NFP_EVENT_TYPE_RESERVED               14
#define   NFP_EVENT_TYPE_GLOBAL_FILTERS_OVFLOW  15
#define NFP_EVENT_SOURCE(_src)                  (((_src) & 0xfff) << 4)
#define NFP_EVENT_SOURCE_of(_ev)                (((_ev) >> 4) & 0xfff)
#define NFP_EVENT_SOURCE_bf                     0, 15, 4
#define NFP_EVENT_PROVIDER(_pvdr)                (((_pvdr) & 0xff) << 16)
#define NFP_EVENT_PROVIDER_of(_ev)              (((_ev) >> 16) & 0xff)
#define NFP_EVENT_PROVIDER_bf                   0, 23, 16
#define NFP_EVENT_MATCH(_pvdr, _src, _type)     (NFP_EVENT_PROVIDER(_pvdr) | \
                                                NFP_EVENT_SOURCE(_src) | \
                                                NFP_EVENT_TYPE(_type))

/*
 * EventManager CSRs
 */
/* EventFilterMap: <base>.Filter0... */
#define NFP_EM_FILTER(_f)                                  (0x0000 + ((_f) * 0x8))

/*
 * Register: EventCombinedStatus
 *
 * Name(s):
 * <base>.CombinedStatus
 */
#define NFP_EM_ALL_STATUS                                  0x0100


/*
 * Register: EventCombinedPendingStatus
 *
 * Name(s):
 * <base>.CombinedPendingStatus
 */
#define NFP_EM_ALL_PENDING                                 0x0101


/*
 * Register: EventConfig
 *
 * Name(s):
 * <base>.Config
 */
#define NFP_EM_CONFIG                                      0x0102



#define NFP_EM_FILTER_STATUS                               0x0000
#define   NFP_EM_FILTER_COUNT32_CNT32(x)                     (((x) & 0xffffffff) << 0)
#define   NFP_EM_FILTER_COUNT32_CNT32_of(x)                  (((x) >> 0) & 0xffffffff)
#define     NFP_EM_FILTER_COUNT32_CNT32_bf                   0, 31, 0
#define     NFP_EM_FILTER_COUNT32_CNT32_msk                  (0xffffffff)
#define     NFP_EM_FILTER_COUNT32_CNT32_shf                  (0)

#define   NFP_EM_FILTER_COUNT16_TMOUT(x)                     (((x) & 7) << 29)
#define   NFP_EM_FILTER_COUNT16_TMOUT_of(x)                  (((x) >> 29) & 7)
#define     NFP_EM_FILTER_COUNT16_TMOUT_bf                   0, 31, 29
#define     NFP_EM_FILTER_COUNT16_TMOUT_msk                  (0x7)
#define     NFP_EM_FILTER_COUNT16_TMOUT_shf                  (29)
#define   NFP_EM_FILTER_COUNT16_UPCNT(x)                     (((x) & 0x3f) << 23)
#define   NFP_EM_FILTER_COUNT16_UPCNT_of(x)                  (((x) >> 23) & 0x3f)
#define     NFP_EM_FILTER_COUNT16_UPCNT_bf                   0, 28, 23
#define     NFP_EM_FILTER_COUNT16_UPCNT_msk                  (0x3f)
#define     NFP_EM_FILTER_COUNT16_UPCNT_shf                  (23)
#define   NFP_EM_FILTER_COUNT16_OVERRIDE(x)                  (((x) & 0x3f) << 16)
#define   NFP_EM_FILTER_COUNT16_OVERRIDE_of(x)               (((x) >> 16) & 0x3f)
#define     NFP_EM_FILTER_COUNT16_OVERRIDE_bf                0, 21, 16
#define     NFP_EM_FILTER_COUNT16_OVERRIDE_msk               (0x3f)
#define     NFP_EM_FILTER_COUNT16_OVERRIDE_shf               (16)
#define   NFP_EM_FILTER_COUNT16_CNT16(x)                     (((x) & 0xffff) << 0)
#define   NFP_EM_FILTER_COUNT16_CNT16_of(x)                  (((x) >> 0) & 0xffff)
#define     NFP_EM_FILTER_COUNT16_CNT16_bf                   0, 15, 0
#define     NFP_EM_FILTER_COUNT16_CNT16_msk                  (0xffff)
#define     NFP_EM_FILTER_COUNT16_CNT16_shf                  (0)


#define   NFP_EM_FILTER_BITMASK16_TMOUT(x)                   (((x) & 7) << 29)
#define   NFP_EM_FILTER_BITMASK16_TMOUT_of(x)                (((x) >> 29) & 7)
#define     NFP_EM_FILTER_BITMASK16_TMOUT_bf                 0, 31, 29
#define     NFP_EM_FILTER_BITMASK16_TMOUT_msk                (0x7)
#define     NFP_EM_FILTER_BITMASK16_TMOUT_shf                (29)
#define   NFP_EM_FILTER_BITMASK16_UPCNT(x)                   (((x) & 0x3f) << 23)
#define   NFP_EM_FILTER_BITMASK16_UPCNT_of(x)                (((x) >> 23) & 0x3f)
#define     NFP_EM_FILTER_BITMASK16_UPCNT_bf                 0, 28, 23
#define     NFP_EM_FILTER_BITMASK16_UPCNT_msk                (0x3f)
#define     NFP_EM_FILTER_BITMASK16_UPCNT_shf                (23)
#define   NFP_EM_FILTER_BITMASK16_OVERRIDE(x)                (((x) & 7) << 20)
#define   NFP_EM_FILTER_BITMASK16_OVERRIDE_of(x)             (((x) >> 20) & 7)
#define     NFP_EM_FILTER_BITMASK16_OVERRIDE_bf              0, 22, 20
#define     NFP_EM_FILTER_BITMASK16_OVERRIDE_msk             (0x7)
#define     NFP_EM_FILTER_BITMASK16_OVERRIDE_shf             (20)
#define   NFP_EM_FILTER_BITMASK16_CNT4(x)                    (((x) & 0xf) << 16)
#define   NFP_EM_FILTER_BITMASK16_CNT4_of(x)                 (((x) >> 16) & 0xf)
#define     NFP_EM_FILTER_BITMASK16_CNT4_bf                  0, 19, 16
#define     NFP_EM_FILTER_BITMASK16_CNT4_msk                 (0xf)
#define     NFP_EM_FILTER_BITMASK16_CNT4_shf                 (16)
#define   NFP_EM_FILTER_BITMASK16_MASK16(x)                  (((x) & 0xffff) << 0)
#define   NFP_EM_FILTER_BITMASK16_MASK16_of(x)               (((x) >> 0) & 0xffff)
#define     NFP_EM_FILTER_BITMASK16_MASK16_bf                0, 15, 0
#define     NFP_EM_FILTER_BITMASK16_MASK16_msk               (0xffff)
#define     NFP_EM_FILTER_BITMASK16_MASK16_shf               (0)

#define   NFP_EM_FILTER_EVENT_TMOUT(x)                       (((x) & 7) << 29)
#define   NFP_EM_FILTER_EVENT_TMOUT_of(x)                    (((x) >> 29) & 7)
#define     NFP_EM_FILTER_EVENT_TMOUT_bf                     0, 31, 29
#define     NFP_EM_FILTER_EVENT_TMOUT_msk                    (0x7)
#define     NFP_EM_FILTER_EVENT_TMOUT_shf                    (29)
#define   NFP_EM_FILTER_EVENT_UPCNT(x)                       (((x) & 0x3f) << 23)
#define   NFP_EM_FILTER_EVENT_UPCNT_of(x)                    (((x) >> 23) & 0x3f)
#define     NFP_EM_FILTER_EVENT_UPCNT_bf                     0, 28, 23
#define     NFP_EM_FILTER_EVENT_UPCNT_msk                    (0x3f)
#define     NFP_EM_FILTER_EVENT_UPCNT_shf                    (23)
#define   NFP_EM_FILTER_EVENT_CNT2(x)                        (((x) & 3) << 20)
#define   NFP_EM_FILTER_EVENT_CNT2_of(x)                     (((x) >> 20) & 3)
#define     NFP_EM_FILTER_EVENT_CNT2_bf                      0, 21, 20
#define     NFP_EM_FILTER_EVENT_CNT2_msk                     (0x3)
#define     NFP_EM_FILTER_EVENT_CNT2_shf                     (20)
#define   NFP_EM_FILTER_EVENT_EVENT(x)                       (((x) & 0xfffff) << 0)
#define   NFP_EM_FILTER_EVENT_EVENT_of(x)                    (((x) >> 0) & 0xfffff)
#define     NFP_EM_FILTER_EVENT_EVENT_bf                     0, 19, 0
#define     NFP_EM_FILTER_EVENT_EVENT_msk                    (0xfffff)
#define     NFP_EM_FILTER_EVENT_EVENT_shf                    (0)


/*
 * Register: EventFilterFlag
 *   [1]       PendingStatus
 *   [0]       Status
 *
 * Name(s):
 * <base>.FilterFlag
 */
#define NFP_EM_FILTER_FLAGS                                0x0001
#define   NFP_EM_FILTER_FLAGS_PENDING_STATUS                 (1 << 1)
#define     NFP_EM_FILTER_FLAGS_PENDING_STATUS_bf            0, 1, 1
#define     NFP_EM_FILTER_FLAGS_PENDING_STATUS_msk           (0x1)
#define     NFP_EM_FILTER_FLAGS_PENDING_STATUS_bit           (1)
#define   NFP_EM_FILTER_FLAGS_STATUS                         (1 << 0)
#define     NFP_EM_FILTER_FLAGS_STATUS_bf                    0, 0, 0
#define     NFP_EM_FILTER_FLAGS_STATUS_msk                   (0x1)
#define     NFP_EM_FILTER_FLAGS_STATUS_bit                   (0)


/*
 * Register: EventFilterMask
 *   [26:24]   FilterType
 *   [23:0]    EventMask
 *
 * Name(s):
 * <base>.FilterMask
 */
#define NFP_EM_FILTER_MASK                                 0x0002
#define   NFP_EM_FILTER_MASK_TYPE(x)                         (((x) & 7) << 24)
#define   NFP_EM_FILTER_MASK_TYPE_of(x)                      (((x) >> 24) & 7)
#define     NFP_EM_FILTER_MASK_TYPE_COUNT32                  (0)
#define     NFP_EM_FILTER_MASK_TYPE_COUNT16                  (1)
#define     NFP_EM_FILTER_MASK_TYPE_MASK32                   (2)
#define     NFP_EM_FILTER_MASK_TYPE_MASK16                   (3)
#define     NFP_EM_FILTER_MASK_TYPE_FIRSTEV                  (4)
#define     NFP_EM_FILTER_MASK_TYPE_LASTEV                   (5)
#define     NFP_EM_FILTER_MASK_TYPE_FIRSTEVUT                (6)
#define     NFP_EM_FILTER_MASK_TYPE_LASTEVUT                 (7)
#define     NFP_EM_FILTER_MASK_TYPE_bf                       0, 26, 24
#define     NFP_EM_FILTER_MASK_TYPE_msk                      (0x7)
#define     NFP_EM_FILTER_MASK_TYPE_shf                      (24)
#define   NFP_EM_FILTER_MASK_EVENT(x)                        (((x) & 0xffffff) << 0)
#define   NFP_EM_FILTER_MASK_EVENT_of(x)                     (((x) >> 0) & 0xffffff)
#define     NFP_EM_FILTER_MASK_EVENT_bf                      0, 23, 0
#define     NFP_EM_FILTER_MASK_EVENT_msk                     (0xffffff)
#define     NFP_EM_FILTER_MASK_EVENT_shf                     (0)


/*
 * Register: EventFilterMatch
 *   [23:0]    EventMatch
 *
 * Name(s):
 * <base>.FilterMatch
 */
#define NFP_EM_FILTER_MATCH                                0x0003
#define   NFP_EM_FILTER_MATCH_EVENT(x)                       (((x) & 0xffffff) << 0)
#define   NFP_EM_FILTER_MATCH_EVENT_of(x)                    (((x) >> 0) & 0xffffff)
#define     NFP_EM_FILTER_MATCH_EVENT_bf                     0, 23, 0
#define     NFP_EM_FILTER_MATCH_EVENT_msk                    (0xffffff)
#define     NFP_EM_FILTER_MATCH_EVENT_shf                    (0)


/*
 * Register: EventFilterAck
 *   [31:0]    EvtStatus
 *
 * Name(s):
 * <base>.FilterAcknowledge
 */
#define NFP_EM_FILTER_ACK                                  0x0004
#define   NFP_EM_FILTER_ACK_EVT_STATUS(x)                    (((x) & 0xffffffff) << 0)
#define   NFP_EM_FILTER_ACK_EVT_STATUS_of(x)                 (((x) >> 0) & 0xffffffff)
#define     NFP_EM_FILTER_ACK_EVT_STATUS_bf                  0, 31, 0
#define     NFP_EM_FILTER_ACK_EVT_STATUS_msk                 (0xffffffff)
#define     NFP_EM_FILTER_ACK_EVT_STATUS_shf                 (0)



/*
 * EventManager register structures
 */
#if defined(__NFP_LANG_MICROC)

struct nfp_event_match {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int reserved:8;
            unsigned int provider:8;
            unsigned int source:12;
            unsigned int type:4;
#           else
            unsigned int type:4;
            unsigned int source:12;
            unsigned int provider:8;
            unsigned int reserved:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_em_all_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int status:32;
#           else
            unsigned int status:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_em_all_pending {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int pending_status:32;
#           else
            unsigned int pending_status:32;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_em_config {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int testen:1;
            unsigned int __reserved_28:3;
            unsigned int clkdivcnt:20;
            unsigned int __reserved_4:4;
            unsigned int clkdiv:4;
#           else
            unsigned int clkdiv:4;
            unsigned int __reserved_4:4;
            unsigned int clkdivcnt:20;
            unsigned int __reserved_28:3;
            unsigned int testen:1;
#           endif
        };
        unsigned int __raw;
    };
};



struct nfp_em_filter_status {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int cnt32:32;
#           else
            unsigned int cnt32:32;
#           endif
        } count32;
        struct {
#           ifdef BIGENDIAN
            unsigned int tmout:3;
            unsigned int upcnt:6;
            unsigned int __reserved_22:1;
            unsigned int override:6;
            unsigned int cnt16:16;
#           else
            unsigned int cnt16:16;
            unsigned int override:6;
            unsigned int __reserved_22:1;
            unsigned int upcnt:6;
            unsigned int tmout:3;
#           endif
        } count16;
        struct {
#           ifdef BIGENDIAN
            unsigned int mask32:32;
#           else
            unsigned int mask32:32;
#           endif
        } bitmask32;
        struct {
#           ifdef BIGENDIAN
            unsigned int tmout:3;
            unsigned int upcnt:6;
            unsigned int override:3;
            unsigned int cnt4:4;
            unsigned int mask16:16;
#           else
            unsigned int mask16:16;
            unsigned int cnt4:4;
            unsigned int override:3;
            unsigned int upcnt:6;
            unsigned int tmout:3;
#           endif
        } bitmask16;
        struct {
#           ifdef BIGENDIAN
            unsigned int tmout:3;
            unsigned int upcnt:6;
            unsigned int __reserved_22:1;
            unsigned int cnt2:2;
            unsigned int event:20;
#           else
            unsigned int event:20;
            unsigned int cnt2:2;
            unsigned int __reserved_22:1;
            unsigned int upcnt:6;
            unsigned int tmout:3;
#           endif
        } event;
        unsigned int __raw;
    };
};

struct nfp_em_filter_flags {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_2:30;
            unsigned int pending_status:1;
            unsigned int status:1;
#           else
            unsigned int status:1;
            unsigned int pending_status:1;
            unsigned int __reserved_2:30;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_em_filter_mask {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_27:5;
            unsigned int type:3;
            unsigned int event:24;
#           else
            unsigned int event:24;
            unsigned int type:3;
            unsigned int __reserved_27:5;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_em_filter_match {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_24:8;
            unsigned int event:24;
#           else
            unsigned int event:24;
            unsigned int __reserved_24:8;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_em_filter_ack {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int evt_status:32;
#           else
            unsigned int evt_status:32;
#           endif
        };
        unsigned int __raw;
    };
};



#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_EVENT_H_ */
