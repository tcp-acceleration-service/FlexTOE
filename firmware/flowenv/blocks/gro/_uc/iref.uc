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
 * @file          me/blocks/gro/_uc/iref.uc
 */

#ifndef __IREF_UC
#define __IREF_UC

/*
 * Indirect Reference Previous ALU Format:
 *
 * Bit  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * ---\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Wrd +-------------------------------+-+-+-+---------+-+-+-----+---+-+
 *     |            DATA 16            |T|C|N|   LEN   |L|B| OVD |OVM|S|
 *     +-------------------------------+-+-+-+---------+-+-+-----+---+-+
 *
 * S -> override signal master from CSR0
 * OVM -> override data/signal masters and/or island
 * OVD -> override data/signal reference
 * B -> override bytemask from CSR0
 * L -> override length
 * N -> override signal number from CSR0
 * C -> override signal context from CSR0
 * T -> override Target|Action|Token from CSR1
 * DATA16 -> override data depending on other flags
 */

#define IREF_OV_SM_shf          0  /* if 1: CSR0[4;16] -> sig_master[4;0] */
#define IREF_OV_SM_msk          0x1
#define IREF_OV_SM_bit          0
#define IREF_OV_SM_wml          0,0,0

#define IREF_OV_MASTER_shf      1  /* See IREF_OVM_* */
#define IREF_OV_MASTER_msk      0x3
#define IREF_OV_MASTER_wml      0,2,1

#define IREF_OV_DATA_shf        3  /* See IREF_OVD_* */
#define IREF_OV_DATA_msk        0x7
#define IREF_OV_DATA_wml        0,5,3

#define IREF_OV_BM_CSR_shf      6  /* if 1: CSR0[8;0] -> byte_mask[8;0] */
#define IREF_OV_BM_CSR_msk      0x1
#define IREF_OV_BM_CSR_bit      6
#define IREF_OV_BM_CSR_wml      0,6,6

#define IREF_OV_LEN_shf         7  /* if 1: PREV[5;8] -> length[5;0] */
#define IREF_OV_LEN_msk         0x1
#define IREF_OV_LEN_bit         7
#define IREF_OV_LEN_wml         0,7,7

#define IREF_LEN_shf            8  /* see OV_LEN */
#define IREF_LEN_msk            0x1F
#define IREF_LEN_wml            0,12,8

#define IREF_OV_SIG_NUM_shf     13 /* if 1: CSR0[4;9] -> sig_ref[4;0] */
#define IREF_OV_SIG_NUM_msk     0x1
#define IREF_OV_SIG_NUM_bit     13
#define IREF_OV_SIG_NUM_wml     0,13,13

#define IREF_OV_SIG_CTX_shf     14 /* if 1: CSR0[3;13] -> sig_ref[3;4] */
#define IREF_OV_SIG_CTX_msk     0x1
#define IREF_OV_SIG_CTX_bit     14
#define IREF_OV_SIG_CTX_wml     0,14,14

#define IREF_OV_TAT_shf         15 /* if 1: see CSR1 */
#define IREF_OV_TAT_msk         0x1
#define IREF_OV_TAT_bit         15
#define IREF_OV_TAT_wml         0,15,15

#define IREF_DATA16_shf         16
#define IREF_DATA16_msk         0xFFFF
#define IREF_DATA16_wml         0,31,16


/* for IREF_OV_MASTER field */
#define IREF_OVM_NONE           0
#define IREF_OVM_DM_ISLD        1  /* CSR0[6;24] -> island[6;0], */
                                   /* CSR0[4;20] -> data_master[4;0] */
#define IREF_OVM_DM_SM_ISLD     2  /* CSR0[6;24] -> island[6;0], */
                                   /* CSR0[4;20] -> data_master[4;0], */
                                   /* CSR0[4;20] -> sig_master[4;0] */
#define IREF_OVM_ISLD           3  /* CSR0[6;24] -> island[6;0] */

/* for IREF_OV_DATA field */
#define IREF_OVD_NONE           0
#define IREF_OVD_DR             1  /* PREV[14;16] -> data_ref[14;0]
#define IREF_OVD_DR_DM          2  /* 2b00|PREV[2;30] -> data_master[4;0], */
                                   /* PREV[14;16] -> data_ref[14;0] */
#define IREF_OVD_DCTX           3  /* PREV[3;23] -> data_ref[3;7] */
#define IREF_OVD_DCTX_SCTX      4  /* PREV[3;23] -> data_ref[3;7] */
                                   /* PREV[3;23] -> sig_ref[3;4] */
#define IREF_OVD_DOFF           5  /* PREV[6;16] -> data_ref[6;0] */
#define IREF_OVD_BM             6  /* PREV[8;16] -> byte_mask[8;0] */
#define IREF_OVD_UNUSED         7

/*
 * CMD_INDIRECT_REF_0
 *
 * Bit  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * ---\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Wrd +---+-----------+-------+-------+-----+-------+-+---------------+
 *     |rsv|  Island   |DMaster|SMaster|Sctx |Signum |r|   Bytemask    |
 *     +---+-----------+-------+-------+-----+-------+-+---------------+
 *
 */

#define IREF_CSR0_BYTEMASK_shf          0
#define IREF_CSR0_BYTEMASK_msk          0xFF
#define IREF_CSR0_BYTEMASK_wml          0,8,0

#define IREF_CSR0_SIGNUM_shf            9
#define IREF_CSR0_SIGNUM_msk            0xF
#define IREF_CSR0_SIGNUM_wml            0,12,9

#define IREF_CSR0_SIGCTX_shf            13
#define IREF_CSR0_SIGCTX_msk            0x7
#define IREF_CSR0_SIGCTX_wml            0,15,13

#define IREF_CSR0_SIGMASTER_shf         16
#define IREF_CSR0_SIGMASTER_msk         0xF
#define IREF_CSR0_SIGMASTER_wml         0,19,16

#define IREF_CSR0_DATAMASTER_shf        20
#define IREF_CSR0_DATAMASTER_msk        0xF
#define IREF_CSR0_DATAMASTER_wml        0,23,20

#define IREF_CSR0_ISLAND_shf            24
#define IREF_CSR0_ISLAND_msk            0x3F
#define IREF_CSR0_ISLAND_wml            0,29,24


/*
 * CMD_INDIRECT_REF_1
 *
 * Bit  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * ---\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Wrd +-------------------------------+-------+---------+---+---+-+-+-+
 *     |           reserved            |Target |  Action |Tok|rsv|G|A|K|
 *     +-------------------------------+-------+---------+---+---+-+-+-+
 *
 *  K - Override Token
 *  A - Override Action
 *  G - Override Target
 *  Tok - Token value
 *  Action - Action value
 *  Target - Target value
 */

#define IREF_CSR1_OV_TOK_shf            0
#define IREF_CSR1_OV_TOK_msk            0x1
#define IREF_CSR1_OV_TOK_bit            0
#define IREF_CSR1_OV_TOK_wml            0,0,0

#define IREF_CSR1_OV_ACT_shf            1
#define IREF_CSR1_OV_ACT_msk            0x1
#define IREF_CSR1_OV_ACT_bit            1
#define IREF_CSR1_OV_ACT_wml            0,1,1

#define IREF_CSR1_OV_TGT_shf            2
#define IREF_CSR1_OV_TGT_msk            0x1
#define IREF_CSR1_OV_TGT_bit            2
#define IREF_CSR1_OV_TGT_wml            0,2,2

#define IREF_CSR1_TOKEN_shf             5
#define IREF_CSR1_TOKEN_msk             0x3
#define IREF_CSR1_TOKEN_wml             0,6,5

#define IREF_CSR1_ACTION_shf            7
#define IREF_CSR1_ACTION_msk            0x1F
#define IREF_CSR1_ACTION_wml            0,11,7

#define IREF_CSR1_TARGET_shf            12
#define IREF_CSR1_TARGET_msk            0xF
#define IREF_CSR1_TARGET_wml            0,15,12


#endif /* __IREF_UC */
