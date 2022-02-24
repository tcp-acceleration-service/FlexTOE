/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef __GRO_CFG_UC
#define __GRO_CFG_UC

#include "config.h"

#macro gro_config_block(BLOCKNUM, CALLER)
    /* declare contexts */
    gro_declare_ctx(BLOCKNUM, CALLER, 0,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 1,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 2,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 3,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 4,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 5,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 6,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    gro_declare_ctx(BLOCKNUM, CALLER, 7,  GRO_ISL, EMEM_ISL, GRO_CTX_REORDERQ_SZ)
    /* declare destinations */
#endm

#endif /* __GRO_CFG_UC */
