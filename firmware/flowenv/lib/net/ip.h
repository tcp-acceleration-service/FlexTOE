/*
 * Copyright (C) 2012-2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/net/ip.h
 * @brief         Definitions for IPv4 and IPv6 header parsing
 *
 * Incomplete, new definitions will be added as/when needed
 */

#ifndef _NET_IP_H_
#define _NET_IP_H_

/**
 * Header length definitions
 * @NET_IP4_LEN             Length of a IPv4 header, no options
 * @NET_IP4_LEN32           Length of a IPv4 header, no options (32bit words)
 * @NET_IP6_LEN             Length of a IPv6 header, no extensions
 * @NET_IP6_LEN32           Length of a IPv6 header, no extensions (32bit words)
 */
#define NET_IP4_LEN              20
#define NET_IP4_LEN32            (NET_IP4_LEN / 4)
#define NET_IP6_LEN              40
#define NET_IP6_LEN32            (NET_IP6_LEN / 4)


/**
 * An incomplete list of IP protocol numbers
 * See: http://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
 * See: http://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml
 */
#define NET_IP_PROTO_HOPOPT   0x00 /** IPv6 Hop-by-Hop Option ext header*/
#define NET_IP_PROTO_ICMP     0x01 /** Internet Control Message Protocol */
#define NET_IP_PROTO_IPV4     0x04 /** IPv4 (encapsulation)*/
#define NET_IP_PROTO_TCP      0x06 /** Transmission Control Protocol */
#define NET_IP_PROTO_UDP      0x11 /** User Datagram Protocol */
#define NET_IP_PROTO_IPV6     0x29 /** IPv6 (encapsulation) */
#define NET_IP_PROTO_ROUTING  0x2b /** Routing Header for IPv6 ext header */
#define NET_IP_PROTO_FRAG     0x2c /** Fragment Header for IPv6 ext header*/
#define NET_IP_PROTO_GRE      0x2f /** Generic Routing Encapsulation */
#define NET_IP_PROTO_ESP      0x32 /** Encapsulating Security Payload */
#define NET_IP_PROTO_AH       0x33 /** Authentication Header */
#define NET_IP_PROTO_ICMPV6   0x3a /** ICMP for IPv6 */
#define NET_IP_PROTO_NONE     0x3b /** No Next Header for IPv6 */
#define NET_IP_PROTO_DSTOPTS  0x3c /** Destination Options IPv6 ext header*/
#define NET_IP_PROTO_SCTP     0x84 /** Stream Control Transmission Protocol */
#define NET_IP_PROTO_MOBILITY 0x87 /** Mobility Header for IPv6 */
#define NET_IP_PROTO_HOST_ID  0x8b /** Host Identity Prot IPv6 ext header*/
#define NET_IP_PROTO_SHIM6    0x8c /** Shim6 Protocol for IPv6 ext header */


/**
 * IP flags and fragment handling
 */
#define NET_IP_FRAG_OFF_MASK 0x1fff     /** Mask for the fragment offset */
#define NET_IP_FLAGS_CE      0x8000     /** Congestion */
#define NET_IP_FLAGS_DF      0x4000     /** Don't fragment flag  */
#define NET_IP_FLAGS_MF      0x2000     /** More fragments flag  */
/* Convenience macro to determine if packet is fragmented */
#define NET_IP_IS_FRAG(_frag) \
    (_frag & (NET_IP_FRAG_OFF_MASK | NET_IP_FLAGS_MF))


#define NET_IP_MCAST_ADDR      0xE0000000
#define NET_IP_MCAST_ADDR_MASK 0xF0000000
#define NET_IP_BCAST_ADDR      0xFFFFFFFF
/* Convenience macro determine if IPv4 address is link-local Broadcast */
#define NET_IP_IS_BCAST(_addr) (_addr == NET_IP_BCAST_ADDR)
/* Convenience macro determine if IPv4 address is Multicast */
#define NET_IP_IS_MCAST(_addr) \
    ((_addr & NET_IP_MCAST_ADDR_MASK) == NET_IP_MCAST_ADDR)
/* Convenience macro determine if destination IPv4 address is
 * link-local Broadcast or Multicast */
#define NET_IP_IS_BMCAST(_addr) \
    (NET_IP_IS_MCAST(_addr) || NET_IP_IS_BCAST(_addr))


#define NET_IP6_MCAST_ADDR      0xFF

#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>

/**
 * IPv4 header structure
 */
__packed struct ip4_hdr {
    uint8_t  ver:4;                     /** Version */
    uint8_t  hl:4;                      /** Header length */
    uint8_t  tos;                       /** Type of service */
    uint16_t len;                       /** Total length */

    uint16_t id;                        /** Identification */
    uint16_t frag;                      /** Fragment offset field */

    uint8_t  ttl;                       /** Time to live */
    uint8_t  proto;                     /** Protocol */
    uint16_t sum;                       /** Checksum */

    uint32_t src;                       /** Source address */
    uint32_t dst;                       /** Destination address */
};


/**
 * IPv6 header structure
 */
/* Structure for a IPv6 address */
struct in6_addr {
    uint8_t  s6_addr[16];
};

__packed struct ip6_hdr {
    uint32_t ver:4;                     /** Version */
    uint32_t tc:8;                      /** Traffic class */
    uint32_t fl:20;                     /** Flow label */

    uint32_t len:16;                    /** Payload length */
    uint32_t nh:8;                      /** Next Header */
    uint32_t hl:8;                      /** Hop Limit */

    struct in6_addr src;                /** Source address */
    struct in6_addr dst;                /** Destination address */
};

/**
 * IPv6 Hop-by-Hop options header (NET_IP_PROTO_HOPOPT)
 */
__packed struct ip6_hbh {
    uint8_t nh;                         /** Next header */
    uint8_t len;                        /** Length of the extension header */
    uint16_t opt_or_pad0;               /** Options or padding */

    uint32_t opt_or_pad1;               /** Options or padding */
    /* Potentially followed by more options */
};

/**
 * IPv6 Routing header (NET_IP_PROTO_ROUTING)
 */
__packed struct ip6_rt {
    uint8_t nh;                         /** Next header */
    uint8_t len;                        /** Length of the extension header */
    uint8_t type;                       /** Routing type */
    uint8_t segleft;                    /** Segments left */

    uint32_t data;                      /** Routing data */
    /* Potentially followed by more routing data */
};

/**
 * IPv6 Fragment Header (NET_IP_PROTO_FRAG)
 */
__packed struct ip6_frag {
    uint8_t nh;                         /** Next header */
    uint8_t res0;                       /** Reserved, initialised 0 */
    uint16_t off:13;                    /** Fragment Offset */
    uint16_t res1:2;                    /** Reserved */
    uint16_t more:1;                    /** More fragments to follow */

    uint32_t id;                        /** Identification */
};

/**
 * IPv6 No Next Header (NET_IP_PROTO_NONE)
 */
__packed struct ip6_none {
    uint8_t nh;                         /** Next header */
    uint8_t len;                        /** Length of the extension header */
    uint16_t pad0;                      /** Padding */

    uint32_t pad1;                      /** Padding */
};

/**
 * IPv6 Destination Options (NET_IP_PROTO_DSTOPTS)
 */
__packed struct ip6_dst {
    uint8_t nh;                         /** Next header */
    uint8_t len;                        /** Length of the extension header */
    uint16_t opt_or_pad0;               /** Options or padding */

    uint32_t opt_or_pad1;               /** Options or padding */
    /* Potentially followed by more options */
};

/**
 * Check if IPv6 address is Multicast
 *
 * @param a     pointer for buffer containing IPv6 address to be evaluated
 *
 * @return      True or False
 */
__intrinsic static int
net_ipv6_is_mc_addr(void *a)
{
    int is_mc_addr;
    ctassert(__is_in_reg_or_lmem(a));
    if (__is_in_lmem(a)) {
        is_mc_addr = (((__lmem struct in6_addr *)a)->s6_addr[0] ==
                      NET_IP6_MCAST_ADDR);
    } else {
        is_mc_addr = (((__gpr struct in6_addr *)a)->s6_addr[0] ==
                      NET_IP6_MCAST_ADDR);
    }
    return is_mc_addr;
}

#endif /* __NFP_LANG_MICROC */

#endif /* _NET_IP_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
