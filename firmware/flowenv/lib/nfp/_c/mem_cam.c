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
 * @file          lib/nfp/_c/mem_cam.c
 * @brief         NFP memory CAM interface
 */

#include <assert.h>
#include <nfp.h>

#include <nfp/mem_cam.h>

__intrinsic void
__mem_cam_lookup(__xrw void *data, __mem40 void *addr,
                 int camsize, int cambits,
                 sync_t sync, SIGNAL_PAIR *sigpair)
{
    struct nfp_mecsr_prev_alu ind;
    unsigned int addr_hi, addr_lo;

    ctassert(__is_ct_const(camsize));
    ctassert(__is_ct_const(cambits));
    ctassert(__is_ct_const(sync));
    ctassert(camsize == 128 || camsize == 256 ||
             camsize == 384 || camsize == 512);
    ctassert(cambits == 8 || cambits == 16 || cambits == 24 || cambits == 32);
    ctassert(sync == sig_done);
    ctassert(__is_read_reg(data));
    ctassert(__is_write_reg(data));

    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;
    addr_lo = (unsigned long long int)addr & 0xffffffff;

    if (camsize == 128) {
        if (cambits == 8)
            __asm mem[cam128_lookup8, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 16)
            __asm mem[cam128_lookup16, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 24)
            __asm mem[cam128_lookup24, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else
            __asm mem[cam128_lookup32, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];

    } else if (camsize == 256) {
        if (cambits == 8)
            __asm mem[cam256_lookup8, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 16)
            __asm mem[cam256_lookup16, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 24)
            __asm mem[cam256_lookup24, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else
            __asm mem[cam256_lookup32, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];

    } else {
        if (camsize == 384 && cambits == 8) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 8;
        } else if (camsize == 384 && cambits == 16) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 9;
        } else if (camsize == 384 && cambits == 24) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 10;
        } else if (camsize == 384) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 11;
        } else if (camsize == 512 && cambits == 8) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 12;
        } else if (camsize == 512 && cambits == 16) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 13;
        } else if (camsize == 512 && cambits == 24) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 14;
        } else {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 15;
        }

        __asm {
            alu[--, --, B, ind];
            mem[cam_lookup, *data, addr_hi, <<8, addr_lo], indirect_ref, \
                sig_done[*__ct_sig_pair(sigpair)];
        }
    }
}

__intrinsic void
mem_cam_lookup(__xrw void *data, __mem40 void *addr,
               int camsize, int cambits)
{
    SIGNAL_PAIR sigpair;

    __mem_cam_lookup(data, addr, camsize, cambits, sig_done, &sigpair);
    wait_for_all(&sigpair);
}

__intrinsic void
__mem_cam_lookup_add(__xrw void *data, __mem40 void *addr,
                     int camsize, int cambits,
                     sync_t sync, SIGNAL_PAIR *sigpair)
{
    struct nfp_mecsr_prev_alu ind;
    unsigned int addr_hi, addr_lo;

    ctassert(__is_ct_const(camsize));
    ctassert(__is_ct_const(cambits));
    ctassert(__is_ct_const(sync));
    ctassert(camsize == 128 || camsize == 256 ||
             camsize == 384 || camsize == 512);
    ctassert(cambits == 8 || cambits == 16 || cambits == 24 || cambits == 32);
    ctassert(sync == sig_done);
    ctassert(__is_read_reg(data));
    ctassert(__is_write_reg(data));

    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;
    addr_lo = (unsigned long long int)addr & 0xffffffff;

    if (camsize == 128) {
        if (cambits == 8)
            __asm mem[cam128_lookup8_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 16)
            __asm mem[cam128_lookup16_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 24)
            __asm mem[cam128_lookup24_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else
            __asm mem[cam128_lookup32_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];

    } else if (camsize == 256) {
        if (cambits == 8)
            __asm mem[cam256_lookup8_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 16)
            __asm mem[cam256_lookup16_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else if (cambits == 24)
            __asm mem[cam256_lookup24_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];
        else
            __asm mem[cam256_lookup32_add, *data, addr_hi, <<8, addr_lo], \
                sig_done[*__ct_sig_pair(sigpair)];

    } else {
        if (camsize == 384 && cambits == 8) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 8;
        } else if (camsize == 384 && cambits == 16) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 9;
        } else if (camsize == 384 && cambits == 24) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 10;
        } else if (camsize == 384) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 11;
        } else if (camsize == 512 && cambits == 8) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 12;
        } else if (camsize == 512 && cambits == 16) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 13;
        } else if (camsize == 512 && cambits == 24) {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 14;
        } else {
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = 15;
        }

        __asm {
            alu[--, --, B, ind];
            mem[cam_lookup_add, *data, addr_hi, <<8, addr_lo], indirect_ref, \
                sig_done[*__ct_sig_pair(sigpair)];
        }
    }
}

__intrinsic void
mem_cam_lookup_add(__xrw void *data, __mem40 void *addr,
                   int camsize, int cambits)
{
    SIGNAL_PAIR sigpair;

    __mem_cam_lookup_add(data, addr, camsize, cambits, sig_done, &sigpair);
    wait_for_all(&sigpair);
}


/*
 * Convenience wrappers around the above generic CAM functions.  These
 * wrappers also ensure that the appropriate search and result data
 * parameters are used.
 */

__intrinsic void
__mem_cam128_lookup8(__xrw struct mem_cam_8bit *data,
                     __mem40 __align16 void *addr,
                     sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 128, 8, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup8(__xrw struct mem_cam_8bit *data,
                   __mem40 __align32 void *addr)
{
    mem_cam_lookup(data, addr, 128, 8);
}

__intrinsic void
__mem_cam128_lookup16(__xrw struct mem_cam_16bit *data,
                      __mem40 __align16 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 128, 16, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup16(__xrw struct mem_cam_16bit *data,
                    __mem40 __align16 void *addr)
{
    mem_cam_lookup(data, addr, 128, 16);
}

__intrinsic void
__mem_cam128_lookup24(__xrw struct mem_cam_24bit *data,
                      __mem40 __align16 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 128, 24, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup24(__xrw struct mem_cam_24bit *data,
                    __mem40 __align16 void *addr)
{
    mem_cam_lookup(data, addr, 128, 24);
}

__intrinsic void
__mem_cam128_lookup32(__xrw struct mem_cam_32bit *data,
                      __mem40 __align16 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 128, 32, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup32(__xrw struct mem_cam_32bit *data,
                    __mem40 __align16 void *addr)
{
    mem_cam_lookup(data, addr, 128, 32);
}

__intrinsic void
__mem_cam256_lookup8(__xrw struct mem_cam_8bit *data,
                     __mem40 __align32 void *addr,
                     sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 256, 8, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup8(__xrw struct mem_cam_8bit *data,
                   __mem40 __align32 void *addr)
{
    mem_cam_lookup(data, addr, 256, 8);
}

__intrinsic void
__mem_cam256_lookup16(__xrw struct mem_cam_16bit *data,
                      __mem40 __align32 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 256, 16, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup16(__xrw struct mem_cam_16bit *data,
                    __mem40 __align32 void *addr)
{
    mem_cam_lookup(data, addr, 256, 16);
}

__intrinsic void
__mem_cam256_lookup24(__xrw struct mem_cam_24bit *data,
                      __mem40 __align32 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 256, 24, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup24(__xrw struct mem_cam_24bit *data,
                    __mem40 __align32 void *addr)
{
    mem_cam_lookup(data, addr, 256, 24);
}

__intrinsic void
__mem_cam256_lookup32(__xrw struct mem_cam_32bit *data,
                      __mem40 __align32 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 256, 32, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup32(__xrw struct mem_cam_32bit *data,
                    __mem40 __align32 void *addr)
{
    mem_cam_lookup(data, addr, 256, 32);
}

__intrinsic void
__mem_cam384_lookup8(__xrw struct mem_cam_8bit *data,
                     __mem40 __align64 void *addr,
                     sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 384, 8, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup8(__xrw struct mem_cam_8bit *data,
                   __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 384, 8);
}

__intrinsic void
__mem_cam384_lookup16(__xrw struct mem_cam_16bit *data,
                      __mem40 __align64 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 384, 16, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup16(__xrw struct mem_cam_16bit *data,
                    __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 384, 16);
}

__intrinsic void
__mem_cam384_lookup24(__xrw struct mem_cam_24bit *data,
                      __mem40 __align64 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 384, 24, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup24(__xrw struct mem_cam_24bit *data,
                    __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 384, 24);
}

__intrinsic void
__mem_cam384_lookup32(__xrw struct mem_cam_32bit *data,
                      __mem40 __align64 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 384, 32, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup32(__xrw struct mem_cam_32bit *data,
                    __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 384, 32);
}

__intrinsic void
__mem_cam512_lookup8(__xrw struct mem_cam_8bit *data,
                     __mem40 __align64 void *addr,
                     sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 512, 8, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup8(__xrw struct mem_cam_8bit *data,
                   __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 512, 8);
}

__intrinsic void
__mem_cam512_lookup16(__xrw struct mem_cam_16bit *data,
                      __mem40 __align64 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 512, 16, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup16(__xrw struct mem_cam_16bit *data,
                    __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 512, 16);
}

__intrinsic void
__mem_cam512_lookup24(__xrw struct mem_cam_24bit *data,
                      __mem40 __align64 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 512, 24, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup24(__xrw struct mem_cam_24bit *data,
                    __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 512, 24);
}

__intrinsic void
__mem_cam512_lookup32(__xrw struct mem_cam_32bit *data,
                      __mem40 __align64 void *addr,
                      sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup(data, addr, 512, 32, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup32(__xrw struct mem_cam_32bit *data,
                    __mem40 __align64 void *addr)
{
    mem_cam_lookup(data, addr, 512, 32);
}

/* Lookup and add */
__intrinsic void
__mem_cam128_lookup8_add(__xrw struct mem_cam_8bit *data,
                         __mem40 __align16 void *addr,
                         sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 128, 8, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup8_add(__xrw struct mem_cam_8bit *data,
                       __mem40 __align32 void *addr)
{
    mem_cam_lookup_add(data, addr, 128, 8);
}

__intrinsic void
__mem_cam128_lookup16_add(__xrw struct mem_cam_16bit *data,
                          __mem40 __align16 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 128, 16, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup16_add(__xrw struct mem_cam_16bit *data,
                        __mem40 __align16 void *addr)
{
    mem_cam_lookup_add(data, addr, 128, 16);
}

__intrinsic void
__mem_cam128_lookup24_add(__xrw struct mem_cam_24bit *data,
                          __mem40 __align16 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 128, 24, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup24_add(__xrw struct mem_cam_24bit *data,
                        __mem40 __align16 void *addr)
{
    mem_cam_lookup_add(data, addr, 128, 24);
}

__intrinsic void
__mem_cam128_lookup32_add(__xrw struct mem_cam_32bit *data,
                          __mem40 __align16 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 128, 32, sync, sigpair);
}

__intrinsic void
mem_cam128_lookup32_add(__xrw struct mem_cam_32bit *data,
                        __mem40 __align16 void *addr)
{
    mem_cam_lookup_add(data, addr, 128, 32);
}

__intrinsic void
__mem_cam256_lookup8_add(__xrw struct mem_cam_8bit *data,
                         __mem40 __align32 void *addr,
                         sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 256, 8, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup8_add(__xrw struct mem_cam_8bit *data,
                       __mem40 __align32 void *addr)
{
    mem_cam_lookup_add(data, addr, 256, 8);
}

__intrinsic void
__mem_cam256_lookup16_add(__xrw struct mem_cam_16bit *data,
                          __mem40 __align32 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 256, 16, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup16_add(__xrw struct mem_cam_16bit *data,
                        __mem40 __align32 void *addr)
{
    mem_cam_lookup_add(data, addr, 256, 16);
}

__intrinsic void
__mem_cam256_lookup24_add(__xrw struct mem_cam_24bit *data,
                          __mem40 __align32 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 256, 24, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup24_add(__xrw struct mem_cam_24bit *data,
                        __mem40 __align32 void *addr)
{
    mem_cam_lookup_add(data, addr, 256, 24);
}

__intrinsic void
__mem_cam256_lookup32_add(__xrw struct mem_cam_32bit *data,
                          __mem40 __align32 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 256, 32, sync, sigpair);
}

__intrinsic void
mem_cam256_lookup32_add(__xrw struct mem_cam_32bit *data,
                        __mem40 __align32 void *addr)
{
    mem_cam_lookup_add(data, addr, 256, 32);
}

__intrinsic void
__mem_cam384_lookup8_add(__xrw struct mem_cam_8bit *data,
                         __mem40 __align64 void *addr,
                         sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 384, 8, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup8_add(__xrw struct mem_cam_8bit *data,
                       __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 384, 8);
}

__intrinsic void
__mem_cam384_lookup16_add(__xrw struct mem_cam_16bit *data,
                          __mem40 __align64 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 384, 16, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup16_add(__xrw struct mem_cam_16bit *data,
                        __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 384, 16);
}

__intrinsic void
__mem_cam384_lookup24_add(__xrw struct mem_cam_24bit *data,
                          __mem40 __align64 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 384, 24, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup24_add(__xrw struct mem_cam_24bit *data,
                        __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 384, 24);
}

__intrinsic void
__mem_cam384_lookup32_add(__xrw struct mem_cam_32bit *data,
                          __mem40 __align64 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 384, 32, sync, sigpair);
}

__intrinsic void
mem_cam384_lookup32_add(__xrw struct mem_cam_32bit *data,
                        __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 384, 32);
}

__intrinsic void
__mem_cam512_lookup8_add(__xrw struct mem_cam_8bit *data,
                         __mem40 __align64 void *addr,
                         sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 512, 8, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup8_add(__xrw struct mem_cam_8bit *data,
                       __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 512, 8);
}

__intrinsic void
__mem_cam512_lookup16_add(__xrw struct mem_cam_16bit *data,
                          __mem40 __align64 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 512, 16, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup16_add(__xrw struct mem_cam_16bit *data,
                        __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 512, 16);
}

__intrinsic void
__mem_cam512_lookup24_add(__xrw struct mem_cam_24bit *data,
                          __mem40 __align64 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 512, 24, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup24_add(__xrw struct mem_cam_24bit *data,
                        __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 512, 24);
}

__intrinsic void
__mem_cam512_lookup32_add(__xrw struct mem_cam_32bit *data,
                          __mem40 __align64 void *addr,
                          sync_t sync, SIGNAL_PAIR *sigpair)
{
    __mem_cam_lookup_add(data, addr, 512, 32, sync, sigpair);
}

__intrinsic void
mem_cam512_lookup32_add(__xrw struct mem_cam_32bit *data,
                        __mem40 __align64 void *addr)
{
    mem_cam_lookup_add(data, addr, 512, 32);
}
