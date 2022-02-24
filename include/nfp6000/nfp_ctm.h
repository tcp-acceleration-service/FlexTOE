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
 * @file          nfp6000/nfp_ctm.h
 * @brief         NFP6000 CTM Misc Engine CSR definitions
 */

#ifndef _NFP6000__NFP_CTM_H_
#define _NFP6000__NFP_CTM_H_



/*
 * Combined XPB BUS offset of the MuMiscReg CSRs
 */
#define NFP_CTM_MISC_XPB_OFF    0x80000

/*
 * CTM CSRs
 */

/*
 * Register: XpbErrConfig
 *   [15:1]    XpbMaxTimeout
 *   [0]       XpbWrIfErr
 *
 * Name(s):
 * <base>.XpbErrConfig
 */
#define NFP_CTM_XPB_ERR_CONFIG                             0x0000
#define   NFP_CTM_XPB_ERR_CONFIG_XPB_MAX_TIMEOUT(x)          (((x) & 0x7fff) << 1)
#define   NFP_CTM_XPB_ERR_CONFIG_XPB_MAX_TIMEOUT_of(x)       (((x) >> 1) & 0x7fff)
#define     NFP_CTM_XPB_ERR_CONFIG_XPB_MAX_TIMEOUT_bf        0, 15, 1
#define     NFP_CTM_XPB_ERR_CONFIG_XPB_MAX_TIMEOUT_msk       (0x7fff)
#define     NFP_CTM_XPB_ERR_CONFIG_XPB_MAX_TIMEOUT_shf       (1)
#define   NFP_CTM_XPB_ERR_CONFIG_XPB_WRITE_IF_ERR            (1 << 0)
#define     NFP_CTM_XPB_ERR_CONFIG_XPB_WRITE_IF_ERR_bf       0, 0, 0
#define     NFP_CTM_XPB_ERR_CONFIG_XPB_WRITE_IF_ERR_msk      (0x1)
#define     NFP_CTM_XPB_ERR_CONFIG_XPB_WRITE_IF_ERR_bit      (0)


/*
 * Register: XpbTOErrStat
 *   [14]      XpbTOErr
 *   [13:8]    XpbClId
 *   [7:0]     XpbDevId
 *
 * Name(s):
 * <base>.XpbTOErrStat
 */
#define NFP_CTM_XPB_TIMEOUT_ERR_STAT                       0x0004
#define   NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_TIMEOUT_ERR       (1 << 14)
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_TIMEOUT_ERR_bf  0, 14, 14
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_TIMEOUT_ERR_msk (0x1)
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_TIMEOUT_ERR_bit (14)
#define   NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_CLUSTER_ID(x)     (((x) & 0x3f) << 8)
#define   NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_CLUSTER_ID_of(x)  (((x) >> 8) & 0x3f)
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_CLUSTER_ID_bf   0, 13, 8
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_CLUSTER_ID_msk  (0x3f)
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_CLUSTER_ID_shf  (8)
#define   NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_DEVICE_ID(x)      (((x) & 0xff) << 0)
#define   NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_DEVICE_ID_of(x)   (((x) >> 0) & 0xff)
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_DEVICE_ID_bf    0, 7, 0
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_DEVICE_ID_msk   (0xff)
#define     NFP_CTM_XPB_TIMEOUT_ERR_STAT_XPB_DEVICE_ID_shf   (0)


/*
 * Register: XpbWrErrStat
 *   [30]      XpbWrErrMulti
 *   [29]      XpbWrErrSingle
 *   [28]      XpbWrErrGlobal
 *   [27:22]   XpbWrErrTgtIsld
 *   [21:20]   XpbWrErrSlaveIdx
 *   [19:14]   XpbWrErrDevID
 *   [13:0]    XpbWrErrRegAddr
 *
 * Name(s):
 * <base>.XpbWrErrStat
 */
#define NFP_CTM_XPB_WR_ERR_STAT                            0x0008
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_MULTI           (1 << 30)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_MULTI_bf      0, 30, 30
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_MULTI_msk     (0x1)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_MULTI_bit     (30)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SINGLE          (1 << 29)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SINGLE_bf     0, 29, 29
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SINGLE_msk    (0x1)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SINGLE_bit    (29)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_GLOBAL          (1 << 28)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_GLOBAL_bf     0, 28, 28
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_GLOBAL_msk    (0x1)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_GLOBAL_bit    (28)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_TGT_ISLD(x)     (((x) & 0x3f) << 22)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_TGT_ISLD_of(x)  (((x) >> 22) & 0x3f)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_TGT_ISLD_bf   0, 27, 22
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_TGT_ISLD_msk  (0x3f)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_TGT_ISLD_shf  (22)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SLAVE_IDX(x)    (((x) & 3) << 20)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SLAVE_IDX_of(x) (((x) >> 20) & 3)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SLAVE_IDX_bf  0, 21, 20
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SLAVE_IDX_msk (0x3)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_SLAVE_IDX_shf (20)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_DEV_ID(x)       (((x) & 0x3f) << 14)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_DEV_ID_of(x)    (((x) >> 14) & 0x3f)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_DEV_ID_bf     0, 19, 14
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_DEV_ID_msk    (0x3f)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_DEV_ID_shf    (14)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_REG_ADDR(x)     (((x) & 0x3fff) << 0)
#define   NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_REG_ADDR_of(x)  (((x) >> 0) & 0x3fff)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_REG_ADDR_bf   0, 13, 0
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_REG_ADDR_msk  (0x3fff)
#define     NFP_CTM_XPB_WR_ERR_STAT_XPB_WR_ERR_REG_ADDR_shf  (0)


/*
 * Register: MUMERingBase
 *   [31:29]   Size
 *   [28]      StatusType
 *   [17:9]    BaseAddress
 *
 * Name(s):
 * <base>.Ring_0_Base...
 */
#define NFP_CTM_RING_BASE(x)                               (0x0100 + ((x) * 0x10))
#define   NFP_CTM_RING_BASE_SIZE(x)                          (((x) & 7) << 29)
#define   NFP_CTM_RING_BASE_SIZE_of(x)                       (((x) >> 29) & 7)
#define     NFP_CTM_RING_BASE_SIZE_128                       (0)
#define     NFP_CTM_RING_BASE_SIZE_256                       (1)
#define     NFP_CTM_RING_BASE_SIZE_512                       (2)
#define     NFP_CTM_RING_BASE_SIZE_1k                        (3)
#define     NFP_CTM_RING_BASE_SIZE_2k                        (4)
#define     NFP_CTM_RING_BASE_SIZE_4k                        (5)
#define     NFP_CTM_RING_BASE_SIZE_8k                        (6)
#define     NFP_CTM_RING_BASE_SIZE_16k                       (7)
#define     NFP_CTM_RING_BASE_SIZE_bf                        0, 31, 29
#define     NFP_CTM_RING_BASE_SIZE_msk                       (0x7)
#define     NFP_CTM_RING_BASE_SIZE_shf                       (29)
#define   NFP_CTM_RING_BASE_STATUS_TYPE                      (1 << 28)
#define     NFP_CTM_RING_BASE_STATUS_TYPE_Empty              (0)
#define     NFP_CTM_RING_BASE_STATUS_TYPE_Full               (0x10000000)
#define     NFP_CTM_RING_BASE_STATUS_TYPE_bf                 0, 28, 28
#define     NFP_CTM_RING_BASE_STATUS_TYPE_msk                (0x1)
#define     NFP_CTM_RING_BASE_STATUS_TYPE_bit                (28)
#define   NFP_CTM_RING_BASE_BASE(x)                          (((x) & 0x1ff) << 9)
#define   NFP_CTM_RING_BASE_BASE_of(x)                       (((x) >> 9) & 0x1ff)
#define     NFP_CTM_RING_BASE_BASE_bf                        0, 17, 9
#define     NFP_CTM_RING_BASE_BASE_msk                       (0x1ff)
#define     NFP_CTM_RING_BASE_BASE_shf                       (9)


/*
 * Register: MUMERingHead
 *   [16:2]    HeadOffset
 *
 * Name(s):
 * <base>.Ring_0_Head...
 */
#define NFP_CTM_RING_HEAD(x)                               (0x0104 + ((x) * 0x10))
#define   NFP_CTM_RING_HEAD_OFF(x)                           (((x) & 0x7fff) << 2)
#define   NFP_CTM_RING_HEAD_OFF_of(x)                        (((x) >> 2) & 0x7fff)
#define     NFP_CTM_RING_HEAD_OFF_bf                         0, 16, 2
#define     NFP_CTM_RING_HEAD_OFF_msk                        (0x7fff)
#define     NFP_CTM_RING_HEAD_OFF_shf                        (2)


/*
 * Register: MUMERingTail
 *   [16:2]    TailOffset
 *
 * Name(s):
 * <base>.Ring_0_Tail...
 */
#define NFP_CTM_RING_TAIL(x)                               (0x0108 + ((x) * 0x10))
#define   NFP_CTM_RING_TAIL_OFF(x)                           (((x) & 0x7fff) << 2)
#define   NFP_CTM_RING_TAIL_OFF_of(x)                        (((x) >> 2) & 0x7fff)
#define     NFP_CTM_RING_TAIL_OFF_bf                         0, 16, 2
#define     NFP_CTM_RING_TAIL_OFF_msk                        (0x7fff)
#define     NFP_CTM_RING_TAIL_OFF_shf                        (2)



/*
 * CTM register structures
 */
#if defined(__NFP_LANG_MICROC)

struct nfp_ctm_xpb_err_config {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_16:16;
            unsigned int xpb_max_timeout:15;
            unsigned int xpb_write_if_err:1;
#           else
            unsigned int xpb_write_if_err:1;
            unsigned int xpb_max_timeout:15;
            unsigned int __reserved_16:16;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_ctm_xpb_timeout_err_stat {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_15:17;
            unsigned int xpb_timeout_err:1;
            unsigned int xpb_cluster_id:6;
            unsigned int xpb_device_id:8;
#           else
            unsigned int xpb_device_id:8;
            unsigned int xpb_cluster_id:6;
            unsigned int xpb_timeout_err:1;
            unsigned int __reserved_15:17;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_ctm_xpb_wr_err_stat {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_31:1;
            unsigned int xpb_wr_err_multi:1;
            unsigned int xpb_wr_err_single:1;
            unsigned int xpb_wr_err_global:1;
            unsigned int xpb_wr_err_tgt_isld:6;
            unsigned int xpb_wr_err_slave_idx:2;
            unsigned int xpb_wr_err_dev_id:6;
            unsigned int xpb_wr_err_reg_addr:14;
#           else
            unsigned int xpb_wr_err_reg_addr:14;
            unsigned int xpb_wr_err_dev_id:6;
            unsigned int xpb_wr_err_slave_idx:2;
            unsigned int xpb_wr_err_tgt_isld:6;
            unsigned int xpb_wr_err_global:1;
            unsigned int xpb_wr_err_single:1;
            unsigned int xpb_wr_err_multi:1;
            unsigned int __reserved_31:1;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_ctm_ring_base {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int size:3;
            unsigned int status_type:1;
            unsigned int __reserved_18:10;
            unsigned int base:9;
            unsigned int __reserved_0:9;
#           else
            unsigned int __reserved_0:9;
            unsigned int base:9;
            unsigned int __reserved_18:10;
            unsigned int status_type:1;
            unsigned int size:3;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_ctm_ring_head {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_17:15;
            unsigned int off:15;
            unsigned int __reserved_0:2;
#           else
            unsigned int __reserved_0:2;
            unsigned int off:15;
            unsigned int __reserved_17:15;
#           endif
        };
        unsigned int __raw;
    };
};

struct nfp_ctm_ring_tail {
    union {
        struct {
#           ifdef BIGENDIAN
            unsigned int __reserved_17:15;
            unsigned int off:15;
            unsigned int __reserved_0:2;
#           else
            unsigned int __reserved_0:2;
            unsigned int off:15;
            unsigned int __reserved_17:15;
#           endif
        };
        unsigned int __raw;
    };
};



#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP6000__NFP_CTM_H_ */
