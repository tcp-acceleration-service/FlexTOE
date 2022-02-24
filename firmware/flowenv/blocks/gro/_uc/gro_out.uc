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
 * @file          me/blocks/gro/_uc/gro_out.uc
 */

#ifndef __GRO_OUT_UC
#define __GRO_OUT_UC

#include <stdmac.uc>
#include <timestamp.uc>
#include <gro.uc>
#include <gro_cfg.uc>

#ifdef GRO_DEBUG
#include <journal.uc>
#endif

#ifndef GRO_RELEASE_RING_TYPE
#define GRO_RELEASE_RING_TYPE MEMRING
#endif

#ifndef GRO_RELEASE_RING_SIZE
#define GRO_RELEASE_RING_SIZE 2048
#endif


/* XXX Get this from somewhere else? */
#define NFP6000_EMEM_ISL_BASE   24


#define GRO_OUT_DISP_CONTEXT    7
#define GRO_OUT_NUM_WORKERS     7
#define GRO_OUT_FIRST_WORKER    0
#define GRO_OUT_LAST_WORKER     (GRO_OUT_FIRST_WORKER + GRO_OUT_NUM_WORKERS - 1)
#define GRO_OUT_NUM_DEST        (GRO_NUM_NFD_DEST + GRO_NUM_IFACE_DEST +\
                                 GRO_NUM_WORKQ_DEST + GRO_NUM_MEM_RING_DEST)
#define GRO_OUT_XMIT_LEN        4
#define GRO_OUT_XBUF_PER_WORKER 3

#define GRO_OUT_HANDLER_ILEN    64
#define GRO_OUT_HANDLER_shf     (log2(GRO_OUT_HANDLER_ILEN))


// Needed by both the dispatcher and worker context
#define LM_XBUF_CSR             ACTIVE_LM_ADDR_0
#define LM_XBUF_PTR             *l$index0
#define LM_XBUF_FLAGS_wrd       0               /* flags for this entry */
#define LM_XBUF_PALU_wrd        1               /* PREV_ALU for dispatcher iref */
#define LM_XBUF_CSR0_wrd        2               /* CSR0 for dispatcher iref */
#define LM_XBUF_SIGMASK_wrd     3               /* sigmask for next worker buf */
#define LM_XBUF_DNEXT_wrd       4               /* next pointer for dispatcher */
#define LM_XBUF_WNEXT_wrd       5               /* next pointer for worker */
#define LM_XBUF_SIZE_LW         8
#define LM_XBUF_SIZE            (LM_XBUF_SIZE_LW << 2)
#define LM_XBUF_shf             (log2(LM_XBUF_SIZE))
#define LM_XBUF_TOTAL_SIZE      (LM_XBUF_SIZE * GRO_OUT_NUM_WORKERS * GRO_OUT_XBUF_PER_WORKER)
#define LM_XBUF_FLAGS           LM_XBUF_PTR[LM_XBUF_FLAGS_wrd]
#define LM_XBUF_PALU            LM_XBUF_PTR[LM_XBUF_PALU_wrd]
#define LM_XBUF_CSR0            LM_XBUF_PTR[LM_XBUF_CSR0_wrd]
#define LM_XBUF_SIGMASK         LM_XBUF_PTR[LM_XBUF_SIGMASK_wrd]
#define LM_XBUF_DNEXT           LM_XBUF_PTR[LM_XBUF_DNEXT_wrd]
#define LM_XBUF_WNEXT           LM_XBUF_PTR[LM_XBUF_WNEXT_wrd]

#define XBUF_FLAG_READY         0
#define XBUF_FLAG_BUSY          1
#define XBUF_FLAG_BUSY_bit      0
#define XBUF_FLAG_TWOPKTS       2
#define XBUF_FLAG_TWOPKTS_bit   1


// Workers can skip the pointer cycle in the NFP6000
// by having a separate LM context for each one.
#define LM_XBUF_CSR_0   ACTIVE_LM_ADDR_0
#define LM_XBUF_PTR_0   *l$index0
#define LM_XBUF_CSR_1   ACTIVE_LM_ADDR_1
#define LM_XBUF_PTR_1   *l$index1
#define LM_XBUF_CSR_2   ACTIVE_LM_ADDR_2
#define LM_XBUF_PTR_2   *l$index2

#define LM_XBUF_SIZE_PER_WORKER (GRO_OUT_XBUF_PER_WORKER * LM_XBUF_SIZE)



/* only needed by the dispatcher context */
#define LM_CTX_CSR              ACTIVE_LM_ADDR_1
#define LM_CTX_PTR              *l$index1
#define LM_CTX_Q_ADDRHI_wrd     0       /* hi 8 bits for ROQ mem */
#define LM_CTX_Q_BASE_wrd       1       /* base of ROQ mem */
#define LM_CTX_Q_ADDRLO_wrd     2       /* next pointer for ROQ */
#define LM_CTX_Q_BM_RESET_wrd   3       /* ROQ addr to do a bitmap reset */
#define LM_CTX_Q_END_wrd        4       /* end of ROQ addr (entry past end) */
#define LM_CTX_RST_EPOCH_wrd    5       /* epoch of the reset */
#define LM_CTX_BM_ADDRHI_wrd    6       /* hi 8 bits for BM mem */
#define LM_CTX_BM_BASE_wrd      7       /* low 32 bits for BM mem */
#define LM_CTX_BM_NEXT_wrd      8       /* low 32 bits for next BM mem to reset*/
#define LM_CTX_BM_END_wrd       9       /* low 32 bits for BM mem to reset*/
#define LM_CTX_SIZE_LW          16
#define LM_CTX_SIZE             (LM_CTX_SIZE_LW << 2)
#define LM_CTX_shf              (log2(LM_CTX_SIZE))
#define LM_CTX_TOTAL_SIZE       (LM_CTX_SIZE * GRO_CTX_PER_BLOCK)
#define LM_CTX_Q_ADDRHI         LM_CTX_PTR[LM_CTX_Q_ADDRHI_wrd]
#define LM_CTX_Q_BASE           LM_CTX_PTR[LM_CTX_Q_BASE_wrd]
#define LM_CTX_Q_ADDRLO         LM_CTX_PTR[LM_CTX_Q_ADDRLO_wrd]
#define LM_CTX_Q_BM_RESET       LM_CTX_PTR[LM_CTX_Q_BM_RESET_wrd]
#define LM_CTX_Q_END            LM_CTX_PTR[LM_CTX_Q_END_wrd]
#define LM_CTX_RST_EPOCH        LM_CTX_PTR[LM_CTX_RST_EPOCH_wrd]
#define LM_CTX_BM_ADDRHI        LM_CTX_PTR[LM_CTX_BM_ADDRHI_wrd]
#define LM_CTX_BM_BASE          LM_CTX_PTR[LM_CTX_BM_BASE_wrd]
#define LM_CTX_BM_NEXT          LM_CTX_PTR[LM_CTX_BM_NEXT_wrd]
#define LM_CTX_BM_END           LM_CTX_PTR[LM_CTX_BM_END_wrd]



#define LM_DEST_CSR             ACTIVE_LM_ADDR_3
#define LM_DEST_PTR             *l$index3

#define LM_DEST_DATA0_wrd       0               /* type-specific data */
#define LM_DEST_DATA1_wrd       1               /* type-specific data */
#define LM_DEST_SIZE_LW         2
#define LM_DEST_SIZE            (LM_DEST_SIZE_LW << 2)
#define LM_DEST_shf             (log2(LM_DEST_SIZE))
#define LM_DEST_TOTAL_SIZE      (LM_DEST_SIZE * GRO_OUT_NUM_DEST)
#define LM_DEST_DATA0           LM_DEST_PTR[LM_DEST_DATA0_wrd]
#define LM_DEST_DATA1           LM_DEST_PTR[LM_DEST_DATA1_wrd]

#ifdef NO_DRAM
    #define NO_DDR_0
    #define NO_DDR_1
    #define NO_DDR_2
    #define emem_cache_upper emem
#endif

#ifdef NO_DDR_0
    #define emem0_cache_upper emem0
    .init_csr xpbm:ExtMu0IsldXpbmMap.Island.ExtMuXpbMap.MuConfigReg.ConfigCPP.DirAccWays 0xff required
#endif

#ifdef NO_DDR_1
    #define emem1_cache_upper emem1
    .init_csr xpbm:ExtMu1IsldXpbmMap.Island.ExtMuXpbMap.MuConfigReg.ConfigCPP.DirAccWays 0xff required
#endif

#ifdef NO_DDR_2
    #if (__nfp_has_island(26))
        #define emem2_cache_upper emem2
        .init_csr xpbm:ExtMu2IsldXpbmMap.Island.ExtMuXpbMap.MuConfigReg.ConfigCPP.DirAccWays 0xff required
    #endif
#endif

/**
 * Destination data needed by type:
 *
 *  - NFD3:
 *    + pci num
 *    + queue num
 *    + credits (per queue)
 *    + Island/ring number (per PCIe: may be inferrable)
 *
 *  - IFACE:
 *    + CSR0 - sequencer & next sequence number
 *
 *  - DROP:
 *    + none
 *
 *  - RING:
 *    + Island/Ring number
 */

#define LM_DEST_NBI_CSR0        LM_DEST_PTR[GRO_DEST_NBI_CSR0_wrd]

/* NFP6000 NFD3 */
#define LM_DEST_NFD3_CREDITS    LM_DEST_PTR[GRO_DEST_NFD3_CREDITS_wrd]
#define LM_DEST_NFD3_RING_ENC   LM_DEST_PTR[GRO_DEST_NFD3_RING_ENC_wrd]

#define LM_DEST_MEM_RING_IREF   LM_DEST_PTR[GRO_DEST_MEM_RING_IREF_wrd]


#define _gro_override_refcnt(NWORDS) ((1 << 7) | ((NWORDS-1) << 8))


#macro gro_out_declare(BLOCKNUM)
.begin

    .init_nn_mode neighbor
    .init_csr mecsr:NNGet 0
    .init_csr mecsr:NNPut 0

    .alloc_mem gro_out_lm_dest lmem me LM_DEST_TOTAL_SIZE GRO_CLP2(LM_DEST_TOTAL_SIZE)

    .alloc_mem gro_out_lm_ctx lmem me LM_CTX_TOTAL_SIZE GRO_CLP2(LM_CTX_TOTAL_SIZE)

    .alloc_mem gro_out_lm_xbuf lmem me LM_XBUF_TOTAL_SIZE GRO_CLP2(LM_XBUF_TOTAL_SIZE)

    gro_declare_block(BLOCKNUM)

    gro_config_block(BLOCKNUM, GRO_OUT)

    #define_eval __ADDR (GRO_DEST_WORKQ_1WORD * GRO_DEST_SIZE)
    .init gro_out_lm_dest+__ADDR _gro_override_refcnt(1)

    #define_eval __ADDR (GRO_DEST_WORKQ_2WORD * GRO_DEST_SIZE)
    .init gro_out_lm_dest+__ADDR _gro_override_refcnt(2)

    #define_eval __ADDR (GRO_DEST_WORKQ_3WORD * GRO_DEST_SIZE)
    .init gro_out_lm_dest+__ADDR _gro_override_refcnt(3)

    #define_eval __ADDR (GRO_DEST_MEM_RING_1WORD * GRO_DEST_SIZE)
    .init gro_out_lm_dest+__ADDR _gro_override_refcnt(1)

    #define_eval __ADDR (GRO_DEST_MEM_RING_2WORD * GRO_DEST_SIZE)
    .init gro_out_lm_dest+__ADDR _gro_override_refcnt(2)

    #define_eval __ADDR (GRO_DEST_MEM_RING_3WORD * GRO_DEST_SIZE)
    .init gro_out_lm_dest+__ADDR _gro_override_refcnt(3)

.end
#endm


#macro gro_start_meta_read(NWORDS)
.begin
    .reg addr_hi

    alu[addr_hi, --, B, LM_CTX_Q_ADDRHI]
    local_csr_wr[CMD_INDIRECT_REF_0, LM_XBUF_CSR0]
    alu[--, --, B, LM_XBUF_PALU]
    mem[read, $, addr_hi, <<8, LM_CTX_Q_ADDRLO, (NWORDS/2)], indirect_ref

.end
#endm


#macro reset_next_bitmap()
.begin
    .reg addr_lo
    .reg qaddr
    .reg tmp

    .reg write $xrst[GRO_TICKET_TAPE_SIZE_LW]
    .xfer_order $xrst
    .sig rst_sig

    /* first adjust LM_CTX_Q_BM_RESET.  This value ranges from
     *   LM_CTX_Q_BASE + (GRO_META_SIZE * GRO_TICKET_PER_TAPE)
     * to
     *   LM_CTX_Q_BASE + (GRO_META_SIZE * GRO_TICKET_PER_TAPE) * Qsize
     */
    alu[qaddr, --, B, LM_CTX_Q_BM_RESET]
    alu[--, qaddr, -, LM_CTX_Q_END]
    bne[set_bm_reset_addr#]
    move(qaddr, LM_CTX_Q_BASE)
set_bm_reset_addr#:
    move(tmp, (GRO_META_SIZE * GRO_TICKET_PER_TAPE))
    alu[LM_CTX_Q_BM_RESET, qaddr, +, tmp]

    // release sequence number 0 of the next release epoch
    br=byte[LM_CTX_RST_EPOCH, 0, 1, reset_epoch_1#], defer[2]
    move(addr_lo, LM_CTX_BM_NEXT)
    alu[LM_CTX_BM_NEXT, LM_CTX_BM_NEXT, +, GRO_TICKET_TAPE_SIZE]
    br=byte[LM_CTX_RST_EPOCH, 0, 2, reset_epoch_2#]
    br=byte[LM_CTX_RST_EPOCH, 0, 3, reset_epoch_3#]

    // See NFP-6XXX Databook section 9.2.2.1.2.10 for bitmap structure

    // We know that these non-signalled writes are safe.  The xfer registers
    // are fixed for the dispatcher thread.
    #pragma warning(disable:5117)

reset_epoch_0#:
    mem[write_atomic, $xreset_epoch_0[0], LM_CTX_BM_ADDRHI, <<8, addr_lo, GRO_TICKET_TAPE_SIZE_LW]
    br[test_bm_end#]

reset_epoch_1#:
    mem[write_atomic, $xreset_epoch_1[0], LM_CTX_BM_ADDRHI, <<8, addr_lo, GRO_TICKET_TAPE_SIZE_LW]
    br[test_bm_end#]

reset_epoch_2#:
    mem[write_atomic, $xreset_epoch_2[0], LM_CTX_BM_ADDRHI, <<8, addr_lo, GRO_TICKET_TAPE_SIZE_LW]
    br[test_bm_end#]

reset_epoch_3#:
    mem[write_atomic, $xreset_epoch_3[0], LM_CTX_BM_ADDRHI, <<8, addr_lo, GRO_TICKET_TAPE_SIZE_LW]

    // Re-enable the warnings for the rest of the code.
    #pragma warning(default:5117)

    // if tape pointer not at the end of the ticket tape space, then done
test_bm_end#:
    move(addr_lo, LM_CTX_BM_NEXT)
    alu[--, addr_lo, -, LM_CTX_BM_END]
    bne[done#]

    // else reset tape pointer and increment the epoch for future resets
    move(LM_CTX_BM_NEXT, LM_CTX_BM_BASE)
    alu[LM_CTX_RST_EPOCH, LM_CTX_RST_EPOCH, +, 1]
    alu[LM_CTX_RST_EPOCH, LM_CTX_RST_EPOCH, AND, GRO_EPOCH_msk]

done#:
.end
#endm


#macro gro_out_dispatch(in_reg, in_reg2, TEST_INPUT_MACRO, NEXT_INPUT_LABEL, NO_INPUT_LABEL)
.begin
    .reg lma
    .reg nrel
    .reg tmp
    .reg new_addr_lo

    #ifdef GRO_DEBUG
        .reg jval
    #endif

    #if (GRO_REL_CTX_shf != 0 || GRO_REL_NREL_shf < 12)

        #error "GRO_REL_CTX_shf must be 0 and GRO_REL_NREL_shf >= 12 for optimization to work"

    #endif

    #if (!streq('TEST_INPUT_MACRO', '--'))

        TEST_INPUT_MACRO(in_reg, NO_INPUT_LABEL)

    #endif

    // OPTIMIZE: make the client shift the context by LM_CTX_shf?
    // Requires assiging gro_out_lm_ctx to a fixed, known address.
    alu[lma, g_ctx_lm_base, OR, in_reg, <<LM_CTX_shf]
    local_csr_wr[LM_CTX_CSR, lma]
    alu[nrel, GRO_REL_NREL_msk, AND, in_reg2, >>GRO_REL_NREL_shf]

    #ifdef GRO_DEBUG
        alu[jval, in_reg, AND~, 0xF, <<28]
        alu[jval, jval, OR, GRO_BLOCK_NUM, <<28]
        journal(debug, jval)
    #endif

dispatch_release_loop#:
    alu[nrel, nrel, -, 2]
    bge[release_two#]
    // Fall through: fast path

    // Read one packet into the next xbuf
release_one#:
    br_bset[LM_XBUF_FLAGS, XBUF_FLAG_BUSY_bit, release_one_sender_wait#]
    gro_start_meta_read(GRO_META_SIZE_LW)
    alu[LM_CTX_Q_ADDRLO, LM_CTX_Q_ADDRLO, +, 16]
    br=byte[LM_CTX_Q_ADDRLO, 0, 0, release_one_test_addrlo_last#]
    br[NEXT_INPUT_LABEL], defer[2]
    local_csr_wr[LM_XBUF_CSR, LM_XBUF_DNEXT]
    alu[LM_XBUF_FLAGS, LM_XBUF_FLAGS, OR, XBUF_FLAG_BUSY]

release_one_sender_wait#:
    ctx_arb[voluntary], br[release_one#]

release_one_test_addrlo_last#:
    alu[new_addr_lo, --, B, LM_CTX_Q_ADDRLO]
    alu[--, new_addr_lo, -, LM_CTX_Q_BM_RESET]
    bne[NEXT_INPUT_LABEL], defer[2]
    local_csr_wr[LM_XBUF_CSR, LM_XBUF_DNEXT]
    alu[LM_XBUF_FLAGS, LM_XBUF_FLAGS, OR, XBUF_FLAG_BUSY]

    reset_next_bitmap()

    alu[--, new_addr_lo, -, LM_CTX_Q_END]
    bne[NEXT_INPUT_LABEL]
    br[NEXT_INPUT_LABEL], defer[1]
    alu[LM_CTX_Q_ADDRLO, --, B, LM_CTX_Q_BASE]


    // Read 2 packets into the next xbuf
release_two_sender_wait#:
    ctx_arb[voluntary]

release_two#:
    br_bset[LM_XBUF_FLAGS, XBUF_FLAG_BUSY_bit, release_two_sender_wait#]

    alu[new_addr_lo,  LM_CTX_Q_ADDRLO, +, 32]
    alu[--, new_addr_lo, -, LM_CTX_Q_BM_RESET]
    bge[release_two_bm_reset#]

resume_release_two_no_q_addr_reset#:
    gro_start_meta_read((GRO_META_SIZE_LW * 2))
    br!=byte[nrel, 0, 0, dispatch_release_loop#], defer[3]
    alu[LM_CTX_Q_ADDRLO, LM_CTX_Q_ADDRLO, +, 32]
    local_csr_wr[LM_XBUF_CSR, LM_XBUF_DNEXT]
    alu[LM_XBUF_FLAGS, LM_XBUF_FLAGS, OR, (XBUF_FLAG_BUSY | XBUF_FLAG_TWOPKTS)]

    br[NEXT_INPUT_LABEL]
    ; cycle1
    ; cycle2


    // Slow path:  release of 2 runs to or past the end of the release queue
release_two_bm_reset#:
    reset_next_bitmap()

    alu[--, new_addr_lo, -, LM_CTX_Q_END]
    blt[resume_release_two_no_q_addr_reset#]

overflow#:
    bgt[overflow_wrap#]

    // Packet exactly hits the end of the release queue
    // Release 2 packets, then reset the queue pointer before going on.
    gro_start_meta_read((GRO_META_SIZE_LW * 2))
    br!=byte[nrel, 0, 0, dispatch_release_loop#], defer[3]
    alu[LM_CTX_Q_ADDRLO, --, B, LM_CTX_Q_BASE]
    local_csr_wr[LM_XBUF_CSR, LM_XBUF_DNEXT]
    alu[LM_XBUF_FLAGS, LM_XBUF_FLAGS, OR, (XBUF_FLAG_BUSY | XBUF_FLAG_TWOPKTS)]

    br[NEXT_INPUT_LABEL]

    // Slow path:  release of 2 would run past the end of the release queue
    // Release 1 packet, reset queue pointer and then release 1 more.
overflow_wrap#:
    gro_start_meta_read(GRO_META_SIZE_LW)
    local_csr_wr[LM_XBUF_CSR, LM_XBUF_DNEXT]
    alu[LM_CTX_Q_ADDRLO, --, B, LM_CTX_Q_BASE]                  // cycle 1
    alu[LM_XBUF_FLAGS, LM_XBUF_FLAGS, OR, XBUF_FLAG_BUSY]       // cycle 2
    nop                                                         // cycle 3

    // Wait for the next xbuf to be ready
overflow_wrap_wait#:
    br_bclr[LM_XBUF_FLAGS, XBUF_FLAG_BUSY_bit, overflow_wrap_continue#]
    ctx_arb[voluntary], br[overflow_wrap_wait#]

overflow_wrap_continue#:
    gro_start_meta_read(GRO_META_SIZE_LW)
    br!=byte[nrel, 0, 0, dispatch_release_loop#], defer[3]
    local_csr_wr[LM_XBUF_CSR, LM_XBUF_DNEXT]
    alu[LM_CTX_Q_ADDRLO, LM_CTX_Q_ADDRLO, +, 16]
    alu[LM_XBUF_FLAGS, LM_XBUF_FLAGS, OR, XBUF_FLAG_BUSY]

    br[NEXT_INPUT_LABEL]

.end
#endm


#macro _gro_test_no_input_nn(in_reg, NO_INPUT_LABEL)

    br_inp_state[NN_EMPTY, NO_INPUT_LABEL]

#endm


#macro gro_out_dispatcher_nn()
.begin
    .reg volatile g_ctx_lm_base
    .reg addr

    /* start at the first xbuf pointer */
    move(addr, gro_out_lm_xbuf)
    local_csr_wr[LM_XBUF_CSR, addr]

    move(g_ctx_lm_base, gro_out_lm_ctx)

    br[dispatch_top#]

no_nn_messages#:
    ctx_arb[voluntary]

dispatch_top#:
    gro_out_dispatch(*n$index, *n$index++, _gro_test_no_input_nn, dispatch_top#,
                     no_nn_messages#)

.end
#endm


/**
 * Sleep for a while when we get an empty ring request.
 *
 * If GRO.OUT is aiming for processing at 40Mpps per block, then there are
 * 30 cycles per packet at 1.2 Ghz.  If we assume a worst case where each
 * packet gets a separate release (complete in-order processing), then 240
 * cycles should let 8 requests queue up, which is as many as we can pull
 * in at once.  So ... fudge it down to to allow for latency of the
 * request itself.
 */
#macro _gro_out_dispatcher_backoff()
    timestamp_sleep((200 / 16))
#endm


#macro _gro_test_no_input_mem_ring(in_reg, NO_INPUT_LABEL)

    br_bclr[in_reg, GRO_REL_VALID_bit, NO_INPUT_LABEL]

#endm


#define GRO_OUT_GETSAFE_NUM     8

#macro gro_out_dispatcher_mem_ring(BLOCKNUM)
.begin

    // used during setup only
    .reg lmaddr

    // These are used throughout the lifetime of the thread
    .reg volatile g_ctx_lm_base

    .reg volatile ring_lo
    .reg volatile ring_hi

    .reg read volatile $mem_ring0[GRO_OUT_GETSAFE_NUM]
    .xfer_order $mem_ring0
    .sig volatile mem_ring0_sig

    .reg read volatile $mem_ring1[GRO_OUT_GETSAFE_NUM]
    .xfer_order $mem_ring1
    .sig volatile mem_ring1_sig

    .reg write volatile $xreset_epoch_0[GRO_TICKET_TAPE_SIZE_LW]
    .xfer_order $xreset_epoch_0

    .reg write volatile $xreset_epoch_1[GRO_TICKET_TAPE_SIZE_LW]
    .xfer_order $xreset_epoch_1

    .reg write volatile $xreset_epoch_2[GRO_TICKET_TAPE_SIZE_LW]
    .xfer_order $xreset_epoch_2

    .reg write volatile $xreset_epoch_3[GRO_TICKET_TAPE_SIZE_LW]
    .xfer_order $xreset_epoch_3

    #define_eval __ISL (gro_release_isl_/**/BLOCKNUM)
    #define_eval __EMEM (__ISL - NFP6000_EMEM_ISL_BASE)
    .alloc_mem gro_release_ring_mem_/**/BLOCKNUM \
        emem/**/__EMEM/**/_cache_upper global \
        GRO_RELEASE_RING_SIZE GRO_RELEASE_RING_SIZE
    .init_mu_ring gro_release_ring_/**/BLOCKNUM gro_release_ring_mem_/**/BLOCKNUM

    // Initialize reset transfer registers (write-once, use many)
    // Epoch 0
    alu[$xreset_epoch_0[0], --, B, 0, <<GRO_EPOCH_TICKET_SEQ_shf]
    move($xreset_epoch_0[1], 0)
    move($xreset_epoch_0[2], 0)
    move($xreset_epoch_0[3], 0)
    // Epoch 1
    alu[$xreset_epoch_1[0], --, B, 1, <<GRO_EPOCH_TICKET_SEQ_shf]
    move($xreset_epoch_1[1], 0)
    move($xreset_epoch_1[2], 0)
    move($xreset_epoch_1[3], 0)
    // Epoch 2
    alu[$xreset_epoch_2[0], --, B, 2, <<GRO_EPOCH_TICKET_SEQ_shf]
    move($xreset_epoch_2[1], 0)
    move($xreset_epoch_2[2], 0)
    move($xreset_epoch_2[3], 0)
    // Epoch 3
    alu[$xreset_epoch_3[0], --, B, 3, <<GRO_EPOCH_TICKET_SEQ_shf]
    move($xreset_epoch_3[1], 0)
    move($xreset_epoch_3[2], 0)
    move($xreset_epoch_3[3], 0)

    // start at the first xbuf pointer
    move(lmaddr, gro_out_lm_xbuf)
    local_csr_wr[LM_XBUF_CSR, lmaddr]

    move(g_ctx_lm_base, gro_out_lm_ctx)

    move(ring_lo, gro_release_ring_/**/BLOCKNUM)
    move(ring_hi, ((gro_release_isl_/**/BLOCKNUM | 0x80) << 24))

    local_csr_wr[SAME_ME_SIGNAL,  ((&ordersig << 3) | (GRO_OUT_FIRST_WORKER << 0))]

    mem[get_safe, $mem_ring0[0], ring_hi, <<8, ring_lo, GRO_OUT_GETSAFE_NUM], sig_done[mem_ring0_sig]
    br[wait_mem_ring0_input#]

    // MAIN LOOP
    // Keep 1 get_safe in flight while processing the next
mem_ring0_last_no_input#:
    #ifdef GRO_DEBUG
        alu[@gro_idle, @gro_idle, +, 1]
        local_csr_wr[MAILBOX1, @gro_idle]
    #endif /* GRO_DEBUG */

wait_mem_ring0_input#:
    ctx_arb[mem_ring0_sig]

    mem[get_safe, $mem_ring1[0], ring_hi, <<8, ring_lo, 8], sig_done[mem_ring1_sig]

    #define_eval __LOOP 0
    #while (__LOOP < GRO_OUT_GETSAFE_NUM - 1)
        #define_eval __NEXT_LOOP (__LOOP + 1)

        mem_ring0_input/**/__LOOP#:
            #if (__LOOP == 0)
                gro_out_dispatch($mem_ring0[__LOOP], $mem_ring0[__LOOP],
                                 _gro_test_no_input_mem_ring,
                                 mem_ring0_input/**/__NEXT_LOOP#, mem_ring1_last_no_input#)
            #else
                gro_out_dispatch($mem_ring0[__LOOP], $mem_ring0[__LOOP],
                                 _gro_test_no_input_mem_ring,
                                 mem_ring0_input/**/__NEXT_LOOP#, wait_mem_ring1_input#)
            #endif

        #define_eval __LOOP (__NEXT_LOOP)
    #endloop

    mem_ring0_input/**/__LOOP#:
        gro_out_dispatch($mem_ring0[__LOOP], $mem_ring0[__LOOP],
                         _gro_test_no_input_mem_ring,
                         wait_mem_ring1_input#, wait_mem_ring1_input#)


mem_ring1_last_no_input#:
    #ifdef GRO_DEBUG
        alu[@gro_idle, @gro_idle, +, 1]
        local_csr_wr[MAILBOX1, @gro_idle]
    #endif /* GRO_DEBUG */

wait_mem_ring1_input#:
    ctx_arb[mem_ring1_sig]

    mem[get_safe, $mem_ring0[0], ring_hi, <<8, ring_lo, 8], sig_done[mem_ring0_sig]

    #define_eval __LOOP 0
    #while (__LOOP < GRO_OUT_GETSAFE_NUM - 1)
        #define_eval __NEXT_LOOP (__LOOP + 1)

        mem_ring1_input/**/__LOOP#:
            #if (__LOOP == 0)
                gro_out_dispatch($mem_ring1[__LOOP], $mem_ring1[__LOOP],
                                 _gro_test_no_input_mem_ring,
                                 mem_ring1_input/**/__NEXT_LOOP#, mem_ring0_last_no_input#)
            #else
                gro_out_dispatch($mem_ring1[__LOOP], $mem_ring1[__LOOP],
                                 _gro_test_no_input_mem_ring,
                                 mem_ring1_input/**/__NEXT_LOOP#, wait_mem_ring0_input#)
            #endif

        #define_eval __LOOP (__NEXT_LOOP)
    #endloop

    mem_ring1_input/**/__LOOP#:
        gro_out_dispatch($mem_ring1[__LOOP], $mem_ring1[__LOOP],
                         _gro_test_no_input_mem_ring,
                         wait_mem_ring0_input#, wait_mem_ring0_input#)

    #undef __ISL
    #undef __EMEM
    #undef __LOOP
    #undef __NEXT_LOOP
.end
#endm


// Precondition:  LM_XBUF_PTR points to the XBUFth buffer for this context
#macro _gro_out_init_xfer_ctx(in_ctx, XBUF, REQPFX, SIGPFX)
.begin
    .reg tmp
    .reg tmp2

    #define_eval __XREQ REQPFX/**/XBUF/**/_0[0]
    #define_eval __SIG SIGPFX/**/XBUF


    /*
     * See NFP-6xxx PRM section 2.1.6.4.6
     * Override SIGNUM from CSR0 == bit 13
     * Override SIGCTX from CSR0 == bit 14
     * Override DATAREF from PREV_ALU == 3b1 at bits 3-5
     * Put the dataref for our xfer register at bits 16-30.
     */
    move(tmp, ((1 << 3) | (1 << 13) | (1 << 14)))
    move(tmp2, (&__XREQ << 2))
    alu[tmp2, tmp2, OR, in_ctx, <<7]        ; 32 xfer regs per ctx * 4 bytes per reg
    alu[LM_XBUF_PALU, tmp, OR, tmp2, <<16]

    /*
     * See NFP-6xxx PRM section 2.1.6.4.6
     * SIGNUM in CSR0 == bits 9-12
     * SIGCTX in CSR0 == bits 13-15
     */
    alu[tmp, --, b, in_ctx, <<13]
    alu[LM_XBUF_CSR0, tmp, OR, (&__SIG), <<9]



    #undef __XREQ
    #undef __SIG

.end
#endm


#macro _gro_out_worker_init_xbufs(in_ctx, in_next_ctx, ORDERSIG, REQPFX, SIGPFX)
.begin
    .reg base_addr
    .reg my_lm_addr
    .reg my_first_lm_addr
    .reg next_lm_addr
    .reg next_first_lm_addr
    .reg off

    /* compute the LM address of our first transfer buffer */
    move(base_addr, gro_out_lm_xbuf)
    mul_step[in_ctx, LM_XBUF_SIZE_PER_WORKER], 24x8_start
    mul_step[in_ctx, LM_XBUF_SIZE_PER_WORKER], 24x8_step1
    mul_step[off, --], 24x8_last
    alu[my_first_lm_addr, base_addr, +, off]
    move(my_lm_addr, my_first_lm_addr)

    /* compute the LM address of the next worker's first transfer buffer */
    mul_step[in_next_ctx, LM_XBUF_SIZE_PER_WORKER], 24x8_start
    mul_step[in_next_ctx, LM_XBUF_SIZE_PER_WORKER], 24x8_step1
    mul_step[off, --], 24x8_last
    alu[next_first_lm_addr, base_addr, +, off]
    move(next_lm_addr, next_first_lm_addr)

    // In general, ctxA_xbufB -> ctxA+1_xbufB.  Exception is the last context
    // which has:
    //   ctxLAST_xbufB -> ctxFIRST_xbuf(B+1 % GRO_OUT_XBUF_PER_WORKER)
    .if (in_next_ctx == GRO_OUT_FIRST_WORKER)

        // the last worker xbuf links are + 1 % GRO_OUT_XBUF_PER_WORKER
        alu[next_lm_addr, next_lm_addr, +, LM_XBUF_SIZE]

    .endif

    #define_eval __LOOP 0
    #while (__LOOP < GRO_OUT_XBUF_PER_WORKER)

        local_csr_wr[LM_XBUF_CSR, my_lm_addr]
        nop
        nop
        nop

        move(LM_XBUF_FLAGS, XBUF_FLAG_READY)
        move(LM_XBUF_DNEXT, next_lm_addr)

        // Initial sigmask == wait on the next xbuf's signal + ordersig
        #define_eval __NXTSIG   ((__LOOP + 1) % GRO_OUT_XBUF_PER_WORKER)
        move(LM_XBUF_SIGMASK, ((1 << &SIGPFX/**/__NXTSIG) | (1 << &ORDERSIG)))

        _gro_out_init_xfer_ctx(in_ctx, __LOOP, REQPFX, SIGPFX)

        #if (__LOOP != (GRO_OUT_XBUF_PER_WORKER - 1))

            alu[my_lm_addr, my_lm_addr, +, LM_XBUF_SIZE]
            move(LM_XBUF_WNEXT, my_lm_addr)

            #if (__LOOP != (GRO_OUT_XBUF_PER_WORKER - 2))

                alu[next_lm_addr, next_lm_addr, +, LM_XBUF_SIZE]

            #else

                .if (in_next_ctx != GRO_OUT_FIRST_WORKER)

                    alu[next_lm_addr, next_lm_addr, +, LM_XBUF_SIZE]

                .else

                    move(next_lm_addr, next_first_lm_addr)

                .endif
            #endif

        #else

            move(LM_XBUF_WNEXT, my_first_lm_addr)

        #endif


        #define_eval __LOOP (__LOOP + 1)
    #endloop


    local_csr_wr[LM_XBUF_CSR_0, my_first_lm_addr]

    alu[my_lm_addr, my_first_lm_addr, +, LM_XBUF_SIZE]
    local_csr_wr[LM_XBUF_CSR_1, my_lm_addr]

    alu[my_lm_addr, my_lm_addr, +, LM_XBUF_SIZE]
    local_csr_wr[LM_XBUF_CSR_2, my_lm_addr]

    #undef __LOOP
    #undef __NXTSIG

.end
#endm


#macro _pad_macro_len[CURLEN, DESIRED_LEN]

    #if (CURLEN > DESIRED_LEN)
        #error "Can not pad function to desired length: it is already too long"
    #endif

    #pragma warning (disable:4702)
    #define_eval __LOOP (CURLEN)
    #while (__LOOP < DESIRED_LEN)
        alu[--, --, B, (__LOOP & 0xFF)]
        #define_eval __LOOP (__LOOP + 1)
    #endloop
    #undef __LOOP
    #pragma warning (default:4702)

#endm


#macro gro_drop_seq(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 6
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[1]
        alu[g_sigmask, --, B, CUR_BUF[LM_XBUF_SIGMASK_wrd]]
    #else
        #define_eval _GRO_H_NINST 4
    #endif
    local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
    alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
    local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#macro gro_iface_xmit(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin
    .reg addr_hi

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 12
        alu[g_sigmask, --, B, CUR_BUF[LM_XBUF_SIGMASK_wrd]]
    #else
        #define_eval _GRO_H_NINST 10
        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    #endif

    /* XXX XXX XXX  Assumes one cycle lost unrecoverably due to jump[] */

    alu[addr_hi, --, B, in_meta[GRO_META_NBI_ADDRHI_wrd]]
    local_csr_wr[CMD_INDIRECT_REF_0, LM_DEST_NBI_CSR0]
    alu[--, --, B, in_meta[GRO_META_NBI_PALU_wrd]]

    nbi[packet_ready_unicast, $, addr_hi, <<8, in_meta[GRO_META_NBI_ADDRLO_wrd]], indirect_ref

    #if (!streq('TWO_REQ_LABEL', '--'))
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[2]
        alu[LM_DEST_NBI_CSR0, LM_DEST_NBI_CSR0, +, g_nbi_seq_incr]
        alu[LM_DEST_NBI_CSR0, LM_DEST_NBI_CSR0, AND, g_nbi_seq_mask]

        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #else
        alu[LM_DEST_NBI_CSR0, LM_DEST_NBI_CSR0, +, g_nbi_seq_incr]
        alu[LM_DEST_NBI_CSR0, LM_DEST_NBI_CSR0, AND, g_nbi_seq_mask]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #endif

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#ifndef NFD_USE_MOCKUP


// TODO:  Use real offsets and add thorough documentation of what we're doing
// here...
#macro gro_nfd_xmit(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin
    .reg word0
    .reg off

    .reg addr_hi
    .reg addr_lo

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 16
    #else
        #define_eval _GRO_H_NINST 15
        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    #endif

    move(out_xfer[1], in_meta[1])
    move(out_xfer[3], in_meta[3])

    alu[off, (127 << 1), AND, in_meta[0], >>(GRO_META_W0_META_START_BIT - 1)]
    #ifdef GRO_EVEN_NFD_OFFSETS_ONLY
        #define_eval _GRO_H_NINST (_GRO_H_NINST - 1)
        move(out_xfer[2], in_meta[2])
    #else
        alu[out_xfer[2], in_meta[2], OR, 1, <<31]
        alu[off, off, OR, in_meta[2], >>31]
    #endif
    alu[word0, in_meta[0], AND, g_hi18_mask]
    alu[out_xfer[0], word0, OR, off]

    alu[addr_hi, LM_DEST_NFD3_RING_ENC, AND, 0xFF, <<24]
    alu[addr_lo, g_ringlo_mask, AND, LM_DEST_NFD3_RING_ENC, >>GRO_DEST_NFD3_RINGLO_shf]

    #if (defined(NFD_VERSION) && (NFD_VERSION == 0x03010000))
        mem[qadd_work, out_xfer[0], addr_hi, <<8, addr_lo, 4], sig_done[OUTSIG]
    #else
        mem[journal, out_xfer[0], addr_hi, <<8, addr_lo, 4], sig_done[OUTSIG]
    #endif

    #if (!streq('TWO_REQ_LABEL', '--'))
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[1]
        alu[g_sigmask, CUR_BUF[LM_XBUF_SIGMASK_wrd], OR, mask(OUTSIG), <<(&OUTSIG)]

        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #else
        alu[g_sigmask, g_sigmask, OR, mask(OUTSIG), <<(&OUTSIG)]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #endif

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#else /* NFD_USE_MOCKUP */


#macro gro_nfd_xmit(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin
    .reg w3
    .reg tmp
    .reg tmp2
    .reg addr_hi
    .reg addr_lo

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 18
    #else
        #define_eval _GRO_H_NINST 17
        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    #endif

    alu[out_xfer[0], --, B, in_meta[1]]
    alu[out_xfer[1], --, B, in_meta[2]]
    alu[out_xfer[2], --, B, in_meta[3]]

    alu[w3, 0x7, AND, in_meta[0], >>13]         // XXX copy flags
    ld_field[w3, 1100, in_meta[0]]              // XXX copy VLAN

    alu[tmp, 0xC0, AND, in_meta[0], <<3]        // XXX copy PCI
    alu[tmp2, 0x3F, AND, in_meta[0], >>7]       // XXX copy QUEUE
    alu[tmp, tmp, OR, tmp2]
    alu[out_xfer[3], w3, OR, tmp, <<8]

    alu[addr_hi, LM_DEST_NFD3_RING_ENC, AND, 0xFF, <<24]
    alu[addr_lo, g_ringlo_mask, AND, LM_DEST_NFD3_RING_ENC, >>GRO_DEST_NFD3_RINGLO_shf]
    mem[qadd_work, out_xfer[0], addr_hi, <<8, addr_lo, 4], sig_done[OUTSIG]

    #if (!streq('TWO_REQ_LABEL', '--'))
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[1]
        alu[g_sigmask, CUR_BUF[LM_XBUF_SIGMASK_wrd], OR, mask(OUTSIG), <<(&OUTSIG)]

        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #else
        alu[g_sigmask, g_sigmask, OR, mask(OUTSIG), <<(&OUTSIG)]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #endif

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#endif /* NFD_USE_MOCKUP */


#macro gro_mem_ring_op(MEMOP, in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin
    .reg addr_hi
    .reg addr_lo

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 13
    #else
        #define_eval _GRO_H_NINST 12
        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    #endif

    #if (GRO_META_RINGHI_shf != 24 || GRO_META_RINGHI_msk != 0xFF)
        #error "Ring metadata format changed for addr hi:  optimization will not work"
    #endif
    alu[addr_hi, in_meta[GRO_META_RINGHI_wrd], AND, 0xFF, <<24]

    alu[addr_lo, g_ringlo_mask, AND, in_meta[GRO_META_RINGLO_wrd], >>GRO_META_RINGLO_shf]

    alu[out_xfer[0], --, B, in_meta[1]]
    alu[out_xfer[1], --, B, in_meta[2]]
    alu[out_xfer[2], --, B, in_meta[3]]
    alu[--, --, B, LM_DEST_MEM_RING_IREF]
    mem[MEMOP, out_xfer[0], addr_hi, <<8, addr_lo, max_3], sig_done[OUTSIG], indirect_ref

    #if (!streq('TWO_REQ_LABEL', '--'))
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[1]
        alu[g_sigmask, CUR_BUF[LM_XBUF_SIGMASK_wrd], OR, mask(OUTSIG), <<(&OUTSIG)]

        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #else
        alu[g_sigmask, g_sigmask, OR, mask(OUTSIG), <<(&OUTSIG)]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #endif

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#macro gro_workq_put(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
    gro_mem_ring_op(qadd_work, in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
#endm


#macro gro_mem_ring_put(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
    gro_mem_ring_op(journal, in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
#endm


#macro _gro_free_ctm_buf(in_meta)
.begin
    .reg addr_hi
    .reg addr_lo

    alu[addr_hi, in_meta[GRO_META_DROP_CTMHI_wrd], AND, 0xFF, <<24]
    alu[addr_lo, g_ringlo_mask, AND, in_meta[GRO_META_DROP_PNUM_wrd], >>GRO_META_DROP_PNUM_shf]
    mem[packet_free, --, addr_hi, <<8, addr_lo]
.end
#endm


#macro _gro_free_mu_buf(in_meta)
.begin
    .reg addr_hi

    // TODO: document (1 << 3)
    alu[addr_hi, in_meta[GRO_META_DROP_RINGHI_wrd], AND, 0xFF, <<24]
    alu[--, (1 << 3), OR, in_meta[GRO_META_DROP_RINGNUM_wrd], <<16]
    mem[fast_journal, --, addr_hi, <<8, in_meta[GRO_META_DROP_BUFH_wrd]], indirect_ref
.end
#endm


#macro gro_drop_ctm_buf(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 9
    #else
        #define_eval _GRO_H_NINST 7
        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    #endif

    #if (GRO_META_DROP_CTMHI_shf != 24 || GRO_META_DROP_CTMHI_msk != 0xFF)
        #error "Packet drop metadata format changed for ctm addr hi:  optimization will not work"
    #endif
    #if (GRO_META_DROP_RINGHI_shf != 24 || GRO_META_DROP_RINGHI_msk != 0xFF)
        #error "Packet drop metadata format changed for ctm addr hi:  optimization will not work"
    #endif
    #if (GRO_META_DROP_RINGNUM_shf != 0 || GRO_META_DROP_RINGNUM_msk != 0x3FF)
        #error "Packet drop metadata format changed for ring num:  optimization will not work"
    #endif

    ; VERIFY: icount _gro_free_ctm_buf 3
    _gro_free_ctm_buf(in_meta)

    #if (!streq('TWO_REQ_LABEL', '--'))
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[1]
        alu[g_sigmask, --, B, CUR_BUF[LM_XBUF_SIGMASK_wrd]]

        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #else
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #endif

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#macro gro_drop_mu_buf(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin

    #if (!streq('TWO_REQ_LABEL', '--'))
        #define_eval _GRO_H_NINST 9
    #else
        #define_eval _GRO_H_NINST 7
        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
    #endif

    #if (GRO_META_DROP_CTMHI_shf != 24 || GRO_META_DROP_CTMHI_msk != 0xFF)
        #error "Packet drop metadata format changed for ctm addr hi:  optimization will not work"
    #endif
    #if (GRO_META_DROP_RINGHI_shf != 24 || GRO_META_DROP_RINGHI_msk != 0xFF)
        #error "Packet drop metadata format changed for ctm addr hi:  optimization will not work"
    #endif
    #if (GRO_META_DROP_RINGNUM_shf != 0 || GRO_META_DROP_RINGNUM_msk != 0x3FF)
        #error "Packet drop metadata format changed for ring num:  optimization will not work"
    #endif

    ; VERIFY: icount _gro_free_mu_buf 3
    _gro_free_mu_buf(in_meta)

    #if (!streq('TWO_REQ_LABEL', '--'))
        br_bset[CUR_BUF[LM_XBUF_FLAGS_wrd], XBUF_FLAG_TWOPKTS_bit, TWO_REQ_LABEL], defer[1]
        alu[g_sigmask, --, B, CUR_BUF[LM_XBUF_SIGMASK_wrd]]

        local_csr_wr[SAME_ME_SIGNAL, g_sig_next_worker]
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #else
        ctx_arb[--], br[ONE_REQ_LABEL], defer[2]
        alu[CUR_BUF[LM_XBUF_FLAGS_wrd], --, B, XBUF_FLAG_READY]
        local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, g_sigmask]
    #endif

    _pad_macro_len(_GRO_H_NINST, GRO_OUT_HANDLER_ILEN)
    #undef _GRO_H_NINST

.end
#endm


#macro gro_out_send_one(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)
.begin
    .reg off
    .reg lm_dest_addr

    #if (GRO_META_DEST_shf != LM_DEST_shf)
        #error "GRO_META_DEST_shf != LM_DEST_shf: Optimization will not work"
    #endif

    alu[off, g_handler_mask, AND, in_meta[GRO_META_TYPE_wrd], <<GRO_OUT_HANDLER_shf]
    jump[off, xmit_drop_seq#], defer[3],
        targets[xmit_drop_seq#, xmit_iface#, xmit_nfd#, xmit_workq#, xmit_mem_ring#,
                xmit_drop_ctm_buf#, xmit_drop_mu_buf#]
    alu[lm_dest_addr, (GRO_META_DEST_msk << LM_DEST_shf), AND, in_meta[GRO_META_DEST_wrd]]
    alu[lm_dest_addr, lm_dest_addr, +, g_lm_dest_base]
    local_csr_wr[LM_DEST_CSR, lm_dest_addr]


xmit_drop_seq#:
    ; VERIFY: icount gro_drop_seq 64
    gro_drop_seq(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

xmit_iface#:
    ; VERIFY: icount gro_iface_xmit 64
    gro_iface_xmit(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

xmit_nfd#:
    ; VERIFY: icount gro_nfd_xmit 64
    gro_nfd_xmit(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

xmit_workq#:
    ; VERIFY: icount gro_workq_put 64
    gro_workq_put(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

xmit_mem_ring#:
    ; VERIFY: icount gro_mem_ring_put 64
    gro_mem_ring_put(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

xmit_drop_ctm_buf#:
    ; VERIFY: icount gro_drop_ctm_buf 64
    gro_drop_ctm_buf(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

xmit_drop_mu_buf#:
    ; VERIFY: icount gro_drop_mu_buf 64
    gro_drop_mu_buf(in_meta, out_xfer, OUTSIG, CUR_BUF, ONE_REQ_LABEL, TWO_REQ_LABEL)

.end
#endm


#macro gro_out_worker(ORDERSIG)
.begin
    .reg volatile g_ringlo_mask
    .reg volatile g_lm_dest_base
    .reg volatile g_sig_next_worker
    .reg volatile g_sigmask
    .reg volatile g_hi18_mask
    .reg volatile g_handler_mask
    .reg volatile g_nbi_seq_incr
    .reg volatile g_nbi_seq_mask
    .reg ctx
    .reg next_ctx
    .reg osig


    #define_eval __XBUF 0
    #while (__XBUF < GRO_OUT_XBUF_PER_WORKER)

        .reg volatile read $meta_/**/__XBUF/**/_0[4]
        .reg volatile read $meta_/**/__XBUF/**/_1[4]
        .xfer_order $meta_/**/__XBUF/**/_0 $meta_/**/__XBUF/**/_1
        .sig volatile insig_/**/__XBUF

        .reg volatile write $out_/**/__XBUF/**/_0[GRO_OUT_XMIT_LEN]
        .xfer_order $out_/**/__XBUF/**/_0
        .sig volatile outsig_/**/__XBUF/**/_0

        .reg volatile write $out_/**/__XBUF/**/_1[GRO_OUT_XMIT_LEN]
        .xfer_order $out_/**/__XBUF/**/_1
        .sig volatile outsig_/**/__XBUF/**/_1

        #define_eval __XBUF (__XBUF + 1)
    #endloop
    #undef __XBUF


    move(g_ringlo_mask, GRO_META_RINGLO_msk)
    move(g_lm_dest_base, gro_out_lm_dest)
    local_csr_rd[ACTIVE_CTX_STS]
    immed[ctx, 0]
    alu[ctx, ctx, AND, 0x7]
    alu[next_ctx, ctx, +, 1]
    .if (next_ctx > GRO_OUT_LAST_WORKER)
        immed[next_ctx, GRO_OUT_FIRST_WORKER]
    .endif
    alu[g_sig_next_worker, next_ctx, OR, (&ORDERSIG), <<3]
    move(g_sigmask, 0)
    move(g_hi18_mask, 0xFFFFC000)
    move(g_handler_mask, (GRO_META_TYPE_msk << GRO_OUT_HANDLER_shf))
    /*
     * must increment the sequence number in a PPC command
     * See NFP-6xxx DB 9.2.2.7.9:  goes in SigMaster, DataMaster,
     * DataMasterIsland.  With a PPC command, we override these in
     * CmdIndirectRef0.  These are in bits 16 to 29 in that register.
     * See NFP-6xxx PRm 4.2.1.1.55.
     */
    move(g_nbi_seq_incr, 0x00010000)
    move(g_nbi_seq_mask, 0x3fffffff)

    _gro_out_worker_init_xbufs(ctx, next_ctx, ORDERSIG, $meta_, insig_)

worker_start#:
    .set_sig ORDERSIG
    .set_sig insig_0
    ctx_arb[ORDERSIG, insig_0]

    #define_eval __XBUF 0
    #while (__XBUF < GRO_OUT_XBUF_PER_WORKER)

        #define_eval __CUR_LMPTR LM_XBUF_PTR_/**/__XBUF
        #define_eval __XBUF_NXT ((__XBUF + 1) % GRO_OUT_XBUF_PER_WORKER)


        first_req_/**/__XBUF#:
            .set $meta_/**/__XBUF/**/_0[0] $meta_/**/__XBUF/**/_0[1]
            .set $meta_/**/__XBUF/**/_0[2] $meta_/**/__XBUF/**/_0[3]
            .io_completed outsig_/**/__XBUF/**/_0
            #ifdef GRO_DEBUG
                alu[@gro_nsent, @gro_nsent, +, 1]
                local_csr_wr[MAILBOX0, @gro_nsent]
            #endif /* GRO_DEBUG */
            gro_out_send_one($meta_/**/__XBUF/**/_0, $out_/**/__XBUF/**/_0,
                             outsig_/**/__XBUF/**/_0, __CUR_LMPTR,
                             first_req_/**/__XBUF_NXT#,
                             second_req_/**/__XBUF#)

        second_req_/**/__XBUF#:
            .set $meta_/**/__XBUF/**/_1[0] $meta_/**/__XBUF/**/_1[1]
            .set $meta_/**/__XBUF/**/_1[2] $meta_/**/__XBUF/**/_1[3]
            .io_completed outsig_/**/__XBUF/**/_1
            #ifdef GRO_DEBUG
                alu[@gro_nsent, @gro_nsent, +, 1]
                local_csr_wr[MAILBOX0, @gro_nsent]
            #endif /* GRO_DEBUG */
            gro_out_send_one($meta_/**/__XBUF/**/_1, $out_/**/__XBUF/**/_1,
                             outsig_/**/__XBUF/**/_1, __CUR_LMPTR,
                             first_req_/**/__XBUF_NXT#, --)


        #define_eval __XBUF (__XBUF + 1)
    #endloop
    #undef __XBUF
    #undef __XBUF_NXT
    #undef __CUR_LMPTR

.end
#endm


// MAIN PROGRAM
gro_out_main#:

    .sig volatile ordersig

    #ifdef GRO_DEBUG
        journal_declare(debug)

        .reg @gro_nsent
        .init @gro_nsent 0
        .reg @gro_idle
        .init @gro_idle 0
    #endif /* GRO_DEBUG */

    gro_out_declare(GRO_BLOCK_NUM)

    .if (CTX() != GRO_OUT_DISP_CONTEXT)

        gro_out_worker(ordersig)

    .else

        #ifdef GRO_DEBUG
            local_csr_wr[MAILBOX0, 0]
            local_csr_wr[MAILBOX1, 0]
            local_csr_wr[MAILBOX2, 0]
            local_csr_wr[MAILBOX3, 0]
        #endif /* GRO_DEBUG */

        gro_out_dispatcher_mem_ring(GRO_BLOCK_NUM)

    .endif

    nop ; suppress ERROR: (A3498)

#endif /* __GRO_OUT_UC */
