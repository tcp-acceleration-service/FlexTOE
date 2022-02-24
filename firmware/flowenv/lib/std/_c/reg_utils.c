/*
 * Copyright (C) 2014-2015, Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/std/_c/reg_utils.c
 * @brief         Functions to treat structures in registers as memory.
 */

#ifndef _STD__REG_UTILS_C_
#define _STD__REG_UTILS_C_

#include <assert.h>
#include <nfp.h>
#include <types.h>
#include <std/reg_utils.h>

/* This module provides functions to manipulate "memory" regions
 * located in registers (or local memory).  Programs often cache data
 * structures in arrays of registers and it is convenient to have
 * functions to zero or compare these structures.  Unfortunately, the
 * compiler does not provide full support for arrays and such located
 * in all types of registers (primarily due to not all registers sets
 * being addressable via index registers).  This module provides some
 * utility functions, manually unrolling loops.
 */

/* The following intrinsic functions could be implemented in a simpler
 * way. For a simplified reg_cp function for only 4 bytes:
 * __intrinsic
 * reg_cp_4B(void *d, void *s)
 * {
 *     uint32_t *dst = d;
 *     uint32_t *src = s;
 *
 *     dst[0] = s[0];
 * }
 *
 * Due to current compiler limitation the void * must be cast to
 * specific register types. For Local Memory a cast with __lmem must
 * be used, while for other register types (GPRs, Transfer Registers,
 * and Next Neighbor Registers), somewhat counterintuitively, a cast
 * to __gpr is sufficient. Without the cast the pointers will default
 * to the wrong size resulting in compilation errors or runtime issues
 * (THSDK-1913).
 */

/* TODO: Investigate if a register type is indexable, convert to a loop. */
/* TODO: Expand to handle larger 128 if in 4 context mode. */


/* MACRO for reg_set and reg_cp switch case implementation */
#define _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, EXEC_MACRO)                     \
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



/* MACRO for reg_eq switch case implementation */
#define _REG_UTILS_EQ_SWITCH_CASE_IMPLEMENT(n)                              \
{                                                                           \
        switch (n) {                                                        \
        case 4:                                                             \
            ret = __REG_EQ( 0);                                             \
            break;                                                          \
        case 8:                                                             \
            ret = (__REG_EQ( 0) && __REG_EQ( 1));                           \
            break;                                                          \
        case 12:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2));           \
            break;                                                          \
        case 16:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3));                                           \
            break;                                                          \
        case 20:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4));                           \
            break;                                                          \
        case 24:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5));           \
            break;                                                          \
        case 28:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6));                                           \
            break;                                                          \
        case 32:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7));                           \
            break;                                                          \
        case 36:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8));           \
            break;                                                          \
        case 40:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9));                                           \
            break;                                                          \
        case 44:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9) && __REG_EQ(10));                           \
            break;                                                          \
        case 48:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9) && __REG_EQ(10) && __REG_EQ(11));           \
            break;                                                          \
        case 52:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9) && __REG_EQ(10) && __REG_EQ(11) &&          \
                   __REG_EQ(12));                                           \
            break;                                                          \
        case 56:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9) && __REG_EQ(10) && __REG_EQ(11) &&          \
                   __REG_EQ(12) && __REG_EQ(13));                           \
            break;                                                          \
        case 60:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9) && __REG_EQ(10) && __REG_EQ(11) &&          \
                   __REG_EQ(12) && __REG_EQ(13) && __REG_EQ(14));           \
            break;                                                          \
        case 64:                                                            \
            ret = (__REG_EQ( 0) && __REG_EQ( 1) && __REG_EQ( 2) &&          \
                   __REG_EQ( 3) && __REG_EQ( 4) && __REG_EQ( 5) &&          \
                   __REG_EQ( 6) && __REG_EQ( 7) && __REG_EQ( 8) &&          \
                   __REG_EQ( 9) && __REG_EQ(10) && __REG_EQ(11) &&          \
                   __REG_EQ(12) && __REG_EQ(13) && __REG_EQ(14) &&          \
                   __REG_EQ(15));                                           \
            break;                                                          \
        }                                                                   \
}


__intrinsic void
reg_set(void *d, unsigned int s_val, size_t n)
{
    /* Make sure the parameters are as we expect */
    ctassert(__is_in_reg_or_lmem(d));
    ctassert(__is_ct_const(n));
    ctassert(n <= 64);
    ctassert((n % 4) == 0);
#ifdef __REG_SET
    #error "Attempting to redefine __REG_SET"
#endif

    if (__is_in_lmem(d)) {
        /* if type lmem must be defined as such */
#define __REG_SET(_x) ((__lmem unsigned int *)d)[_x] = s_val

        _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, __REG_SET);

#undef __REG_SET
    } else {
        /* else is a register type */
#define __REG_SET(_x) ((__gpr unsigned int *)d)[_x] = s_val

        _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, __REG_SET);

#undef __REG_SET
    }
}

__intrinsic void
reg_zero(void *s, size_t n)
{
    reg_set(s, 0, n);
}

__intrinsic void
reg_cp(void *d, const void *s, size_t n)
{
    /* Make sure the parameters are as we expect */
    ctassert(__is_in_reg_or_lmem(d));
    ctassert(__is_in_reg_or_lmem(s));
    ctassert(__is_ct_const(n));
    ctassert(n <= 64);
    ctassert((n % 4) == 0);

#ifdef __REG_CP
    #error "Attempting to redefine __REG_CP"
#endif
    if (__is_in_lmem(d) && __is_in_lmem(s)) {
        /* both dst and src are lmem */
#define __REG_CP(_x) (((__lmem unsigned int *)d)[_x] = \
                      ((__lmem unsigned int *)s)[_x])

        _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, __REG_CP);

#undef __REG_CP
    } else if (__is_in_lmem(d) && __is_in_reg(s)){
        /* dst is lmem and src is reg */
#define __REG_CP(_x) (((__lmem unsigned int *)d)[_x] = \
                      ((__gpr  unsigned int *)s)[_x])

        _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, __REG_CP);

#undef __REG_CP
    } else if (__is_in_reg(d) && __is_in_lmem(s)){
        /* else if dst is reg and src is lmem */
#define __REG_CP(_x) (((__gpr  unsigned int *)d)[_x] = \
                      ((__lmem unsigned int *)s)[_x])

        _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, __REG_CP);

#undef __REG_CP
    } else {
        /* both dst and src are reg */
#define __REG_CP(_x) (((__gpr unsigned int *)d)[_x] = \
                      ((__gpr unsigned int *)s)[_x])

        _REG_UTILS_SWITCH_CASE_IMPLEMENT(n, __REG_CP);

#undef __REG_CP
    }
}

__intrinsic int
reg_eq(const void *s1, const void *s2, size_t n)
{
    int ret = 0;

    /* Make sure the parameters are as we expect */
    ctassert(__is_in_reg_or_lmem(s1));
    ctassert(__is_in_reg_or_lmem(s2));
    ctassert(__is_ct_const(n));
    ctassert(n <= 64);
    ctassert((n % 4) == 0);

#ifdef __REG_EQ
    #error "Attempting to redefine __REG_EQ"
#endif

    if (__is_in_lmem(s1) && __is_in_lmem(s2)) {
        /* both s1 and s2 are lmem */
#define __REG_EQ(_x) (((__lmem unsigned int *)s1)[_x] == \
                      ((__lmem unsigned int *)s2)[_x])

        _REG_UTILS_EQ_SWITCH_CASE_IMPLEMENT(n);

#undef __REG_EQ
    } else if (__is_in_lmem(s1) && __is_in_reg(s2)) {
        /* s1 is lmem and s2 is reg */
#define __REG_EQ(_x) (((__lmem unsigned int *)s1)[_x] == \
                      ((__gpr  unsigned int *)s2)[_x])

        _REG_UTILS_EQ_SWITCH_CASE_IMPLEMENT(n);

#undef __REG_EQ
    } else if (__is_in_reg(s1) && __is_in_lmem(s2)) {
        /* s1 is reg and s2 is lmem */
#define __REG_EQ(_x) (((__gpr  unsigned int *)s1)[_x] == \
                      ((__lmem unsigned int *)s2)[_x])

        _REG_UTILS_EQ_SWITCH_CASE_IMPLEMENT(n);

        #undef __REG_EQ
    } else {
        /* s1 and s2 are reg */
#define __REG_EQ(_x) (((__gpr unsigned int *)s1)[_x] == \
                      ((__gpr unsigned int *)s2)[_x])

        _REG_UTILS_EQ_SWITCH_CASE_IMPLEMENT(n);

#undef __REG_EQ
    }
    return ret;
}

#endif /* !_STD__REG_UTILS_C_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
