/*
 * Copyright (C) 2014-2019,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/std/_c/hash.c
 * @brief         Functions to perform a hash over a register region.
 */

#ifndef _STD__HASH_C_
#define _STD__HASH_C_

#include <assert.h>
#include <nfp.h>
#include <stdint.h>
#include <types.h>

#include <nfp6000/nfp_cls.h>

#include <std/hash.h>
#include <std/reg_utils.h>

/* trick compiler into allowing a register pointer */
#define REGPTR __xrw unsigned *

/* macro for hash_me_crc32 and hash_me_crc32c implementation */
#define _HASH_SWITCH_CASE_IMPLEMENT(n, EXEC_MACRO)                          \
{                                                                           \
    switch (n) {                                                            \
        case 4:                                                             \
            EXEC_MACRO( 0);                                                 \
            break;                                                          \
        case 8:                                                             \
            EXEC_MACRO( 0); EXEC_MACRO( 1);                                 \
            break;                                                          \
        case 12:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2);                 \
            break;                                                          \
        case 16:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            break;                                                          \
        case 20:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4);                                                 \
            break;                                                          \
        case 24:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5);                                 \
            break;                                                          \
        case 28:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6);                 \
            break;                                                          \
        case 32:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            break;                                                          \
        case 36:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8);                                                 \
            break;                                                          \
        case 40:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9);                                 \
            break;                                                          \
        case 44:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9); EXEC_MACRO(10);                 \
            break;                                                          \
        case 48:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9); EXEC_MACRO(10); EXEC_MACRO( 11);\
            break;                                                          \
        case 52:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9); EXEC_MACRO(10); EXEC_MACRO(11); \
            EXEC_MACRO(12);                                                 \
            break;                                                          \
        case 56:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9); EXEC_MACRO(10); EXEC_MACRO(11); \
            EXEC_MACRO(12); EXEC_MACRO(13);                                 \
            break;                                                          \
        case 60:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9); EXEC_MACRO(10); EXEC_MACRO(11); \
            EXEC_MACRO(12); EXEC_MACRO(13); EXEC_MACRO(14);                 \
            break;                                                          \
        case 64:                                                            \
            EXEC_MACRO( 0); EXEC_MACRO( 1); EXEC_MACRO( 2); EXEC_MACRO( 3); \
            EXEC_MACRO( 4); EXEC_MACRO( 5); EXEC_MACRO( 6); EXEC_MACRO( 7); \
            EXEC_MACRO( 8); EXEC_MACRO( 9); EXEC_MACRO(10); EXEC_MACRO(11); \
            EXEC_MACRO(12); EXEC_MACRO(13); EXEC_MACRO(14); EXEC_MACRO(15); \
            break;                                                          \
        }                                                                   \
}



/* Note we could offer a function which does not require the size to
 * be multiple of 32bit but it would get really tedious to unroll */
__intrinsic uint32_t
hash_me_crc32(const void *s, size_t n, uint32_t init)
{
    __gpr uint32_t result;

    /* Make sure the parameters are as we expect */
    ctassert(__is_in_reg_or_lmem(s));
    ctassert(__is_ct_const(n));
    ctassert(n <= 64);
    ctassert((n % 4) == 0);

    crc_write(init);

#ifdef __HASH_ME_CRC32
    #error "Attempting to redefine __HASH_ME_CRC32"
#endif

    /* every type has to be explicitly cast */
    if (__is_in_lmem(s)) {
#define __HASH_ME_CRC32(_x) \
        crc_32_be(((__lmem unsigned int *)s)[_x], crc_bytes_0_3)

        _HASH_SWITCH_CASE_IMPLEMENT(n, __HASH_ME_CRC32)

#undef __HASH_ME_CRC32
    } else {
        /* normal register type */
#define __HASH_ME_CRC32(_x) \
        crc_32_be(((__gpr unsigned int *)s)[_x], crc_bytes_0_3)

        _HASH_SWITCH_CASE_IMPLEMENT(n, __HASH_ME_CRC32)

#undef __HASH_ME_CRC32
    }

    return crc_read();
}


/* Note we could offer a function which does not require the size to
 * be multiple of 32bit but it would get really tedious to unroll */
__intrinsic uint32_t
hash_me_crc32c(const void *s, size_t n, uint32_t init)
{
    __gpr uint32_t result;

    /* Make sure the parameters are as we expect */
    ctassert(__is_in_reg_or_lmem(s));
    ctassert(__is_ct_const(n));
    ctassert(n <= 64);
    ctassert((n % 4) == 0);

    crc_write(init);

#ifdef __HASH_ME_CRC32C
    #error "Attempting to redefine __HASH_ME_CRC32C"
#endif

    /* every type has to be explicitly cast */
    if (__is_in_lmem(s)) {
#define __HASH_ME_CRC32C(_x) \
        crc_iscsi_be(((__lmem unsigned int *)s)[_x], crc_bytes_0_3)

        _HASH_SWITCH_CASE_IMPLEMENT(n, __HASH_ME_CRC32C)

#undef __HASH_ME_CRC32C
    } else {
        /* normal register type */
#define __HASH_ME_CRC32C(_x) \
        crc_iscsi_be(((__gpr unsigned int *)s)[_x], crc_bytes_0_3)

        _HASH_SWITCH_CASE_IMPLEMENT(n, __HASH_ME_CRC32C)

#undef __HASH_ME_CRC32C
    }

    return crc_read();
}

/* Hash mask can support 128 bytes, but allocate half to conserve transfer
   registers */
#define CLS_HASH_MASK_HALF  (64)

void
cls_hash_init(__cls void *mask, uint32_t size)
{
    __xwrite uint32_t mult_val;
    __xwrite uint64_t hashmask[CLS_HASH_MASK_HALF >> 3];

    try_ctassert(size <= 128);

    /* Initialize to all bits set so no bits of the key are ignored */
    hashmask[0] = 0xffffffffffffffff;
    hashmask[1] = 0xffffffffffffffff;
    hashmask[2] = 0xffffffffffffffff;
    hashmask[3] = 0xffffffffffffffff;
    hashmask[4] = 0xffffffffffffffff;
    hashmask[5] = 0xffffffffffffffff;
    hashmask[6] = 0xffffffffffffffff;
    hashmask[7] = 0xffffffffffffffff;

    /* If there are enough transfer registers for one write */
    if (size <= CLS_HASH_MASK_HALF) {
        cls_write(&hashmask[0], (__cls void *)mask, size);
    /* Else, write the first half of the mask, then the remaining bytes */
    } else {
        cls_write(&hashmask[0], (__cls void *)mask, CLS_HASH_MASK_HALF);
        cls_write((void *)&hashmask[0],
                  ((__cls uint8_t *)mask) + CLS_HASH_MASK_HALF,
                  size - CLS_HASH_MASK_HALF);
    }

    /* Configure the Hash Multiplier CSR */
    mult_val = NFP_CLS_HASH_MULT_M4 |
               NFP_CLS_HASH_MULT_M36 |
               NFP_CLS_HASH_MULT_M53 |
               NFP_CLS_HASH_MULT_M63 ;
    cls_write(&mult_val, (__cls void *)NFP_CLS_HASH_MULT, sizeof(uint32_t));
}

uint64_t
cls_hash(__xwrite void *key, __cls void *mask, uint32_t size, uint32_t idx)
{
    __cls void *hash_idx_csr;
    __xread uint64_t data;

    /* Clear the hash index CSR, then perform the hash */
    cls_hash_mask_clr(key, mask, size, idx);

    /* Setup the hash index CSR address, then read the calculated hash */
    hash_idx_csr = (__cls void *)NFP_CLS_HASH_IDX64(idx);
    cls_read((void *)&data, hash_idx_csr, sizeof(uint64_t));

    return data;
}

__intrinsic static uint32_t
hash_toeplitz_block(uint32_t input, __gpr uint32_t prev_result,
                    __gpr uint32_t*sk0, __gpr uint32_t *sk1)
{
    __gpr uint32_t bit_val;
    __gpr uint32_t iter;
    __gpr uint32_t result = 0;
    __gpr uint32_t inputa;

    __asm __attribute(LITERAL_ASM) {
        alu[result, --, B, prev_result]

        /*
         * We make the decision on whether to store the result of XOR in the
         * code below on whether the most significant bit of the input we are
         * looking at is 1 or not using the N condition code of the previous
         * instruction.
         */

        /*
         * Need to take care of the most significant bit of input outside the
         * loop as the loop starts by shifting left the input.
         */
        alu[bit_val, --, B, input]
        bpl[toeplitz_block_skip_1]
        alu[result, result, XOR, *sk0]
toeplitz_block_skip_1:
        dbl_shf[*sk0, *sk0, *sk1, >> 31]
        alu_shf[*sk1, --, B, *sk1, << 1]


        /*
         * Go through the rest of the bits in bitval, looking at the
         * most significant bit and XOR'ing it with the corresponding
         * secret key if that bit is 1. Do two bits each round in the loop
         * covering 30 bits, the least significant bit is handled outside
         * the loop.
         */
        immed[iter, 14]

do_repeat_loop:
        alu_shf[bit_val, --, B, bit_val, << 1]
        bpl[toeplitz_block_skip_2]
        alu[result, result, XOR, *sk0]
toeplitz_block_skip_2:
        dbl_shf[*sk0, *sk0, *sk1, >> 31]
        alu_shf[*sk1, --, B, *sk1, << 1]

        alu_shf[bit_val, --, B, bit_val, << 1]
        bpl[toeplitz_block_skip_3]
        alu[result, result, XOR, *sk0]
toeplitz_block_skip_3:
        alu[iter, iter, -, 1]
        bge[do_repeat_loop], defer[2]
        dbl_shf[*sk0, *sk0, *sk1, >> 31]
        alu_shf[*sk1, --, B, *sk1, << 1]

        /* Take care of the last bit. */
        alu_shf[bit_val, --, B, bit_val, << 1]
        bpl[toeplitz_block_skip_4]
        alu[result, result, XOR, *sk0]
toeplitz_block_skip_4:
        dbl_shf[*sk0, *sk0, *sk1, >> 31]
        alu_shf[*sk1, --, B, *sk1, << 1]
    }

    return result;
}

__intrinsic static void
__hash_toeplitz_copy(void *s, int n)
{
    REGPTR src = s;
    REGPTR dst = s;

    switch (n) {
    case 1:
        dst[1] = src[2];
        break;
    case 2:
        dst[1] = src[2];
        dst[2] = src[3];
        break;
    case 3:
        dst[1] = src[2];
        dst[2] = src[3];
        dst[3] = src[4];
        break;
    case 4:
        dst[1] = src[2];
        dst[2] = src[3];
        dst[3] = src[4];
        dst[4] = src[5];
        break;
    case 5:
        dst[1] = src[2];
        dst[2] = src[3];
        dst[3] = src[4];
        dst[4] = src[5];
        dst[5] = src[6];
        break;
    case 6:
        dst[1] = src[2];
        dst[2] = src[3];
        dst[3] = src[4];
        dst[4] = src[5];
        dst[5] = src[6];
        dst[6] = src[7];
        break;
    case 7:
        dst[1] = src[2];
        dst[2] = src[3];
        dst[3] = src[4];
        dst[4] = src[5];
        dst[5] = src[6];
        dst[6] = src[7];
        dst[7] = src[8];
        break;
    case 8:
        dst[1] = src[2];
        dst[2] = src[3];
        dst[3] = src[4];
        dst[4] = src[5];
        dst[5] = src[6];
        dst[6] = src[7];
        dst[7] = src[8];
        dst[8] = src[9];
        break;
    }
}


/*
 * Toeplitz hash implementation for RSS. Sources:
http://msdn.microsoft.com/en-us/library/windows/hardware/ff570725%28v=vs.85%29.aspx
and
http://download.microsoft.com/download/5/D/6/5D6EAF2B-7DDF-476B-93DC-7CF0072878E6/NDIS_RSS.doc
 */
__intrinsic uint32_t
hash_toeplitz(void *s, size_t n, void *k, size_t nk)
{
    __xrw unsigned *t = s;
    __gpr uint32_t sk[HASH_TOEPLITZ_SECRET_KEY_SZ/sizeof(uint32_t)];
    __gpr int result = 0;
    __gpr uint32_t tmp;
    int i, j, sk_idx, num_words = n >> 2;
    unsigned int current_ctx = ctx();

    /* Make sure the parameters are as we expect */
    ctassert(__is_in_reg_or_lmem(s));
    ctassert(!__is_xfer_reg(s));
    /* support IPv4 only, 4-tuple IPv4, IPv6 only or 4-tuple IPv6 */
    try_ctassert(n == 8 || n == 12 || n == 32 || n == 36);
    ctassert(__is_in_reg_or_lmem(k));
    ctassert(__is_ct_const(nk));
    ctassert(nk == HASH_TOEPLITZ_SECRET_KEY_SZ);

    /* create local copy of the secret key */
    reg_cp((void *)sk, k, nk);

    local_csr_write(local_csr_csr_ctx_pointer, current_ctx);
    __asm __attribute(LITERAL_ASM) { {nop} {nop} {nop} };

    result = hash_toeplitz_block(t[0], result, &sk[0], &sk[1]);
    __hash_toeplitz_copy(sk, num_words - 1);

    result = hash_toeplitz_block(t[1], result, &sk[0], &sk[1]);
    if (n == 8)
        goto out;

    __hash_toeplitz_copy(sk, num_words - 2);

    result = hash_toeplitz_block(t[2], result, &sk[0], &sk[1]);
    if (n == 12)
        goto out;

    __hash_toeplitz_copy(sk, num_words - 3);

    result = hash_toeplitz_block(t[3], result, &sk[0], &sk[1]);
    __hash_toeplitz_copy(sk, num_words - 4);

    result = hash_toeplitz_block(t[4], result, &sk[0], &sk[1]);
    __hash_toeplitz_copy(sk, num_words - 5);

    result = hash_toeplitz_block(t[5], result, &sk[0], &sk[1]);
    __hash_toeplitz_copy(sk, num_words - 6);

    result = hash_toeplitz_block(t[6], result, &sk[0], &sk[1]);
    __hash_toeplitz_copy(sk, num_words - 7);

    result = hash_toeplitz_block(t[7], result, &sk[0], &sk[1]);
    if (n == 32)
        goto out;

    __hash_toeplitz_copy(sk, num_words - 8);

    result = hash_toeplitz_block(t[8], result, &sk[0], &sk[1]);

out:
    return result;
}

#endif /* !_STD__HASH_C_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
