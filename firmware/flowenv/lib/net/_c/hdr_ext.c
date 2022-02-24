/*
 * Copyright 2012-2016 Netronome, Inc.
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
 * @file          lib/net/_c/hdr_ext.c
 * @brief         Header extract code
 */

#include <assert.h>
#include <nfp.h>

#include "arp.h"
#include "eth.h"
#include "hdr_ext.h"
#include "icmp.h"
#include "ip.h"
#include "sctp.h"
#include "tcp.h"
#include "udp.h"
#include "esp.h"
#include "ah.h"
#include "vxlan.h"

/*
 * Some implementation notes, and notes on possible improvements:
 *
 * The functions below currently only work on LM buffers as the source
 * as LM allow arbitrary indexing.  The function should support any
 * type of destination buffers, though this has not been tested
 * extensively.
 *
 * The functions can (and should) be extended to support other sources
 * for the packet data.
 * - Supporting extraction directly from DRAM/CLS/SRAM/CTM should be
 *   pretty straight forward as the C compiler can generate the
 *   required code, for example: *(struct eth_hdr *)dst = *(__dram
 *   struct eth_hdr *)(((__dram char *)src_buf) + off); Note, however this
 *   will obviously cause the compiler to generate memory
 *   transactions.
 * - Transfer registers only provide limited index-ability with
 *   run-time computed indices.  In fact the capability is limited
 *   enough that the C compiler can't generate the code.  To support
 *   source buffers located in transfer registers, the functions below
 *   need to be extended with custom inline assembler functions
 *   explicitly setting the TINDEX CSR and make sure that no context
 *   switches happen.  The nfcc user guide has some sample code for
 *   this.  This code will also will have to deal with re-aligning the
 *   packet header.
 * - GPRs and NN registers: These registers are not indexable at all
 *   so one need to deploy a switch statement of sort to turn the
 *   run-time computable offset into a compile time one, like, e.g:
 *   switch (off) {
 *   case 2:
 *       (struct eth_hdr *)dst = *(struct eth_hdr *)(((char *)src_buf) + 2);
 *       break
 *   case 4:
 *       (struct eth_hdr *)dst = *(struct eth_hdr *)(((char *)src_buf) + 4);
 *       break
 *   [...]
 *   }
 *   This obviously explodes the code store usage of the functions.
 *
 *
 * Note: I tried to enforce certain properties in @dst using ctassert,
 * but both:
 * ctassert(__is_ct_const(dst)); and
 * ctassert(__aligned(dst, 4));
 * failed.
 */

/* Note that you can selectively omit protocols from being detected by
 * adding the OMIT_NET_XXX to your active #defines when compiling this
 * library. By omitting protocols you can also save on code store and control
 * looking for protocols that you wish to not process. All omitted protocol
 * when not found will return as HE_UNKNOWN.
 */

/* ------- Ethertype header extract switch statements -------- */

/* IPv4 */
#ifndef OMIT_NET_ETH_TYPE_IPV4
#define CASE_NET_ETH_TYPE_IPV4 \
    case NET_ETH_TYPE_IPV4: next_proto = HE_IP4; break;
#else
#define CASE_NET_ETH_TYPE_IPV4
#endif

/* VLAN tags */
#ifndef OMIT_NET_ETH_TYPE_TPID
#define CASE_NET_ETH_TYPE_TPID \
    case NET_ETH_TYPE_TPID: next_proto = HE_8021Q; break;
#else
#define CASE_NET_ETH_TYPE_TPID
#endif

/* IPv6 tags */
#ifndef OMIT_NET_ETH_TYPE_IPV6
#define CASE_NET_ETH_TYPE_IPV6 \
    case NET_ETH_TYPE_IPV6: next_proto = HE_IP6; break;
#else
#define CASE_NET_ETH_TYPE_IPV6
#endif

/* ARP */
#ifndef OMIT_NET_ETH_TYPE_ARP
#define CASE_NET_ETH_TYPE_ARP \
    case NET_ETH_TYPE_ARP: next_proto = HE_ARP; break;
#else
#define CASE_NET_ETH_TYPE_ARP
#endif

/* MPLS */
#ifndef OMIT_NET_ETH_TYPE_MPLS
#define CASE_NET_ETH_TYPE_MPLS \
    case NET_ETH_TYPE_MPLS: next_proto = HE_MPLS; break;
#else
#define CASE_NET_ETH_TYPE_MPLS
#endif

/* Transparent Ethernet Bridging */
#ifndef OMIT_NET_ETH_TYPE_TEB
#define CASE_NET_ETH_TYPE_TEB \
    case NET_ETH_TYPE_TEB: next_proto = HE_ETHER; break;
#else
#define CASE_NET_ETH_TYPE_TEB
#endif


/* ------- IP / IPv6 header extract switch statements -------- */

/* ICMP */
#ifndef OMIT_NET_IP_PROTO_ICMP
#define CASE_NET_IP_PROTO_ICMP \
    case NET_IP_PROTO_ICMP: next_proto = HE_ICMP; break;
#else
#define CASE_NET_IP_PROTO_ICMP
#endif

/* TCP */
#ifndef OMIT_NET_IP_PROTO_TCP
#define CASE_NET_IP_PROTO_TCP \
    case NET_IP_PROTO_TCP: next_proto = HE_TCP; break;
#else
#define CASE_NET_IP_PROTO_TCP
#endif

/* UDP */
#ifndef OMIT_NET_IP_PROTO_UDP
#define CASE_NET_IP_PROTO_UDP \
    case NET_IP_PROTO_UDP: next_proto = HE_UDP; break;
#else
#define CASE_NET_IP_PROTO_UDP
#endif

/* GRE */
#ifndef OMIT_NET_IP_PROTO_GRE
#define CASE_NET_IP_PROTO_GRE \
    case NET_IP_PROTO_GRE: next_proto = HE_GRE; break;
#else
#define CASE_NET_IP_PROTO_GRE
#endif

/* ESP */
#ifndef OMIT_NET_IP_PROTO_ESP
#define CASE_NET_IP_PROTO_ESP \
    case NET_IP_PROTO_ESP: next_proto = HE_ESP; break;
#else
#define CASE_NET_IP_PROTO_ESP
#endif

/* Authentication header */
#ifndef OMIT_NET_IP_PROTO_AH
#define CASE_NET_IP_PROTO_AH \
    case NET_IP_PROTO_AH: next_proto = HE_AH; break;
#else
#define CASE_NET_IP_PROTO_AH
#endif

/* ICMPv6 */
#ifndef OMIT_NET_IP_PROTO_ICMPV6
#define CASE_NET_IP_PROTO_ICMPV6 \
    case NET_IP_PROTO_ICMPV6: next_proto = HE_ICMP; break;
#else
#define CASE_NET_IP_PROTO_ICMPV6
#endif

/* SCTP */
#ifndef OMIT_NET_IP_PROTO_SCTP
#define CASE_NET_IP_PROTO_SCTP \
    case NET_IP_PROTO_SCTP: next_proto = HE_SCTP; break;
#else
#define CASE_NET_IP_PROTO_SCTP
#endif

/* IPv6 Extension header types */
/* IPv6 Hop-by-hop option */
#ifndef OMIT_NET_IP_PROTO_HOPOPT
#define CASE_NET_IP_PROTO_HOPOPT \
    case NET_IP_PROTO_HOPOPT: next_proto = HE_IP6_HBH; break;
#else
#define CASE_NET_IP_PROTO_HOPOPT
#endif

/* IPv6 Routing option */
#ifndef OMIT_NET_IP_PROTO_ROUTING
#define CASE_NET_IP_PROTO_ROUTING \
    case NET_IP_PROTO_ROUTING: next_proto = HE_IP6_RT; break;
#else
#define CASE_NET_IP_PROTO_ROUTING
#endif

/* IPv6 Fragment header */
#ifndef OMIT_NET_IP_PROTO_FRAG
#define CASE_NET_IP_PROTO_FRAG \
    case NET_IP_PROTO_FRAG: next_proto = HE_IP6_FRAG; break;
#else
#define CASE_NET_IP_PROTO_FRAG
#endif

/* IP proto 'none' */
#ifndef OMIT_NET_IP_PROTO_NONE
#define CASE_NET_IP_PROTO_NONE \
    case NET_IP_PROTO_NONE: next_proto = HE_NONE; break;
#else
#define CASE_NET_IP_PROTO_NONE
#endif

/* IPv6 Destination options */
#ifndef OMIT_NET_IP_PROTO_DSTOPTS
#define CASE_NET_IP_PROTO_DSTOPTS \
    case NET_IP_PROTO_DSTOPTS: next_proto = HE_IP6_DST; break;
#else
#define CASE_NET_IP_PROTO_DSTOPTS
#endif

/* IPv6 Mobility header */
#ifndef OMIT_NET_IP_PROTO_MOBILITY
#define CASE_NET_IP_PROTO_MOBILITY \
    case NET_IP_PROTO_MOBILITY: next_proto = HE_IP6_MOB; break;
#else
#define CASE_NET_IP_PROTO_MOBILITY
#endif

/* IPv6 Host Identification header */
#ifndef OMIT_NET_IP_PROTO_HOST_ID
#define CASE_NET_IP_PROTO_HOST_ID \
    case NET_IP_PROTO_HOST_ID: next_proto = HE_IP6_HOST_ID; break;
#else
#define CASE_NET_IP_PROTO_HOST_ID
#endif

/* IPv6 Shim6 header */
#ifndef OMIT_NET_IP_PROTO_SHIM6
#define CASE_NET_IP_PROTO_SHIM6 \
    case NET_IP_PROTO_SHIM6: next_proto = HE_IP6_SHIM6; break;
#else
#define CASE_NET_IP_PROTO_SHIM6
#endif

__intrinsic int
he_eth_fit(sz, off)
{
    ctassert(sz >= sizeof(struct eth_hdr));
    return (off + sizeof(struct eth_hdr)) <= sz;
}

#define HE_ETH_FUNC(dst)                                                \
    *dst =  *(__lmem struct eth_hdr *)(((__lmem char *)src_buf) + off); \
                                                                        \
    switch(dst->type) {                                                 \
    CASE_NET_ETH_TYPE_IPV4                                              \
    CASE_NET_ETH_TYPE_TPID                                              \
    CASE_NET_ETH_TYPE_IPV6                                              \
    CASE_NET_ETH_TYPE_ARP                                               \
    CASE_NET_ETH_TYPE_MPLS                                              \
    default: next_proto = HE_UNKNOWN;                                   \
    }

__intrinsic unsigned int
he_eth(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

#ifdef __HE_ETH
    #error "Attempting to redefine __HE_ETH"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_ETH ((__lmem struct eth_hdr *)dst)
        HE_ETH_FUNC(__HE_ETH);
#undef __HE_ETH
    } else {
#define __HE_ETH ((__gpr struct eth_hdr *)dst)
        HE_ETH_FUNC(__HE_ETH);
#undef __HE_ETH
    }
    return HE_RES(next_proto, sizeof(struct eth_hdr));
}

__intrinsic int
he_vlan_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct vlan_hdr));
    return (off + sizeof(struct vlan_hdr)) <= sz;
}

#define HE_VLAN_FUNC(dst)                                               \
    *dst = *(__lmem struct vlan_hdr *)(((__lmem char *)src_buf) + off); \
    switch(dst->type) {                                                 \
    CASE_NET_ETH_TYPE_IPV4                                              \
    CASE_NET_ETH_TYPE_IPV6                                              \
    CASE_NET_ETH_TYPE_MPLS                                              \
    default: next_proto = HE_UNKNOWN;                                   \
    }

__intrinsic unsigned int
he_vlan(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;

#ifdef __HE_VLAN
    #error "Attempting to redefine __HE_VLAN"
#endif

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_VLAN ((__lmem struct vlan_hdr *)dst)
        HE_VLAN_FUNC(__HE_VLAN);
#undef __HE_VLAN
    } else {
#define __HE_VLAN ((__gpr struct vlan_hdr *)dst)
        HE_VLAN_FUNC(__HE_VLAN);
#undef __HE_VLAN
    }

    return HE_RES(next_proto, sizeof(struct vlan_hdr));
}

__intrinsic int
he_arp_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct arp_hdr));
    return (off + sizeof(struct arp_hdr)) <= sz;
}

__intrinsic unsigned int
he_arp(void *src_buf, int off, void *dst)
{
    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
        *((__lmem struct arp_hdr *)dst) =
            *(__lmem struct arp_hdr *)(((__lmem char *)src_buf) + off);
    } else {
        *((__gpr struct arp_hdr *)dst) =
            *(__lmem struct arp_hdr *)(((__lmem char *)src_buf) + off);
    }

    return HE_RES(HE_NONE, sizeof(struct arp_hdr));
}

__intrinsic int
he_ip4_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct ip4_hdr));
    return (off + sizeof(struct ip4_hdr)) <= sz;
}

#ifdef ADD_NET_IP4_CHECK
#define HE_IP4_CHECK(_dst)                                              \
    if (_dst->ver != 4)                                                 \
        next_proto = HE_ERROR_IP4_BAD_VER;                              \
    if (_dst->hl < 5)                                                   \
        next_proto = HE_ERROR_IP4_BAD_HL;                               \
    if (_dst->ttl <= 1)                                                 \
        next_proto = HE_ERROR_IP4_BAD_TTL;
#else
#define HE_IP4_CHECK(_dst)
#endif

#define HE_IP4_FUNC(dst)                                                \
    *dst = *(__lmem struct ip4_hdr *)(((__lmem char *)src_buf) + off);  \
                                                                        \
    switch(dst->proto) {                                                \
    CASE_NET_IP_PROTO_ICMP                                              \
    CASE_NET_IP_PROTO_TCP                                               \
    CASE_NET_IP_PROTO_UDP                                               \
    CASE_NET_IP_PROTO_GRE                                               \
    CASE_NET_IP_PROTO_ESP                                               \
    CASE_NET_IP_PROTO_AH                                                \
    CASE_NET_IP_PROTO_SCTP                                              \
    default: next_proto = HE_UNKNOWN;                                   \
    }                                                                   \
                                                                        \
    HE_IP4_CHECK(dst)                                                   \
                                                                        \
    if (dst->frag & NET_IP_FLAGS_MF)                                    \
        next_proto = HE_UNKNOWN;                                        \
                                                                        \
    ret = HE_RES(next_proto, 4 * dst->hl);

__intrinsic unsigned int
he_ip4(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr int ret;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

#ifdef __HE_IP4
    #error "Attempting to redefine __HE_IP4"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_IP4 ((__lmem struct ip4_hdr *)dst)
        HE_IP4_FUNC(__HE_IP4);
#undef __HE_IP4
    } else {
#define __HE_IP4 ((__gpr struct ip4_hdr *)dst)
        HE_IP4_FUNC(__HE_IP4);
#undef __HE_IP4
    }

    return ret;
}

__intrinsic int
he_ip6_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct ip6_hdr));
    return (off + sizeof(struct ip6_hdr)) <= sz;
}

#ifdef ADD_NET_IP6_CHECK
#define HE_IP6_CHECK(_dst)                                              \
    if (_dst->ver != 6)                                                 \
        next_proto = HE_ERROR_IP6_BAD_VER;                              \
    if (_dst->hl <= 1)                                                  \
        next_proto = HE_ERROR_IP6_BAD_HOP_LIMIT;
#else
#define HE_IP6_CHECK(_dst)
#endif

/* We use this portion of the switch statement in several places for
 * parsing IPv6 and extension header. Define it as a macro to avoid
 * code duplication.*/
#define _IP6_PROTO_SWITCH \
    CASE_NET_IP_PROTO_TCP                                           \
    CASE_NET_IP_PROTO_UDP                                           \
    CASE_NET_IP_PROTO_GRE                                           \
    CASE_NET_IP_PROTO_ICMPV6                                        \
    CASE_NET_IP_PROTO_SCTP                                          \
    CASE_NET_IP_PROTO_HOPOPT                                        \
    CASE_NET_IP_PROTO_ROUTING                                       \
    CASE_NET_IP_PROTO_FRAG                                          \
    CASE_NET_IP_PROTO_ESP                                           \
    CASE_NET_IP_PROTO_AH                                            \
    CASE_NET_IP_PROTO_NONE                                          \
    CASE_NET_IP_PROTO_DSTOPTS                                       \
    CASE_NET_IP_PROTO_MOBILITY                                      \
    CASE_NET_IP_PROTO_HOST_ID                                       \
    CASE_NET_IP_PROTO_SHIM6                                         \
    default: next_proto = HE_UNKNOWN

#define HE_IP6_FUNC(dst)                                                \
    *dst = *(__lmem struct ip6_hdr *)(((__lmem char *)src_buf) + off);  \
    switch(dst->nh) {                                                   \
        _IP6_PROTO_SWITCH;                                              \
    }                                                                   \
                                                                        \
    HE_IP6_CHECK(dst)                                                   \
                                                                        \
    ret = HE_RES(next_proto, sizeof(struct ip6_hdr));

__intrinsic unsigned int
he_ip6(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr int ret;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

#ifdef __HE_IP6
    #error "Attempting to redefine __HE_IP6"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_IP6 ((__lmem struct ip6_hdr *)dst)
        HE_IP6_FUNC(__HE_IP6);
#undef __HE_IP6
    } else {
#define __HE_IP6 ((__gpr struct ip6_hdr *)dst)
        HE_IP6_FUNC(__HE_IP6);
#undef __HE_IP6
    }

    return ret;
}

#undef _IP6_PROTO_SWITCH

__intrinsic int
he_tcp_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct tcp_hdr));
    return (off + sizeof(struct tcp_hdr)) <= sz;
}

#define HE_TCP_FUNC(dst)                                                \
    *dst = *(__lmem struct tcp_hdr *)(((__lmem char *)src_buf) + off);  \
                                                                        \
    next_proto = HE_NONE;                                               \
                                                                        \
    ret = HE_RES(next_proto, 4 * dst->off);

__intrinsic unsigned int
he_tcp(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr int ret;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

#ifdef __HE_TCP
    #error "Attempting to redefine __HE_TCP"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_TCP ((__lmem struct tcp_hdr *)dst)
        HE_TCP_FUNC(__HE_TCP);
#undef __HE_TCP
    } else {
#define __HE_TCP ((__gpr struct tcp_hdr *)dst)
        HE_TCP_FUNC(__HE_TCP);
#undef __HE_TCP
    }

    return ret;
}

__intrinsic int
he_udp_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct udp_hdr));
    return (off + sizeof(struct udp_hdr)) <= sz;
}

#define HE_UDP_FUNC(dst, prt)                                           \
    *dst = *(__lmem struct udp_hdr *)(((__lmem char *)src_buf) + off);  \
    if (prt && dst->dport == prt)                                       \
        next_proto = HE_VXLAN;                                          \
    else                                                                \
        next_proto = HE_NONE;

__intrinsic unsigned int
he_udp(void *src_buf, int off, void *dst, unsigned int vxln_prt)
{
    __gpr unsigned int next_proto;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

#ifdef __HE_UDP
    #error "Attempting to redefine __HE_UDP"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_UDP ((__lmem struct udp_hdr *)dst)
        HE_UDP_FUNC(__HE_UDP, vxln_prt);
#undef __HE_UDP
    } else {
#define __HE_UDP ((__gpr struct udp_hdr *)dst)
        HE_UDP_FUNC(__HE_UDP, vxln_prt);
#undef __HE_UDP
    }

    return HE_RES(next_proto, sizeof(struct udp_hdr));
}

__intrinsic int
he_gre_fit(sz, off)
{
    /* biggest GRE hdr has the optional checksum , key and sequence
     * number: total of 12B
     */
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct gre_hdr) + 12);
    return (off + sizeof(struct gre_hdr) + 12) <= sz;
}

#define HE_GRE_FUNC(dst)                                                \
    *dst = *(__lmem struct gre_hdr *)(((__lmem char *)src_buf) + off);  \
                                                                        \
    flags = dst->flags;                                                 \
                                                                        \
    switch(dst->proto) {                                                \
    CASE_NET_ETH_TYPE_TEB                                               \
    CASE_NET_ETH_TYPE_IPV4                                              \
    CASE_NET_ETH_TYPE_IPV6                                              \
    CASE_NET_ETH_TYPE_MPLS                                              \
    default: next_proto = HE_UNKNOWN;                                   \
    }

__intrinsic unsigned int
he_gre(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr uint32_t flags;
    __gpr uint32_t len;


    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_GRE ((__lmem struct gre_hdr *)dst)
        HE_GRE_FUNC(__HE_GRE);
#undef __HE_GRE
    } else {
#define __HE_GRE ((__gpr struct gre_hdr *)dst)
        HE_GRE_FUNC(__HE_GRE);
#undef __HE_GRE
    }

    /* Work out the length of the whole GRE header */
    len = 4;
    if (flags & NET_GRE_FLAGS_CSUM_PRESENT)
        len += 4;
    if (flags & NET_GRE_FLAGS_KEY_PRESENT)
        len += 4;
    if (flags & NET_GRE_FLAGS_SEQ_PRESENT)
        len += 4;

    return HE_RES(next_proto, len);
}

__intrinsic void
he_gre_nvgre(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto = 0;
    __gpr uint32_t len = 4;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    /* move offset to point to the start of the optional fields */
    off += sizeof(struct gre_hdr);

    if (__is_in_lmem(dst)) {
        *((__lmem struct nvgre_ext_hdr *)dst) =
            *(__lmem struct nvgre_ext_hdr *)(((__lmem char *)src_buf) + off);
    } else {
        *((__gpr struct nvgre_ext_hdr *)dst) =
            *(__lmem struct nvgre_ext_hdr *)(((__lmem char *)src_buf) + off);
    }
}

__intrinsic int
he_vxlan_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct vxlan_hdr));
    return (off + sizeof(struct vxlan_hdr)) <= sz;
}

#define HE_VXLAN_FUNC(dst)                                                \
    *dst = *(__lmem struct vxlan_hdr *)(((__lmem char *)src_buf) + off);

__intrinsic unsigned int
he_vxlan(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr uint32_t len;

    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_VXLAN ((__lmem struct vxlan_hdr *)dst)
        HE_VXLAN_FUNC(__HE_VXLAN);
#undef __HE_VXLAN
    } else {
#define __HE_VXLAN ((__gpr struct vxlan_hdr *)dst)
        HE_VXLAN_FUNC(__HE_VXLAN);
#undef __HE_VXLAN
    }

    next_proto = HE_ETHER;
    len = sizeof(struct vxlan_hdr);

    return HE_RES(next_proto, len);
}


__intrinsic int
he_mpls_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct mpls_hdr));
    return (off + sizeof(struct mpls_hdr)) <= sz;
}

#define HE_MPLS_FUNC(dst)                                                \
    *dst = *(__lmem struct mpls_hdr *)(((__lmem char *)src_buf) + off);

__intrinsic unsigned int
he_mpls(void *src_buf, int off, void *dst)
{
    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_MPLS ((__lmem struct mpls_hdr *)dst)
        HE_MPLS_FUNC(__HE_MPLS);
#undef __HE_MPLS
    } else {
#define __HE_MPLS ((__gpr struct mpls_hdr *)dst)
        HE_MPLS_FUNC(__HE_MPLS);
#undef __HE_MPLS
    }

    return HE_RES(HE_NONE, sizeof(struct mpls_hdr));
}


__intrinsic int
he_sctp_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct sctp_hdr));
    return (off + sizeof(struct sctp_hdr)) <= sz;
}

#define HE_SCTP_FUNC(dst)                                                    \
    *dst = *(__lmem struct sctp_hdr *)(((__lmem char *)src_buf) + off);      \
    ret = HE_RES(HE_NONE, sizeof(struct sctp_hdr));

__intrinsic unsigned int
he_sctp(void *src_buf, int off, void *dst)
{
    __gpr unsigned int ret;
    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_SCTP ((__lmem struct sctp_hdr *)dst)
        HE_SCTP_FUNC(__HE_SCTP);
#undef __HE_SCTP
    } else {
#define __HE_SCTP ((__gpr struct sctp_hdr *)dst)
        HE_SCTP_FUNC(__HE_SCTP);
#undef __HE_SCTP
    }

    return ret;
}


__intrinsic int
he_icmp_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct icmp_hdr));
    return (off + sizeof(struct icmp_hdr)) <= sz;
}

#define HE_ICMP_FUNC(dst)                                                    \
    *dst = *(__lmem struct icmp_hdr *)(((__lmem char *)src_buf) + off);      \
    ret = HE_RES(HE_UNKNOWN, sizeof(struct icmp_hdr));

__intrinsic unsigned int
he_icmp(void *src_buf, int off, void *dst)
{
    __gpr unsigned int ret;
    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_ICMP ((__lmem struct icmp_hdr *)dst)
        HE_ICMP_FUNC(__HE_ICMP);
#undef __HE_ICMP
    } else {
#define __HE_ICMP ((__gpr struct icmp_hdr *)dst)
        HE_ICMP_FUNC(__HE_ICMP);
#undef __HE_ICMP
    }

    return ret;
}


__intrinsic int
he_esp_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct esp_hdr));
    return (off + sizeof(struct esp_hdr)) <= sz;
}

#define HE_ESP_FUNC(dst)                                                    \
    *dst = *(__lmem struct esp_hdr *)(((__lmem char *)src_buf) + off);      \
    ret = HE_RES(HE_UNKNOWN, sizeof(struct esp_hdr));

__intrinsic unsigned int
he_esp(void *src_buf, int off, void *dst)
{
    __gpr unsigned int ret;
    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_ESP ((__lmem struct esp_hdr *)dst)
        HE_ESP_FUNC(__HE_ESP);
#undef __HE_ESP
    } else {
#define __HE_ESP ((__gpr struct esp_hdr *)dst)
        HE_ESP_FUNC(__HE_ESP);
#undef __HE_ESP
    }

    return ret;
}


__intrinsic int
he_ah_fit(sz, off)
{
    ctassert(__is_ct_const(sz));
    ctassert(sz >= sizeof(struct ah_hdr));
    return (off + sizeof(struct ah_hdr)) <= sz;
}

#define HE_AH_FUNC(dst)                                                    \
    *dst = *(__lmem struct ah_hdr *)(((__lmem char *)src_buf) + off);      \
    ret = HE_RES(HE_UNKNOWN, sizeof(struct ah_hdr));

__intrinsic unsigned int
he_ah(void *src_buf, int off, void *dst)
{
    __gpr unsigned int ret;
    ctassert(__is_in_lmem(src_buf));
    ctassert(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_AH ((__lmem struct ah_hdr *)dst)
        HE_AH_FUNC(__HE_AH);
#undef __HE_AH
    } else {
#define __HE_AH ((__gpr struct ah_hdr *)dst)
        HE_AH_FUNC(__HE_AH);
#undef __HE_AH
    }

    return ret;
}

