/*
 * Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/net/mpls.h
 * @brief         Definitions for MPLS header parsing
 *
 */

#ifndef _NET_MPLS_H_
#define _NET_MPLS_H_

/* MPLS RFC 5462 is the latest but basic RFC was RFC 3032 */

/**
 * default MPLS UDP destination port number as per RFC 7510
 */
#define NET_MPLS_PORT             6635

#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>

/**
 * MPLS header structure
 */
__packed struct mpls_hdr {
    union {
        struct {
            unsigned int label:20;              /** Label */
            unsigned int tc:3;                  /** Traffic Class */
            unsigned int s:1;                   /** Bottom of Stack */
            unsigned int ttl:8;                 /** Time to Live */
        };
        unsigned int __raw;
    };
};

#endif /* __NFP_LANG_MICROC */

#endif /* _NET_MPLS_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
