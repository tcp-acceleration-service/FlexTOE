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
 * @file          lib/nfp/_c/me.c
 * @brief         ME related functions
 */

#include <assert.h>
#include <nfp.h>

#include <nfp/me.h>

#include <nfp6000/nfp_me.h>

__intrinsic unsigned int
__ME(void)
{
    unsigned int menum;
    struct nfp_mecsr_active_ctx_sts ctxsts;

    ctxsts.__raw = local_csr_read(local_csr_active_ctx_sts);
    menum = (ctxsts.il_id << 4) + ctxsts.me_id;

    return menum;
}

__intrinsic void
ctx_wait(signal_t sig)
{
    ctassert(sig == kill || sig == voluntary || sig == bpt);

    if (sig == kill)
        __asm ctx_arb[kill];
    else if (sig == voluntary)
        __asm ctx_arb[voluntary];
    else
        __asm ctx_arb[bpt];
}

__intrinsic void
wait_sig_mask(SIGNAL_MASK sigmask)
{
    __asm {
        ctx_arb[--], defer[1]
        local_csr_wr[local_csr_active_ctx_wakeup_events, sigmask]
    }
}

__intrinsic void
signal_ctx(unsigned int ctx, unsigned int sig_no)
{
    local_csr_write(local_csr_same_me_signal,
                    (NFP_MECSR_SAME_ME_SIGNAL_SIG_NO(sig_no) |
                     NFP_MECSR_SAME_ME_SIGNAL_CTX(ctx)));
}

__intrinsic void
signal_next_ctx(unsigned int sig_no)
{
    local_csr_write(local_csr_same_me_signal,
                    (NFP_MECSR_SAME_ME_SIGNAL_NEXT_CTX |
                     NFP_MECSR_SAME_ME_SIGNAL_SIG_NO(sig_no)));
}

__intrinsic void
signal_next_me(unsigned int ctx, unsigned int sig_no)
{
    local_csr_write(local_csr_next_neighbor_signal,
                    (NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_SIG_NO(sig_no) |
                     NFP_MECSR_NEXT_NEIGHBOR_SIGNAL_CTX(ctx)));
}

__intrinsic void
signal_prev_me(unsigned int ctx, unsigned int sig_no)
{
    local_csr_write(local_csr_prev_neighbor_signal,
                    (NFP_MECSR_PREV_NEIGHBOR_SIGNAL_SIG_NO(sig_no) |
                     NFP_MECSR_PREV_NEIGHBOR_SIGNAL_CTX(ctx)));
}

__intrinsic void
set_alarm(unsigned int cycles, SIGNAL *sig)
{
    unsigned sig_num, tslo;

    __implicit_write(sig);
    sig_num = __signal_number(sig);
    tslo = local_csr_read(local_csr_timestamp_low);
    tslo += cycles >> 4;
    local_csr_write(local_csr_active_ctx_future_count, tslo);
    local_csr_write(local_csr_active_future_count_signal, sig_num);
}

__intrinsic void
clear_alarm(void)
{
    local_csr_write(local_csr_active_future_count_signal, 0);
}

__intrinsic void
sleep(unsigned int cycles)
{
    SIGNAL sig;

    __implicit_write(&sig);
    set_alarm(cycles, &sig);
    wait_for_all(&sig);
}

__intrinsic void
halt(void)
{
    __asm halt;
}

__intrinsic int
bit_test(unsigned int data, unsigned int bit_pos)
{
    int result = 1;

    ctassert(__is_ct_const(bit_pos));
    __asm {
        br_bset[data, __ct_const_val(bit_pos), match];
        alu[result, --, B, 0];
        match:
    }

    return result;
}

__intrinsic unsigned int
ffs(unsigned int data)
{
    unsigned result;
    __asm ffs[result, data];
    return result;
}

__intrinsic int
ffs64(unsigned long long int data)
{
    unsigned int hi;
    unsigned int result;

    if ((unsigned int)data != 0) {
        result = ffs((unsigned int)data);
    } else {
        hi = (unsigned int)(data >> 32);
        if (hi != 0)
            result = ffs(hi) + 32;
        else
            result = -1;
    }
    return result;
}


#define __ROTR32(_r, _v, _a) \
    __asm { alu[_r, --, B, _v, >>rot##_a] }

__intrinsic unsigned int
rotr32(unsigned int val, unsigned int amt)
{
    unsigned int result;
    if (__is_ct_const(amt)) {
        if (__is_ct_const(val)) {
            result = (val >> (amt % 32)) | (val << (32 - (amt % 32)));
        } else {
            switch (amt % 32) {
            case  0: result = val;              break;
            case  1: __ROTR32(result, val,  1); break;
            case  2: __ROTR32(result, val,  2); break;
            case  3: __ROTR32(result, val,  3); break;
            case  4: __ROTR32(result, val,  4); break;
            case  5: __ROTR32(result, val,  5); break;
            case  6: __ROTR32(result, val,  6); break;
            case  7: __ROTR32(result, val,  7); break;
            case  8: __ROTR32(result, val,  8); break;
            case  9: __ROTR32(result, val,  9); break;
            case 10: __ROTR32(result, val, 10); break;
            case 11: __ROTR32(result, val, 11); break;
            case 12: __ROTR32(result, val, 12); break;
            case 13: __ROTR32(result, val, 13); break;
            case 14: __ROTR32(result, val, 14); break;
            case 15: __ROTR32(result, val, 15); break;
            case 16: __ROTR32(result, val, 16); break;
            case 17: __ROTR32(result, val, 17); break;
            case 18: __ROTR32(result, val, 18); break;
            case 19: __ROTR32(result, val, 19); break;
            case 20: __ROTR32(result, val, 20); break;
            case 21: __ROTR32(result, val, 21); break;
            case 22: __ROTR32(result, val, 22); break;
            case 23: __ROTR32(result, val, 23); break;
            case 24: __ROTR32(result, val, 24); break;
            case 25: __ROTR32(result, val, 25); break;
            case 26: __ROTR32(result, val, 26); break;
            case 27: __ROTR32(result, val, 27); break;
            case 28: __ROTR32(result, val, 28); break;
            case 29: __ROTR32(result, val, 29); break;
            case 30: __ROTR32(result, val, 30); break;
            case 31: __ROTR32(result, val, 31); break;
            }
        }
    } else {
        unsigned int copy;
        __asm { alu[copy, amt, B, val] }
        __asm { dbl_shf[result, val, copy, >>indirect] }
    }
    return result;
}

#undef __ROTR32


__intrinsic unsigned int
rotl32(unsigned int val, unsigned int amt)
{
    return rotr32(val, 32 - amt);
}


__intrinsic unsigned int
crc_read(void)
{
    return local_csr_read(local_csr_crc_remainder);
}

__intrinsic void
crc_write(unsigned int residue)
{
    local_csr_write(local_csr_crc_remainder, residue);
}

__intrinsic unsigned int
crc_32_be(unsigned int data, crc_bytes_t bspec)
{
    unsigned result;

    ctassert(bspec == crc_bytes_0_3 || bspec == crc_bytes_0_2 ||
             bspec == crc_bytes_0_1 || bspec == crc_byte_0 ||
             bspec == crc_bytes_1_3 || bspec == crc_bytes_2_3 ||
             bspec == crc_byte_3);

    if (bspec == crc_bytes_0_3)
        __asm crc_be[crc_32, result, data], bytes_0_3;
    else if (bspec == crc_bytes_0_2)
        __asm crc_be[crc_32, result, data], bytes_0_2;
    else if (bspec == crc_bytes_0_1)
        __asm crc_be[crc_32, result, data], bytes_0_1;
    else if (bspec == crc_byte_0)
        __asm crc_be[crc_32, result, data], byte_0;
    else if (bspec == crc_bytes_1_3)
        __asm crc_be[crc_32, result, data], bytes_1_3;
    else if (bspec == crc_bytes_2_3)
        __asm crc_be[crc_32, result, data], bytes_2_3;
    else /* bspec == crc_byte_3 */
        __asm crc_be[crc_32, result, data], byte_3;

    return result;
}

__intrinsic unsigned int
crc_iscsi_be(unsigned int data, crc_bytes_t bspec)
{
    unsigned result;

    ctassert(bspec == crc_bytes_0_3 || bspec == crc_bytes_0_2 ||
             bspec == crc_bytes_0_1 || bspec == crc_byte_0 ||
             bspec == crc_bytes_1_3 || bspec == crc_bytes_2_3 ||
             bspec == crc_byte_3);

    if (bspec == crc_bytes_0_3)
        __asm crc_be[crc_iscsi, result, data], bytes_0_3;
    else if (bspec == crc_bytes_0_2)
        __asm crc_be[crc_iscsi, result, data], bytes_0_2;
    else if (bspec == crc_bytes_0_1)
        __asm crc_be[crc_iscsi, result, data], bytes_0_1;
    else if (bspec == crc_byte_0)
        __asm crc_be[crc_iscsi, result, data], byte_0;
    else if (bspec == crc_bytes_1_3)
        __asm crc_be[crc_iscsi, result, data], bytes_1_3;
    else if (bspec == crc_bytes_2_3)
        __asm crc_be[crc_iscsi, result, data], bytes_2_3;
    else /* bspec == crc_byte_3 */
        __asm crc_be[crc_iscsi, result, data], byte_3;

    return result;
}

__intrinsic int
inp_state_test(int statename)
{
    int result = 1;

    ctassert(__is_ct_const(statename));

    __asm {
        br_inp_state[__ct_const_val(statename), match];
        alu[result, --, B, 0];
        match:
    }

    return result;
}

__intrinsic unsigned long long int
me_tsc_read()
{
    unsigned int lo, hi;

    lo = local_csr_read(local_csr_timestamp_low);
    hi = local_csr_read(local_csr_timestamp_high);

    return ((unsigned long long int)hi << 32) | lo;
}

__intrinsic unsigned short int
me_pc_read()
{
    return local_csr_read(local_csr_profile_count);
}
