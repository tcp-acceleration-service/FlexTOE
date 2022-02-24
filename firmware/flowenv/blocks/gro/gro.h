/*
 * Copyright (C) 2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          me/blocks/gro/gro.h
 */

#ifndef __GRO_H
#define __GRO_H

#ifndef __NFP_LANG_ASM
#include <stdint.h>
#endif /* __NFP_LANG_ASM */


#define GRO_GLOBAL_CFG_NUM_BLOCKS_wrd           0
#define GRO_GLOBAL_CFG_CTX_PER_BLOCK_wrd        1
#define GRO_GLOBAL_CFG_SIZE_LW                  2
#define GRO_GLOBAL_CFG_SIZE                     (GRO_GLOBAL_CFG_SIZE_LW << 2)

#ifndef __NFP_LANG_ASM
struct gro_global_config {
    uint32_t num_blocks;
    uint32_t ctx_per_block;
};
#endif /* __NFP_LANG_ASM */

#ifndef __NFP_LANG_ASM
struct gro_addr_enc {
    union {
        struct {
            unsigned int addr_hi:8;
            unsigned int addr_lo:24;
        };
        uint32_t __raw;
    };
};

#endif /* __NFP_LANG_ASM */



#ifndef __NFP_LANG_ASM

struct gro_meta_drop {
    union {
        struct {
            unsigned int unused:25;
            unsigned int dest:4;                /* unused */
            unsigned int type:3;
            unsigned int ctm_addr_hi:8;
            unsigned int unused2:15;
            unsigned int ctm_pkt_num:9;
            unsigned int mu_ring_addr_hi:8;
            unsigned int mu_ring_num:24;
            uint32_t mu_handle;
        };
        uint32_t __raw[4];
    };
};


struct gro_meta_nbi {
    union {
        struct {
            unsigned int unused:25;
            unsigned int dest:4;
            unsigned int type:3;
            uint32_t addr_hi;
            uint32_t addr_lo;
            uint32_t prev_alu;
        };
        uint32_t __raw[4];
    };
};


/* Max supported starting offset for an NFD packet sent via GRO */
#define GRO_NFD_MAX_OFFSET 255

#ifndef NFD_USE_MOCKUP

struct gro_meta_nfd3_w0 {
    union {
        struct {
            unsigned int isl:6;         /* CTM island, zero for MU only pkts */
            unsigned int ctm_only:1;    /* Packet is in CTM only*/
            unsigned int pktnum:9;      /* CTM packet number */
            unsigned int split:2;       /* CTM buffer size */
            unsigned int off:7;         /* Offset where data starts in NFP */
            unsigned int pcie:4;        /* Set for end of packet */
            unsigned int type:3;        /* GRO_DTYPE_NFD */
        };
        unsigned int __raw;
    };
};


#else /* NFD_USE_MOCKUP */

struct gro_meta_nfd3_w0 {
    union {
        struct {
            unsigned int vlan:16;
            unsigned int flags:3;
            unsigned int qid:6;
            unsigned int pcie:4;
            unsigned int type:3;
        };
        unsigned int __raw;
    };
};

#endif /* NFD_USE_MOCKUP */

struct gro_meta_nfd3 {
    struct gro_meta_nfd3_w0 w0;
    uint32_t w1;
    uint32_t w2;
    uint32_t w3;
};


struct gro_meta_memq_w0 {
    unsigned int q_hi:8;
    unsigned int unused:7;
    unsigned int qnum:10;
    unsigned int dest:4;
    unsigned int type:3;
};

struct gro_meta_memq {
    struct gro_meta_memq_w0 w0;
    uint32_t    qdata[3];
};


union gro_meta {
    struct gro_meta_drop drop;
    struct gro_meta_nbi nbi;
    struct gro_meta_nfd3 nfd;
    struct gro_meta_memq memq;
};


#endif /* __NFP_LANG_ASM */

#define GRO_META_SIZE_LW                4
#define GRO_META_SIZE                   (GRO_META_SIZE_LW << 2)

#define GRO_DTYPE_DROP_SEQ              0
#define GRO_DTYPE_IFACE                 1
#define GRO_DTYPE_NFD                   2
#define GRO_DTYPE_WORKQ                 3
#define GRO_DTYPE_RING                  4
#define GRO_DTYPE_DROP_CTM_BUF          5
#define GRO_DTYPE_DROP_MU_BUF           6

#define GRO_META_TYPE_wrd               0
#define GRO_META_TYPE_shf               0
#define GRO_META_TYPE_msk               0x7
#define GRO_META_TYPE_fld               0, 2, 0

#define GRO_META_DEST_wrd               0
#define GRO_META_DEST_shf               3
#define GRO_META_DEST_msk               0xF
#define GRO_META_DEST_fld               0, 7, 3

#define GRO_META_W0_META_START_BIT      7

#define GRO_META_DROP_PNUM_wrd          1
#define GRO_META_DROP_PNUM_shf          0
#define GRO_META_DROP_PNUM_msk          0x1FF
#define GRO_META_DROP_CTMHI_wrd         1
#define GRO_META_DROP_CTMHI_shf         24
#define GRO_META_DROP_CTMHI_msk         0xff
#define GRO_META_DROP_RINGNUM_wrd       2
#define GRO_META_DROP_RINGNUM_shf       0
#define GRO_META_DROP_RINGNUM_msk       0x3FF
#define GRO_META_DROP_RINGHI_wrd        2
#define GRO_META_DROP_RINGHI_shf        24
#define GRO_META_DROP_RINGHI_msk        0xFF
#define GRO_META_DROP_BUFH_wrd          3
#define GRO_META_DROP_BUFH_shf          0
#define GRO_META_DROP_BUFH_msk          0xFFFFFFFFFF

#define GRO_META_NBI_ADDRHI_wrd         1
#define GRO_META_NBI_ADDRLO_wrd         2
#define GRO_META_NBI_PALU_wrd           3

#define GRO_META_RINGLO_wrd             0
#define GRO_META_RINGLO_shf             7
#define GRO_META_RINGLO_msk             0x3FF
#define GRO_META_RINGLO_fld             0, 16, 7

#define GRO_META_RINGHI_wrd             0
#define GRO_META_RINGHI_shf             24
#define GRO_META_RINGHI_msk             0xFF
#define GRO_META_RINGHI_fld             0, 31, 24

#define GRO_META_WORKQ_RINGLO_wrd       GRO_META_RINGLO_wrd
#define GRO_META_WORKQ_RINGLO_shf       GRO_META_RINGLO_shf
#define GRO_META_WORKQ_RINGLO_msk       GRO_META_RINGLO_msk
#define GRO_META_WORKQ_RINGHI_wrd       GRO_META_RINGHI_wrd
#define GRO_META_WORKQ_RINGHI_shf       GRO_META_RINGHI_shf
#define GRO_META_WORKQ_RINGHI_msk       GRO_META_RINGHI_msk

#define GRO_META_MEM_RING_RINGLO_wrd    GRO_META_RINGLO_wrd
#define GRO_META_MEM_RING_RINGLO_shf    GRO_META_RINGLO_shf
#define GRO_META_MEM_RING_RINGLO_msk    GRO_META_RINGLO_msk
#define GRO_META_MEM_RING_RINGHI_wrd    GRO_META_RINGHI_wrd
#define GRO_META_MEM_RING_RINGHI_shf    GRO_META_RINGHI_shf
#define GRO_META_MEM_RING_RINGHI_msk    GRO_META_RINGHI_msk


#ifndef __NFP_LANG_ASM
union gro_dstcfg {
    uint32_t nbi_csr0;
    uint32_t iref;
    struct {
        uint32_t credits;
        struct gro_addr_enc ring;
    };
};
#endif /* __NFP_LANG_ASM */

#define GRO_NUM_IFACE_DEST              2
#define GRO_NUM_NFD_DEST                4
#define GRO_NUM_WORKQ_DEST              3
#define GRO_NUM_MEM_RING_DEST           3

#define GRO_DEST_NFD_BASE               0
#define GRO_DEST_IFACE_BASE             (GRO_DEST_NFD_BASE + GRO_NUM_NFD_DEST)
#define GRO_DEST_WORKQ_BASE             (GRO_DEST_IFACE_BASE + GRO_NUM_IFACE_DEST)
#define GRO_DEST_MEM_RING_BASE          (GRO_DEST_WORKQ_BASE + GRO_NUM_WORKQ_DEST)

#define GRO_DEST_PER_BLOCK              (GRO_NUM_IFACE_DEST +\
                                         GRO_NUM_NFD_DEST +\
                                         GRO_NUM_WORKQ_DEST +\
                                         GRO_NUM_MEM_RING_DEST)

#define GRO_DEST_WORKQ_1WORD            (GRO_DEST_WORKQ_BASE + 0)
#define GRO_DEST_WORKQ_2WORD            (GRO_DEST_WORKQ_BASE + 1)
#define GRO_DEST_WORKQ_3WORD            (GRO_DEST_WORKQ_BASE + 2)

#define GRO_DEST_MEM_RING_1WORD         (GRO_DEST_MEM_RING_BASE + 0)
#define GRO_DEST_MEM_RING_2WORD         (GRO_DEST_MEM_RING_BASE + 1)
#define GRO_DEST_MEM_RING_3WORD         (GRO_DEST_MEM_RING_BASE + 2)

#define GRO_DEST_SIZE_LW                2
#define GRO_DEST_SIZE                   (GRO_DEST_SIZE_LW << 2)

#define GRO_DEST_NBI_CSR0_wrd           0

#define GRO_DEST_MSF_RING_wrd           0

#define GRO_DEST_NFD2_MAX_EP            2
#define GRO_DEST_NFD2_MAX_HOSTID        32
#define GRO_DEST_NFD2_EP_CREDITS_wrd    0
#define GRO_DEST_NFD2_HOST_CREDITS_wrd  1

#define GRO_DEST_NFD3_MAX_PCIE          4
#define GRO_DEST_NFD3_MAX_QID           64
#define GRO_DEST_NFD3_CREDITS_wrd       0
#define GRO_DEST_NFD3_RING_ENC_wrd      1
#define GRO_DEST_NFD3_RINGLO_wrd        1
#define GRO_DEST_NFD3_RINGLO_shf        0
#define GRO_DEST_NFD3_RINGLO_msk        0x3FF
#define GRO_DEST_NFD3_RINGHI_wrd        1
#define GRO_DEST_NFD3_RINGHI_msk        0xFF
#define GRO_DEST_NFD3_RINGHI_shf        24


#define GRO_DEST_MEM_RING_IREF_wrd      0


#define GRO_MAX_REL_PER_MSG             64


#define GRO_REL_CTX_shf                 0
#define GRO_REL_CTX_msk                 0x7
#define GRO_REL_NREL_shf                16
#define GRO_REL_NREL_msk                0x7F
#define GRO_REL_VALID_shf               31
#define GRO_REL_VALID_msk               0x1
#define GRO_REL_VALID_bit               31

#ifndef __NFP_LANG_ASM
struct gro_release {
    union {
        struct {
#if defined(__NFP_LANG_MICROC) || (__BYTE_ORDER == __BIG_ENDIAN)
            unsigned int valid:1; /* XXX overlaps w/ ring put success bit */
            unsigned int pad2:8;
            unsigned int nrel:7;
            unsigned int pad1:13;
            unsigned int ctx:3;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
            unsigned int ctx:3;
            unsigned int pad1:13;
            unsigned int nrel:7;
            unsigned int pad2:8;
            unsigned int valid:1;
#else
    #error "Unknown byte order"
#endif
        };
        unsigned int __raw;
    };
};
#endif /* __NFP_LANG_ASM */



#define GRO_FLD_VAL(FLD, _val)  (((_val) & FLD/**/_msk) << FLD/**/_shf)

#define GRO_CLICTX_RING_LO_wrd  0
#define GRO_CLICTX_RING_LO_shf  0
#define GRO_CLICTX_RING_LO_msk  0x00FFFFFF
#define GRO_CLICTX_RING_HI_wrd  0
#define GRO_CLICTX_RING_HI_shf  24
#define GRO_CLICTX_RING_HI_msk  0xFF
#define GRO_CLICTX_BM_LO_wrd    1
#define GRO_CLICTX_BM_LO_shf    0
#define GRO_CLICTX_BM_LO_msk    0xFFFFFF
#define GRO_CLICTX_BM_HI_wrd    1
#define GRO_CLICTX_BM_HI_shf    24
#define GRO_CLICTX_BM_HI_msk    0xFF
#define GRO_CLICTX_Q_SIZE_wrd   2
#define GRO_CLICTX_Q_SIZE_shf   0
#define GRO_CLICTX_Q_SIZE_msk   0x1F
#define GRO_CLICTX_Q_HI_wrd     2
#define GRO_CLICTX_Q_HI_shf     24
#define GRO_CLICTX_Q_HI_msk     0xFF
#define GRO_CLICTX_Q_LO_wrd     3
#define GRO_CLICTX_Q_LO_shf     0
#define GRO_CLICTX_Q_LO_msk     0xFFFFFFFF
#define GRO_CLICTX_SIZE_LW      4
#define GRO_CLICTX_SIZE         (GRO_CLICTX_SIZE_LW << 2)
#define GRO_CLICTX_shf          4


#ifndef __NFP_LANG_ASM
struct gro_client_ctx {
    unsigned int ring_addr_hi:8;
    unsigned int ring_addr_lo:24;
    unsigned int bm_addr_hi:8;
    unsigned int bm_addr_lo:24;
    unsigned int q_addr_hi:8;
    unsigned int q_size:24;
    unsigned int q_addr_lo;
};
#endif /* __NFP_LANG_ASM */


#define GRO_TICKET_PER_TAPE             64
#define GRO_SEQSKID_VALUE               65
#define GRO_TICKET_msk                  0x3F
#define GRO_TICKET_TAPE_SEQ_shf         6
#define GRO_TICKET_TAPE_SIZE_LW         4
#define GRO_TICKET_TAPE_SIZE            (GRO_TICKET_TAPE_SIZE_LW << 2)
#define GRO_TICKET_TAPE_SIZE_shf        4
#define GRO_TICKET_ERROR                0xFF

#define GRO_TAPE_SEQ_msk                0x7F

#define GRO_NUM_EPOCHS                  4
#define GRO_EPOCH_TICKET_SEQ_mul        256
#define GRO_EPOCH_TICKET_SEQ_shf        8       /* bits 8..9 */
#define GRO_EPOCH_msk                   0x3


#ifndef GRO_CNTR_MEM_UC
#define GRO_CNTR_MEM_UC imem
#endif /* GRO_CNTR_MEM_MICROC */

#ifndef GRO_CNTR_MEM_MICROC
#define GRO_CNTR_MEM_MICROC __declspec(imem)
#endif /* GRO_CNTR_MEM_MICROC */


#endif /* __GRO_H */
