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
 * @file          me/blocks/gro/_uc/gro_cfg_sample.uc
 */

#ifndef __GRO_CFG_UC
#define __GRO_CFG_UC

/* Our parameters */
#define GRO_0_SEQR              1
#define GRO_1_SEQR              2

#define NFD0_RING_ISL           25
#define NFD1_RING_ISL           26
#define NFD2_RING_ISL           24
#define NFD3_RING_ISL           25
#define NFD_RINGHI(isl)         (0x80 | NFD0_RING_ISL)



/* Global mandetory parameters */
#define GRO_NUM_BLOCKS          2
#define GRO_CTX_PER_BLOCK       8


#macro gro_config_block(BLOCKNUM, CALLER)

    gro_declare_ctx(BLOCKNUM, CALLER, 0, 32, 24, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 1, 33, 25, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 2, 34, 26, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 3, 35, 24, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 4, 36, 25, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 5, 37, 26, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 6, 38, 24, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 7, 32, 25, 8192)

    gro_declare_ctx(BLOCKNUM, CALLER, 8, 33, 26, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 9, 34, 24, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 10, 35, 25, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 11, 36, 26, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 12, 37, 24, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 13, 38, 25, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 14, 38, 26, 8192)
    gro_declare_ctx(BLOCKNUM, CALLER, 15, 32, 24, 8192)

    gro_declare_dest_nbi(BLOCKNUM, CALLER, 0, GRO_1_SEQR)
    gro_declare_dest_nbi(BLOCKNUM, CALLER, 1, GRO_1_SEQR)

    gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 0, NFD_RINGHI(NFD0_RING_ISL),
                               nfd3_pcie_out_ring_0)

    gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 1, NFD_RINGHI(NFD1_RING_ISL),
                               nfd3_pcie_out_ring_1)

    gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 2, NFD_RINGHI(NFD2_RING_ISL),
                               nfd3_pcie_out_ring_2)

    gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 3, NFD_RINGHI(NFD3_RING_ISL),
                               nfd3_pcie_out_ring_3)
#endm



#endif /* __GRO_CFG_UC */
