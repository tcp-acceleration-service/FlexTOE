/*
 * Copyright (C) 2012-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/std/_c/cntrs.c
 * @brief         User defined counters support, including 64bits "regular"
 *                counters and "packets and bytes" counters.
 */

#include <nfp6000/nfp_me.h>
#include <assert.h>
#include <std/cntrs.h>

__intrinsic unsigned int
cntr64_get_addr(__mem40 void *base)
{
    unsigned int addr_256b_aligned;

    addr_256b_aligned = ((unsigned long long int)base >> 8) & 0xffffffff;
    return addr_256b_aligned;
}

__intrinsic void
cntr64_clr(unsigned int base, unsigned int offset)
{
    struct nfp_mecsr_prev_alu ind;
    __gpr unsigned int byte_offset;

    byte_offset = offset << 3;
    ind.__raw = 0;
    ind.ov_len = 1;
    ind.length = 0x9; /* 2-1 , and bit LENGTH[3] shoud be set to 1 */
    ind.ove_data = 2;
    ind.data16 = 0;
    __asm alu[--, --, B, ind.__raw];
    __asm mem[atomic_write_imm , --, base, <<8, byte_offset], indirect_ref;
}

__intrinsic void
cntr64_incr(unsigned int base, unsigned int offset)
{
    __gpr unsigned int byte_offset;

    byte_offset = offset << 3;
    __asm mem[incr64, --, base, <<8, byte_offset];
}

__intrinsic void
cntr64_add(unsigned int base, unsigned int offset, unsigned int count)
{
    __gpr unsigned int byte_offset;

    try_ctassert(count > 0);

    byte_offset = offset << 3;
    if (__is_ct_const(count) && count == 1) {
        __asm mem[incr64, --, base, <<8, byte_offset];
    } else if (__is_ct_const(count) && count <= 0xffff) {
        struct nfp_mecsr_prev_alu ind;
        ind.__raw = 0;
        ind.ov_len = 1;
        ind.length = 8; /* 1-1 , and bit LENGTH[3] shoud be set to 1 */
        ind.ove_data = 2;
        ind.data16 = (count & 0xffff);
        __asm alu[--, --, B, ind.__raw];
        __asm mem[add64_imm, --, base, <<8, byte_offset], indirect_ref;
    } else {
        __xwrite unsigned int value[2];
        SIGNAL add_sig;
        value[0] = count;
        value[1] = 0;
        __asm mem[add64, value, base, <<8, byte_offset, 1], ctx_swap[add_sig];
    }
}


__intrinsic unsigned int
cntr64_cls_get_addr(__cls40 void *base)
{
    unsigned int addr_256b_aligned;

    addr_256b_aligned = ((unsigned long long int)base >> 8) & 0xffffffff;
    return addr_256b_aligned;
}

__intrinsic void
cntr64_cls_clr(unsigned int base, unsigned int offset)
{
    __xwrite unsigned long long zero;
    SIGNAL cls_sig;
    __gpr unsigned int byte_offset;

    byte_offset = offset << 3;
    zero = 0;
    __asm cls[write, zero, base, <<8, byte_offset, 2], ctx_swap[cls_sig];
}

__intrinsic void
cntr64_cls_incr(unsigned int base, unsigned int offset)
{
    __gpr unsigned int byte_offset;

    byte_offset = offset << 3;
    __asm cls[incr64, --, base, <<8, byte_offset];
}

__intrinsic void
cntr64_cls_add(unsigned int base, unsigned int offset, unsigned int count)
{
    __gpr unsigned int byte_offset;

    try_ctassert(count > 0);

    byte_offset = offset << 3;
    if (__is_ct_const(count) && count == 1) {
        __asm cls[incr64, --, base, <<8, byte_offset];
    } else if (__is_ct_const(count) && count < 8) {
        __asm cls[add64_imm, --, base, <<8, byte_offset, count];
    } else if (__is_ct_const(count) && count <= 0xffff) {
        struct nfp_mecsr_prev_alu ind;
        ind.__raw = 0;
        ind.ov_len = 1;
        ind.ove_data = 2;
        ind.data16 = (count & 0xffff);
        __asm alu[--, --, B, ind.__raw];
        __asm cls[add64_imm, --, base, <<8, byte_offset], indirect_ref;
    } else {
        __xwrite unsigned int value[2];
        SIGNAL add_sig;
        value[0] = count;
        value[1] = 0;
        __asm cls[add64, value, base, <<8, byte_offset, 2], ctx_swap[add_sig];
    }
}


__intrinsic struct pkt_cntr_addr
pkt_cntr_get_addr(__imem __addr40 void *base)
{
    struct pkt_cntr_addr cntr_addr;

    cntr_addr.hi = ((unsigned long long int)base >> 8) & 0xff000000;
    cntr_addr.lo = ((unsigned long long int)base >> 3) & 0x0007ffff;

    return cntr_addr;
}

__intrinsic void
pkt_cntr_clr(struct pkt_cntr_addr base, unsigned int offset,
             unsigned int base_select, sync_t sync, SIGNAL *sig)
{
    /* Unused read back value */
    __xread unsigned long long read_val;
    union stats_push_cmd push_cmd;

    try_ctassert(base_select <= 3);

    push_cmd.__raw = 0;
    push_cmd.base  = base_select;
    /* Both base.lo and offset are 64 bits addresses */
    push_cmd.addrs = base.lo + offset;
    if (sync == sig_done) {
        __asm mem[stats_push_clear, read_val, base.hi, <<8, push_cmd.__raw, 1], sig_done[*sig];
    } else {
        __asm mem[stats_push_clear, read_val, base.hi, <<8, push_cmd.__raw, 1], ctx_swap[*sig];
    }
}

__intrinsic void
pkt_cntr_add(struct pkt_cntr_addr base, unsigned int offset,
             unsigned int base_select, unsigned short byte_count,
             sync_t sync, SIGNAL *sig)
{
    __xwrite unsigned int xfer;
    union stats_log_cmd log_cmd;
    union stats_addr stts_addr;

    try_ctassert(base_select <= 3);

    log_cmd.__raw = 0;
    log_cmd.bytes = byte_count;
    log_cmd.pack = STATS_ALL_32_BIT_UNPACKED;

    stts_addr.__raw = 0;
    stts_addr.base  = base_select;
    /* Both base.lo and offset are 64 bits addresses */
    stts_addr.addrs = base.lo + offset;

    xfer = stts_addr.__raw;

    if (sync == sig_done) {
        __asm mem[stats_log, xfer, base.hi, <<8, log_cmd.__raw, 2], sig_done[*sig];
    } else {
        __asm mem[stats_log, xfer, base.hi, <<8, log_cmd.__raw, 2], ctx_swap[*sig];
    }
}

__intrinsic void
pkt_cntr_read(struct pkt_cntr_addr base, unsigned int offset,
              unsigned int base_select, unsigned int *pkt_count,
              unsigned long long *byte_count)
{
    __xread unsigned long long read_val;
    union stats_push_cmd push_cmd;
    SIGNAL read_sig;

    try_ctassert(base_select <= 3);

    push_cmd.__raw = 0;
    push_cmd.base  = base_select;
    /* Both base.lo and offset are 64 bits addresses */
    push_cmd.addrs = base.lo + offset;

    __asm mem[stats_push, read_val, base.hi, <<8, push_cmd.__raw, 1], ctx_swap[read_sig];

    *pkt_count  = (read_val >> 3) & 0x1fffffff;
    *byte_count = ((read_val >> 32) | (read_val << 32)) & 0x7ffffffff;
}

__intrinsic void
pkt_cntr_read_and_clr(struct pkt_cntr_addr base, unsigned int offset,
                      unsigned int base_select, unsigned int *pkt_count,
                      unsigned long long  *byte_count)
{
    __xread unsigned long long read_val;
    union stats_push_cmd push_cmd;
    SIGNAL read_sig;

    try_ctassert(base_select <= 3);

    push_cmd.__raw = 0;
    push_cmd.base  = base_select;
    /* Both base.lo and offset are 64 bits addresses */
    push_cmd.addrs = base.lo + offset;

    __asm mem[stats_push_clear, read_val, base.hi, <<8, push_cmd.__raw, 1], ctx_swap[read_sig];

    *pkt_count  = (read_val >> 3) & 0x1fffffff;
    *byte_count = ((read_val >> 32) | (read_val << 32)) & 0x7ffffffff;
}
