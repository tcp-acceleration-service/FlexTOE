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
 * @file          lib/net/icmp.h
 * @brief         Definitions for ICMP header parsing
 *
 * Incomplete, new definitions will be added as/when needed
 */

#ifndef _NET_ICMP_H_
#define _NET_ICMP_H_

/**
 * Header length definitions
 * @NET_ICMP_LEN            Length of ICMP header
 * @NET_ICMP_LEN32          Length of ICMP header (32bit words)
 */
#define NET_ICMP_LEN              8
#define NET_ICMP_LEN32            (NET_ICMP_LEN / 4)

/**
 * ICMP type definitions
 */
#define NET_ICMP_TYPE_ECHO_REPLY     0
#define NET_ICMP_TYPE_DEST_UNREACH   3
#define NET_ICMP_TYPE_REDIRECT       5
#define NET_ICMP_TYPE_ECHO           8
#define NET_ICMP_TYPE_TIME_EXCEEDED 11
#define NET_ICMP_TYPE_PARAM_PROB    12

/**
 * ICMPv6 type definitions
 */
#define NET_ICMP6_TYPE_PKT_TOO_BIG  2
#define NET_ICMP6_TYPE_ECHO_REQ     128
#define NET_ICMP6_TYPE_ECHO_REPLY   129
#define NET_ICMP6_TYPE_ROUTER_SOL   133
#define NET_ICMP6_TYPE_NEIGH_SOL    135
#define NET_ICMP6_TYPE_NEIGH_AD     136

/* Convenience macro determine if ICMP is ECHO */
#define NET_ICMP_IS_ECHO(_type) ((_type == NET_ICMP_TYPE_ECHO_REPLY) ||      \
                                 (_type == NET_ICMP_TYPE_ECHO))

/* Convenience macro determine if ICMP is ERROR */
/* http://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml used
 * for reference.
 */
#define NET_ICMP_IS_ERROR(_type) ((_type == NET_ICMP_TYPE_DEST_UNREACH) ||   \
                                  (_type == NET_ICMP_TYPE_REDIRECT) ||       \
                                  (_type == NET_ICMP_TYPE_TIME_EXCEEDED) ||     \
                                  (_type == NET_ICMP_TYPE_PARAM_PROB))

/* Convenience macro determine if ICMPv6 is ECHO */
#define NET_ICMP6_IS_ECHO(_type) ((_type == NET_ICMP6_TYPE_ECHO_REPLY) ||    \
                                 (_type == NET_ICMP6_TYPE_ECHO_REQ))

/* Convenience macro determine if ICMPv6 is ERROR */
#define NET_ICMP6_IS_ERROR(_type) (_type < NET_ICMP6_TYPE_ECHO_REQ)

#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>
#include <net/ip.h>

/**
 * ICMP Header structure
 */
__packed struct icmp_hdr {
    uint8_t type;    /** ICMP type */
    uint8_t code;    /** ICMP code */
    uint16_t csum;   /** ICMP checksum */

    /* only valid for echo types */
    uint16_t eid;    /** ICMP identifier */
    uint16_t eseq;   /** ICMP sequence number */
};

/**
 * ICMP Error msg structure
 */
__packed struct icmpv4_err {
    uint8_t type;    /** ICMP type */
    uint8_t code;    /** ICMP code */
    uint16_t csum;   /** ICMP checksum */
    struct ip4_hdr ip;
    uint32_t data[2];
};

#endif /* __NFP_LANG_MICROC */

#endif /* _NET_ICMP_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
