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
 * @file          me/lib/gro/libgro.c
 */

#ifndef __LIBGRO_C
#define __LIBGRO_C

#include <assert.h>
#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp6000/nfp_me.h>
#include <nfp/me.h>
#include <nfp/mem_atomic.h>
#include <nfp/mem_bulk.h>
#include <gro/gro_cli.h>


#define GRO_DIRECT_ACCESS       0x80

#if (GRO_NUM_BLOCKS > 8)
    #error "Currently, only 8 global reorder blocks supported"
#endif
#if (_nfp_has_island("emem1") && _nfp_has_island("emem2"))
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
#elif (_nfp_has_island("emem1"))
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


__import __shared __emem struct gro_client_ctx gro_ctxcfg[GRO_TOTAL_CTX];

__shared __lmem struct gro_client_ctx gro_cli_lm_ctx[GRO_TOTAL_CTX];

__import __shared GRO_CNTR_MEM_MICROC __align256
    uint64_t gro_cli_cntrs[GRO_TOTAL_CTX*2];


__intrinsic static int
log2(unsigned int x)
{
    int l2;
    __asm ffs[l2, x];
    return l2;
}


#define ASM(_str) __asm{_str}

#define _link_sym(_s) __link_sym(#_s)

#define GRO_RING_DECLARE2(_ringname, _memname) \
    ASM(.alloc_resource _ringname _memname##_queues global 1)
#define GRO_RING_DECLARE1(_ringname, _memname) \
    GRO_RING_DECLARE2(_ringname, _memname)
#define GRO_RING_DECLARE(block) \
    GRO_RING_DECLARE1(gro_release_ring_##block, gro_release_mem_##block)

#if GRO_NUM_BLOCKS > 0
    GRO_RING_DECLARE(0)
#endif

#if GRO_NUM_BLOCKS > 1
    GRO_RING_DECLARE(1)
#endif

#if GRO_NUM_BLOCKS > 2
    GRO_RING_DECLARE(2)
#endif

#if GRO_NUM_BLOCKS > 3
    GRO_RING_DECLARE(3)
#endif

#if GRO_NUM_BLOCKS > 4
    GRO_RING_DECLARE(4)
#endif

#if GRO_NUM_BLOCKS > 5
    GRO_RING_DECLARE(5)
#endif

#if GRO_NUM_BLOCKS > 6
    GRO_RING_DECLARE(6)
#endif

#if GRO_NUM_BLOCKS > 7
    GRO_RING_DECLARE(7)
#endif


__intrinsic void
gro_cli_init(void)
{
    __xread struct gro_client_ctx xctx;
    int i;
    for (i = 0; i < GRO_TOTAL_CTX; i++) {
        mem_read64(&xctx, &gro_ctxcfg[i], sizeof(xctx));
        gro_cli_lm_ctx[i] = xctx;
    }
}


__intrinsic void
gro_cli_build_drop_seq_meta(__xwrite struct gro_meta_drop *meta)
{
    meta->__raw[0] = (GRO_DTYPE_DROP_SEQ << GRO_META_TYPE_shf);
    meta->__raw[1] = 0;
    meta->__raw[2] = 0;
    meta->__raw[3] = 0;
}


__intrinsic void
gro_cli_build_drop_ctm_buf_meta(__xwrite struct gro_meta_drop *meta,
                                unsigned int isl, unsigned int pkt_num)
{
    meta->__raw[0] = (GRO_DTYPE_DROP_CTM_BUF << GRO_META_TYPE_shf);
    meta->__raw[1] = ((0x80 | isl) << GRO_META_DROP_CTMHI_shf) |
                     (pkt_num << GRO_META_DROP_PNUM_shf);
    meta->__raw[2] = 0;
    meta->__raw[3] = 0;
}


__intrinsic void
gro_cli_build_drop_mu_buf_meta(__xwrite struct gro_meta_drop *meta,
                               unsigned int ring_addr_hi, unsigned int ring_num,
                               unsigned int mu_handle)
{
    meta->__raw[0] = (GRO_DTYPE_DROP_MU_BUF << GRO_META_TYPE_shf);
    meta->__raw[1] = 0;
    meta->__raw[2] = (ring_addr_hi << GRO_META_DROP_RINGHI_shf) |
                     (ring_num << GRO_META_DROP_RINGNUM_shf);
    meta->__raw[3] = mu_handle;
}


__intrinsic void
gro_cli_build_memq_meta(__xwrite struct gro_meta_memq *meta, int type,
                        unsigned int isl, unsigned int qnum, int nwords,
                        int qd0, int qd1, int qd2)
{
    struct gro_meta_memq_w0 w0 = { 0 };

    w0.type = type;
    if (type == GRO_DTYPE_WORKQ) {
        w0.dest = GRO_DEST_WORKQ_1WORD + nwords - 1;
    } else {
        w0.dest = GRO_DEST_MEM_RING_1WORD + nwords - 1;
    }
    w0.qnum = qnum;
    w0.q_hi = isl | GRO_DIRECT_ACCESS;

    meta->w0 = w0;
    meta->qdata[0] = qd0;
    meta->qdata[1] = qd1;
    meta->qdata[2] = qd2;
}


__intrinsic void
gro_cli_build_workq_meta1(__xwrite struct gro_meta_memq *meta,
                          unsigned int isl, unsigned int qnum, int qd0)
{
    gro_cli_build_memq_meta(meta, GRO_DTYPE_WORKQ, isl, qnum, 1, qd0, 0, 0);
}


__intrinsic void
gro_cli_build_workq_meta2(__xwrite struct gro_meta_memq *meta,
                          unsigned int isl, unsigned int qnum, int qd0, int qd1)
{
    gro_cli_build_memq_meta(meta, GRO_DTYPE_WORKQ, isl, qnum, 2, qd0, qd1, 0);
}


__intrinsic void
gro_cli_build_workq_meta3(__xwrite struct gro_meta_memq *meta,
                          unsigned int isl, unsigned int qnum, int qd0, int qd1,
                          int qd2)
{
    gro_cli_build_memq_meta(meta, GRO_DTYPE_WORKQ, isl, qnum, 3, qd0, qd1, qd2);
}


__intrinsic void
gro_cli_build_mem_ring_meta1(__xwrite struct gro_meta_memq *meta,
                             unsigned int isl, unsigned int qnum, int qd0)
{
    gro_cli_build_memq_meta(meta, GRO_DTYPE_RING, isl, qnum, 1, qd0, 0, 0);
}


__intrinsic void
gro_cli_build_mem_ring_meta2(__xwrite struct gro_meta_memq *meta,
                             unsigned int isl, unsigned int qnum, int qd0,
                             int qd1)
{
    gro_cli_build_memq_meta(meta, GRO_DTYPE_RING, isl, qnum, 2, qd0, qd1, 0);
}


__intrinsic void
gro_cli_build_mem_ring_meta3(__xwrite struct gro_meta_memq *meta,
                             unsigned int isl, unsigned int qnum, int qd0,
                             int qd1, int qd2)
{
    gro_cli_build_memq_meta(meta, GRO_DTYPE_RING, isl, qnum, 3, qd0, qd1, qd2);
}



__intrinsic void
gro_cli_build_nbi_meta(__xwrite struct gro_meta_nbi *meta, unsigned int pkt_isl,
                       unsigned int pkt_num, unsigned int ctm_sz,
                       unsigned int ms_off, unsigned int eoff, unsigned int nbi,
                       unsigned int txq)
{
    unsigned int dm;

    try_ctassert(pkt_isl < 64);
    try_ctassert(pkt_num < 512);
    try_ctassert(((ms_off % 8) == 0) && (ms_off <= 56));
    try_ctassert(nbi < 2);
    try_ctassert(txq < 1024);

    /*
     * See the NFP 6xxx Databook section 9.2.2.7.9 for the format of the
     * various packet processing complete fields.
     */
    meta->__raw[0] = (GRO_DTYPE_IFACE << GRO_META_TYPE_shf) |
                     ((GRO_DEST_IFACE_BASE + nbi) << GRO_META_DEST_shf);
    dm = (__ME() & 1) + 2;
    meta->addr_hi = (nbi << 30) | (pkt_isl << 24) | (dm << 20);
    meta->addr_lo = eoff | (pkt_num << 16);

    /*
     * Word 3: (previous ALU instruction value)
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
     *
     *  * Length field itself is the encoded modifier script starting offset.
     *
     *  * TXQ goes in data_ref[10;0] which is in [10;16] of the previous ALU.
     *
     *  * The CTM size goes in data_ref[2;12] which is at [2;28] of the
     *    previous ALU.
     */
#define __NBI_IREF_LO8 ((1 << 0) | (1 << 1) | (1 << 3) | (1 << 6) | (1 << 7))

    meta->prev_alu = __NBI_IREF_LO8 | (((ms_off / 8) - 1) << 8) | (txq << 16) |
                     (ctm_sz << 28);

#undef __NBI_IREF_LO8
}


/* TODO: use real NFD headers for this: but they don't have the */
/* #defines we need yet.  Also, not thrilled about making GRO   */
/* dependent on NFD yet. */


#ifndef NFD_USE_MOCKUP


__intrinsic void
gro_cli_nfd_desc2meta(__xwrite struct gro_meta_nfd3 *odesc,
                      void *idescp, unsigned int pcie, unsigned int qid)
{
    unsigned int *idesc = idescp;
    struct gro_meta_nfd3_w0 w0;
    unsigned int off;

    /*
     * We sacrifice some flexibility by requring a maximum starting offset of
     * 255 bytes instead of 4095 in order to send through GRO.  We steal those
     * bits and a few other adjacent fixed ones for the GRO type and
     * destination (pcie) fields.  If we restrict starting offsets to even
     * values, then we can save a few cycles here and a single cycle in GRO.OUT.
     */
    w0.__raw = idesc[0];
    off = idesc[0] & GRO_NFD_MAX_OFFSET;
    w0.type = GRO_DTYPE_NFD;
    w0.pcie = pcie;

    w0.off = off >> 1;

    odesc->w0 = w0;
    odesc->w1 = idesc[1];
#ifdef GRO_EVEN_NFD_OFFSETS_ONLY
    odesc->w2 = (idesc[2] & 0xFF00FFFF) | (qid << 16);
#else
    odesc->w2 = (idesc[2] & 0x7F00FFFF) | (qid << 16) | (off << 31);
#endif
    odesc->w3 = idesc[3];
}


#else /* NFD_USE_MOCKUP */


/* TODO:  incorporate cbs and bls into the metadata here */
__intrinsic void
gro_cli_build_nfd_meta(__xwrite struct gro_meta_nfd3 *meta, unsigned int pcie,
                       unsigned int qid, unsigned int flags,
                       unsigned int pktnum, unsigned int ctm_isl,
                       unsigned int cbs, unsigned int bls,
                       unsigned int mu_addr, unsigned int offset,
                       unsigned int len, unsigned int meta_len,
                       unsigned int vlan)
{
    struct gro_meta_nfd3_w0 w0;
    w0.type = GRO_DTYPE_NFD;
    w0.qid = qid;
    w0.pcie = pcie;
    w0.flags = flags;
    w0.vlan = vlan;

    meta->w0 = w0;
    meta->w1 = (mu_addr >> (40 - 11 - 8)) | (offset << 8) |
               (pktnum << 16) | (ctm_isl << 26);
    meta->w2 = (mu_addr << 11);
    meta->w3 = len | (meta_len << 16) | (64 << 24) | (1 << 31);
}


__intrinsic void
gro_cli_nfd_desc2meta(__xwrite struct gro_meta_nfd3 *odesc,
                      void *idescp, unsigned int pcie, unsigned int qid)
{
    unsigned int *idesc = idescp;
    struct gro_meta_nfd3_w0 w0;

    w0.type = GRO_DTYPE_NFD;
    w0.pcie = pcie;
    w0.qid = (idesc[3] >> 8) & 0x3F;
    w0.flags = idesc[3] & 0x7;
    w0.vlan = (idesc[3] >> 16);
    odesc->w0 = w0;
    odesc->w1 = idesc[0];
    odesc->w2 = idesc[1];
    odesc->w3 = idesc[2];
}


#endif /* NFD_USE_MOCKUP */


__intrinsic void
_gro_cli_cntr_enqueued_incr(unsigned int ctx)
{
    mem_add64_imm(1, &gro_cli_cntrs[2 * ctx]);
}


__intrinsic void
_gro_cli_cntr_released_add(unsigned int ctx, unsigned int amt)
{
    mem_add64_imm(amt, &gro_cli_cntrs[2 * ctx + 1]);
}


__intrinsic void
_gro_cli_release(__lmem struct gro_client_ctx *clictx, unsigned int ctx,
                 unsigned int nrel)
{
    unsigned int addr_hi;
    unsigned int addr_lo;
    struct gro_release rel;
    __xrw struct gro_release xrel;
    SIGNAL_PAIR putsig;

    rel.__raw = 0;
    rel.ctx = ctx % GRO_CTX_PER_BLOCK;
    rel.nrel = nrel;
    rel.valid = 1;
    xrel = rel;

    addr_hi = clictx->ring_addr_hi << 24;
    addr_lo = clictx->ring_addr_lo;

    do {
        __asm {
            mem[put, xrel, addr_hi, <<8, addr_lo, 1], sig_done[putsig]
            ctx_arb[putsig]
        }
    } while (!xrel.valid);

    _gro_cli_cntr_released_add(ctx, nrel);
}


__intrinsic void
gro_cli_abort_ticket(unsigned int ctx, unsigned int tape, unsigned int seq)
{
    local_csr_write(local_csr_mailbox_0, 0x71CE7E44);
    local_csr_write(local_csr_mailbox_1, ctx);
    local_csr_write(local_csr_mailbox_2, tape);
    local_csr_write(local_csr_mailbox_3, seq);
    __asm {
        ctx_arb[bpt]
        alu[--, --, B, 0]
forever:
        beq[forever]
    }
}



__intrinsic void
gro_cli_abort_tape(unsigned int ctx, unsigned int tape, unsigned int nrel)
{
    local_csr_write(local_csr_mailbox_0, 0x77BE4404);
    local_csr_write(local_csr_mailbox_1, ctx);
    local_csr_write(local_csr_mailbox_2, tape);
    local_csr_write(local_csr_mailbox_3, nrel);
    __asm {
        ctx_arb[bpt]
        alu[--, --, B, 0]
forever:
        beq[forever]
    }
}


static __intrinsic int
ticket_error_is_fatal(unsigned int addr_hi, unsigned int addr_lo,
                      unsigned int ticket_seq, unsigned int epoch)
{
    unsigned int prev_epoch = (epoch == 0) ? (GRO_NUM_EPOCHS - 1) : epoch - 1;
    unsigned int skid_marker = GRO_SEQSKID_VALUE |
                               (prev_epoch << GRO_EPOCH_TICKET_SEQ_shf);
    unsigned int tape_epoch;
    unsigned int tape_seq;
    unsigned int rel_seq;

    SIGNAL tape_read_sig;

    __xread unsigned int ticket_tape[3];

    __asm { mem[read_atomic, ticket_tape, addr_hi, <<8, addr_lo, 3], ctx_swap[tape_read_sig] }

    if (ticket_tape[0] == skid_marker)
        return 0;

    tape_epoch = (ticket_tape[0] >> GRO_EPOCH_TICKET_SEQ_shf) & GRO_EPOCH_msk;
    if (tape_epoch != epoch)
        return 1;

    tape_seq = ticket_tape[0] & GRO_TAPE_SEQ_msk;
    if (tape_seq == ticket_seq)
        return 0;

    if (tape_seq > ticket_seq)
        return 1;

    rel_seq = ticket_seq - tape_seq - 1;
    if (rel_seq < 32) {
        if (ticket_tape[1] & (1 << rel_seq))
            return 1;
    } else {
        rel_seq -= 32;
        if (ticket_tape[2] & (1 << rel_seq))
            return 1;
    }

    return 0;
}


static __intrinsic int
cascaded_ticket_error_is_fatal(unsigned int addr_hi, unsigned int addr_lo,
                               unsigned int epoch)
{
    unsigned int prev_epoch = (epoch == 0) ? (GRO_NUM_EPOCHS - 1) : epoch - 1;
    unsigned int skid_marker = GRO_SEQSKID_VALUE |
                               (prev_epoch << GRO_EPOCH_TICKET_SEQ_shf);
    SIGNAL tape_read_sig;
    __xread unsigned int tape_seq;

    __asm { mem[read_atomic, tape_seq, addr_hi, <<8, addr_lo, 1], ctx_swap[tape_read_sig] }

    if (tape_seq == skid_marker)
        return 0;

    if (tape_seq == (epoch << GRO_EPOCH_TICKET_SEQ_shf))
        return 0;

    return 1;
}


__intrinsic void
gro_cli_send(__xwrite void *meta, unsigned int ctx, unsigned int seq)
{
    unsigned int adj_seq;
    int nrel;
    unsigned int tape;
    unsigned int ticket_seq;
    unsigned int rel_ctx;
    unsigned int addr_hi;
    unsigned int addr_lo;
    unsigned int ntapes;
    unsigned int epoch;

    __lmem struct gro_client_ctx *clictx;

    __xrw uint32_t ticket;

    SIGNAL meta_sig;
    SIGNAL_PAIR ticket_sig;

    clictx = &gro_cli_lm_ctx[ctx];
    epoch = (seq >> clictx->q_size) % GRO_NUM_EPOCHS;
    ticket_seq = (seq & GRO_TICKET_msk) + 1;
    ticket = ticket_seq | (epoch << GRO_EPOCH_TICKET_SEQ_shf);

    /* Write the metadata */
    adj_seq = seq & ((1 << clictx->q_size) - 1);
    addr_hi = clictx->q_addr_hi << 24;
    addr_lo = clictx->q_addr_lo | (adj_seq * GRO_META_SIZE);
    __asm {
        mem[write32, *meta, addr_hi, <<8, addr_lo, GRO_META_SIZE_LW], ctx_swap[meta_sig]
    }

    /* Release the ticket */
    tape = adj_seq / GRO_TICKET_PER_TAPE;
    addr_hi = clictx->bm_addr_hi << 24;
    addr_lo = clictx->bm_addr_lo | (tape * GRO_TICKET_TAPE_SIZE);
    __asm {
        mem[release_ticket, ticket, addr_hi, <<8, addr_lo, 1], sig_done[ticket_sig]
        ctx_arb[ticket_sig]
    }

    while (ticket == GRO_TICKET_ERROR) {
        if (ticket_error_is_fatal(addr_hi, addr_lo, ticket_seq, epoch))
            gro_cli_abort_ticket(ctx, tape, seq);
        /* Assume 40 cycles/pkt * 64 pkt/bm = 2560 cycles/bm.  Sleep that */
        /* before trying again to give the GRO.OUT MEs time to catch up. */
        /* Theoretically, the avg time to wait would be half of this, but */
        /* that assumes we're always at the first bitmap in the skid.  */
        /* We could be much deeper.  Besides, there's still an entire live */
        /* bitmap to send first:  we're not going to fall that far back. */
        sleep(2560);
        /* TODO increment counter */
        __asm {
            mem[release_ticket, ticket, addr_hi, <<8, addr_lo, 1], sig_done[ticket_sig]
            ctx_arb[ticket_sig]
        }
    }

    /* increment enqueued counter: packet enqueued successfully */
    _gro_cli_cntr_enqueued_incr(ctx);

    if (ticket != 0) {

        _gro_cli_release(clictx, ctx, ticket);

        if (ticket + ticket_seq > GRO_TICKET_PER_TAPE) {
            ntapes = 1 << (clictx->q_size - GRO_TICKET_TAPE_SEQ_shf);

            do {

                tape = (tape + 1);
                if (tape >= ntapes) {
                    tape = 0;
                    epoch = (epoch + 1) % GRO_NUM_EPOCHS;
                }
                ticket = epoch << GRO_EPOCH_TICKET_SEQ_shf;
                addr_lo = clictx->bm_addr_lo | (tape * GRO_TICKET_TAPE_SIZE);

                for (;;) {
                    __asm {
                        mem[release_ticket, ticket, addr_hi, <<8, addr_lo, 1], sig_done[ticket_sig]
                        ctx_arb[ticket_sig]
                    }

                    if (ticket != GRO_TICKET_ERROR)
                        break;

                    if (cascaded_ticket_error_is_fatal(addr_hi, addr_lo, epoch))
                        gro_cli_abort_ticket(ctx, tape, seq | (1 << 31));

                    sleep(2560);
                }

                if (ticket < 1)
                    gro_cli_abort_tape(ctx, tape, ticket);

                nrel = ticket - 1;
                if (nrel > 0)
                    _gro_cli_release(clictx, ctx, nrel);

            } while (nrel >= GRO_TICKET_PER_TAPE);
        }
    }

    __implicit_read(meta, GRO_META_SIZE);
}


#endif /* __LIBGRO_C */
