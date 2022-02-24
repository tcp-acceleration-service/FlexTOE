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
 * @file          lib/net/gre.h
 * @brief         Definitions for GRE header parsing
 *
 */

#ifndef _NET_GRE_H_
#define _NET_GRE_H_

/**
 * GRE has several forms depending on the RFC (1701, 2784, DRAFT NVGRE
 * or 2890. Here we focus on DRAFT NVGRE and 2890.
 * NVGRE has flags C=0, S=0 and K=1. Proto=0x6558 (ethernet bridging)
 * NVGRE is a particular case of of 2890
 */

/**
 * GRE Flags
 */
#define NET_GRE_FLAGS_SEQ_PRESENT      0x1
#define NET_GRE_FLAGS_KEY_PRESENT      0x2
#define NET_GRE_FLAGS_CSUM_PRESENT     0x8

/**
 * Convenience macro to determine if a GRE header is NVGRE
 * (assumming proto is NET_ETH_TYPE_TEB)
 */
#define NET_GRE_IS_NVGRE(flags)                   \
    ((!(flags & NET_GRE_FLAGS_CSUM_PRESENT) &&          \
      !(flags & NET_GRE_FLAGS_SEQ_PRESENT) &&           \
      (flags & NET_GRE_FLAGS_KEY_PRESENT))              \
     ? 1 : 0)

#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>

/**
 * GRE header structure
 */
__packed struct gre_hdr {
    unsigned int flags:4;               /** Flags */
    unsigned int reserved0:9;           /** Reserved bits */
    unsigned int version:3;             /** Version */
    uint16_t proto;                     /** Protocol */
};

__packed struct nvgre_ext_hdr {
    unsigned int vsid:24;              /** Virtual subnet ID */
    unsigned int flowID:8;             /** Flow ID */
};
#endif /* __NFP_LANG_MICROC */

#endif /* _NET_GRE_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
