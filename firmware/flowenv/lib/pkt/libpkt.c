/*
 * Copyright (C) 2014-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file        libpkt.c
 * @brief       Basic packet operations for the NFP6xxx
 */
#include <assert.h>
#include <nfp.h>
#include <stdint.h>
#include <types.h>

#include <nfp/me.h>
#include <nfp/mem_bulk.h>

#include <nfp6000/nfp_me.h>

#include <pkt/pkt.h>


/* MAC Egress Command format */
#define MAC_EGR_CMD_L3_CSUM_EN_shf 31
#define MAC_EGR_CMD_L3_CSUM_EN     (1 << MAC_EGR_CMD_L3_CSUM_EN_shf)
#define MAC_EGR_CMD_L4_CSUM_EN_shf 30
#define MAC_EGR_CMD_L4_CSUM_EN     (1 << MAC_EGR_CMD_L4_CSUM_EN_shf)

/* B0 and later allow the modification script at an offset up to 128B */
#if (__REVISION_MIN < __REVISION_B0)
#define MS_MAX_OFF  64
#else
#define MS_MAX_OFF  128
#endif

/*
 * This operation is supplied as a function and not a macro because
 * experience with the 'nfcc' compiler has shown that a simple,
 * single expression will not compile correctly and that the type
 * casting for the intermediate values must be done carefully.
 *
 * A 40-bit packet-address mode pointer in CTM is built as follows:
 *
 *  Bits[2;38] -- Must be 0b10 for "direct access"
 *  Bits[6;32] -- The island of the CTM. (can use '0' for the local island)
 *  Bits[1;31] -- Must be set to 1 to enable packet-addressing mode
 *  Bits[6;25] -- reserved
 *  Bits[9;16] -- The packet number of the CTM buffer
 *  Bits[2;14] -- reserved
 *  Bits[14;0] -- The offset within the CTM buffer
 *
 * Unfortunately, this is only partly documented in the NFP DB.
 */
__intrinsic __mem40 void *
pkt_ctm_ptr40(unsigned char isl, unsigned int pnum, unsigned int off)
{
    __gpr unsigned int lo;
    __gpr unsigned int hi;

    hi = 0x80 | isl;
    lo = 0x80000000u | (pnum << 16) | off;

    return (__mem40 void *)
        (((unsigned long long)hi << 32) | (unsigned long long)lo);
}


/*
 * A 32-bit packet-address mode pointer in CTM is built as follows:
 *
 *  Bits[1;31] -- Must be set to 1 to enable packet-addressing mode
 *  Bits[6;25] -- reserved
 *  Bits[9;16] -- The packet number of the CTM buffer
 *  Bits[2;14] -- reserved
 *  Bits[14;0] -- The offset within the CTM buffer
 *
 * Unfortunately, this is only partly documented in the NFP DB.
 *
 * Note that because the top 8 bits are left as 0 in the actual memory
 * command that uses this address, this type of address can only refer
 * to a packet in ones local CTM memory.
 */
__intrinsic __mem32 void *
pkt_ctm_ptr32(unsigned int pnum, unsigned int off)
{
    return (__mem32 void *)((1 << 31) | (pnum << 16) | off);
}


__intrinsic unsigned int
pkt_csum_read(void *src_buf, int off)
{
    unsigned int ret;

    /* This should probably also work for proper memory, but is not tested */
    ctassert(__is_in_reg_or_lmem(src_buf));
    ctassert(__is_ct_const(off));

    if (__is_in_lmem(src_buf))
        ret = *(__lmem unsigned int *)(((__lmem char *)src_buf) + off);
    else
        ret = *(__gpr unsigned int *)(((__gpr char *)src_buf) + off);

    return ret;
}

__intrinsic void
__pkt_status_read(unsigned char isl, unsigned int pnum,
                  __xread pkt_status_t *pkt_status, sync_t sync,
                  SIGNAL *sig_ptr)
{
    __gpr unsigned int addr;

    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);
    assert(pnum <= MAX_PKT_NUM_mask);

    addr = MAX_PKT_NUM_of(pnum);

    if (isl == 0) {
        if (sync == sig_done)
            __asm mem[packet_read_packet_status, *pkt_status, addr, 0, 1], \
                sig_done[*sig_ptr];
        else
            __asm mem[packet_read_packet_status, *pkt_status, addr, 0, 1], \
               ctx_swap[*sig_ptr];
    } else {
        /* When sending to a non-local island requires using a full 40-bit
         * address.  The top 8 bits of this address must be as follows:
         *   [2;38] -- Must be 0b10 to represent "direct access" locality.
         *   [6;32] -- The island of the CTM holding the packet.
         *
         * We put these 8 bits in the top 8 bits of addr_hi, and then use
         * the <<8 to put them into place in the 40-bit address. */
        __gpr unsigned int addr_hi = (isl | 0x80) << 24;

        if (sync == sig_done)
          __asm mem[packet_read_packet_status, *pkt_status, addr_hi, <<8, \
                    addr, 1], sig_done[*sig_ptr];
        else
          __asm mem[packet_read_packet_status, *pkt_status, addr_hi, <<8, \
                    addr, 1], ctx_swap[*sig_ptr];
    }
}

__intrinsic void
pkt_status_read(unsigned char isl, unsigned int pnum,
                __xread pkt_status_t *pkt_status)
{
    SIGNAL add_thread_sig;

    __pkt_status_read(isl, pnum, pkt_status, ctx_swap, &add_thread_sig);
}


__intrinsic size_t
pkt_ctm_data_size(unsigned int pkt_len, unsigned int pkt_offset,
                  enum PKT_CTM_SIZE ctm_buf_size)
{
    size_t tbytes;
    size_t ret;

    /* Calculate total bytes in CTM buffer */
    tbytes = 256 << ctm_buf_size;
    /* Subtract used bytes at the front */
    tbytes -= pkt_offset;

    if (pkt_len > tbytes)
        ret = tbytes;
    else
        ret = pkt_len;

    return ret;
}


__intrinsic size_t
pkt_emem_data_size(unsigned int pkt_len, unsigned int pkt_offset,
                   enum PKT_CTM_SIZE ctm_buf_size)
{
    size_t tbytes;
    size_t ret;

    /* Calculate total bytes in CTM buffer */
    tbytes = 256 << ctm_buf_size;
    /* Subtract used bytes at the front */
    tbytes -= pkt_offset;

    if (pkt_len > tbytes)
        ret = (pkt_len - tbytes);
    else
        ret = 0;

    return ret;
}


__intrinsic void
__pkt_mac_egress_cmd_write(__mem40 void *pbuf, unsigned char off,
                           int l3_csum_ins, int l4_csum_ins,
                           __xwrite uint32_t *xcmd, sync_t sync, SIGNAL *sig)
{
    /* Check for an illegal packet offset for MAC egress command word */
    __RT_ASSERT(off >= 20);

    /* Write a MAC egress command word for TCP/UDP checksum insertion */
    (*xcmd) = (l3_csum_ins ? MAC_EGR_CMD_L3_CSUM_EN : 0)
              | (l4_csum_ins ? MAC_EGR_CMD_L4_CSUM_EN : 0);

    __mem_write32(xcmd, (__mem40 uint8_t *) pbuf + off - 4, sizeof(*xcmd),
                  sizeof(*xcmd), sync, sig);

    return;
}


__intrinsic void
pkt_mac_egress_cmd_write(__mem40 void *pbuf, unsigned char off,
                         int l3_csum_ins, int l4_csum_ins)
{
    SIGNAL sig;
    __xwrite uint32_t cmd;

    __pkt_mac_egress_cmd_write(pbuf, off, l3_csum_ins, l4_csum_ins, &cmd,
                               ctx_swap, &sig);
    return;
}

__intrinsic struct pkt_ms_info
pkt_gen_msi_info(unsigned char off)
{
    __gpr struct pkt_ms_info msi;

    ctassert(__is_ct_const(off));

    /* Check for an illegal packet offset for direct modification script */
    __RT_ASSERT((off >= 16) && (off <= (MS_MAX_OFF + 16)));

    /* Check if a no-op modification script is possible */
    if (off <= MS_MAX_OFF && (off & 7) == 0) {
        /* Write a no-op modification script right before the packet start */
        msi.off_enc = (off >> 3) - 2;
    } else {
        /* Determine a starting offset for the 8-byte modification script that
         * is closest to the start of packet, that is 8-byte aligned, and that
         * is still within the 120-byte (56-byte for A0) offset limit */
        unsigned char ms_off = MS_MAX_OFF - 8;

        if (off < MS_MAX_OFF)
            ms_off = (off & ~0x7) - 8;

        /* write a delete modification script to remove any excess bytes */
        msi.off_enc = (ms_off >> 3) - 1;
    }

    /* Set the length adjustment to point to the start of packet. */
    msi.len_adj = off;

    return msi;
}

__intrinsic struct pkt_ms_info
__pkt_msd_write(__mem40 void *pbuf, unsigned char off,
                __xwrite uint32_t xms[2], size_t size, sync_t sync,
                SIGNAL *sig)
{
    __gpr struct pkt_ms_info msi;

    /* Check for an illegal packet offset for direct modification script */
    __RT_ASSERT((off >= 16) && (off <= (MS_MAX_OFF + 16)));

    /* Check if a no-op modification script is possible */
    if (off <= MS_MAX_OFF && (off & 7) == 0) {
        /* Write a no-op modification script right before the packet start */
        msi.off_enc = (off >> 3) - 2;

        xms[0] = (NBI_PM_TYPE(NBI_PM_TYPE_DIRECT) |
                  NBI_PM_OPCODE(NBI_PKT_MS_INSTRUCT_NOOP, 0, 0));
        xms[1] = 0;

        __mem_write64(xms, (__mem40 unsigned char *)pbuf + off - 8, size,
                      size, sync, sig);
    } else {
        /* Determine a starting offset for the 8-byte modification script that
         * is closest to the start of packet, that is 8-byte aligned, and that
         * is still within the 120-byte (56-byte for A0) offset limit */
        unsigned char ms_off = MS_MAX_OFF - 8;

        if (off < MS_MAX_OFF)
            ms_off = (off & ~0x7) - 8;

        /* write a delete modification script to remove any excess bytes */
        msi.off_enc = (ms_off >> 3) - 1;

        xms[0] = (NBI_PM_TYPE(NBI_PM_TYPE_DIRECT) |
                  NBI_PM_OPCODE(NBI_PKT_MS_INSTRUCT_DELETE,
                                off - ms_off - 8, 0));
        xms[1] = 0;

        __mem_write64(xms, (__mem40 unsigned char *) pbuf + ms_off, size,
                      size, sync, sig);
    }

    /* Set the length adjustment to point to the start of packet. */
    msi.len_adj = off;

    return msi;
}


__intrinsic struct pkt_ms_info
pkt_msd_write(__mem40 void *pbuf, unsigned char off)
{
    SIGNAL sig;
    __xwrite uint32_t ms[2];

    return __pkt_msd_write(pbuf, off, ms, sizeof(ms), ctx_swap, &sig);
}


__intrinsic void
__pkt_nbi_recv_with_hdrs(__xread void *meta, size_t msize, uint32_t off,
                         sync_t sync, SIGNAL *sig)
{
    struct nfp_mecsr_prev_alu ind;
    unsigned int addr = (off >> 2);
    unsigned int count = (msize >> 2);

    ctassert(__is_ct_const(sync));
    ctassert(sync == ctx_swap || sync == sig_done);
    ctassert(__is_ct_const(msize));
    ctassert(off % 4 == 0);
    ctassert(msize % 4 == 0);
    ctassert(msize >= 24);
    ctassert(msize <= 128);

    if (msize <= 64) {
        if (sync == ctx_swap)
            __asm mem[packet_add_thread, *meta, addr, 0, count], ctx_swap[*sig];
        else
            __asm mem[packet_add_thread, *meta, addr, 0, count], sig_done[*sig];
    } else {
        ind.__raw = 0;
        ind.ov_len = 1;
        ind.length = count - 1;

        if (sync == ctx_swap) {
            __asm alu[--, --, B, ind.__raw];
            __asm mem[packet_add_thread, *meta, addr, 0, __ct_const_val(count)], ctx_swap[*sig], indirect_ref;
        } else {
            __asm alu[--, --, B, ind.__raw];
            __asm mem[packet_add_thread, *meta, addr, 0, __ct_const_val(count)], sig_done[*sig], indirect_ref;
        }
    }
}


__intrinsic void
pkt_nbi_recv_with_hdrs(__xread void *meta, size_t msize, uint32_t off)
{
    SIGNAL add_thread_sig;
    __pkt_nbi_recv_with_hdrs(meta, msize, off, ctx_swap, &add_thread_sig);
}


__intrinsic void
__pkt_nbi_recv(__xread void *meta, size_t msize, sync_t sync, SIGNAL *sig)
{
    __pkt_nbi_recv_with_hdrs(meta, msize, 0, sync, sig);
}


__intrinsic void
pkt_nbi_recv(__xread void *meta, size_t msize)
{
    pkt_nbi_recv_with_hdrs(meta, msize, 0);
}


__intrinsic void
pkt_nbi_send(unsigned char isl, unsigned int pnum,
             __gpr const struct pkt_ms_info *msi,
             unsigned int len, unsigned int nbi, unsigned int txq,
             unsigned int seqr, unsigned int seq,
             enum PKT_CTM_SIZE ctm_buf_size)
{
    __gpr unsigned int addr_hi;
    __gpr unsigned int addr_lo;
    __gpr struct pkt_iref_csr0 csr0;
    __gpr struct pkt_iref_palu palu;

    /*
     * The "packet ready" commands require a special encoding in the address
     * field, which include the length, or _ending_offset_, of the packet.
     * To get the ending offset, we add the packet length to the starting
     * offset of the packet, including any MAC egress command word.  The
     * starting offset is encoded in the 'msi->len_adj' field.
     *
     * See NFP 6xxx Databook Section 9.2.2.7.9 "Packet Processing Complete
     * Target Command and Packet Ready Master Command"
     */
    addr_hi = PKT_READY_ADDR_HI_FIELDS(nbi, isl, __ME());
    addr_lo = PKT_READY_ADDR_LO_FIELDS(pnum, len + msi->len_adj);

    csr0.__raw = 0;
    csr0.seqr = seqr;
    csr0.seq = seq;
    local_csr_write(local_csr_cmd_indirect_ref_0, csr0.__raw);

    /*
     * XXX We clear the reserved bits of the previous ALU instruction
     * structure by assigning the whole 32-bit value the MAGIC constant.
     * This constant starts at bit 0 of the structure anyways.
     */
    palu.__raw = PKT_IREF_PALU_MAGIC;
    palu.nbi = (unsigned int)ctm_buf_size;
    palu.txq = txq;
    palu.ms_off = msi->off_enc;

    __asm {
        alu[--, --, B, palu.__raw];
        nbi[packet_ready_unicast, --, addr_hi, <<8, addr_lo], indirect_ref;
    }
}


__intrinsic void
pkt_nbi_send_dont_free(unsigned char isl, unsigned int pnum,
                       __gpr const struct pkt_ms_info *msi,
                       unsigned int len, unsigned int nbi, unsigned int txq,
                       unsigned int seqr, unsigned int seq,
                       enum PKT_CTM_SIZE ctm_buf_size)
{
    __gpr unsigned int addr_hi;
    __gpr unsigned int addr_lo;
    __gpr struct pkt_iref_csr0 csr0;
    __gpr struct pkt_iref_palu palu;

    /*
     * The "packet ready" commands require a special encoding in the address
     * field, which include the length, or _ending_offset_, of the packet.
     * To get the ending offset, we add the packet length to the starting
     * offset of the packet, including any MAC egress command word.  The
     * starting offset is encoded in the 'msi->len_adj' field.
     *
     * See NFP 6xxx Databook Section 9.2.2.7.9 "Packet Processing Complete
     * Target Command and Packet Ready Master Command"
     */
    addr_hi = PKT_READY_ADDR_HI_FIELDS(nbi, isl, __ME());
    addr_lo = PKT_READY_ADDR_LO_FIELDS(pnum, len + msi->len_adj);

    csr0.__raw = 0;
    csr0.seqr = seqr;
    csr0.seq = seq;
    local_csr_write(local_csr_cmd_indirect_ref_0, csr0.__raw);

    /*
     * XXX We clear the reserved bits of the previous ALU instruction
     * structure by assigning the whole 32-bit value the MAGIC constant.
     * This constant starts at bit 0 of the structure anyways.
     */
    palu.__raw = PKT_IREF_PALU_MAGIC;
    palu.nbi = (unsigned int)ctm_buf_size;
    palu.txq = txq;
    palu.ms_off = msi->off_enc;

    __asm {
        alu[--, --, B, palu.__raw];
        nbi[packet_ready_multicast_dont_free, --, addr_hi, <<8, addr_lo], \
            indirect_ref;
    }
}


__intrinsic void
pkt_nbi_drop_seq(unsigned char isl, unsigned int pnum,
                 __gpr const struct pkt_ms_info *msi,
                 unsigned int len, unsigned int nbi, unsigned int txq,
                 unsigned int seqr, unsigned int seq,
                 enum PKT_CTM_SIZE ctm_buf_size)
{
    __gpr unsigned int addr_hi;
    __gpr unsigned int addr_lo;
    __gpr struct pkt_iref_csr0 csr0;
    __gpr struct pkt_iref_palu palu;

    /*
     * The "packet ready" commands require a special encoding in the address
     * field, which include the length, or _ending_offset_, of the packet.
     * To get the ending offset, we add the packet length to the starting
     * offset of the packet, including any MAC egress command word.  The
     * starting offset is encoded in the 'msi->len_adj' field.
     *
     * See NFP 6xxx Databook Section 9.2.2.7.9 "Packet Processing Complete
     * Target Command and Packet Ready Master Command"
     */
    addr_hi = PKT_READY_ADDR_HI_FIELDS(nbi, isl, __ME());
    addr_lo = PKT_READY_ADDR_LO_FIELDS(pnum, len + msi->len_adj);

    /* XXX cheat and initialize the structure to 0 by assigning the
     * sequencer to the whole value. */
    csr0.__raw = seqr;
    csr0.seq = seq;
    local_csr_write(local_csr_cmd_indirect_ref_0, csr0.__raw);

    /* XXX cheat by knowing that the least significant byte is the 'magic' */
    /* byte saves an extra initialization step. */
    palu.__raw = PKT_IREF_PALU_MAGIC;
    palu.nbi = (unsigned int)ctm_buf_size;
    palu.txq = txq;
    palu.ms_off = msi->off_enc;

    __asm {
        alu[--, --, B, palu.__raw];
        nbi[packet_ready_drop, --, addr_hi, <<8, addr_lo], indirect_ref;
    }
}

__intrinsic void
pkt_ctm_free(unsigned int isl, unsigned int pnum)
{
    __gpr unsigned int addr_hi = 0;

    if (isl != 0)
        addr_hi = (0x80 | isl) << 24;

    __asm mem[packet_free, --, addr_hi, <<8, pnum];
}

__intrinsic unsigned int
pkt_ctm_alloc(__cls struct ctm_pkt_credits *credits,
              unsigned char isl, enum PKT_CTM_SIZE size,
              unsigned char alloc_internal,
              int replenish_credits)
{
    __gpr unsigned int addr_hi = 0;
    __gpr unsigned int addr_lo = 0;
    __gpr unsigned int ind;
    __xread struct pe_pkt_alloc_res pe_res;
    SIGNAL sig_alloc;
    __xrw struct ctm_pkt_credits credits_update;
    SIGNAL sig_cls;
    __gpr unsigned int pnum;

    /* Allocate one packet credit and one buffer credit if requested */
    if (alloc_internal)
        pkt_ctm_get_credits(credits, 1, 1, replenish_credits);

    if (isl != 0)
        addr_hi = (0x80 | isl) << 24;

    ind = size << NFP_MECSR_PREV_ALU_LENGTH_shift;
    __asm {
        alu[ind, ind, OR, 1, <<NFP_MECSR_PREV_ALU_OV_LEN_bit];
        mem[packet_alloc_poll, pe_res, addr_hi, <<8, addr_lo, 1],\
            indirect_ref, ctx_swap[sig_alloc];
    }

    /* An all 1s response indicates failure to allocate */
    if (pe_res.__raw == 0xffffffff) {
        pnum = 0xffffffff;
    } else {
        pnum = pe_res.pnum;
        if((pe_res.pkt_credit > 0) || (pe_res.buf_credit > 0)) {
            /* credits where returned add them back to the buckets */
            credits_update.pkts = pe_res.pkt_credit;
            credits_update.bufs = pe_res.buf_credit;
            __asm cls[add, credits_update, credits, 0, 2], \
                ctx_swap[sig_cls];
        }
    }

    return pnum;
}

__intrinsic void
pkt_ctm_init_credits(__cls struct ctm_pkt_credits *credits,
                     unsigned int pkt_credits, unsigned int buf_credits)
{
    credits->pkts = pkt_credits;
    credits->bufs = buf_credits;
}

__intrinsic void
pkt_ctm_poll_pe_credit(__cls struct ctm_pkt_credits *credits)
{
    __xread struct pe_credit_get_res pe_res;
    __xrw struct ctm_pkt_credits credits_update;
    SIGNAL sig_pe;
    SIGNAL sig_cls;
    __gpr unsigned int master = 0;

    /* poll for returned credits */
    __asm mem[packet_credit_get, pe_res, 0, <<8, master, 1], ctx_swap[sig_pe];

    /* If needed, update the credits management structure */
    if((pe_res.pkt_credit > 0) || (pe_res.buf_credit > 0)) {
        credits_update.pkts = pe_res.pkt_credit;
        credits_update.bufs = pe_res.buf_credit;

        __asm cls[add, credits_update, credits, 0, 2], ctx_swap[sig_cls];
    }
}

__intrinsic void
pkt_ctm_get_credits(__cls struct ctm_pkt_credits *credits,
                    unsigned int pkt_credits, unsigned int buf_credits,
                    int replenish_credits)
{
    __xrw struct ctm_pkt_credits credits_update;
    __xwrite struct ctm_pkt_credits credits_add_back;
    SIGNAL sig_cls;

    credits_update.pkts = pkt_credits;
    credits_update.bufs = buf_credits;
    __asm cls[test_subsat, credits_update, credits, 0, 2], ctx_swap[sig_cls];

    /* Check the returned number of credits (the value before the sub) */
    while ((credits_update.pkts < pkt_credits) ||
           (credits_update.bufs < buf_credits)) {

        /* Allow other threads to run */
        sleep(PKT_CTM_CRED_ALLOC_FAIL_SLEEP);

        /* Either the packet or the buffer credits have run out
         * return the credit(s) acquired, and wait for both */
        if (credits_update.pkts >= pkt_credits)
            /* Add back what we asked for */
            credits_add_back.pkts = pkt_credits;
        else
            /* Since the value in the management struct saturated
             * to 0 we should add back the value before the sub */
            credits_add_back.pkts = credits_update.pkts;

        if (credits_update.bufs >= buf_credits)
            credits_add_back.bufs = buf_credits;
        else
            credits_add_back.bufs = credits_update.bufs;

        __asm cls[add, credits_add_back, credits, 0, 2], ctx_swap[sig_cls];

        if (replenish_credits)
            pkt_ctm_poll_pe_credit(credits);

        /* Try again */
        credits_update.pkts = pkt_credits;
        credits_update.bufs = buf_credits;
        __asm cls[test_subsat, credits_update, credits, 0, 2],  \
            ctx_swap[sig_cls];
    }
}
