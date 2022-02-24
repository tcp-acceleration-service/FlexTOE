/*
 * Copyright (C) 2014-2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/_c/xpb.c
 * @brief         XPB related functions
 */

#include <assert.h>
#include <nfp.h>
#include <nfp/xpb.h>
#include <nfp6000/nfp_me.h>


/*
 * Cluster target XPB command address format:
 *      [31   ] 1'b0
 *      [30   ] Global/local XPB steering bit (0 = local, 1 = global)
 *      [29:24] Island number
 *      [23:22] XPB slave index
 *      [21:16] XPB target device ID
 *      [15: 2] XPB target register address
 *      [ 1: 0] 2'b0
 */
#define XPB_ADDR_MASK  0x7FFFFFFC

#define XPB_CMD(cmdname, data, addr, size, max_size, sync, sig)        \
    do {                                                               \
        struct nfp_mecsr_prev_alu ind;                                 \
        unsigned int cnt     = size >> 2;                              \
        unsigned int max_cnt = max_size >> 2;                          \
                                                                       \
        try_ctassert((addr & ~XPB_ADDR_MASK) == 0);                    \
        /* Note: From 1-16 words can be transferred. */                \
        try_ctassert(__is_aligned(size, 4));                           \
        try_ctassert(size >= 4 && size <= 64);                         \
        ctassert(__is_aligned(max_size, 4));                           \
        ctassert(max_size >= 4 && max_size <= 64);                     \
        ctassert(__is_ct_const(sync));                                 \
        ctassert(sync == sig_done || sync == ctx_swap);                \
                                                                       \
        if (__is_ct_const(size)) {                                     \
            if (cnt <= 8) {                                            \
                if (sync == sig_done) {                                \
                    __asm { ct[cmdname, *data, addr, 0,                \
                               __ct_const_val(cnt)], sig_done[*sig] }; \
                } else {                                               \
                    __asm { ct[cmdname, *data, addr, 0,                \
                               __ct_const_val(cnt)], ctx_swap[*sig] }; \
                }                                                      \
            } else {                                                   \
                /* Set up length in PrevAlu for the indirect */        \
                ind.__raw  = 0;                                        \
                ind.ov_len = 1;                                        \
                ind.length = cnt - 1;                                  \
                                                                       \
                if (sync == sig_done) {                                \
                    __asm { alu[--, --, B, ind.__raw] };               \
                    __asm { ct[cmdname, *data, addr, 0,                \
                               __ct_const_val(cnt)], sig_done[*sig],   \
                               indirect_ref };                         \
                } else {                                               \
                    __asm { alu[--, --, B, ind.__raw] };               \
                    __asm { ct[cmdname, *data, addr, 0,                \
                               __ct_const_val(cnt)], ctx_swap[*sig],   \
                               indirect_ref };                         \
                }                                                      \
            }                                                          \
        } else {                                                       \
            /* Set up length in PrevAlu for the indirect */            \
            ind.__raw  = 0;                                            \
            ind.ov_len = 1;                                            \
            ind.length = cnt - 1;                                      \
                                                                       \
            if (sync == sig_done) {                                    \
                __asm { alu[--, --, B, ind.__raw] };                   \
                __asm { ct[cmdname, *data, addr, 0,                    \
                           __ct_const_val(max_cnt)], sig_done[*sig],   \
                           indirect_ref };                             \
            } else {                                                   \
                __asm { alu[--, --, B, ind.__raw] };                   \
                __asm { ct[cmdname, *data, addr, 0,                    \
                           __ct_const_val(max_cnt)], ctx_swap[*sig],   \
                           indirect_ref };                             \
            }                                                          \
        }                                                              \
    } while (0)

__intrinsic void
__xpb_read(__xread void *data, unsigned int addr, size_t size,
           const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));
    XPB_CMD(xpb_read, data, addr, size, max_size, sync, sig);
}

__intrinsic unsigned int
xpb_read(unsigned int addr)
{
    SIGNAL sig;
    __xread unsigned int xfer;

    __xpb_read(&xfer, addr, sizeof(xfer), sizeof(xfer), ctx_swap, &sig);

    return xfer;
}

__intrinsic void
__xpb_write(__xwrite void *data, unsigned int addr, size_t size,
            const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));
    XPB_CMD(xpb_write, data, addr, size, max_size, sync, sig);
}

__intrinsic void
xpb_write(unsigned int addr, unsigned int val)
{
    SIGNAL sig;
    __xwrite unsigned int xfer = val;

    __xpb_write(&xfer, addr, sizeof(xfer), sizeof(xfer), ctx_swap, &sig);
}
