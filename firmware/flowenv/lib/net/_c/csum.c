/*
 * Copyright 2015-2018 Netronome, Inc.
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
 * @file          lib/net/_c/csum.c
 * @brief         Full checksum calculation functions
 */

#include <assert.h>
#include <nfp.h>
#include <stdint.h>

#include <std/reg_utils.h>
#include <net/ip.h>
#include <net/csum.h>
#include <nfp/mem_bulk.h>

#define UINT32_REG(_a) ((__is_in_lmem(_a)) ? ((__lmem uint32_t *)_a)        \
                                           : ((__gpr  uint32_t *)_a))

__intrinsic uint32_t
ones_sum_add(uint32_t sum1, uint32_t sum2)
{
    __gpr uint32_t ret;
    __asm alu[ret , sum1, +, sum2];
    __asm alu[ret, ret, +carry, 0];
    return ret;
}

__intrinsic uint16_t
ones_sum_fold16(uint32_t sum)
{
    uint32_t ret;
    ret = (sum >> 16) + (uint16_t)(sum);
    ret = (ret >> 16) + (uint16_t)(ret);
    return ret;
}

/* computes the pseudo header checksum */
__intrinsic uint32_t
ones_sum_pseudo(uint32_t ip_type, uint32_t protocol, void *ip, void *l4_hdr,
                uint32_t l4_len)
{
    __gpr uint32_t sum = 0;
    __gpr uint32_t tmp;

    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_in_reg_or_lmem(l4_hdr));

    /* Shared part of pseudo header for UDP and TCP */
    sum = ones_sum_add(sum, UINT32_REG(ip)[3]);
    sum = ones_sum_add(sum, UINT32_REG(ip)[4]);

    if (ip_type == NET_ETH_TYPE_IPV6) {
        sum = ones_sum_add(sum, UINT32_REG(ip)[2]);
        sum = ones_sum_add(sum, UINT32_REG(ip)[5]);
        sum = ones_sum_add(sum, UINT32_REG(ip)[6]);
        sum = ones_sum_add(sum, UINT32_REG(ip)[7]);
        sum = ones_sum_add(sum, UINT32_REG(ip)[8]);
        sum = ones_sum_add(sum, UINT32_REG(ip)[9]);
    }

    sum = ones_sum_add(sum, UINT32_REG(l4_hdr)[0]);
    sum = ones_sum_add(sum, UINT32_REG(l4_hdr)[1]);

    if (ip_type == NET_ETH_TYPE_IPV6) {
        sum = ones_sum_add(sum, protocol);
    } else {
        tmp = protocol << 16;
    }

    if (protocol == NET_IP_PROTO_TCP) {
        sum = ones_sum_add(sum, UINT32_REG(l4_hdr)[2]);
        sum = ones_sum_add(sum, UINT32_REG(l4_hdr)[3]);
        sum = ones_sum_add(sum, UINT32_REG(l4_hdr)[4]);
    }

    if (ip_type == NET_ETH_TYPE_IPV6) {
        sum = ones_sum_add(sum, l4_len);
    } else {
        tmp |= l4_len;
        sum = ones_sum_add(sum, tmp);
    }

    return sum;
}

/* computes checksum over a word array: len must be <= 64 */
__intrinsic uint32_t
ones_sum_warr(__xread uint32_t *buf, uint32_t len)
{
    __gpr uint32_t sum = 0;
    __gpr uint32_t t_val;
    __gpr uint32_t val;
    __gpr uint32_t rem;
    __gpr uint32_t nwords;
    __gpr uint32_t nskip;
    t_val = ((__ctx() << 5) | __xfer_reg_number(buf)) << 2;

    __asm local_csr_wr[t_index, t_val]

    if (__is_ct_const(len)) {
        __asm __attribute(LITERAL_ASM) { {nop} {nop} {nop} }
    }

    __asm __attribute(ASM_HAS_JUMP)
    {
        alu[nwords, --, B, len, >>2]
        alu[nskip, 16, -, nwords]
        jump[nskip, sum16w], targets[sum16w, sum15w, sum14w, sum13w,\
                                     sum12w, sum11w, sum10w, sum09w,\
                                     sum08w, sum07w, sum06w, sum05w,\
                                     sum04w, sum03w, sum02w, sum01w,\
                                     sum00w] , defer[1]
            alu[--, --, B, 0]

        sum16w:  alu[sum, sum, +, *$index++]
        sum15w:  alu[sum, sum, +carry, *$index++]
        sum14w:  alu[sum, sum, +carry, *$index++]
        sum13w:  alu[sum, sum, +carry, *$index++]
        sum12w:  alu[sum, sum, +carry, *$index++]
        sum11w:  alu[sum, sum, +carry, *$index++]
        sum10w:  alu[sum, sum, +carry, *$index++]
        sum09w:  alu[sum, sum, +carry, *$index++]
        sum08w:  alu[sum, sum, +carry, *$index++]
        sum07w:  alu[sum, sum, +carry, *$index++]
        sum06w:  alu[sum, sum, +carry, *$index++]
        sum05w:  alu[sum, sum, +carry, *$index++]
        sum04w:  alu[sum, sum, +carry, *$index++]
        sum03w:  alu[sum, sum, +carry, *$index++]
        sum02w:  alu[sum, sum, +carry, *$index++]
        sum01w:  alu[sum, sum, +carry, *$index++]
        sum00w:  alu[sum, sum, +carry, 0]
    }
    rem = len & 3;
    if (rem > 0) {
        __asm alu[val, --, B, *$index]
        if (rem == 1) {
            val = val & 0xFF000000;
        } else if (rem == 2) {
            val = val & 0xFFFF0000;
        } else if (rem == 3) {
            val = val & 0xFFFFFF00;
        }
        sum = ones_sum_add(sum, val);
    }
    return sum;
}

/* computes the checksum over a memory region */
/* len can be arbitrary */
__intrinsic uint32_t
ones_sum_mem(__mem40 void *mem, int32_t len)
{
    __xread uint32_t pkt_cache[16];
    __gpr int curr_len;
    __mem40 void* pkt_ptr;
    SIGNAL read_sig;
    __gpr uint32_t sum = 0;

    if (len > 0) {
        pkt_ptr = mem;
        while (len > 0) {
            if (len > sizeof(pkt_cache))
                curr_len = sizeof(pkt_cache);
            else
                curr_len = len;
            /* The read size must be a mult of 8 bytes */
            __mem_read64(pkt_cache, pkt_ptr, ((curr_len + 7) & 0x78),
                         sizeof(pkt_cache), ctx_swap, &read_sig);
            sum = ones_sum_add(sum, ones_sum_warr(pkt_cache, curr_len));
            __implicit_read(pkt_cache);
            ((__mem40 uint8_t*)pkt_ptr) += curr_len;
            len -= curr_len;
        }
    }
    return sum;
}

__intrinsic uint16_t
net_csum_mod(uint32_t orig_csum, uint32_t orig_val, uint32_t new_val)
{
    __gpr uint32_t new_csum;

    new_csum = (~orig_csum & 0xFFFF) + (~orig_val & 0xFFFF) + new_val;

    return ~ones_sum_fold16(new_csum);
}

__intrinsic uint16_t
net_csum_ipv4(void *ip, __mem40 void *pkt_ptr)
{
    return __net_csum_ipv4(ip, pkt_ptr, 1);
}

__intrinsic uint16_t
__net_csum_ipv4(void *ip, __mem40 void *pkt_ptr,  const uint32_t test_ip_opt)
{
    __gpr uint32_t sum = 0;
    __gpr uint32_t opt_size;
    __gpr uint32_t hl;
    __xread uint32_t ip_opts[10];
    SIGNAL read_sig;

    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_ct_const(test_ip_opt));

    /* Sum up the standard IP header */
    sum = ones_sum_add(sum, UINT32_REG(ip)[0]);
    sum = ones_sum_add(sum, UINT32_REG(ip)[1]);
    sum = ones_sum_add(sum, UINT32_REG(ip)[2]);
    sum = ones_sum_add(sum, UINT32_REG(ip)[3]);
    sum = ones_sum_add(sum, UINT32_REG(ip)[4]);

    /* Handle IP Options if exist */
    if (__is_in_lmem(ip)) {
        hl = ((__lmem struct ip4_hdr *)ip)->hl;
    } else {
        hl = ((__gpr struct ip4_hdr *)ip)->hl;
    }

    if (test_ip_opt) {
        if (hl > NET_IP4_LEN32) {
            opt_size = (hl - NET_IP4_LEN32) * sizeof(uint32_t);
            ((__mem40 uint8_t*)pkt_ptr) -= opt_size;
            /* The read size must be a mult of 8 bytes */
            __mem_read64(ip_opts, pkt_ptr, ((opt_size + 7) & 0x78),
                         sizeof(ip_opts), ctx_swap, &read_sig);
            sum = ones_sum_add(sum, ones_sum_warr(ip_opts, opt_size));
            __implicit_read(ip_opts);
        }
    }

    return ~ones_sum_fold16(sum);
}

__intrinsic uint16_t
net_csum_l4_ip(uint32_t ip_type, uint32_t protocol,
               void *ip, void *l4_hdr,
               __mem40 void* pkt_ctm, uint32_t ctm_len,
               __mem40 void* pkt_mem, uint32_t mem_len)
{
    __gpr uint32_t sum = 0;
    __gpr uint32_t tmp;
    __gpr uint32_t l4_len;

    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_in_reg_or_lmem(l4_hdr));

    if (protocol == NET_IP_PROTO_UDP) {
        if (__is_in_lmem(l4_hdr)) {
            l4_len = ((__lmem struct udp_hdr*)l4_hdr)->len;
        } else {
            l4_len = ((__gpr struct udp_hdr*)l4_hdr)->len;
        }
        /* Skip UDP header */
        if (ctm_len > 0) {
            ctm_len -= sizeof(struct udp_hdr);
            ((__mem40 uint8_t*)pkt_ctm) += sizeof(struct udp_hdr);
        } else {
            mem_len -= sizeof(struct udp_hdr);
            ((__mem40 uint8_t*)pkt_mem) += sizeof(struct udp_hdr);
        }
    } else {
        l4_len = ctm_len + mem_len;

        /* Skip basic TCP header */
        if (ctm_len > 0) {
            ctm_len -= sizeof(struct tcp_hdr);
            ((__mem40 uint8_t*)pkt_ctm) += sizeof(struct tcp_hdr);
        } else {
            mem_len -= sizeof(struct tcp_hdr);
            ((__mem40 uint8_t*)pkt_mem) += sizeof(struct tcp_hdr);
        }
    }

    sum = ones_sum_pseudo(ip_type, protocol, ip, l4_hdr, l4_len);

    if (ctm_len + mem_len  > 0) {
        sum = ones_sum_add(sum, ones_sum_mem(pkt_ctm, ctm_len));
        sum = ones_sum_add(sum, ones_sum_mem(pkt_mem, mem_len));
    }

    return ~ones_sum_fold16(sum);
}

__intrinsic uint16_t
net_csum_ipv4_udp(void *ip, void* udp,
                  __mem40 void* pkt_ctm, uint32_t ctm_len,
                  __mem40 void* pkt_mem, uint32_t mem_len)
{
    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_in_reg_or_lmem(udp));

    return net_csum_l4_ip(NET_ETH_TYPE_IPV4, NET_IP_PROTO_UDP, ip, udp,
                          pkt_ctm, ctm_len, pkt_mem, mem_len);
}

__intrinsic uint16_t
net_csum_ipv4_tcp(void *ip, void* tcp,
                  __mem40 void* pkt_ctm, uint32_t ctm_len,
                  __mem40 void* pkt_mem, uint32_t mem_len)
{
    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_in_reg_or_lmem(tcp));

    return net_csum_l4_ip(NET_ETH_TYPE_IPV4, NET_IP_PROTO_TCP, ip, tcp,
                          pkt_ctm, ctm_len, pkt_mem, mem_len);
}

__intrinsic uint16_t
net_csum_ipv6_udp(void *ip, void *udp,
                  __mem40 void *pkt_ctm, uint32_t ctm_len,
                  __mem40 void *pkt_mem, uint32_t mem_len)
{
    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_in_reg_or_lmem(udp));

    return net_csum_l4_ip(NET_ETH_TYPE_IPV6, NET_IP_PROTO_UDP, ip, udp,
                          pkt_ctm, ctm_len, pkt_mem, mem_len);
}

__intrinsic uint16_t
net_csum_ipv6_tcp(void *ip, void *tcp,
                  __mem40 void *pkt_ctm, uint32_t ctm_len,
                  __mem40 void *pkt_mem, uint32_t mem_len)
{
    ctassert(__is_in_reg_or_lmem(ip));
    ctassert(__is_in_reg_or_lmem(tcp));

    return net_csum_l4_ip(NET_ETH_TYPE_IPV6, NET_IP_PROTO_TCP, ip, tcp,
                          pkt_ctm, ctm_len, pkt_mem, mem_len);
}
