/*
 * Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/_c/remote_me.c
 * @brief         Interface for local-ME-to-remote-ME related functions
 */

#include <assert.h>
#include <nfp.h>
#include <nfp/remote_me.h>

/*
 * Cluster target reflect command address format:
 *      [31:30] 2'b0
 *      [29:24] Island number
 *      [23:17] 7'b0
 *      [16   ] Xfer/CSR register select (0 = Transfer register, 1 = CSR)
 *      [15:14] 2'b0
 *      [13:10] Master ID
 *      [ 9: 2] First register address
 *      [ 1: 0] 2'b0
 */
#define CT_REFLECT_ISL_MASK    ((1 << 6) - 1)
#define CT_REFLECT_MASTER_MASK ((1 << 4) - 1)
#define CT_REFLECT_REG_MASK    ((1 << 8) - 1)

#define CT_REFLECT_MAX_ME_NUM  (CT_REFLECT_MASTER_MASK - 4)

#define MEREG_CT_REFLECT_ADDR(_isl, _me, _is_csr, _addr)                  \
    ((((_isl) & CT_REFLECT_ISL_MASK) << 24) | ((_is_csr) ? 0x10000 : 0) | \
     ((((_me) + 4) & CT_REFLECT_MASTER_MASK) << 10) |                     \
     (((_addr) & CT_REFLECT_REG_MASK) << 2))


static __intrinsic void
remote_me_reg_check_params(unsigned int island, unsigned int me,
                           unsigned int reg_is_csr, unsigned int reg_addr,
                           size_t size)
{
    try_ctassert((island & ~CT_REFLECT_ISL_MASK) == 0);
    try_ctassert(me <= CT_REFLECT_MAX_ME_NUM);
    try_ctassert((reg_addr & ~CT_REFLECT_REG_MASK) == 0);
    /* Note: From 1-14 words can be transferred. */
    try_ctassert(__is_aligned(size, 4));
    /* TODO: Add indirect reference support for sizes greater than 8. */
    try_ctassert(size >= 4 && size <= 32);
}

__intrinsic void
__remote_me_reg_read_signal_local(__xread void *data, unsigned int island,
                                  unsigned int me, unsigned int reg_is_csr,
                                  unsigned int reg_addr, size_t size,
                                  sync_t sync, SIGNAL *local_sig)
{
    unsigned int addr = MEREG_CT_REFLECT_ADDR(island, me, reg_is_csr,
                                              reg_addr);
    unsigned int cnt = size >> 2;
    unsigned int local_sig_num = __signal_number(local_sig);

    ctassert(__is_read_reg(data));
    remote_me_reg_check_params(island, me, reg_is_csr, reg_addr, size);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    /* Signal local ME only. */
    if (sync == sig_done) {
        __asm ct[reflect_read_sig_init, *data, addr, 0, cnt], sig_done[*local_sig];
    } else {
        __asm ct[reflect_read_sig_init, *data, addr, 0, cnt], ctx_swap[*local_sig];
    }
}

__intrinsic void
remote_me_reg_read_signal_local(__xread void *data, unsigned int island,
                                unsigned int me, unsigned int reg_is_csr,
                                unsigned int reg_addr, size_t size)
{
    SIGNAL local_sig;

    __remote_me_reg_read_signal_local(data, island, me, reg_is_csr, reg_addr,
                                      size, ctx_swap, &local_sig);
}

__intrinsic void
remote_me_reg_read_signal_none(__xread void *data, unsigned int island,
                               unsigned int me, unsigned int reg_is_csr,
                               unsigned int reg_addr, size_t size)
{
    unsigned int addr = MEREG_CT_REFLECT_ADDR(island, me, reg_is_csr,
                                              reg_addr);
    unsigned int cnt = size >> 2;

    ctassert(__is_read_reg(data));
    remote_me_reg_check_params(island, me, reg_is_csr, reg_addr, size);

    /* Do not send any ME signals. */
    __asm ct[reflect_read_none, *data, addr, 0, cnt];
}

__intrinsic void
__remote_me_reg_write_signal_local(__xwrite void *data, unsigned int island,
                                   unsigned int me, unsigned int reg_is_csr,
                                   unsigned int reg_addr, size_t size,
                                   sync_t sync, SIGNAL *local_sig)
{
    unsigned int addr = MEREG_CT_REFLECT_ADDR(island, me, reg_is_csr,
                                              reg_addr);
    unsigned int cnt = size >> 2;
    unsigned int local_sig_num = __signal_number(local_sig);

    ctassert(__is_write_reg(data));
    remote_me_reg_check_params(island, me, reg_is_csr, reg_addr, size);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    /* Signal local ME only. */
    if (sync == sig_done) {
        __asm ct[reflect_write_sig_init, *data, addr, 0, cnt], sig_done[*local_sig];
    } else {
        __asm ct[reflect_write_sig_init, *data, addr, 0, cnt], ctx_swap[*local_sig];
    }
}

__intrinsic void
remote_me_reg_write_signal_local(__xwrite void *data, unsigned int island,
                                 unsigned int me, unsigned int reg_is_csr,
                                 unsigned int reg_addr, size_t size)
{
    SIGNAL local_sig;

    __remote_me_reg_write_signal_local(data, island, me, reg_is_csr, reg_addr,
                                       size, ctx_swap, &local_sig);
}

__intrinsic void
remote_me_reg_write_signal_none(__xwrite void *data, unsigned int island,
                                unsigned int me, unsigned int reg_is_csr,
                                unsigned int reg_addr, size_t size)
{
    unsigned int addr = MEREG_CT_REFLECT_ADDR(island, me, reg_is_csr,
                                              reg_addr);
    unsigned int cnt = size >> 2;

    ctassert(__is_write_reg(data));
    remote_me_reg_check_params(island, me, reg_is_csr, reg_addr, size);

    /* Do not send any ME signals. */
    __asm ct[reflect_write_none, *data, addr, 0, cnt];
}

__intrinsic unsigned int
remote_csr_read(unsigned int island, unsigned int me, unsigned int reg_addr)
{
    __xread unsigned int data;

    remote_me_reg_read_signal_local(&data, island, me, 1, reg_addr,
                                    sizeof(data));

    return data;
}

__intrinsic void
remote_csr_write(unsigned int island, unsigned int me, unsigned int reg_addr,
                 unsigned int val)
{
    __xwrite unsigned int data = val;

    remote_me_reg_write_signal_local(&data, island, me, 1, reg_addr,
                                     sizeof(data));
}
