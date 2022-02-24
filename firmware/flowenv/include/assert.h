/*
 * Copyright (C) 2012-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          include/assert.h
 * @brief         Assert macros
 */
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <nfp.h>

/*
 * Compile time asserts are always enabled.  Runtime asserts are
 * disabled if NDEBUG or NOASSERT is defined.
 */

#define __CT_ASSERT(expr) __ct_assert(expr, #expr)

/**
 * Perform compile time assertion
 * @param expr          expression to test
 */
#define ctassert(expr)                          \
    do {                                        \
        __intrinsic_begin();                    \
        __CT_ASSERT(expr);                      \
        __intrinsic_end();                      \
    } while (0)

/**
 * Attempt to perform compile time assertion
 * @param expr          expression to test
 *
 * Assertion is not performed unless @expr is a constant expression.
 */
#define try_ctassert(expr)                      \
    do {                                        \
        __intrinsic_begin();                    \
        if (__is_ct_const(expr))                \
            __CT_ASSERT(expr);                  \
        __intrinsic_end();                      \
    } while (0)

/**
 * Print error message and halt compilation
 * @param msg           message to print
 */
#define cterror(msg)                            \
    do {                                        \
        __intrinsic_begin();                    \
        __ct_assert(0, msg);                    \
        __intrinsic_end();                      \
    } while (0)


#if defined(NDEBUG) || defined(NOASSERT)

/* Turn assert() into a try_ctassert() if compile time constant */
#define assert(expr)                            \
    do {                                        \
        __intrinsic_begin();                    \
        if (__is_ct_const(expr))                \
            __CT_ASSERT(expr);                  \
        __intrinsic_end();                      \
    } while (0)

#else /* !NDEBUG */

/*
 * Lightweight runtime asserts.  The ME is suspended and mailbox1
 * contains the address where the problem occurred. mailbox 0 is set
 * to zero.  Excluding the condition check the assert uses about 4
 * instructions.  You need to link against libnfp to use runtime asserts.
 */

__intrinsic void ___rt_assert(void *addr);

__intrinsic void
__rt_assert(void)
{
    __gpr unsigned int reg;
    __asm {
        load_addr[reg, label];
    label:
    }
    ___rt_assert((void *) reg);
}

#define __RT_ASSERT(expr) (void)((expr) || (__rt_assert(), 0))

/**
 * Perform assertion
 * @param expr          expression to test
 *
 * If expression is a compile time constant then do a compile time
 * assertion.  Othterwise do a run time assertion.
 */
#define assert(expr)                            \
    do {                                        \
        __intrinsic_begin();                    \
        if (__is_ct_const(expr))                \
            __CT_ASSERT(expr);                  \
        else                                    \
            __RT_ASSERT(expr);                  \
        __intrinsic_end();                      \
    } while (0)


#endif /* !NDEBUG */

#endif /* !_ASSERT_H_ */
