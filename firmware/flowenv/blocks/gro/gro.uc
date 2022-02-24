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
 * @file          me/blocks/gro/gro.uc
 */

#ifndef __GRO_NFP6000_UC
#define __GRO_NFP6000_UC

/**
 * Main microcode interface file for the Global Reorder Block.
 *
 *
 * OVERVIEW
 *
 * Microcode clients that use the GRO component must include this file.
 * The firmware build itself also requires creating a gro_cfg.uc file that
 * declares the reorder contexts and parameter of the post-reorder
 * destinations that the firmware will use.
 *
 * The GRO subsystem consists of one or more "blocks" and an ME client
 * API for interfacing with those blocks.  Each block consists of single
 * ME that manages a set of reorder contexts and releases packets assigned
 * to that context in order by sequence number. A reorder context is a
 * sequencing domain independent from all the other reorder contexts in the
 * system. The GRO subsystem does not assign packets to reorder contexts, nor
 * does it assign them sequence numbers.  This is the job of the application.
 * The GRO subsystem simply ensures that packets within a given reorder context
 * leave the reorder subsystem in order of their sequence number.
 *
 * In actual fact, the reorder ME clients actually participate in the ordering
 * operation as do the atomic engines in memory-units within the NFP system.
 * The GRO client API abstracts these details away from the application.
 *
 * Each GRO block must have exactly the same number of reorder contexts and
 * that number must be a power of 2.  The number of GRO blocks, however,
 * does not need to be a power of 2.  The GRO subsystem requires the following
 * resources for each reorder context of size N (which must be a power of 2):
 *
 *  + a reorder queue of size N*16 in an NFP memory unit
 *  + a release bitmap in an NFP CTM memory of size N/4 bytes.
 *  + 16-bytes of local memory storage in all client MEs
 *
 * Each reorder block also requires:
 *
 *  + an NFP EMU ingress ring (size flexible within EMU constraints)
 *  + an NFP EMU queue entry for the ingress ring
 *
 * The number of entries of a reorder context must be larger than the
 * maximum number of in-flight packets that the application can assign to
 * that context at any one point in time.  The size may currently be no
 * larger than 65536 entries.  A typical size would probably be around
 * 8192 entries.  This requires a 2K bitmap and 128K of reorder queue memory.
 *
 *
 * CLIENT API CALLS
 *
 * - gro_cli_declare() -- declare global and local resources in a client ME.
 *
 * - gro_cli_init() -- initialize ME local resources in a GRO client and
 *      wait for the reorder subysstem to initialize itself.
 *
 * - gro_cli_build_workq_meta() -- build a GRO release request for a packet
 *      destined for an MU workQ.
 *
 * - gro_cli_build_mem_ring_meta() -- build a GRO release request for a packet
 *      destined for an MU ring.
 *
 * - gro_cli_build_nbi_meta() -- build a GRO release request for a packet
 *      destined for an NFP 6xxx NBI.
 *
 * - gro_cli_build_drop_meta() -- build a GRO release request to drop a sequence
 *      number from consideration in reordering.
 *
 * - gro_cli_drop_seq() -- a convenience macro that wraps
 *      gro_cli_build_drop_meta() and gro_cli_send(). Drops a sequence number
 *      from consideration in reordering.
 *
 * - gro_cli_send() -- Send a GRO release request.
 *
 *
 * TYPICAL USE
 *
 * gro_cli_declare()
 *
 * .reg $gro_meta[GRO_META_SIE_LW]
 * .xfer_order $gro_meta
 *
 * .if (ctx() == 0)
 *    ...
 *    gro_cli_init()
 *    ...
 * .else
 *    // wait for initialization to complete
 * .endif
 *
 * .while (1)
 *   ...
 *   // receive packet, extract contex, sequence number, etc...
 *   ...
 *   gro_cli_build_nbi_meta($gro_meta, pkt_island, pkt_num, ctm_size, pkt_off,
 *                          pkt_length, destination_nbi, destination_txq)
 *   gro_cli_send(pkt_ctx, pkt_seq, $gro_meta, 0)
 *   ...
 * .endw
 *
 *
 * GLOBAL CONFIGURATION FILE
 *
 * The application must provide a global configuration file named "gro_cfg.uc"
 * that declares global parameters of the GRO subsystem and configures the
 * reorder contexts and the destinations that the GRO subsystem will sink
 * packets to.  There is an example of such a file included with this
 * distribution.  Both GRO clients and the GRO blocks will include this file.
 * The file declares the contexts and destinations by creating a single macro
 * called gro_config_block().  Each GRO block will invoke this macro once.
 * Each GRO client will invoke the macro once for every block in the system
 * in the gro_cli_declare() macro.  The gro_cfg.uc file should also set the
 * GRO_NUM_BLOCKS and GRO_CTX_PER_BLOCK global preprocessor configuration
 * parameters.  The former determines the total number of GRO blocks in
 * the system (and by extension the number of RO.OUT MEs in the system).  The
 * latter parameter determines how many reorder contexts each such block
 * will manage and must be a power of 2.
 *
 * The structure of gro_config_block() should be roughly as follows:
 *
 * // Pick appropriate numbers here for ones application
 * #define GRO_NUM_BLOCKS       4
 * #define GRO_CTX_PER_BLOCK    4
 *
 * #define NBI0_RO_SEQR_BASE    1
 * #define NBI1_RO_SEQR_BASE    1
 *
 *
 * #macro gro_config_block(BLOCKNUM, CALLER)
 *
 *   // Declare contexts
 *   gro_declare_ctx(BLOCKNUM, CALLER, ...)
 *   gro_declare_ctx(BLOCKNUM, CALLER, ...)
 *   gro_declare_ctx(BLOCKNUM, CALLER, ...)
 *
 *   // Declare NBI destinations
 *   // NOTE: different sequencers needed for each GRO block
 *   gro_declare_dest_nbi(BLOCKNUM, CALLER, 0, (NBI0_RO_SEQR_BASE + BLOCKNUM))
 *   gro_declare_dest_nbi(BLOCKNUM, CALLER, 1, (NBI1_RO_SEQR_BASE + BLOCKNUM))
 *
 *   // Declare NFD3 destinations, once per active PCIe.
 *   gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 0, ...)
 *   gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 1, ...)
 *   gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 2, ...)
 *   gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, 3, ...)
 *
 * #endm
 *
 */

#include <nfp_chipres.h>
#include "gro.h"
#include <stdmac.uc>
#include <timestamp.uc>
#include <gro_cfg.uc>

#define GRO_CLP2(x)     (1 << log2(x, 1))

#if (__NFPTYPE != __NFP6000)

    #error "This version of gro.uc is only for NFP6xxx processors"

#endif


#ifndef GRO_NUM_BLOCKS
#error "GRO_NUM_BLOCKS must be #defined"
#endif

#ifndef GRO_CTX_PER_BLOCK
#error "GRO_CTX_PER_BLOCK must be #defined"
#endif

#if ((GRO_CTX_PER_BLOCK & (GRO_CTX_PER_BLOCK - 1)) != 0)
#error GRO_CTX_PER_BLOCK must be a power of 2
#endif

#define GRO_CTX_BLOCK_MASK (GRO_CTX_PER_BLOCK-1)

#define GRO_TOTAL_CTX (GRO_NUM_BLOCKS * GRO_CTX_PER_BLOCK)

/*
 * The amount of time to wait when a ticket release hits the skid.
 * If we are in the skid then we can expect it to take an average
 * of 40 * 64 = 2560 cycles to release the first entry in the skid.
 * That is if we are at the front of the skid!
 */
#define GRO_SEQ_OVERFLOW_TICKS      (2560 / 16)

#macro gro_declare_block(BLOCKNUM)

    #if (GRO_NUM_BLOCKS > 8)
        #error "Currently, only 8 global reorder blocks supported"
    #endif
    #if (__nfp_has_island(25) && __nfp_has_island(26))
        #define gro_release_mem_0 emem0
        #define gro_release_isl_0 24
        #define gro_release_mem_1 emem1
        #define gro_release_isl_1 25
        #define gro_release_mem_2 emem2
        #define gro_release_isl_2 26
        #define gro_release_mem_3 emem0
        #define gro_release_isl_3 24
        #define gro_release_mem_4 emem1
        #define gro_release_isl_4 25
        #define gro_release_mem_5 emem2
        #define gro_release_isl_5 26
        #define gro_release_mem_6 emem0
        #define gro_release_isl_6 24
        #define gro_release_mem_7 emem1
        #define gro_release_isl_7 25
    #elif (__nfp_has_island(25))
        #define gro_release_mem_0 emem0
        #define gro_release_isl_0 24
        #define gro_release_mem_1 emem1
        #define gro_release_isl_1 25
        #define gro_release_mem_2 emem0
        #define gro_release_isl_2 24
        #define gro_release_mem_3 emem1
        #define gro_release_isl_3 25
        #define gro_release_mem_4 emem0
        #define gro_release_isl_4 24
        #define gro_release_mem_5 emem1
        #define gro_release_isl_5 25
        #define gro_release_mem_6 emem0
        #define gro_release_isl_6 24
        #define gro_release_mem_7 emem1
        #define gro_release_isl_7 25
    #else
        #define gro_release_mem_0 emem0
        #define gro_release_isl_0 24
        #define gro_release_mem_1 emem0
        #define gro_release_isl_1 24
        #define gro_release_mem_2 emem0
        #define gro_release_isl_2 24
        #define gro_release_mem_3 emem0
        #define gro_release_isl_3 24
        #define gro_release_mem_4 emem0
        #define gro_release_isl_4 24
        #define gro_release_mem_5 emem0
        #define gro_release_isl_5 24
        #define gro_release_mem_6 emem0
        #define gro_release_isl_6 24
        #define gro_release_mem_7 emem0
        #define gro_release_isl_7 24
    #endif

    #define_eval __MEM 'gro_release_mem_/**/BLOCKNUM'
    #define_eval __QRES '__MEM/**/_queues'
    .alloc_resource gro_release_ring_/**/BLOCKNUM __QRES global 1
    #undef __MEM
    #undef __QRES

    .alloc_mem _gro_ctxcfg emem global                  \
        (GRO_CLICTX_SIZE * GRO_TOTAL_CTX)               \
        GRO_CLP2(GRO_CLICTX_SIZE * GRO_TOTAL_CTX)

    .alloc_mem _gro_dstcfg emem global                          \
        (GRO_DEST_SIZE * GRO_NUM_BLOCKS * GRO_DEST_PER_BLOCK)   \
        GRO_CLP2(GRO_DEST_SIZE * GRO_NUM_BLOCKS * GRO_DEST_PER_BLOCK)

    .alloc_mem _gro_global_config emem global GRO_GLOBAL_CFG_SIZE

    .init _gro_global_config+0 GRO_NUM_BLOCKS
    .init _gro_global_config+4 GRO_CTX_PER_BLOCK

    .alloc_mem _gro_cli_cntrs GRO_CNTR_MEM_UC global \
        (GRO_TOTAL_CTX * 16) 256

#endm


/* TODO:  Need to do this in a chip-independent manner */
#define GRO_USE_CACHE_UPPER 128
#define ISL_IS_EMU_UPPER(isl) ((isl & GRO_USE_CACHE_UPPER) != 0)
#define ISL_IS_EMU(isl) (((isl) >= 24) && ((isl) <= 26))
#define ISL_IS_IMU(isl) (((isl) == 28) || ((isl) == 29))


#macro __gro_build_mem_name(ISL)
    #if (strstr('ISL', '.'))
        #define_eval __GRO_MEM_NAME 'ISL'
    #elif (!isnum(ISL))
        #define_eval __GRO_MEM_NAME 'ISL'
    #elif (ISL_IS_EMU_UPPER(ISL))
        #define_eval __ISL (ISL & ~GRO_USE_CACHE_UPPER)
        #define_eval __GRO_MEM_NAME 'i/**/__ISL.emem_cache_upper'
        #undef __ISL
    #elif (ISL_IS_EMU(ISL))
        #define_eval __GRO_MEM_NAME 'i/**/ISL.emem'
    #elif (ISL_IS_IMU(ISL))
        #define_eval __GRO_MEM_NAME 'i/**/ISL.imem'
    #else
        #define_eval __GRO_MEM_NAME 'i/**/ISL.ctm'
    #endif
#endm


/**
 * Declare a reorder context.
 *
 * The gro_config_block() macro must invoke this once for each context in the
 * system.
 *
 * @param BLOCKNUM      Should be passed the unmodified value from the
 *                      gro_config_block() parameter.
 *
 * @param CALLER        Should be passed the unmodified value from the
 *                      gro_config_block() parameter.
 *
 * @param CTXNUM        The context number to configure.
 *
 * @param BMSIL         The NFP island with the memory unit of the context's
 *                      bitmap.
 *
 * @param QISL          The NFP island with the memory unit to hold the
 *                      context's reorder queue.
 *
 * @param NUMENT        The number of entries in the reorder context.  Must be a
 *                      power of 2 >= 64.
 *
 * @param SKIDLEN       Determines skid length for sequence number wrap
 *                      prevention. Value in # of entries but must be a multiple
 *                      of 64.  (defaults to 0: no skid)
 *
 * @param ISEQ          The initial sequence number of the context (default 0)
 *
 * @param BMADDR        The address to start the reorder bitmap.
 *                      (defaults to '--': let the linker pick)
 *
 * @param QADDR         The address to start the reorder queue.
 *                      (defaults to '--': let the linker pick)
 */
#macro gro_declare_ctx(BLOCKNUM, CALLER, CTXNUM, BMISL, QISL, NUMENT, SKIDLEN, ISEQ, BMADDR, QADDR)

    #if (BLOCKNUM == (CTXNUM / GRO_CTX_PER_BLOCK))

        #define_eval __BCTX (CTXNUM % GRO_CTX_PER_BLOCK)

        #if (SKIDLEN % 64 != 0)
            #error "Invalid sequence skid length (must be a multiple of 64)"
        #endif

        #if (SKIDLEN == 0)
            #warning "Although SKIDLEN = 0 is probably OK, is safer to set it to 64"
        #endif

        #if (SKIDLEN > NUMENT - 64)
            #error "Invalid sequence skid length"
        #endif

        #if (ISEQ >= NUMENT)
            #error "Invalid initial sequence number"
        #endif

        #if (!streq('BMADDR', '--'))
            #define_eval __BMOFF    '+BMADDR'
        #else
            #define_eval __BMOFF    ''
        #endif
        #if (!streq('QADDR', '--'))
            #define_eval __QOFF    '+QADDR'
        #else
            #define_eval __QOFF     ''
        #endif

        __gro_build_mem_name(BMISL)
        .alloc_mem _gro_bm_/**/BLOCKNUM/**/_/**/__BCTX __GRO_MEM_NAME global \
            ((NUMENT/64)*16) ((NUMENT/64)*16)

        __gro_build_mem_name(QISL)
        .alloc_mem _gro_q_/**/BLOCKNUM/**/_/**/__BCTX __GRO_MEM_NAME __QOFF global \
            (NUMENT * GRO_META_SIZE) (NUMENT * GRO_META_SIZE)

        #define_eval __SKIDOFF (NUMENT - SKIDLEN)

        #if (streq('CALLER', 'GRO_OUT'))

            /* Global Config */
            #define_eval __CFGADDR      (((BLOCKNUM * GRO_CTX_PER_BLOCK) + __BCTX) * GRO_CLICTX_SIZE)
            .init _gro_ctxcfg+__CFGADDR \
                (((gro_release_ring_/**/BLOCKNUM & GRO_CLICTX_RING_LO_msk) << GRO_CLICTX_RING_LO_shf) |\
                 (((gro_release_isl_/**/BLOCKNUM | 0x80) & GRO_CLICTX_RING_HI_msk) << GRO_CLICTX_RING_HI_shf))

            #define_eval __CFGADDR (__CFGADDR + 4)
            .init _gro_ctxcfg+__CFGADDR    \
                (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX & GRO_CLICTX_BM_LO_msk) << GRO_CLICTX_BM_LO_shf) | \
                 (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX >> 32) & GRO_CLICTX_BM_HI_msk) << GRO_CLICTX_BM_HI_shf))

            #define_eval __CFGADDR (__CFGADDR + 4)
            .init _gro_ctxcfg+__CFGADDR    \
                (((log2(NUMENT) & GRO_CLICTX_Q_SIZE_msk) << GRO_CLICTX_Q_SIZE_shf) |\
                 (((_gro_q_/**/BLOCKNUM/**/_/**/__BCTX >> 32) & GRO_CLICTX_Q_HI_msk) << GRO_CLICTX_Q_HI_shf))

            #define_eval __CFGADDR (__CFGADDR + 4)
            .init _gro_ctxcfg+__CFGADDR    \
                (_gro_q_/**/BLOCKNUM/**/_/**/__BCTX & GRO_CLICTX_Q_LO_msk)


            /* GRO.OUT LM_CTX_* config */

            /* Q_ADDRHI */
            #define_eval __CFGADDR  (__BCTX * LM_CTX_SIZE)
            .init gro_out_lm_ctx+__CFGADDR \
                ((_gro_q_/**/BLOCKNUM/**/_/**/__BCTX >> 8) & 0xFF000000)

            /* Q_BASE */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                (_gro_q_/**/BLOCKNUM/**/_/**/__BCTX & 0xFFFFFFFF)

            /* Q_ADDRLO */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                ((_gro_q_/**/BLOCKNUM/**/_/**/__BCTX & 0xFFFFFFFF) + \
                 (ISEQ * GRO_META_SIZE))

            /* Q_BM_RESET */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                ((_gro_q_/**/BLOCKNUM/**/_/**/__BCTX & 0xFFFFFFFF) + \
                 ((ISEQ + GRO_TICKET_PER_TAPE) / GRO_TICKET_PER_TAPE * \
                  GRO_TICKET_PER_TAPE * GRO_META_SIZE))

            /* Q_END */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                ((_gro_q_/**/BLOCKNUM/**/_/**/__BCTX & 0xFFFFFFFF) + \
                 (NUMENT * GRO_META_SIZE))

            /* RST_EPOCH */
            #define_eval __CFGADDR (__CFGADDR + 4)
            #if ((ISEQ / 64 * 64) + __SKIDOFF < NUMENT)
                .init gro_out_lm_ctx+__CFGADDR ((ISEQ / NUMENT) % GRO_NUM_EPOCHS)
            #else
                .init gro_out_lm_ctx+__CFGADDR (((ISEQ / NUMENT) + 1) % GRO_NUM_EPOCHS)
            #endif

            /* BM_ADDRHI */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX) >> 8) & 0xFF000000)

            /* BM_BASE */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                ((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX) & 0xFFFFFFFF)

            /* BM_NEXT */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX) & 0xFFFFFFFF) + \
                 (((ISEQ / 64 * 64) + __SKIDOFF) % NUMENT / GRO_TICKET_PER_TAPE * GRO_TICKET_TAPE_SIZE))

            /* BM_END */
            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_out_lm_ctx+__CFGADDR \
                (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX) & 0xFFFFFFFF) +
                 (NUMENT / GRO_TICKET_PER_TAPE * GRO_TICKET_TAPE_SIZE))


            /* GRO bitmap initialization */
            #define_eval __BMOFF 0
            #define_eval __BMEND ((NUMENT / 64) * 16)
            #define_eval __IEPOCH (ISEQ / NUMENT % 8)
            #if (__IEPOCH > 0)
                #define_eval __IEPOCH_MINUS1 (GRO_NUM_EPOCHS - 1)
            #else
                #define_eval __IEPOCH_MINUS1 ((__IEPOCH - 1) & GRO_EPOCH_msk)
            #endif
            #while (__BMOFF < __BMEND)

                // NFP6xxx Databook 9.2.2.1.2.10
                // First word contains the sequence number in the top 10 bits.
                // Start all ticket tapes at sequence 0 except for the first
                // tape which should start at sequence number 1.
                #if (__BMOFF == (ISEQ/64))

                    // Initial tape
                    .init _gro_bm_/**/BLOCKNUM/**/_/**/__BCTX+__BMOFF (((ISEQ % 64) + 1) + (__IEPOCH * GRO_EPOCH_TICKET_SEQ_mul))
                    // XXX IMPLIED 0x00000000 0x00000000 0x00000000

                #elif ((SKIDLEN > 0) && \
                       ((__BMOFF < (ISEQ/64*16) && ((ISEQ/64 < SKIDLEN/64) || (__BMOFF >= (ISEQ/64 - SKIDLEN/64)*16))) || \
                        ((ISEQ/64*64 + __SKIDOFF < NUMENT) && (__BMOFF >= (ISEQ/64 + __SKIDOFF/64)*16))))

                    // In the tape skid
                    .init _gro_bm_/**/BLOCKNUM/**/_/**/__BCTX+__BMOFF (GRO_SEQSKID_VALUE | (__IEPOCH_MINUS1 * GRO_EPOCH_TICKET_SEQ_mul))
                    // XXX IMPLIED 0x00000000 0x00000000 0x00000000

                #elif (__IEPOCH != 0)

                    // Not in the tape skid, but initial epoch non-zero
                    .init _gro_bm_/**/BLOCKNUM/**/_/**/__BCTX+__BMOFF (__IEPOCH * GRO_EPOCH_TICKET_SEQ_mul)
                    // XXX IMPLIED 0x00000000 0x00000000 0x00000000

                #else

                    // Not in initial tape skid and initial epoch is 0
                    // XXX IMPLIED .init _gro_bm_/**/BLOCKNUM/**/_/**/__BCTX+__BMOFF 0x00000000 0x00000000 0x00000000 0x00000000

                #endif

                #define_eval __BMOFF (__BMOFF + 16)
            #endloop

            #undef __IEPOCH
            #undef __IEPOCH_MINUS1
            #undef __BMOFF
            #undef __BMEND

        #endif

        #if (streq('CALLER', 'CLIENT'))

            #define_eval __CFGADDR  (((BLOCKNUM * GRO_CTX_PER_BLOCK) + __BCTX) * GRO_CLICTX_SIZE)
            .init gro_cli_lm_ctx+__CFGADDR \
                (((gro_release_ring_/**/BLOCKNUM & GRO_CLICTX_RING_LO_msk) << GRO_CLICTX_RING_LO_shf) |\
                 (((gro_release_isl_/**/BLOCKNUM | 0x80)& GRO_CLICTX_RING_HI_msk) << GRO_CLICTX_RING_HI_shf))

            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_cli_lm_ctx+__CFGADDR \
                (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX & GRO_CLICTX_BM_LO_msk) << GRO_CLICTX_BM_LO_shf) | \
                 (((_gro_bm_/**/BLOCKNUM/**/_/**/__BCTX >> 32) & GRO_CLICTX_BM_HI_msk) << GRO_CLICTX_BM_HI_shf))

            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_cli_lm_ctx+__CFGADDR \
                (((log2(NUMENT) & GRO_CLICTX_Q_SIZE_msk) << GRO_CLICTX_Q_SIZE_shf) |\
                 (((_gro_q_/**/BLOCKNUM/**/_/**/__BCTX >> 32) & GRO_CLICTX_Q_HI_msk) << GRO_CLICTX_Q_HI_shf))

            #define_eval __CFGADDR (__CFGADDR + 4)
            .init gro_cli_lm_ctx+__CFGADDR \
                (_gro_q_/**/BLOCKNUM/**/_/**/__BCTX & GRO_CLICTX_Q_LO_msk)

        #endif

        #undef __BCTX
        #undef __QOFF
        #undef __CFGADDR
        #undef __SKIDOFF
        #undef __GRO_MEM_NAME

    #endif /* BLOCKNUM == (CTXNUM / GRO_CTX_PER_BLOCK) */

#endm


/**
 * See the 10-parameter variant.
 */
#macro gro_declare_ctx(BLOCKNUM, CALLER, CTXNUM, BMISL, QISL, NUMENT)
    gro_declare_ctx(BLOCKNUM, CALLER, CTXNUM, BMISL, QISL, NUMENT, 64, 0, --, --)
#endm


/**
 * See the 10-parameter variant.
 */
#macro gro_declare_ctx(BLOCKNUM, CALLER, CTXNUM, BMISL, QISL, NUMENT, SKIDLEN)
    gro_declare_ctx(BLOCKNUM, CALLER, CTXNUM, BMISL, QISL, NUMENT, SKIDLEN, 0, --, --)
#endm


/**
 * Declare an NFP 6xxx NBI reorder destination.
 *
 * The gro_config_block() macro must invoke this once for each NBI in the
 * system.  It must invoke it with a different SEQR parameter for
 * each separate GRO block.
 *
 * @param BLOCKNUM      Should be passed the unmodified value from the
 *                      gro_config_block() parameter.
 *
 * @param CALLER        Should be passed the unmodified value from the
 *                      gro_config_block() parameter.
 *
 * @param NBINUM        The number of the NBI whose destination to configure.
 *
 * @param SEQR          The TM sequencer for this reorder block to use
 *                      with the given NBI.
 *
 */
#macro gro_declare_dest_nbi(BLOCKNUM, CALLER, NBINUM, SEQR)

    #if (NBINUM < 0 || NBINUM >= GRO_NUM_IFACE_DEST)
        #error "Invalid NBI destination number!"
    #endif


    #if (streq('CALLER', 'GRO_OUT'))

        #define_eval __DSTNUM (BLOCKNUM * GRO_DEST_PER_BLOCK + GRO_DEST_IFACE_BASE + NBINUM)

        /*
         * GRO must set the sequencer and sequence number for the
         * packet.  See NFP-6xxx DB 9.2.2.7.9:  This goes in the bytemask
         * With a PPC command, we override this in CmdIndirectRef0.
         * These are in bits 0 to 7 in that register.  See NFP-6xxx
         * PRM 4.2.1.1.55. The sequence number goes in bits 12-29
         * but these all get set initially to 0.
         */
        #define_eval __DSTADDR      (__DSTNUM * GRO_DEST_SIZE)
        .init _gro_dstcfg+__DSTADDR    SEQR

        #define_eval __DSTNUM   (GRO_DEST_IFACE_BASE + NBINUM)
        #define_eval __DSTADDR  (__DSTNUM * GRO_DEST_SIZE)
        .init gro_out_lm_dest+__DSTADDR     SEQR

        #undef __DSTADDR
        #undef __DSTNUM

    #endif


#endm


/**
 * Declare an NFD3 reorder destination.
 *
 * The gro_config_block() macro must invoke this once for each NFD3
 * PCI.OUT queue in the system.
 *
 * @param BLOCKNUM      Should be passed the unmodified value from the
 *                      gro_config_block() parameter.
 *
 * @param CALLER        Should be passed the unmodified value from the
 *                      gro_config_block() parameter.
 *
 * @param PCIE          The PCIe number of this destination.
 *
 * @param RINGHI        The high 8 bits of the ring address to send to
 *                      this NFD3 destination.
 *
 * @param RINGLO        The low 10 bits of the ring address to send to
 *                      this NFD3 destination.
 */
#macro gro_declare_dest_nfd3_allq(BLOCKNUM, CALLER, PCIE, RINGHI, RINGLO)

    #if (PCIE < 0 || PCIE >= GRO_DEST_NFD3_MAX_PCIE)
        #error "Invalid NFD3 PCIe number!"
    #endif

    #if (streq('CALLER', 'GRO_OUT'))

        #define_eval __DSTNUM (BLOCKNUM * GRO_DEST_PER_BLOCK + GRO_DEST_NFD_BASE + PCIE)

        #define_eval __DSTADDR      (__DSTNUM * GRO_DEST_SIZE)
        // XXX IMPLIED .init _gro_dstcfg+__DSTADDR    0

        #define_eval __DSTADDR (__DSTADDR + 4)
        .init _gro_dstcfg+__DSTADDR \
            (((RINGHI & GRO_DEST_NFD3_RINGHI_msk) << GRO_DEST_NFD3_RINGHI_shf) | \
             ((RINGLO & GRO_DEST_NFD3_RINGLO_msk) << GRO_DEST_NFD3_RINGLO_shf))

        #define_eval __DSTNUM (GRO_DEST_NFD_BASE + PCIE)

        #define_eval __DSTADDR  (__DSTNUM * GRO_DEST_SIZE)
        // XXX IMPLIED .init gro_out_lm_dest+__DSTADDR    0

        #define_eval __DSTADDR  (__DSTADDR + 4)
        .init gro_out_lm_dest+__DSTADDR \
            (((RINGHI & GRO_DEST_NFD3_RINGHI_msk) << GRO_DEST_NFD3_RINGHI_shf) | \
             ((RINGLO & GRO_DEST_NFD3_RINGLO_msk) << GRO_DEST_NFD3_RINGLO_shf))

        #undef __DSTADDR
        #undef __DSTNUM

    #endif

#endm


/**
 * Declare global and local resources for the GRO subsystem.
 * Must be called within each GRO client ME.
 */
#macro gro_cli_declare()

    .alloc_mem gro_cli_lm_ctx lmem me (GRO_CLICTX_SIZE * GRO_TOTAL_CTX) \
        GRO_CLP2(GRO_CLICTX_SIZE * GRO_TOTAL_CTX)

    #define_eval __BLOCK 0
    #while (__BLOCK < GRO_NUM_BLOCKS)

        gro_declare_block(__BLOCK)
        gro_config_block(__BLOCK, CLIENT)

        #define_eval __BLOCK (__BLOCK + 1)
    #endloop
    #undef __BLOCK

#endm


#macro _gro_cli_init(WAIT)
#endm


/**
 * Initialize the ME-local resoruces for the reorder subsystem to
 * be ready.  An ME should call this once at initialization
 * time within an ME and only within a single thread.  The ME MUST
 * invoke this call before calling any gro_nbi_send*() or
 * gro_nbi_drop_seq() calls.
 */
#macro gro_cli_init()
    _gro_cli_init(1)
#endm


#macro _gro_cli_cntr_enqueued_incr(in_ctx)
.begin
    .reg addr_hi
    .reg addr_lo
    .reg iref

    move(addr_hi, (_gro_cli_cntrs >> 8))
    #if (isnum(in_ctx))
        move(addr_lo, (in_ctx << 4))
    #else
        alu[addr_lo, --, B, in_ctx, <<4]
    #endif

    // XXX I am seeing issues w/ mem[incr64] which is why I am using
    // the add64_imm instead.  Some values aren't incrementing when
    // they should.  The fact that sequence numbers lined up with
    // the "released" counters (which uses add64_imm) instead of the
    // "enqueued" counters (which use incr64) leads me to believe
    // that the incr64 is in error.
    ; override length  = (1 << 7)
    ; override dataref = (2 << 3)
    ; length[2] = 1 for 64-bit operations = (1 << 10)
    ; length[3] = 1 for to pull operand from dataref = (1 << 11)
    move(iref, ((2 << 3) | (1 << 7) | (1 << 10) | (1 << 11)))
    alu[--, iref, OR, 1, <<16]
    mem[add64_imm, --, addr_hi, <<8, addr_lo], indirect_ref

.end
#endm


#macro _gro_cli_cntr_released_add(in_ctx, in_amt)
.begin
    .reg addr_hi
    .reg addr_lo
    .reg iref

    move(addr_hi, (_gro_cli_cntrs >> 8))
    #if (isnum(in_ctx))
        move(addr_lo, ((in_ctx << 4) | 8))
    #else
        alu[addr_lo, 8, OR, in_ctx, <<4]
    #endif

    ; override length  = (1 << 7)
    ; override dataref = (2 << 3)
    ; length[2] = 1 for 64-bit operations = (1 << 10)
    ; length[3] = 1 for to pull operand from dataref = (1 << 11)
    move(iref, ((2 << 3) | (1 << 7) | (1 << 10) | (1 << 11)))
    alu[--, iref, OR, in_amt, <<16]
    mem[add64_imm, --, addr_hi, <<8, addr_lo], indirect_ref
.end
#endm


#macro _gro_cli_release_nowait(in_clictx, in_ctx, in_nrel, in_xfer, INSIG)
.begin
    .reg addr_lo
    .reg addr_hi
    .reg tmp

    // leave in place: don't shift
    alu[addr_hi, in_clictx[GRO_CLICTX_RING_HI_wrd], AND, GRO_CLICTX_RING_HI_msk, <<GRO_CLICTX_RING_HI_shf]
    alu[addr_lo, in_clictx[GRO_CLICTX_RING_LO_wrd], AND~, GRO_CLICTX_RING_HI_msk, <<GRO_CLICTX_RING_HI_shf]

    #if (is_ct_const(in_ctx))
        move(tmp, (in_ctx & GRO_CTX_BLOCK_MASK))
    #else
        alu[tmp, in_ctx, AND, GRO_CTX_BLOCK_MASK]
    #endif
    alu[tmp, tmp, OR, in_nrel, <<GRO_REL_NREL_shf]
    alu[in_xfer, tmp, OR, 1, <<GRO_REL_VALID_bit]

    mem[put, in_xfer, addr_hi, <<8, addr_lo, 1], sig_done[INSIG]
    _gro_cli_cntr_released_add(in_ctx, in_nrel)
.end
#endm


#macro _gro_cli_release(in_clictx, in_ctx, in_nrel)
.begin
    .reg addr_lo
    .reg addr_hi
    .reg tmp

    .reg $gro_release
    .sig release_sig

    // leave addr_hi in place: don't shift
    alu[addr_hi, in_clictx[GRO_CLICTX_RING_HI_wrd], AND, GRO_CLICTX_RING_HI_msk, <<GRO_CLICTX_RING_HI_shf]
    alu[addr_lo, in_clictx[GRO_CLICTX_RING_LO_wrd], AND~, GRO_CLICTX_RING_HI_msk, <<GRO_CLICTX_RING_HI_shf]

    #if (is_ct_const(in_ctx) && is_ct_const(in_nrel))
        move(tmp, ((in_ctx & (GRO_CTX_PER_BLOCK-1)) | (in_nrel << GRO_REL_NREL_shf)))
    #else
        #if (is_ct_const(in_ctx))
            move(tmp, (in_ctx & (GRO_CTX_PER_BLOCK-1)))
        #else
            alu[tmp, in_ctx, AND, (GRO_CTX_PER_BLOCK-1)]
        #endif
        alu[tmp, tmp, OR, in_nrel, <<GRO_REL_NREL_shf]
    #endif
    alu[$gro_release, tmp, OR, 1, <<GRO_REL_VALID_bit]

again#:
    mem[put, $gro_release, addr_hi, <<8, addr_lo, 1], sig_done[release_sig]
    ctx_arb[release_sig]
    br_bclr[$gro_release, 31, again#]

    _gro_cli_cntr_released_add(in_ctx, in_nrel)
.end
#endm


#macro _gro_cli_reset_tape(in_clictx, in_ticket_tape, in_tape_xfer, TAPESIG)
.begin

    .reg addr_lo
    .reg addr_hi

    alu[addr_hi, in_clictx[GRO_CLICTX_BM_HI_wrd], AND, GRO_CLICTX_BM_HI_msk, <<GRO_CLICTX_BM_HI_shf]
    alu[addr_lo, in_clictx[GRO_CLICTX_BM_LO_wrd], AND~, GRO_CLICTX_BM_HI_msk, <<GRO_CLICTX_BM_HI_shf]
    alu[addr_lo, addr_lo, OR, in_ticket_tape, <<GRO_TICKET_TAPE_SIZE_shf]
    mem[write_atomic, in_tape_xfer[0], addr_hi, <<8, addr_lo, 4], sig_done[TAPESIG]

.end
#endm


/*
 * Load the client's local memory context for this block.
 *
 * Need to wait 3 cycles between issuing this call and using the data.
 */
#macro _gro_cli_load_lm_ctx(in_ctx, LMPTR)
.begin
    .reg lma
    immed[lma, gro_cli_lm_ctx]
    alu[lma, lma, OR, in_ctx, <<GRO_CLICTX_shf]
    local_csr_wr[ACTIVE_LM_ADDR_/**/LMPTR, lma]
.end
#endm


#macro _gro_cli_test_ticket_error(in_addr_hi, in_addr_lo, in_ticket_seq, in_epoch, ERROR_LABEL)
.begin

    .reg skid_marker
    .reg tape_epoch
    .reg tape_seq

    // Ticket tapes are 4 words long, but only the first 3 are relevant to us
    .reg $tape[3]
    .xfer_order $tape

    .sig tape_read_sig

    mem[atomic_read, $tape[0], addr_hi, <<8, addr_lo, 3], ctx_swap[tape_read_sig], defer[2]
    alu[skid_marker, epoch, -, 1]
    alu[skid_marker, skid_marker, AND, GRO_EPOCH_msk]

    alu[skid_marker, GRO_SEQSKID_VALUE, OR, skid_marker, <<GRO_EPOCH_TICKET_SEQ_shf]

    // If the tape matches the skid marker (from the previous epoch)
    // it's simple overflow:  not an error
    alu[--, $tape[0], -, skid_marker]
    beq[no_error#]

    // Otherwise if we are not in the same epoch, its definitely an error
    alu[tape_epoch, GRO_EPOCH_msk, AND, $tape[0], >>GRO_EPOCH_TICKET_SEQ_shf]
    alu[--, in_epoch, -, tape_epoch]
    bne[ERROR_LABEL]

    // If the ticket seq equals the tape seq, then the bitmap was just
    // de-skidded ... no error
    alu[tape_seq, $tape[0], AND, GRO_TAPE_SEQ_msk]
    alu[--, in_ticket_seq, -, tape_seq]
    beq[no_error#]

    // If the ticket seq is less than the tape seq, then it's definitely
    // a duplicate.
    blt[ERROR_LABEL]

    // So we have:  tape_epoch == epoch && in_seq > tape_seq
    // We need to test to see if the bit for this seq is set in the bitmask.
    // If so, it's an error, otherwise the tape was just de-skidded.
    alu[tape_seq, in_ticket_seq, -, tape_seq]
    alu[tape_seq, tape_seq, -, 1]
    .if (tape_seq < 32)
        alu[--, tape_seq, OR, 0]
        alu[--, $tape[1], AND, 1, <<indirect]
        bne[ERROR_LABEL]
    .else
        alu[tape_seq, tape_seq, -, 32]
        alu[--, tape_seq, OR, 0]
        alu[--, $tape[2], AND, 1, <<indirect]
        bne[ERROR_LABEL]
    .endif

no_error#:
.end
#endm


#macro _gro_cli_test_cascade_error(in_addr_hi, in_addr_lo, in_epoch, ERROR_LABEL)
.begin

    .reg skid_marker

    // Ticket tapes are 4 words long, but only the first 3 are relevant to us
    .reg $tape_seq

    .sig tape_read_sig

    mem[atomic_read, $tape_seq, addr_hi, <<8, addr_lo, 1], ctx_swap[tape_read_sig], defer[2]
    alu[skid_marker, epoch, -, 1]
    alu[skid_marker, skid_marker, AND, GRO_EPOCH_msk]

    // If we are at the skid: no error
    alu[skid_marker, GRO_SEQSKID_VALUE, OR, skid_marker, <<GRO_EPOCH_TICKET_SEQ_shf]
    alu[--, $tape_seq, -, skid_marker]
    beq[no_error#]

    // If the tape is anything other than freshly reset for us: error
    alu[skid_marker, 0, OR, in_epoch, <<GRO_EPOCH_TICKET_SEQ_shf]
    alu[--, $tape_seq, -, skid_marker]
    bne[ERROR_LABEL]

no_error#:
.end
#endm


#macro _gro_cli_send(in_ctx, in_seq, in_meta, META_SIZE_LW, LMPTR)
.begin

    .reg epoch
    .reg ticket_tape
    .reg ticket_seq
    .reg ctx_seq
    .reg seq_mask
    .reg num_release
    .reg ticket_seq_hi
    .reg ntapes

    .reg addr_hi
    .reg addr_lo

    .reg $ticket

    .sig meta_sig
    .sig ticket_sig


    #define_eval __GRO_CTX '*l$index/**/LMPTR'

    // Load the reorder context information: Need to wait 3 cycles before use
    _gro_cli_load_lm_ctx(in_ctx, LMPTR)

    // Compute the sequence number we'll use in the ticket tape:
    //   epoch = (seq / Qsize) % GRO_NUM_EPOCHS
    //   ticket_seq = ((in_seq % 64) + 1)
    //   tape_seq = ticket_seq | (epoch * GRO_EPOCH_TICKET_SEQ_mul)
    #if (is_ct_const(in_seq))
        immed[ticket_seq, (GRO_TICKET_msk & in_seq)]            // cycle 1
    #else
        alu[ticket_seq, GRO_TICKET_msk, AND, in_seq]            // cycle 1
    #endif
    alu[ticket_seq, ticket_seq, +, 1]                           // cycle 2
    nop                                                         // cycle 3

    #if (is_ct_const(in_seq))
        alu[--, __GRO_CTX[GRO_CLICTX_Q_SIZE_wrd], OR, 0]
        alu[epoch, --, B, in_seq, >>indirect]
        alu[epoch, epoch, AND, GRO_EPOCH_msk]
    #else
        alu[--, __GRO_CTX[GRO_CLICTX_Q_SIZE_wrd], OR, 0]
        alu[epoch, GRO_EPOCH_msk, AND, in_seq, >>indirect]
    #endif

    alu[$ticket, ticket_seq, OR, epoch, <<GRO_EPOCH_TICKET_SEQ_shf]

    // ctx_seq = (seq % Qsize)
    alu[--, __GRO_CTX[GRO_CLICTX_Q_SIZE_wrd], OR, 0]
    alu[seq_mask, --, B, 1, <<indirect]
    alu[seq_mask, seq_mask, -, 1]
    alu[ctx_seq, in_seq, AND, seq_mask]

    // Write the metadata to the appropriate location in the queue.
    // We must wait for this to finish before releasing the ticket.
    alu[addr_hi, __GRO_CTX[GRO_CLICTX_Q_HI_wrd], AND, GRO_CLICTX_Q_HI_msk, <<GRO_CLICTX_Q_HI_shf]
    alu[addr_lo, __GRO_CTX[GRO_CLICTX_Q_LO_wrd], OR, ctx_seq, <<(log2(GRO_META_SIZE))]
    mem[write32, in_meta[0], addr_hi, <<8, addr_lo, META_SIZE_LW], ctx_swap[meta_sig]

    // ticket_tape = ctx_seq / GRO_TICKET_PER_TAPE
    alu[ticket_tape, --, B, ctx_seq, >>(log2(GRO_TICKET_PER_TAPE))]

    // addr = ctx.hi || (ctx.lo + ticket_tape * ticket_tape_size)
    alu[addr_hi, __GRO_CTX[GRO_CLICTX_BM_HI_wrd], AND, GRO_CLICTX_BM_HI_msk, <<GRO_CLICTX_BM_HI_shf]
    alu[addr_lo, __GRO_CTX[GRO_CLICTX_BM_LO_wrd], AND~, GRO_CLICTX_BM_HI_msk, <<GRO_CLICTX_BM_HI_shf]
    alu[addr_lo, addr_lo, OR, ticket_tape, <<GRO_TICKET_TAPE_SIZE_shf]
release_first_ticket#:
    mem[release_ticket, $ticket, addr_hi, <<8, addr_lo, 1], sig_done[ticket_sig]
    ctx_arb[ticket_sig]

    // Test for someone else releasing the same ticket number
    br=byte[$ticket, 0, GRO_TICKET_ERROR, ticket_error_seq#]

    _gro_cli_cntr_enqueued_incr(in_ctx)

    // if no release, then we are done
    br=byte[$ticket, 0, 0, done#]

    // Check whether we are at the end of the current tape
    alu[ticket_seq_hi, $ticket, +, ticket_seq]
    alu[--, ticket_seq_hi, -, GRO_TICKET_PER_TAPE]
    ble[final_release#], defer[1]
    alu[num_release, --, B, $ticket]

    // ntapes = (Qsize / GRO_TICKET_PER_TAPE)
    alu[--, __GRO_CTX[GRO_CLICTX_Q_SIZE_wrd], OR, 0]
    alu[ntapes, --, B, 1, <<indirect]
    alu[ntapes, --, B, ntapes, >>(log2(GRO_TICKET_PER_TAPE))]

    // Reset the current block and then test for releases in the next block.
    // Repeat this until we stop releasing through the end of blocks.
    .repeat

        _gro_cli_release(__GRO_CTX, in_ctx, num_release)

        // if ( ++tape >= ntapes) { tape = 0; epoch = (epoch + 1) % GRO_NUM_EPOCHS }
        alu[ticket_tape, ticket_tape, +, 1]
        .if (ticket_tape >= ntapes)
            move(ticket_tape, 0)
            alu[epoch, epoch, +, 1]
            alu[epoch, epoch, AND, GRO_EPOCH_msk]
        .endif

        // Release ticket 0 of the next tape
        alu[$ticket, --, B, epoch, <<GRO_EPOCH_TICKET_SEQ_shf]
        alu[addr_lo, __GRO_CTX[GRO_CLICTX_BM_LO_wrd], AND~, GRO_CLICTX_BM_HI_msk, <<GRO_CLICTX_BM_HI_shf]
        alu[addr_lo, addr_lo, OR, ticket_tape, <<GRO_TICKET_TAPE_SIZE_shf]
release_cascaded_ticket#:
        mem[release_ticket, $ticket, addr_hi, <<8, addr_lo, 1], sig_done[ticket_sig]
        ctx_arb[ticket_sig]

        // Sanity:  should never happen
        br=byte[$ticket, 0, GRO_TICKET_ERROR, ticket_error_cascade#]

        // Number to actually release doesn't count ticket 0
        alu[num_release, $ticket, -, 1]

        // Sanity:  should never happen
        blt[tape_error#]

    .until (num_release < GRO_TICKET_PER_TAPE)

    // If there are no more tickets to release, we are done
    alu[--, --, B, num_release]
    beq[done#]

    // Release from the final ticket.
    br[final_release#]

    //
    // Error paths written to conserve instructions
    //
ticket_error_seq#:
    // addr_lo and addr_hi should still refer to the tape address
    // check for sequence skid marker
    _gro_cli_test_ticket_error(addr_hi, addr_lo, ticket_seq, epoch, ticket_error#)
    timestamp_sleep(GRO_SEQ_OVERFLOW_TICKS)
    br[release_first_ticket#]

ticket_error_cascade#:
    _gro_cli_test_cascade_error(addr_hi, addr_lo, epoch, cascade_error#)
    timestamp_sleep(GRO_SEQ_OVERFLOW_TICKS)
    br[release_cascaded_ticket#]

ticket_error#:
    // Duplicate ticket or way out of sequence
    move(addr_hi, 0x71CE7E44)
    local_csr_wr[MAILBOX3, in_seq]

error_loop#:
    local_csr_wr[MAILBOX0, addr_hi]
    local_csr_wr[MAILBOX1, in_ctx]
    local_csr_wr[MAILBOX2, ticket_tape]
    ctx_arb[bpt]
    br[error_loop#]                                ; NOT REACHED

cascade_error#:
    move(addr_hi, 0x7C4E1E44)
    local_csr_wr[MAILBOX3, 0]
    br[error_loop#]

tape_error#:
    move(addr_hi, 0x77BE4404)
    local_csr_wr[MAILBOX3, $ticket]
    br[error_loop#]

    //
    // Send last release message
    //
final_release#:
    _gro_cli_release(__GRO_CTX, in_ctx, num_release)

done#:

    #undef __GRO_CTX

.end
#endm


/**
 * Release a packet in context 'in_ctx' with sequence number
 * 'in_seq' and constructed metadata in transfer registers 'in_meta'.
 * This call will make use of local memory pointer 'LMPTR' (must be
 * in the range (0..3) to look up information about the reorder
 * context. So the caller should save this local memory pointer before
 * invoking this call if they wish to preserve it.
 *
 * @param in_ctx        The reorder context of the packet.
 * @param in_seq        The sequence number of the packet.
 * @param in_meta       Write transfer registers containing the
 *                      metadata of the packet built with gro_cli_build*
 * @param LMPTR         The index of the local memory pointer to use
 *                      to look up reorder context information.
 *                      (must be an integer).
 */
#macro gro_cli_send(in_ctx, in_seq, in_meta, LMPTR)
    _gro_cli_send(in_ctx, in_seq, in_meta, GRO_META_SIZE_LW, LMPTR)
#endm


#macro _gro_cli_build_mu_meta(out_xmeta, TYPE, in_isl, in_qnum, in_w0, in_w1, in_w2)
.begin
    .reg word
    .reg dest

    #define_eval __REFCNT 1
    move(out_xmeta[1], in_w0)

    #if (!streq('in_w1', '--'))

        move(out_xmeta[2], in_w1)

        #define_eval __REFCNT 2

        #if (!streq('in_w2', '--'))

            #define_eval __REFCNT 3
            move(out_xmeta[3], in_w2)

        #else

            move(out_xmeta[3], 0)

        #endif

    #else

        move(out_xmeta[2], 0)

    #endif

    // 0x80 == direct access locality: should always be fine with a ring op
    #if (is_ct_const(in_isl))
        move(word, (TYPE | ((in_isl | 0x80) << GRO_META_RINGHI_shf)))
    #else
        move(word, (TYPE | (0x80 << GRO_META_RINGHI_shf)))
        alu[word, word, OR, in_isl, <<GRO_META_RINGHI_shf]
    #endif

    #if (is_ct_const(in_qnum) && in_qnum > 255)
        .reg tmp
        move(tmp, in_qnum)
        alu[word, word, OR, tmp, <<GRO_META_MEM_RING_RINGLO_shf]
    #else
        alu[word, word, OR, in_qnum, <<GRO_META_MEM_RING_RINGLO_shf]
    #endif

    #if (TYPE == GRO_DTYPE_WORKQ)
        move(dest, (GRO_DEST_WORKQ_1WORD + __REFCNT - 1))
    #else
        move(dest, (GRO_DEST_MEM_RING_1WORD + __REFCNT - 1))
    #endif
    alu[out_xmeta[0], word, OR, dest, <<GRO_META_DEST_shf]

    #undef __REFCNT

.end
#endm



/**
 * Build reorder release metadata for a packet destined for an MU work queue.
 * This variant places 3 words on the work queue.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island of the memory unit containing the work queue
 * @param in_qnum       The queue array entry number of the work queue
 * @param in_w0         Word 0 to place on the workQ
 * @param in_w1         Word 1 to place on the workQ
 * @param in_w2         Word 2 to place on the workQ
 */
#macro gro_cli_build_workq_meta(out_xmeta, in_isl, in_qnum, in_w0, in_w1, in_w2)
    _gro_cli_build_mu_meta(out_xmeta, GRO_DTYPE_WORKQ, in_isl, in_qnum, in_w0, in_w1, in_w2)
#endm


/**
 * Build reorder release metadata for a packet destined for an MU work queue.
 * This variant places 2 words on the work queue.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island of the memory unit containing the work queue
 * @param in_qnum       The queue array entry number of the work queue
 * @param in_w0         Word 0 to place on the workQ
 * @param in_w1         Word 1 to place on the workQ
 */
#macro gro_cli_build_workq_meta(out_xmeta, in_isl, in_qnum, in_w0, in_w1)
    _gro_cli_build_mu_meta(out_xmeta, GRO_DTYPE_WORKQ, in_isl, in_qnum, in_w0, in_w1, --)
#endm


/**
 * Build reorder release metadata for a packet destined for an MU work queue.
 * This variant places 1 word on the work queue.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island of the memory unit containing the work queue
 * @param in_qnum       The queue array entry number of the work queue
 * @param in_w0         Word 0 to place on the workQ
 */
#macro gro_cli_build_workq_meta(out_xmeta, in_isl, in_qnum, in_w0)
    _gro_cli_build_mu_meta(out_xmeta, GRO_DTYPE_WORKQ, in_isl, in_qnum, in_w0, --, --)
#endm


/**
 * Build reorder release metadata for a packet destined for an MU ring.
 * This variant places 3 words on the ring.  The ring is assumed to not
 * overflow.  The caller must use some other flow control mechanism to ensure
 * this.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island of the memory unit containing the ring
 * @param in_qnum       The queue array entry number of the ring
 * @param in_w0         Word 0 to place on the ring
 * @param in_w1         Word 1 to place on the ring
 * @param in_w2         Word 2 to place on the ring
 */
#macro gro_cli_build_mem_ring_meta(out_xmeta, in_isl, in_qnum, in_w0, in_w1, in_w2)
    _gro_cli_build_mu_meta(out_xmeta, GRO_DTYPE_RING, in_isl, in_qnum, in_w0, in_w1, in_w2)
#endm


/**
 * Build reorder release metadata for a packet destined for an MU ring.
 * This variant places 2 words on the ring.  The ring is assumed to not
 * overflow.  The caller must use some other flow control mechanism to ensure
 * this.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island of the memory unit containing the ring
 * @param in_qnum       The queue array entry number of the ring
 * @param in_w0         Word 0 to place on the ring
 * @param in_w1         Word 1 to place on the ring
 */
#macro gro_cli_build_mem_ring_meta(out_xmeta, in_isl, in_qnum, in_w0, in_w1)
    _gro_cli_build_mu_meta(out_xmeta, GRO_DTYPE_RING, in_isl, in_qnum, in_w0, in_w1, --)
#endm


/**
 * Build reorder release metadata for a packet destined for an MU ring.
 * This variant places 1 word on the ring.  The ring is assumed to not
 * overflow.  The caller must use some other flow control mechanism to ensure
 * this.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island of the memory unit containing the ring
 * @param in_qnum       The queue array entry number of the ring
 * @param in_w0         Word 0 to place on the ring
 */
#macro gro_cli_build_mem_ring_meta(out_xmeta, in_isl, in_qnum, in_w0)
    _gro_cli_build_mu_meta(out_xmeta, GRO_DTYPE_RING, in_isl, in_qnum, in_w0, --, --)
#endm


/**
 * Build metadata for sending a packet out an NFP 6xxx NBI after reorder.
 *
 * @param out_xmeta     The location to put the output metadata
 * @param in_isl        The island containing the CTM buffer of the packet.
 * @param in_pktn       The packet number of the CTM buffer.
 * @param in_ctm_sz     The size of the CTM buffer (2 bits.  size = 2^(8+sz))
 * @param in_off        The starting offset of the packet modifier script.
 * @param in_len        The length of the packet starting from the BEGINNING
 *                      of the CTM buffer.
 * @param in_nbi        The NBI to send the packet out.
 * @param in_txq        The TXQ in the NBI to send the packet onto.
 */
#macro gro_cli_build_nbi_meta(out_xmeta, in_isl, in_pktn, in_ctm_sz, in_off, in_len, in_nbi, in_txq)
.begin

    .reg tmp
    .reg dm

    /*
     * See the NFP 6xxx Databook section 9.2.2.7.9 for the format of the various packet
     * processing complete fields.  This version sends directly to NBI.
     */

    /* Word 0 */
    move(tmp, GRO_DEST_IFACE_BASE)
    alu[tmp, tmp, +, in_nbi]
    alu[out_xmeta[GRO_META_TYPE_wrd], GRO_DTYPE_IFACE, OR, tmp, <<GRO_META_DEST_shf]

    /* Word 1 == addr_hi of the packet_complete_unicast */
    local_csr_rd[ACTIVE_CTX_STS]        // addr[31:28] = CTM Data master
    immed[tmp, 0]                       // Load balance these so odd and even
    alu[tmp, 1, AND, tmp, >>3]          // MEs use different ports.
    alu[tmp, tmp, +, 2]                 // DM == 2 + MEID & 1
    alu[tmp, --, B, tmp, <<20]          // addr[31:28] == addr_hi[23:20]
    alu[tmp, tmp, OR, in_nbi, <<30]     // addr[39:38] == NBI number
    alu[out_xmeta[GRO_META_NBI_ADDRHI_wrd], tmp, OR, in_isl, <<24] // addr[32:37] == CTM island

    /* Word 2 == addr_lo of the packet_complete_unicast */
    alu[out_xmeta[GRO_META_NBI_ADDRLO_wrd], in_len, OR, in_pktn, <<16]

    /*
     * Word 3:
     *
     * previous ALU for PPC must override:
     *  * Signal master from CSR0 (seq number)  (1 << 0)
     *
     *  * Data master from CSR0 (seq number)    (1 << 1)
     *    Data master island from CSR0 (seq number)
     *
     *  * Data reference (from prev alu DATA16, for TXQ and NBI) (1 << 3)
     *
     *  * Bytemask from CSR0 (TM sequencer)  (1 << 6)
     *
     *  * Length (from prev ALU length, for offset of rewrite script) (1 << 7)
     */
    #define_eval __NBI_IREF_LO8 ((1 << 0) | (1 << 1) | (1 << 3) | (1 << 6) | (1 << 7))

    // length = offset/8 - 1
    alu[tmp, --, B, in_off, >>3]
    alu[tmp, tmp, -, 1]

    alu[tmp, __NBI_IREF_LO8, OR, tmp, <<8]

    /*
     * The TXQ goes in data_ref[10;0] which is in [10;16] of the previous ALU.
     * The CTM size goes in data_ref[2;12] which is at [2;28] of the prev ALU.
     */
    alu[tmp, tmp, OR, in_txq, <<16]
    alu[out_xmeta[GRO_META_NBI_PALU_wrd], tmp, OR, in_ctm_sz, <<28]

    #undef __NBI_IREF_LO8

.end
#endm


/**
 * Build metadata to drop a packet sequence number from reorder processing.
 * A client ME should invoke this whenever they drop a packet that the GRO
 * subsystem was tracking, followed by a gro_cli_send().
 *
 * @param out_xmeta     The location to put the output metadata
 */
#macro gro_cli_build_drop_seq_meta(out_xmeta)
.begin

    immed[out_xmeta[0], (GRO_DTYPE_DROP_SEQ << GRO_META_TYPE_shf)]
    immed[out_xmeta[1], 0]
    immed[out_xmeta[2], 0]
    immed[out_xmeta[3], 0]

.end
#endm


/**
 * A convenience macro to drop a packet sequence number from reorder
 * processing.  A client ME should invoke this whenever they drop a packet
 * that the GRO subsystem was tracking.
 *
 * Equivalient to:
 *    gro_cli_build_drop_seq_meta($meta)
 *    gro_cli_send(ctx, seq, $meta, LMPTR)
 *
 * @param in_ctx        The reorder context of the sequence number.
 * @param in_seq        The sequence number to drop.
 * @param LMPTR         The local memory pointer to use to look up
 *                      the reorder context information.  (must
 *                      be an integer).
 */
#macro gro_cli_drop_seq(in_ctx, in_seq, LMPTR)
.begin

    // No need to write the entire metadata: 1 word type will do
    .reg $drop_meta[1]
    .xfer_order $drop_meta

    immed[$drop_meta[0], (GRO_DTYPE_DROP_SEQ << GRO_META_TYPE_shf)]
    _gro_cli_send(in_ctx, in_seq, $drop_meta, 1, LMPTR)

.end
#endm


#ifndef NFD_USE_MOCKUP


/**
 * Convert NFD descriptor to GRO metadata to send a packet to NFD.
 */
// TODO:  Use real offsets and add thorough documentation of what we're doing
// here...
#macro gro_cli_nfd_desc2meta(out_grod, in_nfdd, in_pcie, in_qid)
.begin

    .reg word
    .reg off
    .reg mask

    move(word, in_nfdd[0])
    // Store even 8-bit offset in 7-bit field
    alu[off, 127, AND, word, >>1]
    move(mask, 0x3FFF)
    alu[word, word, AND~, mask]
    alu[off, GRO_DTYPE_NFD, OR, off, <<GRO_META_W0_META_START_BIT]
    #if (!is_ct_const(in_pcie) || (in_pcie != 0))
        alu[off, off, OR, in_pcie, <<GRO_META_DEST_shf]
    #endif
    alu[out_grod[0], off, OR, word]

    move(out_grod[1], in_nfdd[1])
    alu[word, in_nfdd[2], AND~, 0xFF, <<16]
    #ifdef GRO_EVEN_NFD_OFFSETS_ONLY
        alu[out_grod[2], word, OR, in_qid, <<16]
    #else
        alu[word, word, OR, in_qid, <<16]
        alu[word, word, AND~, 1, <<31]
        alu[out_grod[2], word, OR, in_nfdd[0], <<31]
    #endif
    move(out_grod[3], in_nfdd[3])

.end
#endm


#else /* NFD_USE_MOCKUP */

/**
 * Build metadata to send a packet to NFD.
 */
#macro gro_cli_build_nfd_meta(out_arr, in_pci, in_qid, in_flags, in_pktnum, \
                              in_ctm_isl, in_cbs, in_bls, in_mu_addr, in_off,\
                              in_len, in_meta_len, in_vlan)
.begin

    .reg word
    .reg tmp

    // XXX use official definitions
    alu[word, --, B, in_mu_addr, >>(40 - 11 - 8)]
    alu[word, word, OR, in_off, <<8]
    alu[word, word, OR, in_pktnum, <<16]
    alu[out_arr[1], word, OR, in_ctm_isl, <<26]

    alu[out_arr[2], --, B, in_mu_addr, <<11]

    alu[word, in_len, OR, 1, <<31]
    alu[word, word, OR, in_meta_len, <<16]
    alu[out_arr[3], word, OR, 64, <<24]

    alu[word, GRO_DTYPE_NFD, OR, in_vlan, <<16]
    alu[word, word, OR, in_qid, <<3]
    alu[word, word, OR, in_pci, <<9]
    alu[out_arr[0], word, OR, in_flags, <<13]

.end
#endm


/**
 * Build metadata to send a packet to NFD.
 */
#macro gro_cli_nfd_desc2meta(out_arr, in_meta, in_pcie, in_qid)
.begin

    .reg word
    .reg tmp

    // XXX use official definitions
    move(tmp, in_pcie)                          // XXX copy pci
    alu[word, GRO_DTYPE_NFD, OR, tmp, <<3]
    alu[tmp, 0x3F, AND, in_meta[3], >>8]        // XXX copy queue
    alu[word, word, OR, tmp, <<7]
    ld_field[word, 1100, in_meta[3]]            // XXX copy vlan
    alu[tmp, in_meta[3], AND, 0x7]              // XXX copy flags
    alu[out_arr[0], word, OR, tmp, <<13]
    alu[out_arr[1], --, B, in_meta[0]]
    alu[out_arr[2], --, B, in_meta[1]]
    alu[out_arr[3], --, B, in_meta[2]]

.end
#endm


#endif /* NFD_USE_MOCKUP */



#macro gro_cli_build_drop_ctm_buf_meta(out_xmeta, in_isl, in_pkt_num)
.begin

    .reg tmp

    immed[out_xmeta[0], (GRO_DTYPE_DROP_CTM_BUF << GRO_META_TYPE_shf)]

    // 0x80 == direct access locality: required for mem[packet_free...]
    #if (is_ct_const(in_isl) || is_rt_const(in_isl))
        move(tmp, (in_isl | 0x80))
    #else
        alu[tmp, 0x80, OR, in_isl]
    #endif
    alu[out_xmeta[GRO_META_DROP_CTMHI_wrd], in_pkt_num, OR, tmp, <<GRO_META_DROP_CTMHI_shf]

.end
#endm


#macro gro_cli_drop_ctm_buf(in_ctx, in_seq, in_isl, in_pkt_num, LMPTR)
.begin

    .reg $drop_meta[2]
    .xfer_order $drop_meta

    gro_cli_build_drop_ctm_buf_meta($drop_meta, in_isl, in_pkt_num)
    _gro_cli_send(in_ctx, in_seq, $drop_meta, 2, LMPTR)

.end
#endm


#macro gro_cli_build_drop_mu_buf_meta(out_xmeta, in_ring_hi, in_ring_num, in_buf_handle)
.begin

    move(out_xmeta[0], (GRO_DTYPE_DROP_MU_BUF << GRO_META_TYPE_shf))

    move(out_xmeta[1], 0)

    #if (isnum(in_ring_hi) && isnum(in_ring_num))
        move(out_xmeta[GRO_META_DROP_RINGNUM_wrd],
             ((in_ring_hi << GRO_META_DROP_RINGHI_shf) |
              (in_ring_num << GRO_META_DROP_RINGNUM_shf)))
    #else
        alu[out_xmeta[GRO_META_DROP_RINGNUM_wrd], in_ring_num, OR, in_ring_hi,
            <<GRO_META_DROP_RINGHI_shf]
    #endif

    move(out_xmeta[GRO_META_DROP_BUFH_wrd], in_buf_handle)

.end
#endm


#macro gro_cli_drop_mu_buf(in_ctx, in_seq, in_ring_hi, in_ring_num, in_buf_handle, LMPTR)
.begin

    .reg $drop_meta[4]
    .xfer_order $drop_meta

    gro_cli_build_drop_mu_buf_meta($drop_meta, in_ring_hi, in_ring_num, in_buf_handle)
    _gro_cli_send(in_ctx, in_seq, $drop_meta, 4, LMPTR)

.end
#endm


#endif /* __GRO_NFP6000_UC */
