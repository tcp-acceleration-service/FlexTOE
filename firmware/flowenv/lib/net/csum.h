/*
 * Copyright 2012-2018 Netronome, Inc.
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
 * @file          lib/net/csum.h
 * @brief         Full checksum calculation functions
 *
 */

#ifndef _NET_CSUM_H_
#define _NET_CSUM_H_

#include <stdint.h>

/**
 * This file contains API for 16bit one's complement checksum calculations.
 * Currently supporting IPv4 header checksum and TCP/UDP checksum for both
 * IPv4 and IPv6.
 * No current support for ICMP or ICMPv6 checksum.
 *
 * Few assumptions and limitations :
 * - The pkt buffer pointers (for both CTM and External memory) are assumed
 *   to be pointing to a 2 byte aligned address.
 * - L4 headers should not split between the CTM and External memory parts.
 * - If CTM buffer exists it is assumed that the packet headers are placed
 *   in CTM.
 */

/**
 * Add, with carry, two 1s compliment sums.
 * @param sum1  The first sum
 * @param sum2  The second sum
 * @return The sum of both values + the carry
 */
__intrinsic uint32_t ones_sum_add(uint32_t sum1, uint32_t sum2);

/**
 * Fold a 32-bit checksum into a 16 bit value.
 * @param sum  The 32 bits sum to fold
 * @return The 16 bits sum after folding
 */
__intrinsic uint16_t ones_sum_fold16(uint32_t sum);

/**
 * Compute the TCP/UDP pseudo header checksum.
 * @param ip_type   NET_ETH_TYPE_IPV4 or NET_ETH_TYPE_IPV6
 * @param protocol  NET_IP_PROTO_TCP or NET_IP_PROTO_UDP
 * @param ip        Pointer to the IP header
 * @param l4_hdr    Pointer to the L4 header
 * @param l4_len    The length of the L4 payload
 * @return A 32 bits sum of the pseudo header
 *
 * @ip and @l4_hdr must be located in LMEM or GPRs.
 *
 */
__intrinsic uint32_t ones_sum_pseudo(uint32_t ip_type, uint32_t protocol,
                                     void *ip, void *l4_hdr, uint32_t l4_len);

/**
 * Compute checksum over a word array.
 * @param buf   The buffer
 * @param len   The length (in bytes) of the array, must be <= 64. Does not
 *              have to be a mult of 4. If not, then the 4 - (len % 4)
 *              least significant bytes of the last word are omitted
 *              from the sum.
 * @return A 32 bits sum of the entire array
 */
__intrinsic uint32_t ones_sum_warr(__xread uint32_t *buf, uint32_t len);

/**
 * Compute checksum over a memory region.
 * @param buf   The buffer of 32 bits words
 * @param len   The length (in bytes) of the memory region, can be arbitrary
 * @return A 32 bits sum of the entire array
 */
__intrinsic uint32_t ones_sum_mem(__mem40 void *mem, int32_t len);

/**
 * Recalculate the checksum based on a single 16 bit value change.
 * @param orig_csum Original header checksum
 * @param orig_val  Original value of the 16bit to change
 * @param new_val   New value for the Original value of the 16bit to change
 * @return New checksum
 *
 * It is up to the caller to extract all of the relevant 16bit values.
 */
__intrinsic uint16_t net_csum_mod(uint32_t orig_csum, uint32_t orig_val,
                                  uint32_t new_val);

/**
 * Calculate the IPv4 header checksum.
 * @param ip        Pointer to the IPv4 header
 * @param pkt_ptr   Pointer to the packet's L4 header start
 * @return  IPv4 header checksum
 *
 * @ip (struct ip4_hdr) must be located in LMEM or GPRs.
 * If the IPv4 header contains options this function will access the packet's
 * buffer to load them, it is assumed that @pkt_ptr points to the beginning
 * of the payload (after any IPv4 options), therefore to access the options
 * fields this pointer will be adjusted to get to the first options word.
 * The header must be word aligned.
 *
 * This function can be used for checksum calculation if the checksum
 * field within the IPv4 header is set to 0 by the caller, or for
 * checksum verification.
 * In the case of checksum verification 0 will be returned if the checksum
 * value in the IP header is correct.
 */
__intrinsic uint16_t net_csum_ipv4(void *ip, __mem40 void *pkt_ptr);

/**
 * Calculate the IPv4 header checksum.
 * @param ip            Pointer to the IPv4 header
 * @param pkt_ptr       Pointer to the packet's L4 header start
 * @param test_ip_opt   A flag indicating if IP options should be considered
 * @return  IPv4 header checksum
 *
 * @ip (struct ip4_hdr) must be located in LMEM or GPRs.
 * If the IPv4 header contains options this function will access the packet's
 * buffer to load them, it is assumed that @pkt_ptr points to the beginning
 * of the payload (after any IPv4 options), therefore to access the options
 * fields this pointer will be adjusted to get to the first options word.
 * The header must be word aligned.
 *
 * @test_ip_opt is a compile time constant flag that gives the option to
 * disable/enable (0/1) the inclusion of the IP options in the checksum
 * calculation. It is recommended to enable it.
 *
 * This function can be used for checksum calculation if the checksum
 * field within the IPv4 header is set to 0 by the caller, or for
 * checksum verification.
 * In the case of checksum verification 0 will be returned if the checksum
 * value in the IP header is correct.
 */
__intrinsic uint16_t __net_csum_ipv4(void *ip, __mem40 void *pkt_ptr,
                                     const uint32_t test_ip_opt);

/**
 * Calculate the checksum of an IP packet.
 * @param ip_type   The IP type NET_ETH_TYPE_IPV4 or NET_ETH_TYPE_IPV6
 * @param protocol  The L4 protocol NET_IP_PROTO_TCP or NET_IP_PROTO_UDP
 * @param ip        Pointer to the IP header
 * @param l4_hdr    Pointer to the L4 header
 * @param pkt_ctm   Address of the start of the L4 header in CTM
 * @param ctm_len   The length of the L4 header + payload in CTM (can be 0).
 *                  Must be at least 8 for UDP and 20 for TCP if there is
 *                  a CTM buffer.
 * @param pkt_mem   Memory Unit Address of the remainder of the packet
 *                  contents not in CTM. May include the UDP/TCP header if
 *                  there is no CTM buffer.
 * @param mem_len   The length of the payload (and optionally L4 header if no
 *                  CTM) in external memory (can be 0).
 *                  Must be at least 8 for UDP and 20 for TCP if there is
 *                  no CTM buffer.
 * @return The calculated checksum
 *
 * @ip and @l4_hdr must be located in LMEM or GPRs.
 *
 * For checksum calculation the user must zero the checksum field in the
 * UDP/TCP header. For checksum verification 0 will be returned if the
 * checksum value in the UDP/TCP header is correct.
 *
 * For UDP, RFC768 instructs :
 * "If the computed  checksum  is zero,  it is transmitted  as all ones (the
 * equivalent  in one's complement  arithmetic)."
 * The return value from this function will NOT be adjusted to 0xFFFF in case
 * of a 0 result. It is the caller's responsibility to do so if the function
 * was called for checksum calculation (rather than just verification).
 *
 * A note about TCP options:
 * Since the TCP header structure does not contain the TCP options, if
 * TCP options are present (this is checked internally) the @ctm_len/@mem_len
 * and @ctm_ptr/@mem_ptr will be adjusted properly to account for the options
 * fields.
 */
__intrinsic uint16_t net_csum_l4_ip(uint32_t ip_type, uint32_t protocol,
                                    void *ip, void *l4_hdr,
                                    __mem40 void* pkt_ctm, uint32_t ctm_len,
                                    __mem40 void* pkt_mem, uint32_t mem_len);

/**
 * Calculate the checksum of an IPv4 UDP packet.
 * @param ip        Pointer to the IPv4 header
 * @param udp       Pointer to the UDP header
 * @param pkt_ctm   Address of the start of the L4 header in CTM
 * @param ctm_len   The length of the L4 header + payload in CTM (can be 0).
 *                  Must be at least 8 if there is a CTM buffer.
 * @param pkt_mem   Memory Unit Address of the remainder of the packet
 *                  contents not in CTM. May include the UDP header if there
 *                  is no CTM buffer.
 * @param mem_len   The length of the payload (and optionally L4 header if no
 *                  CTM) in external memory (can be 0).
 *                  Must be at least 8 if there is no CTM buffer.
 * @return The calculated checksum
 *
 * @ip (struct ip4_hdr) and @udp (struct udp_hdr) must be located in LMEM
 * or GPRs.
 *
 * For checksum calculation the user must zero the checksum field in the
 * UDP header. For checksum verification 0 will be returned if the checksum
 * value in the UDP header is correct.
 *
 * RFC768 instructs :
 * "If the computed  checksum  is zero,  it is transmitted  as all ones (the
 * equivalent  in one's complement  arithmetic)."
 * The return value from this function will NOT be adjusted to 0xFFFF in case
 * of a 0 result. It is the caller's responsibility to do so if the function
 * was called for checksum calculation (rather than just verification).
 */
__intrinsic uint16_t net_csum_ipv4_udp(void *ip, void *udp,
                                       __mem40 void *pkt_ctm,
                                       uint32_t ctm_len,
                                       __mem40 void *pkt_mem,
                                       uint32_t mem_len);

/**
 * Calculate the checksum of an IPv4 TCP packet.
 * @param ip        Pointer to the IPv4 header
 * @param tcp       Pointer to the TCP header
 * @param pkt_ctm   Address of the start of the L4 header in CTM
 * @param ctm_len   The length of the L4 header (including optional TCP
 *                  options) + payload in CTM (can be 0).
 *                  Must be at least 20 if there is a CTM buffer.
 * @param pkt_mem   Memory Unit Address of the remainder of the packet
 *                  contents not in CTM. May include the TCP header if there
 *                  is no CTM buffer.
 * @param mem_len   The length of the payload (and optionally L4 header if no
 *                  CTM) in external memory (can be 0).
 *                  Must be at least 20 if there is no CTM buffer.
 * @return The calculated checksum
 *
 * @ip (struct ip4_hdr) and @tcp (struct tcp_hdr) must be located in
 * LMEM or GPRs.
 *
 * For checksum calculation the user must zero the checksum field in the
 * TCP header. For checksum verification 0 will be returned if the checksum
 * value in the TCP header is correct.
 *
 * A note about TCP options:
 * Since the TCP header structure does not contain the TCP options, if
 * TCP options are present (this is checked internally) the @ctm_len/@mem_len
 * and @ctm_ptr/@mem_ptr will be adjusted properly to account for the options
 * fields.
 */
__intrinsic uint16_t net_csum_ipv4_tcp(void *ip, void *tcp,
                                       __mem40 void *pkt_ctm,
                                       uint32_t ctm_len,
                                       __mem40 void *pkt_mem,
                                       uint32_t mem_len);

/**
 * Calculate the checksum of an IPv6 UDP packet.
 * @param ip        Pointer to the IPv6 header
 * @param udp       Pointer to the UDP header
 * @param pkt_ctm   Address of the start of the L4 header in CTM
 * @param ctm_len   The length of the L4 header + payload in CTM (can be 0).
 *                  Must be at least 8 if there is a CTM buffer.
 * @param pkt_mem   Memory Unit Address of the remainder of the packet
 *                  contents not in CTM. May include the UDP header if there
 *                  is no CTM buffer.
 * @param mem_len   The length of the payload (and optionally L4 header if no
 *                  CTM) in external memory (can be 0).
 *                  Must be at least 8 if there is no CTM buffer.
 * @return The calculated checksum
 *
 * @ip (struct ip6_hdr) and @udp (struct udp_hdr) must be located in
 * LMEM or GPRs.
 *
 * For checksum calculation the user must zero the checksum field in the
 * UDP header. For checksum verification 0 will be returned if the checksum
 * value in the UDP header is correct.
 *
 * RFC768 instructs :
 * "If the computed  checksum  is zero,  it is transmitted  as all ones (the
 * equivalent  in one's complement  arithmetic)."
 * The return value from this function will NOT be adjusted to 0xFFFF in case
 * of a 0 result. It is the caller's responsibility to do so if the function
 * was called for checksum calculation (rather than just verification).
 */
__intrinsic uint16_t net_csum_ipv6_udp(void *ip, void *udp,
                                       __mem40 void *pkt_ctm,
                                       uint32_t ctm_len,
                                       __mem40 void *pkt_mem,
                                       uint32_t mem_len);

/**
 * Calculate the checksum of an IPv6 TCP packet.
 * @param ip        Pointer to the IPv6 header
 * @param tcp       Pointer to the TCP header
 * @param pkt_ctm   Address of the start of the L4 header in CTM
 * @param ctm_len   The length of the L4 header (including optional TCP
 *                  options) + payload in CTM (can be 0).
 *                  Must be at least 20 if there is a CTM buffer.
 * @param pkt_mem   Memory Unit Address of the remainder of the packet
 *                  contents not in CTM. May include the TCP header if there
 *                  is no CTM buffer.
 * @param mem_len   The length of the payload (and optionally L4 header if no
 *                  CTM) in external memory (can be 0).
 *                  Must be at least 20 if there is no CTM buffer.
 * @return The calculated checksum
 *
 * @ip (struct ip6_hdr) and @tcp (struct tcp_hdr) must be located in
 * LMEM or GPRs.
 *
 * For checksum calculation the user must zero the checksum field in the
 * TCP header. For checksum verification 0 will be returned if the checksum
 * value in the TCP header is correct.
 *
 * A note about TCP options:
 * Since the TCP header structure does not contain the TCP options, if
 * TCP options are present (this is checked internally) the @ctm_len/@mem_len
 * and @ctm_ptr/@mem_ptr will be adjusted properly to account for the options
 * fields.
 */
__intrinsic uint16_t net_csum_ipv6_tcp(void *ip, void *tcp,
                                       __mem40 void *pkt_ctm,
                                       uint32_t ctm_len,
                                       __mem40 void *pkt_mem,
                                       uint32_t mem_len);

#endif /* _PKT_CSUM_H_ */
