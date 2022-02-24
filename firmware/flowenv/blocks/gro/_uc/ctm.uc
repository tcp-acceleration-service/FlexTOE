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
 * @file          me/blocks/gro/_uc/ctm.uc
 */

#ifndef __CTM_UC
#define __CTM_UC

#include <stdmac.uc>
#define CTM_SIZE_256    0
#define CTM_SIZE_512    1
#define CTM_SIZE_1K     2
#define CTM_SIZE_2K     3


#macro ctm_packet_alloc(out_xfer, size_enc, SIGACTION, sig, in_ctm_isl)
.begin
    .reg addr_hi
    #if (isnum(in_ctm_isl))
        move(addr_hi, ((0x80 | in_ctm_isl) << 24))
    #else
        immed[addr_hi, 0x8000, <<16]
        alu[addr_hi, addr_hi, OR, in_ctm_isl, <<24]
    #endif
    mem[packet_alloc, out_xfer, addr_hi, <<8, 0, size_enc], SIGACTION[sig]
.end
#endm


#macro ctm_packet_alloc(out_xfer, size_enc, SIGACTION, sig)
    ctm_packet_alloc(out_xfer, size_enc, SIGACTION, sig, 0)
#endm


#macro ctm_alloc_poll(out_xfer, size_enc, SIGACTION, sig, in_ctm_isl)
.begin
    .reg addr_hi
    #if (isnum(in_ctm_isl))
        move(addr_hi, ((0x80 | in_ctm_isl) << 24))
    #else
        immed[addr_hi, 0x8000, <<16]
        alu[addr_hi, addr_hi, OR, in_ctm_isl, <<24]
    #endif
    mem[packet_alloc_poll, out_xfer, addr_hi, <<8, 0, size_enc], SIGACTION[sig]
.end
#endm


#macro ctm_alloc_poll(out_xfer, size_enc, sig)
    ctm_alloc_poll(out_xfer, size_enc, sig, 0)
#endm


/* doesn't happen unless we are polling */
#define CTM_ALLOC_FAIL(_ctm_buf) ((_ctm_buf) == 0xFFFFFFFF)

#macro ctm_test_alloc_fail(in_ctm_resp, LABEL, NDEFER)
.begin
    alu[--, in_ctm_resp, +, 1]
    #if (streq('NDEFER', '--'))
        beq[LABEL]
    #else
        beq[LABEL], defer[NDEFER]
    #endif
.end
#endm


/* see section 9.2.2.7.3 in the DB */
#macro ctm_alloc_get_pktnum(out_pktnum, in_ctm_resp)
.begin
    .reg mask
    immed[mask, 0x3FF]
    alu[out_pktnum, mask, AND, in_ctm_resp, >>20]
.end
#endm


#macro ctm_alloc_get_pktcred(out_pktcred, in_ctm_resp)
.begin
    .reg mask
    immed[mask, 0x7FF]
    alu[out_pktcred, mask, AND, in_ctm_resp, >>9]
.end
#endm


#macro ctm_alloc_get_bufcred(out_bufcred, in_ctm_resp)
.begin
    .reg mask
    immed[mask, 0x1FF]
    alu[out_bufcred, mask, AND, in_ctm_resp]
.end
#endm


#macro ctm_build_addr(out_addr, in_pktnum, in_offset)
.begin
    .reg addr
    #if (!isnum(in_offset))
        alu[addr, in_offset, OR, 1, <<31]
    #else
        move(addr, (in_offset | (1 << 31)))
    #endif
    alu[out_addr, addr, OR, in_pktnum, <<16]
.end
#endm


#macro ctm_off_from_addr(out_off, in_ctm_addr)
.begin
    .reg mask
    immed[mask, 0x3FF]
    alu[out_off, in_ctm_addr, AND, mask]
.end
#endm

#macro ctm_write_header(in_isl, in_ctm_addr, in_mu_handle, in_blist, in_len)
.begin
    /* TODO */
.end
#endm


#macro ctm_packet_free(in_pktnum, in_ctm_isl)
.begin
    .reg addr_hi
    #if (isnum(in_ctm_isl))
        move(addr_hi, ((0x80 | in_ctm_isl) << 24))
    #else
        immed[addr_hi, 0x8000, <<16]
        alu[addr_hi, addr_hi, OR, in_ctm_isl, <<24]
    #endif
    mem[packet_free, --, addr_hi, <<8, in_pktnum]
.end
#endm


#macro ctm_packet_free(in_pktnum)
    ctm_packet_free(in_pktnum, 0)
#endm


#endif /* __CTM_UC */
