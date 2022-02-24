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
 * @file          me/blocks/gro/_uc/wsm.uc
 */

#ifndef __WSM_UC
#define __WSM_UC

/* Single word field manipulation macros */

#macro __WSM_GET_MASKLEN(FLD)
    #if (FLD/**/_msk == 0xFFFFFFFF)
        #define_eval __WSM_MASKLEN 32
    #else
        #define_eval __WSM_MASKLEN (log2((FLD/**/_msk)+1))
    #endif
#endm


#macro _wsm_validate_size(INMACRO, EXPECTED, ACTUAL)

    #if (EXPECTED != 0) && (EXPECTED != ACTUAL)

        #error Macro INMACRO is expected to take EXPECTED instructions requires ACTUAL

    #endif

#endm


/*
 * Load a mask into an I/O register.  The mask will be a contiguous
 * set of 1 bites with length MASKLEN.  If NINSTR != 0 then the
 * macro must take exactly NINSTR instructions.
 */
#macro _wsm_loadmask(io_mask, IN_VAL, MASKLEN, INMACRO, NINSTR)

    #if (MASKLEN <= 16)

        _wsm_validate_size(INMACRO, NINSTR, 1)
        immed[io_mask, IN_VAL]

    #else

        _wsm_validate_size(INMACRO, NINSTR, 2)
        immed[io_mask, 0xFFFF]
        immed_w1[io_mask, (IN_VAL >> 16)]

    #endif

#endm


#macro sm_extract(out_word, in_word, FLD, NINSTR)
__WSM_GET_MASKLEN(FLD)

    #if (__WSM_MASKLEN <= 8)

        _wsm_validate_size('sm_extract', NINSTR, 1)
        alu[out_word, FLD/**/_msk, AND, in_word, >>FLD/**/_shf]

    #elif (__WSM_MASKLEN == 32 && FLD/**/_shf == 0)

        alu[out_word, --, B, in_word]

    #elif (__WSM_MASKLEN == 16 && FLD/**/_shf == 0)

        _wsm_validate_size(sm_extract, NINSTR, 1)
        ld_field_w_clr[out_word, 0011, in_word]

    #elif (__WSM_MASKLEN >= 24 && FLD/**/_shf == 0)

        _wsm_validate_size('sm_extract', NINSTR, 1)
        #define_eval __WSM_TMP ((1 << (32 - __WSM_MASKLEN)) - 1)
        alu[out_word, in_word, AND~, __WSM_TMP, <<__WSM_MASKLEN]
        #undef __WSM_TMP

    #else

        #ifdef __WSM_LOADMASK_NI
            #error "__WSM_LOADMASK_NI is already #defined!"
        #endif
        #if (NINSTR == 0)
            #define_eval __WSM_LOADMASK_NI 0
        #elif (NINSTR == 1)
            #error Macro sm_extract is expected to take 1 instruction but will require at least 2
        #else
            #define_eval __WSM_LOADMASK_NI (NINSTR - 1)
        #endif

        .begin
            .reg _wsm_mask
            _wsm_loadmask(_wsm_mask, FLD/**/_msk, __WSM_MASKLEN, 'sm_extract',
                          __WSM_LOADMASK_NI)
            alu[out_word, _wsm_mask, AND, in_word, >>FLD/**/_shf]
        .end

        #undef __WSM_LOADMASK_NI

    #endif

#undef __WSM_MASKLEN
#endm


#macro sm_extract(out_word, in_word, FLD)

    sm_extract(out_word, in_word, FLD, 0)

#endm


#macro sm_init(out_word, FLD, in_val, NINSTR)

    _wsm_validate_size('sm_init', NINSTR, 1)
    alu[out_word, --, B, in_val, <<FLD/**/_shf]

#endm


#macro sm_init(out_word, FLD, in_val)

    sm_init(out_word, FLD, in_val, 0)

#endm



#macro sm_clear_to(out_word, in_word, FLD, NINSTR)
__WSM_GET_MASKLEN(FLD)
.begin

    #if (__WSM_MASKLEN <= 8)

        _wsm_validate_size('sm_clear', NINSTR, 1)
        alu[out_word, in_word, AND~, FLD/**/_msk, <<FLD/**/_shf]

    #elif (__WSM_MASKLEN >= 24 && FLD/**/_shf == 0)

        _wsm_validate_size('sm_clear', NINSTR, 1)
        #define_eval __WSM_TMP ((1 << (32 - __WSM_MASKLEN)) - 1)
        alu[out_word, in_word, AND, __WSM_TMP, <<__WSM_MASKLEN]
        #undef __WSM_TMP

    #else

        #ifdef __WSM_LOADMASK_NI
            #error "__WSM_LOADMASK_NI is already #defined!"
        #endif
        #if (NINSTR == 0)
            #define_eval __WSM_LOADMASK_NI 0
        #elif (NINSTR == 1)
            #error Macro wsm_clear is expected to take 1 instruction but will require at least 2
        #else
            #define_eval __WSM_LOADMASK_NI (NINSTR - 1)
        #endif

        .begin
            .reg _wsm_mask
            _wsm_loadmask(_wsm_mask, FLD/**/_msk, __WSM_MASKLEN, 'sm_clear',
                          __WSM_LOADMASK_NI)
            alu[out_word, in_word, AND~, _wsm_mask, <<FLD/**/_shf]
        .end

        #undef __WSM_LOADMASK_NI

    #endif
.end
#undef __WSM_MASKLEN
#endm


#macro sm_clear_to(out_word, in_word, FLD)

    sm_clear_to(out_word, in_word, FLD, 0)

#endm


#macro sm_or_to(out_word, in_word, FLD, in_val, NINSTR)
.begin

    _wsm_validate_size('sm_or_to', NINSTR, 1)
    alu[out_word, in_word, OR, in_val, <<FLD/**/_shf]

.end
#endm


#macro sm_or_to(out_word, in_word, FLD, in_val)

    sm_or_to(out_word, in_word, FLD, in_val, 0)

#endm


#macro sm_set_to(out_word, in_word, FLD, in_val, NINSTR)
.begin

    #ifdef __WSM_SET_NI
        #error "__WSM_SET_NI is already #defined!"
    #endif
    #if (NINSTR == 0)
        #define_eval __WSM_SET_NI 0
    #elif (NINSTR == 1)
        #error Macro wsm_set is expected to take 1 instruction but will require at least 2
    #else
        #define_eval __WSM_SET_NI (NINSTR - 1)
    #endif

    sm_clear_to(out_word, in_word, FLD, __WSM_SET_NI)
    sm_or(out_word, in_word, FLD, in_val, 1)

    #undef __WSM_SET_NI

.end
#endm


#macro sm_set_to(out_word, in_word, FLD, in_val)

    sm_set_to(out_word, in_word, FLD, in_val, 0)

#endm



#macro sm_clear(io_word, FLD, NINSTR)

    sm_clear_to(io_word, io_word, FLD, NINSTR)

#endm


#macro sm_clear(io_word, FLD)

    sm_clear(io_word, FLD, 0)

#endm


#macro sm_or(io_word, FLD, in_val, NINSTR)

    sm_or_to(io_word, io_word, FLD, in_val, NINSTR)

#endm


#macro sm_or(io_word, FLD, in_val)

    sm_or(io_word, FLD, in_val, 0)

#endm


#macro sm_set(io_word, FLD, in_val, NINSTR)

    sm_set_to(io_word, io_word, FLD, in_val, NINSTR)

#endm


#macro sm_set(io_word, FLD, in_val)

    sm_set(io_word, FLD, in_val, 0)

#endm




/* Word Array Macros */
#macro wsm_extract(out_val, in_warr, FLD, NINSTR)

    sm_extract(out_val, in_warr[FLD/**/_wrd], FLD, NINSTR)

#endm


#macro wsm_extract(out_val, in_warr, FLD)

    wsm_extract(out_val, in_warr, FLD, 0)

#endm


#macro wsm_init(out_warr, FLD, in_val, NINSTR)

    sm_init(out_warr[FLD/**/_wrd], FLD, in_val, NINSTR)

#endm


#macro wsm_init(out_warr, FLD, in_val)

    wsm_init(out_warr, FLD, in_val, 0)

#endm

#macro wsm_clear_to(out_warr, in_word, FLD, NINSTR)

    sm_clear_to(out_warr[FLD/**/_wrd], in_word, FLD, NINSTR)

#endm


#macro wsm_clear_to(out_warr, in_word, FLD)

    wsm_clear_to(out_warr, in_word, FLD, 0)

#endm


#macro wsm_or_to(out_warr, in_word, FLD, in_val, NINSTR)

    sm_or_to(out_warr[FLD/**/_wrd], in_word, FLD, in_val, NINSTR)

#endm


#macro wsm_or_to(out_warr, in_word, FLD, in_val)

    wsm_or_to(out_warr, in_word, FLD, in_val, 0)

#endm


#macro wsm_set_to(out_warr, in_word, FLD, in_val, NINSTR)

    sm_set_to(out_warr[FLD/**/_wrd], in_word, FLD, in_val, NINSTR)

#endm


#macro wsm_set_to(out_warr, in_word, FLD, in_val)

    wsm_set_to(out_warr, in_word, FLD, in_val, 0)

#endm


#macro wsm_clear(io_warr, FLD, NINSTR)

    sm_clear(io_warr[FLD/**/_wrd], FLD, NINSTR)

#endm


#macro wsm_clear(io_warr, FLD)

    wsm_clear(io_warr, FLD, 0)

#endm


#macro wsm_or(io_warr, FLD, in_val, NINSTR)

    sm_or(io_warr[FLD/**/_wrd], FLD, in_val, NINSTR)

#endm


#macro wsm_or(io_warr, FLD, in_val)

    wsm_or(io_warr, FLD, in_val, 0)

#endm


#macro wsm_set(io_warr, FLD, in_val, NINSTR)

    sm_set(io_warr[FLD/**/_wrd], FLD, in_val, NINSTR)

#endm


#macro wsm_set(io_warr, FLD, in_val)

    wsm_set(io_warr, FLD, in_val, 0)

#endm


#endif /* __WSM_UC */
