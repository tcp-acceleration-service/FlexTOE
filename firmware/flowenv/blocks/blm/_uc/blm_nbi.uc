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
 * @file        blm_nbi.uc
 * @brief
 */

#ifndef __BLM_API_UC__
#define __BLM_API_UC__

#include <stdmac.uc>
#include <ring_utils.uc>

/** @file blm_api.uc Buffer List Manager Macros
 * @addtogroup blm_api Buffer List Manager
 * @{
 *
 * @name Buffer List Manager Macros
 * @{
 *
 */

// API Quick Reference:
// blm_create_global_bpool(nbuffers, rtype, rnum, bbase, bsize)
// blm_cfg_blq(NbiNum, blq, size, head, tail, direction)
// blm_cfg_blq_evnts(NbiNum, blq, direction, ethres, estatus, enotfull)
// blm_init_blq_mu(NbiNum, blq, mu_base, buf_size, num_buffers, head)
// blm_init_blq(NbiNum, blq, grnum, rtype, nbuffers, head)
// blm_blq_status(NbiNum, blq, direction, oxfer, sig_name, sig_action)
// blm_poptome(NbiNum, blq, oxfer, n)
// blm_poptomem(NbiNum, blq, target, addr, n)
// blm_push_mem2dma(NbiNum, blq, addr, offset, n, addToHead)
// blm_recycle(snbi, sblq, dnbi, dblq, n)
// blm_bpool_config(NbiNum, bpool, DropEn, CtmOffset, PriBlist, SecBlist, SplitLength, BpeHead)
// blm_bpe_config(NbiNum, BpeNum, CtmNum, PktCredit, BufCredit, ChainEnd)
// blm_cfg_dma(NbiNum, CtmPollSearchEna, RateLimitEnable, CtmPollIntvl, CtmPollEna, ErrMask)
// blm_set_dma_credits(NbiNum, credit0, credit1, credit2, credit3, thrs0, thrs1, thrs2, thrs3)
// blm_set_dma_credit_rate(NbiNum, cr0, cr1, cr2, cr3)

#define xpb_csr_addr(island, slave_idx, tgt_dev, csr_offset)    ((0 <<31)       | \
                                                                (0 <<30)        | \
                                                                (8 <<24)        | \
                                                                (island <<24)   | \
                                                                (slave_idx <<22)| \
                                                                (tgt_dev <<16)  | \
                                                                (csr_offset))

#define NBI_CPP_ADDRESS(NbiNum)          ((0x000000)   | (NbiNum <<30))
#define NBI_DMA_CPP_BLQ_OFFSET(blq)      ((0x008000)   | (blq <<3))
#define NBI_TM_CPP_BLQ_OFFSET(blq)       ((0x288000)   | (blq <<3)) /* NBITMCPP base = 0x200000 + TM BLQ offset = 0x88000 */
#define NBI_TM_CPP_BLQ_RD_OFFSET(blq)    ((0x280000)   | (blq <<7)) /* NBITMCPP base = 0x200000 + TM BLQ_RD CSR offset = 0x90000 */

/// @cond INTERNAL_MACROS
/**
 */
#macro _blm_immed32(_reg, val)
    #ifdef __L16
        #undef __L16
    #endif
    #ifdef __H16
        #undef __H16
    #endif
    #define_eval __L16        (val & 0xffff)
    #define_eval __H16        ((val >>16) & 0xffff)
    immed[_reg, __L16]
    #if __H16 > 0
        immed_w1[_reg, __H16]
    #endif
    #undef __L16
    #undef __H16
#endm /* _blm_immed32 */

/**
 *  Wait for signal to be asserted, optionally swapping out
 *
 *  @param WAIT_TYPE  THD_SWAP/THD_SPIN/NONE
 */
#macro _blm_signal_wait(wait_sig, WAIT_TYPE)
.begin

    #if( streq('WAIT_TYPE', 'THD_SWAP') )
        ctx_arb[wait_sig]
    #elif( streq('WAIT_TYPE', 'THD_SPIN') )
        wait_signal_spin_loop#:
            br_!signal[wait_sig, wait_signal_spin_loop#]
    #endif

.end
#endm /* _blm_signal_wait */

/**
 *  Get NBI CPP base address
 *
 *  @param oaddr        Out GPR which contains the NBI CPP base address
 *  @param NbiNum       NBI number. 0: NBI Island 8, 1: NBI Island 9
 */
#macro GET_NBI_CPP_BASE_ADDRESS(oaddr, NbiNum)
    #if (isnum(NbiNum))
        #define_eval __addr    NBI_CPP_ADDRESS(NbiNum)
        _blm_immed32(oaddr, __addr)
    #else
        #define_eval __addr    NBI_CPP_ADDRESS(0)
        _blm_immed32(oaddr, __addr)
        alu[oaddr, oaddr, OR, NbiNum, <<30]
    #endif
    #undef __addr
#endm /* NBI_CPP_ADDRESS */

#macro GET_NBI_DMA_CPP_BLQ_OFFSET(ooffset, blq)
    #if (isnum(blq))
        #define_eval __offset  NBI_DMA_CPP_BLQ_OFFSET(blq)
        _blm_immed32(ooffset, __offset)
    #else
        #define_eval __offset  NBI_DMA_CPP_BLQ_OFFSET(0)
        _blm_immed32(ooffset, __offset)
        alu[ooffset, ooffset, OR, blq, <<3]
    #endif
    #undef __offset
#endm /* GET_NBI_DMA_CPP_BLQ_OFFSET */

#macro GET_NBI_TM_CPP_BLQ_OFFSET(ooffset, blq)
    #if (isnum(blq))
        #define_eval __offset  NBI_TM_CPP_BLQ_OFFSET(blq)
        _blm_immed32(ooffset, __offset)
    #else
        #define_eval __offset  NBI_TM_CPP_BLQ_OFFSET(0)
        _blm_immed32(ooffset, __offset)
        alu[ooffset, ooffset, OR, blq, <<3]
    #endif
    #undef __offset
#endm /* GET_NBI_TM_CPP_BLQ_OFFSET */

#macro GET_NBI_TM_CPP_BLQ_RD_OFFSET(ooffset, blq)
    #if (isnum(blq))
        #define_eval __offset  NBI_TM_CPP_BLQ_RD_OFFSET(blq)
        _blm_immed32(ooffset, __offset)
    #else
        #define_eval __offset  NBI_TM_CPP_BLQ_RD_OFFSET(0)
        _blm_immed32(ooffset, __offset)
        alu[ooffset, ooffset, OR, blq, <<7]
    #endif
    #undef __offset
#endm /* GET_NBI_TM_CPP_BLQ_RD_OFFSET */

/**
 *  Generic nbi XPB read macro for xpb_read/xpb_write
 *
 */
#macro _nbi_xpb_op(NbiNum, op, xfer, len, slave, dev, csr_offset, sig, sig_action)
.begin
    #define_eval __addr            xpb_csr_addr(NbiNum, slave, dev, csr_offset)
    addr          =   __addr
    #if (streq('op', 'read'))
        ct[xpb_read, xfer, addr, 0, len], sig_done[sig]
    #elif (streq('op', 'write'))
        ct[xpb_write, xfer, addr, 0, len], sig_done[sig]
    #endif
    #undef __addr
    #undef __nbi_xpb_csr_offset
.end
#endm /* _nbi_xpb_op */

/**
 *  NBI XPB read macro
 *
 */
#macro nbi_xpb_read(NbiNum, oxfer, len, slave, dev, csr_offset, sig, sig_action)
    _nbi_xpb_op(NbiNum, read, oxfer, len, slave, dev, csr_offset, sig, sig_action)
#endm /* nbi_xpb_read */

/**
 *  NBI XPB write macro
 *
 */
#macro nbi_xpb_write(NbiNum, in_xfer, len, slave, dev, csr_offset, sig, sig_action)
    _nbi_xpb_op(NbiNum, write, in_xfer, len, slave, dev, csr_offset, sig, sig_action)
#endm /* nbi_xpb_write */
/// @endcond


/**
 * Sample NBI DMA initialization sequence
 */
#macro nbi_dma_init()
.begin
    // blm_init_blq_mu(NbiNum, blq, mu_base, buf_size, num_buffers, head)
    blm_init_blq_mu(0, 0, 0x13800000, 2048, 1023, 0) // IMU_28: 0x9c00000000 >>11
    blm_init_blq_mu(0, 1, 0x13800400, 2048, 1023, 0x400) // (0x9c00000000 + (2048*1024)) >>11
    blm_init_blq_mu(0, 2, 0x13800800, 2048, 1023, 0x800)
    blm_init_blq_mu(0, 3, 0x13800c00, 2048, 1023, 0xc00)
    //blm_cfg_blq(NbiNum, blq, size, head, tail, direction)
    blm_cfg_blq(0, 0, 1, 0, 0x3fe, ingress)
    blm_cfg_blq(0, 1, 1, 0x400, 0x7fe, ingress)
    blm_cfg_blq(0, 2, 1, 0x800, 0xbfe, ingress)
    blm_cfg_blq(0, 3, 1, 0xc00, 0xffe, ingress)
    blm_cfg_blq(0, 0, 1, 0, 0x0, egress)
    blm_cfg_blq(0, 1, 1, 0x400, 0x400, egress)
    blm_cfg_blq(0, 2, 1, 0x800, 0x800, egress)
    blm_cfg_blq(0, 3, 1, 0xc00, 0xc00, egress)
    //blm_cfg_dma(NbiNum, CtmPollSearchEna, RateLimitEnable, CtmPollIntvl, CtmPollEna, ErrMask)
    blm_cfg_dma(0, 1, 0, 3, 1, 0)
    //blm_cfg_blq_evnts(NbiNum, blq, direction, ethres, estatus, enotfull)
    /*
    blm_cfg_blq_evnts(0, 0, ingress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 1, ingress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 2, ingress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 3, ingress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 0, egress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 1, egress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 2, egress, 1, 1, 1)
    blm_cfg_blq_evnts(0, 3, egress, 1, 1, 1)
    */
    //blm_set_dma_credits(NbiNum, credit0, credit1, credit2, credit3, thrs0, thrs1, thrs2, thrs3)
    blm_set_dma_credits(0, 3, 3, 3, 3, 3, 3, 3, 3)
    //blm_set_dma_credit_rate(NbiNum, cr0, cr1, cr2, cr3)
    blm_set_dma_credit_rate(0, 0, 0, 0, 0)
    //blm_bpool_config(NbiNum, bpool, DropEn, CtmOffset, PriBlist, SecBlist, SplitLength, BpeHead)
    blm_bpool_config(0, 0, 0, 1, 0, 1, 2, 0)
    //blm_bpe_config(NbiNum, BpeNum, CtmNum, PktCredit, BufCredit, ChainEnd)
    blm_bpe_config(0, 0, 32, 512, 64, 0)
    blm_bpe_config(0, 1, 33, 512, 64, 1)
.end
#endm /* nbi_dma_init */

/** This macro pushes 'n' buffer pointers onto specified blq# to Ingress NBI DMA.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param ixfer         In xfer array. Array of xfer regs should map correctly to number of buffers. ixfer[n]
 * @param n             Number of buffers to push
 * @param sig_name      Signal to use
 * @param sig_action    SIG_NONE or SIG_WAIT
 */
#macro _blm_push(NbiNum, blq, ixfer, addr, offset, n, sig_name, sig_action)
.begin

    nbi[write, ixfer, addr, <<8, offset, n], sig_done[sig_name]
    #if (streq('sig_action', 'SIG_WAIT'))
        ctx_arb[sig_name]
    #endif
.end
#endm /* blm_push */

/** This macro creates a global buffer pool ring and fills it with buffers
 *
 * @param nbuffers      Number of buffers
 * @param bsize         Size of each buffer (at 2KB boundary, in bytes)
 * @param bbase         40-bit MU start address for buffers right shifted by 11-bits - IMU or EMU only (byte address)
                        Example: If IMU base address(island 28) = 0x9c00100000, bbase = 0x9c00100000 >>11 = 0x13800200
 * @param rtype         Type of ring: DDR_RING, CLS_RING, CTM_RING
 *                      NOTE: Ring must be initailized prior to calling this macro. Ring size >= nbuffers
 * @param rnum          Ring number
 */
#macro blm_create_global_bpool(nbuffers, bsize, bbase, rtype, rnum)
.begin
    .reg $pkt_buf[2]
    .reg tmp_pkt_buf
    .reg num_pkt_buffers
    .reg tmp_buf_size
    .sig dram_pkt_sig
    .xfer_order $pkt_buf

    #if (isnum(bbase))
        immed32(tmp_pkt_buf, bbase)
    #else
        alu[tmp_pkt_buf, --, b, bbase]
    #endif
    #if (isnum(bsize))
       #define_eval __INCR  (bsize >> 11)
       immed32(tmp_buf_size, __INCR)
       #undef __INCR
    #else
       alu[tmp_buf_size, --, b, bsize,>>11]
    #endif
    immed32(num_pkt_buffers, 0)
    alu[$pkt_buf[0], --, b, tmp_pkt_buf]

    .while (num_pkt_buffers < nbuffers)
        ru_enq_to_ring($pkt_buf, rtype, 1, rnum, dram_pkt_sig, SIG_WAIT, PUT_BLIND)
        alu[num_pkt_buffers, num_pkt_buffers, +, 1]
        alu[tmp_pkt_buf, tmp_pkt_buf, +, tmp_buf_size]
        alu[$pkt_buf[0], --, b, tmp_pkt_buf]
    .endw

.end
#endm /* blm_create_global_bpool */

/** This macro configures a buffer list queue in NBI DMA or NBI TM
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           NBI Buffer list queue number, 0,1,2,or 3
 * @param size          Size of the buffer list queue, valid values: 0:512, 1:1K, 2:2K, 3:4K
 * @param head          Head pointer
 * @param tail          Tail pointer
 * @param direction     Ingress or Egress
 */
#macro blm_cfg_blq(NbiNum, blq, size, head, tail, direction)
.begin
    .reg $x[2]
    .reg addr
    .reg _head _tail
    .reg offset
    .reg val
    .sig nbi_sig
    .xfer_order $x

    #if (isnum(NbiNum))
        #if ((NbiNum < 0) || (NbiNum > 1))
            #error "Not a valid NbiNum. NbiNum = [0|1]."
        #endif
    #else
        #error "NbiNum must be constant."
    #endif

    #if (isnum(blq))
        #if ((blq < 0) || (blq > 3))
            #error "Not a valid blq. blq = [0...3]."
        #endif
    #else
        #error "blq must be constant."
    #endif

    #if (isnum(size))
        #if ((size < 0) || (size > 3))
            #error "Not a valid size for blq. size = [0...3]."
        #endif
    #else
        #error "size must be constant."
    #endif

    #if(streq('direction',"ingress"))
        GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
        GET_NBI_DMA_CPP_BLQ_OFFSET(offset, blq)
    #elif(streq('direction',"egress"))
        GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
        GET_NBI_TM_CPP_BLQ_OFFSET(offset, blq)
    #else
        #error "Invalid direction. direction = [ingress|egress]."
    #endif

    #if (isnum(head))
        _blm_immed32(_head, head)
    #else
        alu[_head, --, b, head]
    #endif
    #if (isnum(tail))
        _blm_immed32(_tail, tail)
    #else
        alu[_tail, --, b, tail]
    #endif

    alu[val, _tail, OR, _head, <<12]
    alu[$x[0], --, b, val]
    alu[val, --, b, size, <<4]
    alu[$x[1], --, b, val]

    nbi[write, $x[0], addr, <<8, offset, 1], sig_done[nbi_sig]
    wait_blq_cfg#:
      br_!signal[nbi_sig, wait_blq_cfg#]
.end
#endm /* blm_cfg_blq */

/** This macro configures events for given buffer list queue.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           NBI Buffer list queue number, 0,1,2,or 3
 * @param direction     Ingress or Egress
 * @ethres              Event threshold, at which NBI generates event. Valid values: 0:16, 1:32, 2:64, 3:128
 * @estatus             Enables BLQ Status Events when set. 0:Disable, 1:Enable
 * @enotfull            Enables the generation of BLQ Events when the queue transitions from full to not full. 0:Disable, 1:Enable
 */
#macro blm_cfg_blq_evnts(NbiNum, blq, direction, ethres, estatus, enotfull)
.begin
    .reg $x[1]
    .reg addr
    .reg val
    .reg n
    .reg tmp
    .sig nbi_sig
    .xfer_order $x

    #if (isnum(ethres))
        #if ((ethres < 0) || (ethres > 3))
          #error "Not a valid value for threshold. range = [0...3]."
        #endif
    #else
        #error "ethres must be constant."
    #endif

    #if (streq('direction','ingress'))
        #define_eval __addr xpb_csr_addr(NbiNum, 0, 16, 0xc)
        _blm_immed32(addr, __addr)
    #elif (streq('direction','egress'))
        #define_eval __addr xpb_csr_addr(NbiNum, 0, 20, 0x8)
        _blm_immed32(addr, __addr)
    #else
        #error "Invalid direction. direction = [ingress|egress]."
    #endif
    #undef __addr

    ct[xpb_read, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_nbi_rd0#:
      br_!signal[nbi_sig, wait_nbi_rd0#]

    alu[val, --, b, $x[0]]
    alu[n, --, b, blq]

    // Clear BLQEventStatus
    alu[--, n, OR, 0]
    alu[val, val, AND~, 1, <<indirect]
    // clear BLQEventNotEmpty
    alu[n, n, +, 4]
    alu[--, n, OR, 0]
    alu[val, val, AND~, 1, <<indirect]
    // clear EventThreshold
    immed[tmp, 0x3000]
    alu[n, --, b, blq, <<1]
    alu[--, n, OR, 0]
    alu[val, val, AND~, tmp, <<indirect]

    // Config BLQEventStatus
    alu[n, --, b, blq]
    alu[--, n, OR, 0]
    alu[val, val, OR, estatus, <<indirect]

    // Config BLQEventNotEmpty
    alu[n, n, +, 4]
    alu[--, n, OR, 0]
    alu[val, val, OR, enotfull, <<indirect]

    // Config EventThreshold
    alu[n, --, b, blq, <<1]
    alu[tmp, --, b, ethres, <<12]
    alu[--, n, OR, 0]
    alu[val, val, OR, tmp, <<indirect]

    alu[$x[0], --, b, val]

    ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_nbi_wr0#:
      br_!signal[nbi_sig, wait_nbi_wr0#]
.end
#endm /* blm_cfg_blq_evnts */

/** This macro initializes a BLQ with given memory unit buffers - only IMU or EMU
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param mu_base       Memory unit start address for buffers (byte address and right shifted by 11-bits)
 * @param bsize         Buffer size @ 2K boundaries (in bytes)
 * @nbuffers            Number of buffers (not to exceed BLQ size as configured using API: 'blm_cfg_blq')
 * @head                Head pointer for blq. Must be multiple of 512 and in range [0, 4096].
 */
#macro blm_init_blq_mu(NbiNum, blq, mu_base, bsize, nbuffers, head)
.begin
    .reg $pkt_buf[2]
    .reg tmp_pkt_buf
    .reg num_pkt_buffers
    .reg tmp_buf_size
    .reg addr
    .reg offset
    .sig dram_pkt_sig
    .sig blsig
    .xfer_order $pkt_buf

    #if (isnum(mu_base))
       immed32(tmp_pkt_buf, mu_base)
    #else
        alu[tmp_pkt_buf, --, b, imu_base]
    #endif
    #if (isnum(bsize))
       #define_eval __INCR  (bsize >> 11)
       immed32(tmp_buf_size, __INCR)
       #undef __INCR
    #else
       alu[tmp_buf_size, --, b, bsize,>>11]
    #endif

    #if (isnum(head))
        _blm_immed32(offset, head)
    #else
        alu[offset, --, b, head]
    #endif
    alu[offset, --, b, offset, <<3]
    alu[$pkt_buf[1], --, b, 0]
    GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
    immed[num_pkt_buffers, 0]
    alu[$pkt_buf[0], --, b, tmp_pkt_buf]

    .while (num_pkt_buffers < nbuffers)
        nbi[write, $pkt_buf[0], addr, <<8, offset, 1], sig_done[nbi_sig]
        wait_nbi_wr_sig#:
          br_!signal[nbi_sig, wait_nbi_wr_sig#]
        alu[num_pkt_buffers, num_pkt_buffers, +, 1]
        alu[tmp_pkt_buf, tmp_pkt_buf, +, tmp_buf_size]
        alu[$pkt_buf[0], --, b, tmp_pkt_buf]
        alu[offset, offset, +, 8]
    .endw

.end
#endm /* blm_init_blq_mu */

/** This macro initializes a BLQ with buffers from a speicified ring.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @grnum               Ring number
 * @param rtype         Type of memory for ring, DRAM, CLS, CTM
 * @nbuffers            Number of buffers (not to exceed BLQ size as configured using API: 'blm_cfg_blq')
 * @head                Head pointer for blq. Must be multiple of 512 and in range [0, 4096].
 */
#macro blm_init_blq(NbiNum, blq, grnum, rtype, nbuffers, head)
.begin
    .reg $pkt_buf[2]
    .reg tmp_pkt_buf
    .reg num_pkt_buffers
    .reg tmp_buf_size
    .reg addr
    .reg offset
    .sig dram_pkt_sig
    .sig blsig
    .sig nbi_sig
    .xfer_order $pkt_buf

    #if (isnum(head))
        _blm_immed32(offset, head)
    #else
        alu[offset, --, b, head]
    #endif
    alu[offset, --, b, offset, <<3]
    alu[$pkt_buf[1], --, b, 0]
    GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
    immed[num_pkt_buffers, 0]
    .while (num_pkt_buffers < nbuffers)
        ru_deq_from_ring($pkt_buf, rtype, 1, grnum, dram_pkt_sig, SIG_WAIT, PUT_BLIND)
        alu[$pkt_buf[0], --, b, $pkt_buf[0]]

        nbi[write, $pkt_buf[0], addr, <<8, offset, 1], sig_done[nbi_sig]
        wait_blq_cfg#:
          br_!signal[nbi_sig, wait_blq_cfg#]

        alu[num_pkt_buffers, num_pkt_buffers, +, 1]
        alu[offset, offest, +, 8]
    .endw

.end
#endm /* blm_init_blq */

/** This macro reads the status of a given BLQ number
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param direction     ingress or egress
 * @param oxfer         Out xfer result. Requires array of 2 xfer regs. oxfer[2]
 * @param sig_name      Signal to use
 * @param sig_action    SIG_NONE or SIG_WAIT
 */
#macro blm_blq_status(NbiNum, blq, direction, oxfer, sig_name, sig_action)
.begin
    .reg addr
    .reg offset

    #if (streq('direction','ingress'))
        GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
        GET_NBI_DMA_CPP_BLQ_OFFSET(offset, blq)
    #elif (streq('direction','egress'))
        GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
        GET_NBI_TM_CPP_BLQ_OFFSET(offset, blq)
    #else
        #error "Invalid direction. direction = [ingress|egress]."
    #endif

    nbi[read, oxfer, addr, <<8, offset, 1], sig_done[sig_name]

    #if (streq('sig_action', 'SIG_WAIT'))
        ctx_arb[sig_name]
    #endif

.end
#endm /* blm_blq_status */

/** This macro pops 'n' buffer pointers from specified blq# from Egress NBI to local ME xfer registers.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param oxfer         Out xfer array. Array of xfer regs should map correctly to number of buffers. oxfer[n*2]. n=1, pops 2  buffers.
 * @param n             Number of buffers to pop
 * @param sig_name      Signal to use
 * @param sig_action    SIG_NONE or SIG_WAIT
 */
#macro blm_poptome(NbiNum, blq, oxfer, n, sig_name, sig_action)
.begin
    .reg addr
    .reg offset
    .reg override_data

    #if (!isnum(n))
        #error "Length must be constant."
    #endif
    GET_NBI_CPP_BASE_ADDRESS(addr, NbiNum)
    GET_NBI_TM_CPP_BLQ_RD_OFFSET(offset, blq)
    #if (n <= 8)
        nbi[read, oxfer, addr, <<8, offset, n], sig_done[sig_name]
    #else
        alu[--, --, b, ((n <<1)|1), <<7]
        nbi[read, oxfer, addr, <<8, offset, max_8], indirect_ref, sig_done[sig_name]
    #endif
    #if (streq('sig_action', 'SIG_WAIT'))
        ctx_arb[sig_name]
    #endif
.end
#endm /* blm_poptome */

/** This macro pops 'n' buffer pointers from specified blq# from Egress NBI to main memory.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param addr          See description for 'addr' @ blm_push_emu_ring2dma() macro
 * @param offset        EMU ring number
 * @param n             Number of buffers to pop = ((n+1)*2)
 * @param sig_name      Signal to use
 * @param sig_action    SIG_NONE or SIG_WAIT
 */
#macro blm_pop2emu_ring(NbiNum, blq, addr, offset, n)
.begin
    .reg override_data
    .reg ov
    .reg $x[2]
    .xfer_order $x
    .set $x[0]
    .set $x[1]

    /**
    ov = (1<<20)      ; data_master (TM = 0 or 1)
    ov = ov | (1<<16) ; sig_master (TM = 0 or 1)
    ov = ov | (8<<24) ; Master_Island
    ov = ov | (NbiNum<<24) ; LSB of Master_Island
    */
    #define_eval __ov   (0 | \
                         (1<<20) | \
                         (1<<16) | \
                         (8<<24))
    #if (isnum(NbiNum))
        #define_eval __ov       (__ov | (NbiNum<<24))
        _blm_immed32(ov, __ov)
    #else
        _blm_immed32(ov, __ov)
        alu[ov, ov, OR, NbiNum, <<24]
    #endif
    #undef __ov
    local_csr_wr[CMD_INDIRECT_REF_0, ov]

    #if n > 16
        #error "Number of buffers must be <= 16, in multiples of 2 i.e. 2,4,6..."
    #endif
    #define_eval __len  ((n)-1)
    /**
    override_data = (2<<1) ;override (OM) data_master, signal_master, data_master_island
    override_data = override_data | (1<<3) ;override data_ref (OD)
    override_data = override_data | (1<<7) ;override  LEN
    override_data = override_data | (n<<8) ;override  LEN
    override_data = override_data | (1<<13) ;override (Sig_Ref, 0=no signal)
    override_data = override_data | (blq<<23) ; QueueSelect = data_ref[2;7] (blq<<(16+7)).
    */
    #define_eval  __override_data       (0              | \
                                         (2<<1)         | \
                                         (1<<3)         | \
                                         (1<<7)         | \
                                         (__len<<8)     | \
                                         (1<<13))
    #if (isnum(blq))
        #define_eval __override_data        (__override_data | (blq<<23))
        _blm_immed32(override_data, __override_data)
    #else
        _blm_immed32(override_data, __override_data)
        alu[override_data, override_data, OR, blq, <<23]
    #endif
    #undef __len
    #undef __override_data
    #if (isnum(addr))
        .reg __addr
        _blm_immed32(__addr, addr)
    #else
        #define_eval __addr  addr
    #endif
    #if (isnum(offset))
        .reg __offset
        _blm_immed32(__offset, offset)
    #else
        #define_eval __offset  offset
    #endif
    .sig bogus_sig
    alu[--,--,B,override_data]
    mem[journal, $x[0], __addr, <<8, __offset, 1], indirect_ref, sig_done[bogus_sig]
    .io_completed bogus_sig
    #ifdef __addr
      #undef __addr
    #endif
    #ifdef __offset
      #undef __offset
    #endif
.end
#endm /* blm_pop2emu_ring */

/** This macro pops 'n' buffer pointers from specified blq# from Egress NBI to main memory.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param addr          Starting Address of target memory. n*8 bytes required
 * @param offset        Offset from 'addr'. Final 40-bit address = ((addr << 8) + offset)
 * @param n             Number of buffers to pop = ((n+1)*2)
 */
#macro blm_poptomem(NbiNum, blq, addr, offset, n)
.begin
    .reg override_data
    .reg ov
    .reg $x[2]
    .xfer_order $x
    .set $x[0]
    .set $x[1]
    .sig dummy_sig

    /**
    ov = (1<<20)      ; data_master (TM = 0 or 1)
    ov = ov | (1<<16) ; sig_master (TM = 0 or 1)
    ov = ov | (8<<24) ; Master_Island
    ov = ov | (NbiNum<<24) ; LSB of Master_Island
    */
    #define_eval __ov   (0 | \
                         (1<<20) | \
                         (1<<16) | \
                         (8<<24))
    #if (isnum(NbiNum))
        #define_eval __ov       (__ov | (NbiNum<<24))
        _blm_immed32(ov, __ov)
    #else
        _blm_immed32(ov, __ov)
        alu[ov, ov, OR, NbiNum, <<24]
    #endif
    #undef __ov
    local_csr_wr[CMD_INDIRECT_REF_0, ov]
    #define_eval __len  ((n/2)-1)
    /**
    override_data = (2<<1) ;override (OM) data_master, signal_master, data_master_island
    override_data = override_data | (1<<3) ;override data_ref (OD)
    override_data = override_data | (1<<7) ;override  LEN
    override_data = override_data | (n<<8) ;override  LEN
    override_data = override_data | (1<<13) ;override (Sig_Ref, 0=no signal)
    override_data = override_data | (blq<<23) ; QueueSelect = data_ref[2;7] (blq<<(16+7)).
    */
    #define_eval  __override_data       (0       | \
                                         (2<<1)  | \
                                         (1<<3)  | \
                                         (1<<7)  | \
                                         (__len<<8)  | \
                                         (1<<13))
    #if (isnum(blq))
        #define_eval __override_data        (__override_data | (blq<<23))
        _blm_immed32(override_data, __override_data)
    #else
        _blm_immed32(override_data, __override_data)
        alu[override_data, override_data, OR, blq, <<23]
    #endif
    alu[--,--,B,override_data]
    mem[write, $x[0], addr, <<8, offset, 1], sig_done[dummy_sig], indirect_ref
    .io_completed dummy_sig
    #undef __override_data
    #undef __len
.end
#endm /* blm_poptomem */

/** This macro push's 'n' buffer pointers to specified blq# in Ingress NBI from main memory.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param addr          EMU Address: bit[31:30] = MU Locality,
 *                              For direct Access (0x2):
 *                                bit[29:24] = Island_number
 *                              For Hi-Locality (0x0)/Low-Locality Access(0x1):
 *                                bit[29]=1, bit[28:27]=EMUIsland & 0x3
 * @param offset        EMU ring number
 * @param n             Number of buffers to push. MUST be multiple of 2. n = [2,4,6,8,....,32]
 * @param addToHead     if = 1, add buffers to Head else to Tail
 */
#macro blm_push_emu_ring2dma(NbiNum, blq, addr, offset, n, addToHead)
.begin
    .reg override_data
    .reg ov
    .reg $x[2]
    .xfer_order $x
    .set $x[0]
    .set $x[1]

    /**
    ov = (2<<20)      ; data_master (NBI DMA = 2 or 3)
    ov = ov | (2<<16) ; sig_master (NBI DMA = 2 or 3)
    ov = ov | (8<<24) ; Master_Island
    ov = ov | (NbiNum<<24) ; LSB of Master_Island
    */
    #define_eval __ov   (0 | \
                         (2<<20) | \
                         (2<<16) | \
                         (8<<24))
    #if (isnum(NbiNum))
        #define_eval __ov       (__ov | (NbiNum<<24))
        _blm_immed32(ov, __ov)
    #else
        _blm_immed32(ov, __ov)
        alu[ov, ov, OR, NbiNum, <<24]
    #endif
    #undef __ov
    local_csr_wr[CMD_INDIRECT_REF_0, ov]

    #if (!isnum(addToHead))
        #error "addToHead must be constant."
    #endif

    #if ( (addToHead < 0) | (addToHead > 1))
        #error "addToHead must be 0 or 1."
    #endif

    #if (!isnum(n))
        #error "Number of buffers must be constant."
    #endif

    #if n > 16
        #error "Number of buffers must be <= 16, in multiples of 2 i.e. 2,4,6..."
    #endif
    #define_eval __len  ((n)-1)

    /**
    override_data = (2<<1) ;override (OM) data_master, signal_master, data_master_island
    override_data = override_data | (1<<3) ;override data_ref (OD)
    override_data = override_data | (1<<7) ;override  LEN
    override_data = override_data | (n<<8) ;override  LEN value
    override_data = override_data | (1<<13) ;override (Sig_Ref, 0=no signal)
    override_data = override_data | (blq<<23) ; QueueSelect = data_ref[2;7] (blq<<(16+7)).
    override_data = override_data | (0<<25)   ; AddToHead   = data_ref[1;9] = 1 else AddToTail.
    */
    #define_eval  __override_data       (0            | \
                                         (2<<1)       | \
                                         (1<<3)       | \
                                         (1<<7)       | \
                                         (__len<<8)   | \
                                         (1<<13)      | \
                                         (addToHead<<25))
    #if (isnum(blq))
        #define_eval __override_data        (__override_data | (blq<<23))
        _blm_immed32(override_data, __override_data)
    #else
        _blm_immed32(override_data, __override_data)
        alu[override_data, override_data, OR, blq, <<23]
    #endif
    #undef __override_data
    #undef __len
    #if (isnum(addr))
        .reg __addr
        _blm_immed32(__addr, addr)
    #else
        #define_eval __addr  addr
    #endif
    #if (isnum(offset))
        .reg __offset
        _blm_immed32(__offset, offset)
    #else
        #define_eval __offset  offset
    #endif
    .sig bogus_sig
    alu[--,--,B,override_data]
    mem[get, $x[0], __addr, <<8, __offset, 1], indirect_ref, sig_done[bogus_sig]
    .io_completed bogus_sig
    #ifdef __addr
        #undef __addr
    #endif
    #ifdef __ofset
        #undef __ofset
    #endif
.end
#endm /* blm_push_emu_ring2dma */

/** This macro push's 'n' buffer pointers to specified blq# in Ingress NBI from main memory.
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param blq           Buffer list queue number, 0,1,2,or 3
 * @param addr          Starting Address of target memory. n*8 bytes required
 * @param offset        Offset from 'addr'. Final 40-bit address = ((addr << 8) + offset)
 * @param n             Number of buffers to push. MUST be multiple of 2. n = [2,4,6,8,....,32]
 * @param addToHead     if = 1, add buffers to Head else to Tail
 */
#macro blm_push_mem2dma(NbiNum, blq, addr, offset, n, addToHead)
.begin
    .reg override_data
    .reg ov
    .reg $x[2]
    .xfer_order $x
    .set $x[0]
    .set $x[1]
    .sig dummy_sig

    /**
    ov = (2<<20)      ; data_master (NBI DMA = 2 or 3)
    ov = ov | (2<<16) ; sig_master (NBI DMA = 2 or 3)
    ov = ov | (8<<24) ; Master_Island
    ov = ov | (NbiNum<<24) ; LSB of Master_Island
    */
    #define_eval __ov   (0 | \
                         (2<<20) | \
                         (2<<16) | \
                         (8<<24))
    #if (isnum(NbiNum))
        #define_eval __ov       (__ov | (NbiNum<<24))
        _blm_immed32(ov, __ov)
    #else
        _blm_immed32(ov, __ov)
        alu[ov, ov, OR, NbiNum, <<24]
    #endif
    #undef __ov
    local_csr_wr[CMD_INDIRECT_REF_0, ov]

    #if (!isnum(addToHead))
        #error "addToHead must be constant."
    #endif

    #if ( (addToHead < 0) | (addToHead > 1))
        #error "addToHead must be 0 or 1."
    #endif

    #if (!isnum(n))
        #error "Number of buffers must be constant."
    #endif

    #define_eval __len  ((n/2)-1)

    /**
    override_data = (2<<1) ;override (OM) data_master, signal_master, data_master_island
    override_data = override_data | (1<<3) ;override data_ref (OD)
    override_data = override_data | (1<<7) ;override  LEN
    override_data = override_data | (n<<8) ;override  LEN
    override_data = override_data | (1<<13) ;override (Sig_Ref, 0=no signal)
    override_data = override_data | (blq<<23) ; QueueSelect = data_ref[2;7] (blq<<(16+7)).
    override_data = override_data | (0<<25)   ; AddToHead   = data_ref[1;9] = 1 else AddToTail.
    */
    #define_eval  __override_data       (0            | \
                                         (2<<1)       | \
                                         (1<<3)       | \
                                         (1<<7)       | \
                                         (__len<<8)   | \
                                         (1<<13)      | \
                                         (addToHead<<25))
    #if (isnum(blq))
        #define_eval __override_data        (__override_data | (blq<<23))
        _blm_immed32(override_data, __override_data)
    #else
        _blm_immed32(override_data, __override_data)
        alu[override_data, override_data, OR, blq, <<23]
    #endif
    #undef __override_data
    #undef __len
    #if (isnum(addr))
        .reg __addr
        _blm_immed32(__addr, addr)
    #else
        #define_eval __addr  addr
    #endif
    #if (isnum(offset))
        .reg __offset
        _blm_immed32(__offset, offset)
    #else
        #define_eval __offset  offset
    #endif
    alu[--,--,B,override_data]
    mem[read, $x[0], __addr, <<8, __offset, 1], sig_done[dummy_sig], indirect_ref
    .io_completed dummy_sig
    #ifdef __addr
        #undef __addr
    #endif
    #ifdef __ofset
        #undef __ofset
    #endif
.end
#endm /* blm_push_mem2dma */

/** This macro pops 'n' buffer pointers from specified blq# from Egress NBI and pushes directly to Ingress NBI.
 *
 * @param snbi           Source NBI number, 0 or 1
 * @param sblq           Source Buffer list queue number, 0,1,2,or 3
 * @param dnbi           Destination NBI number, 0 or 1
 * @param dblq           Destination Buffer list queue number, 0,1,2,or 3
 * @param n              Number of buffers to pop = ((n+1)*2)
 */
#macro blm_recycle(snbi, sblq, dnbi, dblq, n)
.begin
    .reg addr
    .reg offset
    .reg override_data
    .reg ov
    .reg $x[2]
    .xfer_order $x

    #if (!isnum(n))
        #error "Length must be constant."
    #endif

    #define_eval __ov   (0 | \
                        (2<<20) | \
                        (2<<16) | \
                        (8<<24))
    #if (isnum(dnbi))
        #define_eval __ov       (__ov | (dnbi<<24))
        _blm_immed32(ov, __ov)
    #else
        _blm_immed32(ov, __ov)
        alu[ov, ov, OR, dnbi, <<24]
    #endif
    #undef __ov
    local_csr_wr[CMD_INDIRECT_REF_0, ov]

    /**
    override_data = (1<<1) ;override (OM) data_master, data_master_island
    override_data = override_data | (1<<3) ;override (OD)
    override_data = override_data | (1<<7) ;override  LEN
    override_data = override_data | (n<<8) ;override  LEN
    override_data = override_data | (1<<13) ;override (Sig_Ref, 0=no signal)
    override_data = override_data | (dblq<<23) ; QueueSelect = data_ref[2;7].
    override_data = override_data | (1<<25)   ; AddToHead   = data_ref[1;9].
    */
    #define_eval __override_data        (0              | \
                                        (1<<1)          | \
                                        (1<<3)          | \
                                        (1<<7)          | \
                                        (n<<8)          | \
                                        (1<<13)         | \
                                        (1<<25))
    #if (isnum(dblq))
        #define_eval __override_data        (__override_data | (dblq<<23))
        _blm_immed32(override_data, __override_data)
    #else
        _blm_immed32(override_data, __override_data)
        alu[override_data, override_data, OR, dblq, <<23]
    #endif
    #undef __override_data

    GET_NBI_CPP_BASE_ADDRESS(addr, snbi)
    GET_NBI_TM_CPP_BLQ_RD_OFFSET(offset, sblq)

    .sig bogus_sig
    alu[--,--,B,override_data]
    nbi[read, $x[0], addr, <<8, offset, 1], indirect_ref, sig_done[bogus_sig]
    .io_completed bogus_sig
.end
#endm /* blm_recycle */

/** This macro configures NBI DMA.
 *
 * @param NbiNum                NBI number, 0 or 1
 * @param CtmPollSearchEna      0: Disable, 1: Enable
 * @param RateLimitEnable       0: Disable, 1: Enable (Selects whether to limit the bandwidth on the four Push Buses)
 * @param CtmPollIntvl          Configures the amount of time between packet allocation poll commands to a CTM.
 *                               0x0 Polling interval set to 64us
 *                               0x1 Polling interval set to 128us
 *                               0x2 Polling interval set to 256us
 *                               0x3 Polling interval set to 512us
 * @param CtmPollEna            0: Disable, 1: Enable. Configures whether the polling function is enabled or disabled
 * @param ErrMask               Mask of which errors to be enabled. For more details see NbiDmaCfg Register.
 *                               0x0: Hardware will abort the DMA process
 *                               0x1: Hardware ignores the multi-bit ECC error and the DMA process continues
 */
#macro blm_cfg_dma(NbiNum, CtmPollSearchEna, RateLimitEnable, CtmPollIntvl, CtmPollEna, ErrMask)
.begin
    .reg addr
    .reg dma_cfg_csr
    .reg $x[1]
    .sig nbi_sig
    .xfer_order $x

    #define_eval __NbiNum     (NbiNum + 1)
    #define_eval __cfg_csr      (0 | \
                                 (CtmPollSearchEna <<0) | \
                                 (RateLimitEnable <<1)  | \
                                 (CtmPollIntvl <<4)     | \
                                 (CtmPollEna <<6)       | \
                                 (__NbiNum <<7)         | \
                                 (ErrMask <<10))
    _blm_immed32(dma_cfg_csr, __cfg_csr)
    #undef __cfg_csr

    #define_eval __nbidmacfg      (0x0)
    #define_eval __addr            xpb_csr_addr(NbiNum, 0, 16, __nbidmacfg)
    _blm_immed32(addr, __addr)
    alu[$x[0], --, b, dma_cfg_csr]
    ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_dma_cfg#:
      br_!signal[nbi_sig, wait_dma_cfg#]
    #undef __addr
    #undef __nbidmacfg
    #undef __NbiNum
.end
#endm /* blm_cfg_dma */

/** This macro sets NBI DMA credits.
 *
 * @param NbiNum                NBI number, 0 or 1
 * @param credit0               Sets the number of credits to subtract from the leaky bucket for push bus 0
 *                              A value of zero equates to 1 credits. A value of 3 equates to 4 credits
 * @param credit1               Sets the number of credits to subtract from the leaky bucket for push bus 1
 *                              A value of zero equates to 1 credits. A value of 3 equates to 4 credits
 * @param credit2               Sets the number of credits to subtract from the leaky bucket for push bus 2
 *                              A value of zero equates to 1 credits. A value of 3 equates to 4 credits
 * @param credit3               Sets the number of credits to subtract from the leaky bucket for push bus 3
 *                              A value of zero equates to 1 credits. A value of 3 equates to 4 credits
 * @param thrs0                 Sets the threshold that the leaky bucket is closed. Values are in increments of 16 Credits.
 *                              A value of 0 equates to 16 credits or 128 bytes.
 *                              A value of 3 equates to 64 credits or 512 bytes.
 *                              A value of zero equates to 1 credits.
 *                              A value of 3 equates to 4 credits
 */
#macro blm_set_dma_credits(NbiNum, credit0, credit1, credit2, credit3, thrs0, thrs1, thrs2, thrs3)
.begin
    .reg addr
    .reg dma_crdt_csr
    .reg $x[1]
    .sig nbi_sig
    .xfer_order $x

    #define_eval __crdt_csr     (0 | \
                                (credit0 <<0) | \
                                (credit1 <<2) | \
                                (credit2 <<4) | \
                                (credit3 <<6) | \
                                (thrs0 <<8)   | \
                                (thrs1 <<10)  | \
                                (thrs2 <<12)  | \
                                (thrs3 <<14))
    _blm_immed32(dma_crdt_csr, __crdt_csr)
    #undef __crdt_csr

    #define_eval __nbidmacrdt      (0x14)
    #define_eval __addr            xpb_csr_addr(NbiNum, 0, 16, __nbidmacrdt)
    _blm_immed32(addr, __addr)
    alu[$x[0], --, b, dma_crdt_csr]
    ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_dma_crdt_cfg#:
      br_!signal[nbi_sig, wait_dma_crdt_cfg#]
    #undef __addr
    #undef __nbidmacrdt
.end
#endm /* blm_set_dma_credits */

/** This macro sets NBI DMA credit rate.
 *
 * @param NbiNum                NBI number, 0 or 1
 * @param cr0                   Sets the credit rate for push bus 0. Values are in 4 cycle increments.
 *                               0x0 Credits get subtracted every 4 cycles
 *                               0x1 Credits get subtracted every 8 cycles
 *                               0x2 Credits get subtracted every 12 cycles
 *                               0x3 Credits get subtracted every 16 cycles
 * @param cr1                   Sets the credit rate for push bus 1. Values are in 4 cycle increments.
 *                               0x0 Credits get subtracted every 4 cycles
 *                               0x1 Credits get subtracted every 8 cycles
 *                               0x2 Credits get subtracted every 12 cycles
 *                               0x3 Credits get subtracted every 16 cycles
 * @param cr2                   Sets the credit rate for push bus 2. Values are in 4 cycle increments.
 *                               0x0 Credits get subtracted every 4 cycles
 *                               0x1 Credits get subtracted every 8 cycles
 *                               0x2 Credits get subtracted every 12 cycles
 *                               0x3 Credits get subtracted every 16 cycles
 * @param cr3                   Sets the credit rate for push bus 3. Values are in 4 cycle increments.
 *                               0x0 Credits get subtracted every 4 cycles
 *                               0x1 Credits get subtracted every 8 cycles
 *                               0x2 Credits get subtracted every 12 cycles
 *                               0x3 Credits get subtracted every 16 cycles
 */
#macro blm_set_dma_credit_rate(NbiNum, cr0, cr1, cr2, cr3)
.begin
    .reg addr
    .reg dma_crdt_csr
    .reg $x[1]
    .sig nbi_sig
    .xfer_order $x

    #define_eval __crdt_csr     (0         | \
                                 (cr0 <<0) | \
                                 (cr1 <<2) | \
                                 (cr2 <<4) | \
                                 (cr3 <<6))
    _blm_immed32(dma_crdt_csr, __crdt_csr)
    #undef __crdt_csr

    #define_eval __nbidmacrdt      (0x10)
    #define_eval __addr            xpb_csr_addr(NbiNum, 0, 16, __nbidmacrdt)
    _blm_immed32(addr, __addr)
    alu[$x[0], --, b, dma_crdt_csr]
    ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_dma_crdt_cfg#:
      br_!signal[nbi_sig, wait_dma_crdt_cfg#]
    #undef __addr
    #undef __nbidmacrdt
.end
#endm /* blm_set_dma_credit_rate */

/** This macro configures a a buffer pool
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param bpool         Buffer Pool number: 0...7
 * @param DropEn        Drop when no packet/buffer credits or Primary/Secondary list empty. En=1, Dis=0
 * @param CtmOffset     Packet start offset in CTM. 0 = 32B, 1 = 64B. Recomended 64B offset.
 * @param PriBlist      Primaly buffer list to use. When this is empty, Secondary list is used. 0...3
 * @param SecBlist      Secondary buffer list to use, if primary is empty. 0...3
 * @param SplitLength   Define CTM buffer size. 0=256B, 1=512B, 2=1KB, 3=2KB.
 * @param BpeHead       Head of the Buffer pool Entry. 0...31. BPE's in a buffer pool must be consecutive.
 */
#macro blm_bpool_config(NbiNum, bpool, DropEn, CtmOffset, PriBlist, SecBlist, SplitLength, BpeHead)
.begin
    .reg dma_csr_bpcfg
    .reg addr
    .reg $x[1]
    .sig nbi_sig
    .xfer_order $x

    #if isnum(NbiNum)
        #if ((NbiNum < 0) || (NbiNum > 1))
            #error "NBI should be 0 or 1."
        #endif
    #else
        #error "NBI should be constant."
    #endif

    #if isnum(bpool)
        #if ((bpool < 0) || (bpool > 7))
            #error "Buffer pool should be in range [0, 7]."
        #endif
    #else
        #error "Buffer pool should be constant."
    #endif

    #if isnum(DropEn)
        #if ((DropEn < 0) || (DropEn > 1))
          #error "DropEn should be 0 or 1."
        #endif
    #else
        #error "DropEn should be constant."
    #endif

    #if isnum(CtmOffset)
        #if ((CtmOffset < 0) || (CtmOffset > 1))
            #error "CtmOffset should be 0 or 1."
        #endif
    #else
        #error "CtmOffset should be constant."
    #endif

    #if isnum(PriBlist)
        #if ((PriBlist < 0) || (PriBlist > 3))
            #error "PriBlist should be in range: [0, 3]."
        #endif
    #else
        #error "PriBlist should be constant."
    #endif

    #if isnum(SecBlist)
        #if ((SecBlist < 0) || (SecBlist > 3))
            #error "SecBlist should be in range: [0, 3]."
        #endif
    #else
        #error "SecBlist should be constant."
    #endif

    #if isnum(SplitLength)
        #if ((SplitLength < 0) || (SplitLength > 3))
            #error "SplitLength should be in range: [0, 3]."
        #endif
    #else
        #error "SplitLength should be constant."
    #endif

    #if isnum(BpeHead)
        #if ((BpeHead < 0) || (BpeHead > 31))
            #error "BpeHead should be in range: [0, 31]."
        #endif
    #else
        #error "BpeHead should be constant."
    #endif

    #define_eval __bpcfg        (0                      | \
                                 (DropEn << 13)         | \
                                 (CtmOffset << 12)      | \
                                 (PriBlist << 10)       | \
                                 (SecBlist << 8)        | \
                                 (SplitLength << 5)     | \
                                 (BpeHead))

    _blm_immed32(dma_csr_bpcfg, __bpcfg)
    #undef __bpcfg

    #define_eval __bp_offset      (0x20 + (bpool <<2))
    #define_eval __addr           xpb_csr_addr(NbiNum, 0, 16, __bp_offset)
    _blm_immed32(addr, __addr)
    alu[$x[0], --, b, dma_csr_bpcfg]
    ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_bp_cfg#:
      br_!signal[nbi_sig, wait_bp_cfg#]
    #undef __addr
    #undef __bp_offset


.end
#endm /* blm_bpool_config */

/** This macro configures a buffer pool entry
 *
 * @param NbiNum        NBI number, 0 or 1
 * @param BpeNum        Buffer Pool number: 0...7
 * @param CtmNum        CTM island number to be used for this BPE. 6-bit number: 0-63.
 * @param PktCredit     Number of packet credits allowed for this CTM.
 * @param BufCredit     Number of buffer credits allowed for this CTM.
 * @param ChainEnd      End of chain. Should only be set for last BPE in a buffer pool. 0 or 1.
 */
#macro blm_bpe_config(NbiNum, BpeNum, CtmNum, PktCredit, BufCredit, ChainEnd)
.begin
    .reg dma_csr_bpecfg
    .reg addr
    .reg $x[1]
    .sig nbi_sig
    .xfer_order $x

    #if isnum(NbiNum)
        #if ((NbiNum < 0) || (NbiNum > 1))
            #error "NBI should be 0 or 1."
        #endif
    #else
        #error "NBI should be constant."
    #endif

    #if isnum(BpeNum)
        #if ((BpeNum < 0) || (BpeNum > 31))
            #error "BpeNum should be in range [0, 31]."
        #endif
    #else
        #error "BpeNum should be constant."
    #endif

    #if isnum(CtmNum)
        #if ((CtmNum < 0) || (CtmNum > 63))
            #error "CtmNum should be range: [0, 63]."
        #endif
    #else
        #error "CtmNum should be constant."
    #endif

    #if isnum(ChainEnd)
        #if ((ChainEnd < 0) || (ChainEnd > 1))
            #error "ChainEnd should be 0 or 1."
        #endif
    #else
        #error "ChainEnd should be constant."
    #endif

    #define_eval __bpecfg       (0                 | \
                                 (BpeNum << 27)    | \
                                 (CtmNum << 21)    | \
                                 (PktCredit << 10) | \
                                 (BufCredit))
    _blm_immed32(dma_csr_bpecfg,  __bpecfg)
    #undef __bpecfg

    #define_eval __bpe_offset      (0x40 + (BpeNum <<2))
    #define_eval __addr            xpb_csr_addr(NbiNum, 0, 16, __bpe_offset)
    _blm_immed32(addr, __addr)
    alu[$x[0], --, b, dma_csr_bpecfg]
    ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
    wait_bpe_cfg#:
      br_!signal[nbi_sig, wait_bpe_cfg#]
    #undef __addr
    #undef __bpe_offset

    #if ChainEnd == 1
        #define_eval __bpchain_end_offset      (0x18)
        #define_eval __addr                    xpb_csr_addr(NbiNum, 0, 16, __bpchain_end_offset)
        _blm_immed32(addr, __addr)
        ct[xpb_read, $x[0], addr, 0, 1], sig_done[nbi_sig]
        wait_bpce_rcfg#:
          br_!signal[nbi_sig, wait_bpce_rcfg#]
        alu[dma_csr_bpecfg, --, b, $x[0]]
        alu[dma_csr_bpecfg, dma_csr_bpecfg, OR, 1, <<BpeNum]
        alu[$x[0], --, b, dma_csr_bpecfg]
        ct[xpb_write, $x[0], addr, 0, 1], sig_done[nbi_sig]
        wait_bpce_wcfg#:
          br_!signal[nbi_sig, wait_bpce_wcfg#]
        #undef __addr
        #undef __bpchain_end_offset
    #endif

.end
#endm /* blm_bpe_config */

/** @}
 * @}
 */

#endif /* __BLM_API_UC__ */
