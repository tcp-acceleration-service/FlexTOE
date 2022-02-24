/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 1982, 1986, 1990, 1993
 *      The Regents of the University of California.
 * Copyright(c) 2010-2014 Intel Corporation.
 * Copyright(c) 2014 6WIND S.A.
 * All rights reserved.
 */

#ifndef _RTE_IP_H_
#define _RTE_IP_H_

#include <stdint.h>
#include <stddef.h>

#include <rte/byteorder.h>

/* NOTE: Ignore this */
#define PKT_TX_TCP_SEG       (1ULL << 50)

/**
 * IPv4 Header
 */
struct rte_ipv4_hdr {
	uint8_t  version_ihl;		/**< version and header length */
	uint8_t  type_of_service;	/**< type of service */
	uint16_t total_length;		/**< length of packet */
	uint16_t packet_id;		/**< packet ID */
	uint16_t fragment_offset;	/**< fragmentation offset */
	uint8_t  time_to_live;		/**< time to live */
	uint8_t  next_proto_id;		/**< protocol ID */
	uint16_t hdr_checksum;		/**< header checksum */
	uint32_t src_addr;		/**< source address */
	uint32_t dst_addr;		/**< destination address */
} __attribute__((__packed__));

/**
 * @internal Calculate a sum of all words in the buffer.
 * Helper routine for the rte_raw_cksum().
 *
 * @param buf
 *   Pointer to the buffer.
 * @param len
 *   Length of the buffer.
 * @param sum
 *   Initial value of the sum.
 * @return
 *   sum += Sum of all words in the buffer.
 */
static inline uint32_t
__rte_raw_cksum(const void *buf, size_t len, uint32_t sum)
{
	/* workaround gcc strict-aliasing warning */
	uintptr_t ptr = (uintptr_t)buf;
	typedef uint16_t __attribute__((__may_alias__)) u16_p;
	const u16_p *u16_buf = (const u16_p *)ptr;

	while (len >= (sizeof(*u16_buf) * 4)) {
		sum += u16_buf[0];
		sum += u16_buf[1];
		sum += u16_buf[2];
		sum += u16_buf[3];
		len -= sizeof(*u16_buf) * 4;
		u16_buf += 4;
	}
	while (len >= sizeof(*u16_buf)) {
		sum += *u16_buf;
		len -= sizeof(*u16_buf);
		u16_buf += 1;
	}

	/* if length is in odd bytes */
	if (len == 1)
		sum += *((const uint8_t *)u16_buf);

	return sum;
}

/**
 * @internal Reduce a sum to the non-complemented checksum.
 * Helper routine for the rte_raw_cksum().
 *
 * @param sum
 *   Value of the sum.
 * @return
 *   The non-complemented checksum.
 */
static inline uint16_t
__rte_raw_cksum_reduce(uint32_t sum)
{
	sum = ((sum & 0xffff0000) >> 16) + (sum & 0xffff);
	sum = ((sum & 0xffff0000) >> 16) + (sum & 0xffff);
	return (uint16_t)sum;
}

/**
 * Process the non-complemented checksum of a buffer.
 *
 * @param buf
 *   Pointer to the buffer.
 * @param len
 *   Length of the buffer.
 * @return
 *   The non-complemented checksum.
 */
static inline uint16_t
rte_raw_cksum(const void *buf, size_t len)
{
	uint32_t sum;

	sum = __rte_raw_cksum(buf, len, 0);
	return __rte_raw_cksum_reduce(sum);
}

/**
 * Process the IPv4 checksum of an IPv4 header.
 *
 * The checksum field must be set to 0 by the caller.
 *
 * @param ipv4_hdr
 *   The pointer to the contiguous IPv4 header.
 * @return
 *   The complemented checksum to set in the IP packet.
 */
static inline uint16_t
rte_ipv4_cksum(const struct rte_ipv4_hdr *ipv4_hdr)
{
	uint16_t cksum;
	cksum = rte_raw_cksum(ipv4_hdr, sizeof(struct rte_ipv4_hdr));
	return (cksum == 0xffff) ? cksum : (uint16_t)~cksum;
}

/**
 * Process the pseudo-header checksum of an IPv4 header.
 *
 * The checksum field must be set to 0 by the caller.
 *
 * Depending on the ol_flags, the pseudo-header checksum expected by the
 * drivers is not the same. For instance, when TSO is enabled, the IP
 * payload length must not be included in the packet.
 *
 * When ol_flags is 0, it computes the standard pseudo-header checksum.
 *
 * @param ipv4_hdr
 *   The pointer to the contiguous IPv4 header.
 * @param ol_flags
 *   The ol_flags of the associated mbuf.
 * @return
 *   The non-complemented checksum to set in the L4 header.
 */
static inline uint16_t
rte_ipv4_phdr_cksum(const struct rte_ipv4_hdr *ipv4_hdr, uint64_t ol_flags)
{
	struct ipv4_psd_header {
		uint32_t src_addr; /* IP address of source host. */
		uint32_t dst_addr; /* IP address of destination host. */
		uint8_t  zero;     /* zero. */
		uint8_t  proto;    /* L4 protocol type. */
		uint16_t len;      /* L4 length. */
	} psd_hdr;

	psd_hdr.src_addr = ipv4_hdr->src_addr;
	psd_hdr.dst_addr = ipv4_hdr->dst_addr;
	psd_hdr.zero = 0;
	psd_hdr.proto = ipv4_hdr->next_proto_id;
	if (ol_flags & PKT_TX_TCP_SEG) {
		psd_hdr.len = 0;
	} else {
		psd_hdr.len = rte_cpu_to_be_16(
			(uint16_t)(rte_be_to_cpu_16(ipv4_hdr->total_length)
				- sizeof(struct rte_ipv4_hdr)));
	}
	return rte_raw_cksum(&psd_hdr, sizeof(psd_hdr));
}

/**
 * Process the IPv4 UDP or TCP checksum.
 *
 * The IPv4 header should not contains options. The IP and layer 4
 * checksum must be set to 0 in the packet by the caller.
 *
 * @param ipv4_hdr
 *   The pointer to the contiguous IPv4 header.
 * @param l4_hdr
 *   The pointer to the beginning of the L4 header.
 * @return
 *   The complemented checksum to set in the IP packet
 *   or 0 on error
 */
static inline uint16_t
rte_ipv4_udptcp_cksum(const struct rte_ipv4_hdr *ipv4_hdr, const void *l4_hdr)
{
	uint32_t cksum;
	uint32_t l3_len, l4_len;

	l3_len = rte_be_to_cpu_16(ipv4_hdr->total_length);
	if (l3_len < sizeof(struct rte_ipv4_hdr))
		return 0;

	l4_len = l3_len - sizeof(struct rte_ipv4_hdr);

	cksum = rte_raw_cksum(l4_hdr, l4_len);
	cksum += rte_ipv4_phdr_cksum(ipv4_hdr, 0);

	cksum = ((cksum & 0xffff0000) >> 16) + (cksum & 0xffff);
	cksum = (~cksum) & 0xffff;
	if (cksum == 0)
		cksum = 0xffff;

	return (uint16_t)cksum;
}

#endif /* _RTE_IP_H_ */
