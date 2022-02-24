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
 * @file          me/blocks/gro/_uc/pktutil.uc
 */

#ifndef __PKTUTIL_UC
#define __PKTUTIL_UC

#include <iref.uc>


#ifndef PKT_NBI_OFFSET
#warning "No PKT_NBI_OFFSET given: assuming 64 for NOP rewrite scripts"
#define PKT_NBI_OFFSET 64
#endif /* PKT_NBI_OFFSET */


/**
 * Packet Metadata Format
 * Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Word  +-----------+-------------------+---+---------------------------+
 *    0  | CTM Number| Packet Number     |Bls|     Packet Length         |
 *       +-+---+-----+-------------------+---+---------------------------+
 *    1  |S|Rsv|                   MU Pointer                            |
 *       +-+---+-----+-------------------+-----+---------+---------------+
 *    2  |           | Sequence Number   |     | SeqRNum |               |
 *       +---+-------+-------------------+-----+---------+---------------+
 *    3  |                      Preclass Meta 1                          |
 *       +---------------------------------------------------------------+
 *    4  |                      Preclass Meta 2                          |
 *       +---------------------------------------------------------------+
 *    5  |                      Preclass Meta 3                          |
 *       +---------------------------------------------------------------+
 *
 *      NOTE that Word 2 is actually defined by the NBI load so it can
 *      easily change with a different load.
 *
 *      S -> 1 if packet is split between CTM and MU data
 *      Bls -> Buffer List
 */


#define PKT_META_CTM_NUM_wrd    0
#define PKT_META_CTM_NUM_shf    26
#define PKT_META_CTM_NUM_msk    0x3F
#define PKT_META_CTM_NUM_wml    0,31,26

#define PKT_META_PKTNUM_wrd     0
#define PKT_META_PKTNUM_shf     16
#define PKT_META_PKTNUM_msk     0x3FF
#define PKT_META_PKTNUM_wml     0,25,16

#define PKT_META_BUFLIST_wrd    0
#define PKT_META_BUFLIST_shf    14
#define PKT_META_BUFLIST_msk    0x3
#define PKT_META_BUFLIST_wml    0,15,14

#define PKT_META_LEN_wrd        0
#define PKT_META_LEN_shf        0
#define PKT_META_LEN_msk        0x3FFF
#define PKT_META_LEN_wml        0,13,0

#define PKT_META_SPLIT_wrd      1
#define PKT_META_SPLIT_shf      31
#define PKT_META_SPLIT_msk      1
#define PKT_META_SPLIT_bit      31
#define PKT_META_SPLIT_wml      1,31,31

#define PKT_META_MUPTR_wrd      1
#define PKT_META_MUPTR_shf      0
#define PKT_META_MUPTR_msk      0x1FFFFFFF
#define PKT_META_MUPTR_wml      1,28,0

/* XXX these values are subject to a lot of change */
#define PKT_META_SEQNUM_wrd     2
#define PKT_META_SEQNUM_shf     16
#define PKT_META_SEQNUM_msk     0xFFF
#define PKT_META_SEQNUM_wml     2,27,16

#define PKT_META_SEQER_wrd      2
#define PKT_META_SEQER_shf      8
#define PKT_META_SEQER_msk      0x1F
#define PKT_META_SEQER_wml      2,12,8
/* XXX these values are subject to a lot of change */


/*
 * Packet Metadata Format
 * Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Word  +-+---------+-------------------+-------------------------------+
 *    0  |E|Reserved |  Packet Number    |       Offset into Packet      |
 *       +-+---------+-------------------+-------------------------------+
 */
#define PKT_ADDR_ENABLE_shf     31
#define PKT_ADDR_ENABLE_msk     1
#define PKT_ADDR_ENABLE_bit     31
#define PKT_ADDR_ENABLE_wml     0,31,31

#define PKT_ADDR_PKTNUM_shf     16
#define PKT_ADDR_PKTNUM_msk     0x1FF
#define PKT_ADDR_PKTNUM_wml     0,25,16

#define PKT_ADDR_OFFSET_shf     0
#define PKT_ADDR_OFFSET_msk     0xFFFF
#define PKT_ADDR_OFFSET_wml     0,15,0


/*
 * Packet address format for transmission
 *
 * Bit  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * ---\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Wrd +-------------+-----------------+---+---------------------------+
 *  0  |  Unused     |  Packet Number  |Rsv|       Packet Length       |
 *     +-------------+-----------------+---+---------------------------+
 *
 * Bit  3 3 3 3 3 3 3 3
 * ---\ 9 8 7 6 5 4 3 2
 * Wrd +---------------+
 *  1  |  Unused       |
 *     +---------------+
 *
 * Note that the packet length MUST include the initial packet offset
 * as well as the packet data.  But the value that comes in from NBI
 * does not include this offset.
 */

#define PKT_SEND_PKTNUM_shf     16
#define PKT_SEND_PKTNUM_msk     0x1FF
#define PKT_SEND_PKTNUM_wml     0,24,16

#define PKT_SEND_PKTLEN_shf     0
#define PKT_SEND_PKTLEN_msk     0x3FFF
#define PKT_SEND_PKTLEN_wml     0,13,0


/*
 * Data Reference encoding for packet transmission.
 *
 * Bit  1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * ---\ 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Wrd +---+-+-+-------------------+
 *     |NBI|r|R|     TX Queue      |
 *     +---+-+-+-------------------+
 *
 * NBI - NBI # to send to
 * R - retry transmission
 * TX Queue - TX queue in the system to send to.
 */
#define PE_SEND_DR_NBI_shf      12
#define PE_SEND_DR_NBI_msk      0x3
#define PE_SEND_DR_NBI_wml      0,13,12

/* DO NOT SET:  NBI ONLY! */
#define PE_SEND_DR_RETRY_shf    10
#define PE_SEND_DR_RETRY_msk    1
#define PE_SEND_DR_RETRY_wml    0,10,10

#define PE_SEND_DR_TXQ_shf      0
#define PE_SEND_DR_TXQ_msk      0x3FF
#define PE_SEND_DR_TXQ_wml      0,9,0

#define PE_SEND_RWS_LENC(pktoff, len) (((pktoff) - (len) - 8) / 8)


/* Rewrite Script word0 format */

#define TM_RWS_DIRECT_wrd       0
#define TM_RWS_DIRECT_shf       31
#define TM_RWS_DIRECT_bit       31
#define TM_RWS_DIRECT_wml       0,31,31

#define TM_RWS_OPCODE_wrd       0
#define TM_RWS_OPCODE_shf       16
#define TM_RWS_OPCODE_msk       0xFF
#define TM_RWS_OPCODE_wml       0,23,16

#define TM_RWS_OFFSET_wrd       0
#define TM_RWS_OFFSET_shf       8
#define TM_RWS_OFFSET_msk       0xFF
#define TM_RWS_OFFSET_wml       0,15,8

#define TM_RWS_RDATA_LEN_wrd    0
#define TM_RWS_RDATA_LEN_shf    0
#define TM_RWS_RDATA_LEN_msk    0x1F
#define TM_RWS_RDATA_LEN_wml    0,4,0

#define TM_RWS_OP_NOP           0xE0    /* OP(3)=7|BYTES(4)=?|MOD_RDATA(1)=? */

#define TM_RWS_OP_DEL_LEN_shf   (TM_RWS_OPCODE_shf + 1)


/* Definitions for a nop rewrite script to send with the packet */


#define PKT_RWS_NOP_W0          ((1 << TM_RWS_DIRECT_shf) | \
                                 (TM_RWS_OP_NOP << TM_RWS_OPCODE_shf) | \
                                 (0 << TM_RWS_OFFSET_shf) | \
                                 (0 << TM_RWS_RDATA_LEN_shf))
#define PKT_RWS_NOP_W0_HI16     ((1 << (TM_RWS_DIRECT_shf - 16)) | \
                                 (TM_RWS_OP_NOP << (TM_RWS_OPCODE_shf - 16)))

#define PKT_RWS_NOP_W1          0

#define PKT_RWS_NOP_SIZE_LW     2
#define PKT_RWS_NOP_SIZE        (PKT_RWS_NOP_SIZE_LW * 4)



#define PKT_SEND_RWS_NOP_LENC    PE_SEND_RWS_LENC(PKT_NBI_OFFSET, PKT_RWS_NOP_SIZE)


/*
 * Need to override byte mask w/ sequencer, (sig master|data master|island) with
 * sequence number, data_ref with (txq, retry, nbi) and length w/ the encoded
 * rewrite script length.  We'll take the first 2 from CSR0  Rewrite script
 * will go in prev_alu's length field and the data_ref from prev_alu's DATA16
 * field.
 *
 * This low 16 bytes of the previous ALU of the indirect reference would not
 * vary in common usage.  But the upper 16 bits specify where to send the
 * packet, so they commonly would vary.
 */
#define PKT_SEND_RWS_NOP_IREF_LO \
                        ((1 << IREF_OV_SM_shf) | \
                         (IREF_OVM_DM_ISLD << IREF_OV_MASTER_shf) | \
                         (IREF_OVD_DR << IREF_OV_DATA_shf) | \
                         (1 << IREF_OV_BM_CSR_shf) | \
                         (1 << IREF_OV_LEN_shf) | \
                         (PKT_SEND_RWS_NOP_LENC << IREF_LEN_shf))

#define PKT_SEND_RWS_NOP_IREF_LO_NOOFF \
                        ((1 << IREF_OV_SM_shf) | \
                         (IREF_OVM_DM_ISLD << IREF_OV_MASTER_shf) | \
                         (IREF_OVD_DR << IREF_OV_DATA_shf) | \
                         (1 << IREF_OV_BM_CSR_shf) | \
                         (1 << IREF_OV_LEN_shf))





/**
 * Generate an PKT_RWS_NOP_SIZE_LW-byte nop rewrite script.
 *
 * @out_rws     Location to put the rewrite script:  must be an indexable
 *              location
 */
#macro pkt_send_build_rws_nop(out_rws)
.begin

    move(out_rws[0], PKT_RWS_NOP_W0)
    move(out_rws[1], PKT_RWS_NOP_W1)

.end
#endm


/**
 * Build the low 16 bits of an indirect reference word for transmitting
 * a packet with a NOP packet rewrite script.
 *
 * @io_iref_lo          The lower indirect reference override register
 * @in_frame_off        The offset of the packet frame in the buffer
 *                      (the rewrite script goes @ PKT_RWS_NOP_SIZE before it)
 */
#macro pkt_send_rws_nop_build_iref_lo(io_iref_lo, in_frame_off)
.begin
    .reg scratch

    alu[scratch, in_frame_off, -, (8 + PKT_RWS_NOP_SIZE)]
    alu[scratch, --, B, scratch, >>3]
    immed[io_iref_lo, PKT_SEND_RWS_NOP_IREF_LO_NOOFF]
    alu[io_iref_lo, io_iref_lo, OR, scratch, <<IREF_LEN_shf]
.end
#endm


/**
 * Build the high 16 bits of an indirect reference word for transmitting
 * a packet with a NOP packet rewrite script.
 *
 * @io_data16   a read/write location to store the data16 portion of the iref
 * @in_nbi      the NBI to send to
 * @in_txq      the TX queue w/in the NBI to send to
 */
#macro pkt_send_rws_nop_build_data16(io_data16, in_nbi, in_txq)
.begin

    #define_eval __PKTTMP 0

    #if (isnum(in_nbi))
        #define_eval __PKTTMP (__PKTTMP | ((in_nbi) << PE_SEND_DR_NBI_shf))
    #endif

    #if (isnum(in_txq))
        #define_eval __PKTTMP (__PKTTMP | ((in_txq) << PE_SEND_DR_TXQ_shf))
    #endif


    immed[io_data16, __PKTTMP]

    #if (!isnum(in_nbi))
        alu[io_data16, io_data16, OR, in_nbi, <<PE_SEND_DR_NBI_shf]
    #endif

    #if (!isnum(in_txq))
        alu[io_data16, io_data16, OR, in_txq, <<PE_SEND_DR_TXQ_shf]
    #endif

    #undef __PKTTMP

.end
#endm


/*
 * Need to override byte mask w/ sequencer, (sig master|data master|island) with
 * sequence number, and data_ref with nbi We'll take the first 2 from CSR0.
 * The data_ref from prev_alu's DATA16 field.
 *
 * This low 16 bytes of the previous ALU of the indirect reference would not
 * vary in common usage.  But the upper 16 bits specify which NBI to send
 * the completion to.
 */
#define PKT_DROP_IREF_LO \
                        ((1 << IREF_OV_SM_shf) | \
                         (IREF_OVM_DM_ISLD << IREF_OV_MASTER_shf) | \
                         (IREF_OVD_DR << IREF_OV_DATA_shf) | \
                         (1 << IREF_OV_BM_CSR_shf))

#define PKT_DROP_NBI_IREF_shf   (IREF_DATA16_shf + PE_SEND_DR_NBI_shf)



/*
 * This macro assumes that packet offsets are set at 64 for the
 * sake of simplicity.
 *
 * Register use:
 *   2 additional GPRs
 *   2 write transfer registers
 * Signal use:
 *   1 signal
 *
 * Instruction count:
 *   37 + whatever would be included for the "unable to send"
 *   cases.  (one placeholder instruction there for now)
 *
 * Cycle count:
 *   The only defer slot that can possibly be filled by the
 *   optimizer is the mem[write..] operation at the end.
 *   Other branches are optional or completely filled.
 *   It's more than likely even this one can't be filled so all the
 *   cycle counts given below should be 3 higher.  These are the #s
 *   in parentheses.
 *
 *   Offset = 64:                       9 (12) cycles
 *   Offset > 64 && <= 80:              21 (24) cycles
 *   Offset < 64 and unaligned:         20 (23) cycles
 *   Offset < 64 and 8-byte aligned:    21 (24) cycles
 *
 *   Offset < 16:                       Assumed never to happen, if
 *                                      we handle it, all costs above except
 *                                      for "= 64" increase by 2.  Cost
 *                                      to actual detect would be 12 cycles.
 *
 *   Offset > 80:                       13 cycles to detect.  A fair
 *                                      but unknown # of cycles to generate
 *                                      an indirect script to handle this
 *                                      required beyond this + 2 more for
 *                                      the write and 1 for the branch.
 *
 *
 */
#macro pkt_gen_nop_rewrite(io_iref_lo, in_frame_addr)
.begin
    .reg scratch
    .reg off

    .reg write $rws[2]
    .xfer_order $rws

    .sig rws_sig

    ; Optimize for the 'offset hasn't moved' case
    ld_field_w_clr[off, 0011, in_frame_addr]
    br=byte[off, 0, 64, off_equals_64#], defer[3]
    immed[scratch, PKT_RWS_NOP_W0_HI16]
    alu[$rws[0], --, B, scratch, <<16]
    alu[$rws[1], --, B, 0]

    ; Check whether the offset is above or below 64
    alu[--, off, -, 64]
    bgt[off_above_64#]

off_below_64#:
    ; We know the offset starts below the 64-byte boundary, but we still
    ; need to know whether it is 8-byte aligned or not.
    ;
    ; XXX
    ; In principle we should also check that the offset is at least 16, because
    ; because an initial offset for a rewrite script of less than 8 is
    ; not permitted.  We'll assume in this case that this isn't the case.
    ; To handle this case we must do
    ;   alu[--, off, -, 16]
    ;   blt[completely_unable_to_send_label#]
    ; XXX
    alu[scratch, off, AND, 0x7]
    beq[off_below_64_aligned#]

off_below_64_unaligned#:
    ; In this case we must round down to an address multiple of 8.
    ; The 'scratch' variable currently holds the offset % 8.
    ; First comput the rewrite script, then the indirect reference.
    alu[scratch, scratch, -, 1]
    alu[scratch, --, B, scratch, <<TM_RWS_OP_DEL_LEN_shf]
    alu[$rws[0], scratch, OR, 1, <<TM_RWS_DIRECT_shf]

    ; compute the indirect reference for this offset goto write
    alu[scratch, --, B, off, >>3]
    alu[scratch, off, -, ((8 + PKT_RWS_NOP_SIZE) >> 3)]
    immed[io_iref_lo, PKT_SEND_RWS_NOP_IREF_LO_NOOFF]
    alu[io_iref_lo, io_iref_lo, OR, scratch, <<IREF_LEN_shf]

    br[issue_write#], defer[2]
    alu[off, off, AND~, 7]
    alu[off, off, -, PKT_RWS_NOP_SIZE]

off_below_64_aligned#:
    ; 8-byte aligned offset below 64:  we have NOP
    ; already in the script, but we need to
    ; recalculate the indrect referecence and
    ; the address to put the rewrite script
    alu[scratch, off, -, (8 + PKT_RWS_NOP_SIZE)]
    alu[scratch, --, B, scratch, >>3]
    immed[io_iref_lo, PKT_SEND_RWS_NOP_IREF_LO_NOOFF]
    br[issue_write#], defer[2]
    alu[io_iref_lo, io_iref_lo, OR, scratch, <<IREF_LEN_shf]
    alu[off, off, -, PKT_RWS_NOP_SIZE]

off_above_64#:
    ; in this case we must HOPE AND PRAY that the offset isn't above 80
    ; otherwise we are completely screwed.  The rewrite script can not
    ; delete more than 16 bytes.
    alu[scratch, off, -, 64]
    alu[--, scratch, -, 16]
    bgt[off_above_80#]

    ;
    immed[off, 56]
    immed[io_iref_lo, PKT_SEND_RWS_NOP_IREF_LO]

    ; compute the delete rewrite script for the padding
    ; the delete OP code is 0 so we only have to shift
    ; the length into place and then the direct script bit
    alu[scratch, scratch, -, 1]
    br[issue_write#], defer[2]
    alu[scratch, --, B, scratch, <<TM_RWS_OP_DEL_LEN_shf]
    alu[$rws[0], scratch, OR, 1, <<TM_RWS_DIRECT_shf]

completely_unable_to_send_label#:
    ; This case occurs when the packet offset starts less than
    ; 16 bytes into the buffer offset.  A rewrite script can start
    ; no less than offset 8.
    ctx_arb[bpt]

off_above_80#:
    ; in this case I have no idea what we do.  The rewrite script
    ; can't delete that much.  We'd need to run with a series
    ; of indirect rewrite scripts of some sort.
    ctx_arb[bpt]


off_equals_64#:
    alu[off, off, -, PKT_RWS_NOP_SIZE]
    immed[io_iref_lo, PKT_SEND_RWS_NOP_IREF_LO]

issue_write#:
    ld_field_w_clr[scratch, 1100, in_frame_addr]
    mem[write, $rws[0], scratch, off, 1], ctx_swap[rws_sig]

.end
#endm


#endif /* __PKTUTIL_UC */
