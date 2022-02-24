/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_BYTEORDER_X86_H_
#define _RTE_BYTEORDER_X86_H_

#include <stdint.h>
#include <rte/common.h>
#include <endian.h>

/*
 * Compile-time endianness detection
 */
#define RTE_BIG_ENDIAN    1
#define RTE_LITTLE_ENDIAN 2
#if defined __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define RTE_BYTE_ORDER RTE_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif /* __BYTE_ORDER__ */
#elif defined __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
#define RTE_BYTE_ORDER RTE_BIG_ENDIAN
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif /* __BYTE_ORDER */
#elif defined __BIG_ENDIAN__
#define RTE_BYTE_ORDER RTE_BIG_ENDIAN
#elif defined __LITTLE_ENDIAN__
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif
#if !defined(RTE_BYTE_ORDER)
#error Unknown endianness.
#endif

#define RTE_STATIC_BSWAP16(v) \
	((((uint16_t)(v) & UINT16_C(0x00ff)) << 8) | \
	 (((uint16_t)(v) & UINT16_C(0xff00)) >> 8))

#define RTE_STATIC_BSWAP32(v) \
	((((uint32_t)(v) & UINT32_C(0x000000ff)) << 24) | \
	 (((uint32_t)(v) & UINT32_C(0x0000ff00)) <<  8) | \
	 (((uint32_t)(v) & UINT32_C(0x00ff0000)) >>  8) | \
	 (((uint32_t)(v) & UINT32_C(0xff000000)) >> 24))

#define RTE_STATIC_BSWAP64(v) \
	((((uint64_t)(v) & UINT64_C(0x00000000000000ff)) << 56) | \
	 (((uint64_t)(v) & UINT64_C(0x000000000000ff00)) << 40) | \
	 (((uint64_t)(v) & UINT64_C(0x0000000000ff0000)) << 24) | \
	 (((uint64_t)(v) & UINT64_C(0x00000000ff000000)) <<  8) | \
	 (((uint64_t)(v) & UINT64_C(0x000000ff00000000)) >>  8) | \
	 (((uint64_t)(v) & UINT64_C(0x0000ff0000000000)) >> 24) | \
	 (((uint64_t)(v) & UINT64_C(0x00ff000000000000)) >> 40) | \
	 (((uint64_t)(v) & UINT64_C(0xff00000000000000)) >> 56))

/*
 * These macros are functionally similar to rte_cpu_to_(be|le)(16|32|64)(),
 * they take values in host CPU order and return them converted to the
 * intended endianness.
 *
 * They resolve at compilation time to integer constants which can safely be
 * used with static initializers, since those cannot involve function calls.
 *
 * On the other hand, they are not as optimized as their rte_cpu_to_*()
 * counterparts, therefore applications should refrain from using them on
 * variable values, particularly inside performance-sensitive code.
 */
#if RTE_BYTE_ORDER == RTE_BIG_ENDIAN
#define RTE_BE16(v) (rte_be16_t)(v)
#define RTE_BE32(v) (rte_be32_t)(v)
#define RTE_BE64(v) (rte_be64_t)(v)
#define RTE_LE16(v) (rte_le16_t)(RTE_STATIC_BSWAP16(v))
#define RTE_LE32(v) (rte_le32_t)(RTE_STATIC_BSWAP32(v))
#define RTE_LE64(v) (rte_le64_t)(RTE_STATIC_BSWAP64(v))
#elif RTE_BYTE_ORDER == RTE_LITTLE_ENDIAN
#define RTE_BE16(v) (rte_be16_t)(RTE_STATIC_BSWAP16(v))
#define RTE_BE32(v) (rte_be32_t)(RTE_STATIC_BSWAP32(v))
#define RTE_BE64(v) (rte_be64_t)(RTE_STATIC_BSWAP64(v))
#define RTE_LE16(v) (rte_be16_t)(v)
#define RTE_LE32(v) (rte_be32_t)(v)
#define RTE_LE64(v) (rte_be64_t)(v)
#else
#error Unsupported endianness.
#endif

/*
 * The following types should be used when handling values according to a
 * specific byte ordering, which may differ from that of the host CPU.
 *
 * Libraries, public APIs and applications are encouraged to use them for
 * documentation purposes.
 */
typedef uint16_t rte_be16_t; /**< 16-bit big-endian value. */
typedef uint32_t rte_be32_t; /**< 32-bit big-endian value. */
typedef uint64_t rte_be64_t; /**< 64-bit big-endian value. */
typedef uint16_t rte_le16_t; /**< 16-bit little-endian value. */
typedef uint32_t rte_le32_t; /**< 32-bit little-endian value. */
typedef uint64_t rte_le64_t; /**< 64-bit little-endian value. */

/*
 * An internal function to swap bytes in a 16-bit value.
 *
 * It is used by rte_bswap16() when the value is constant. Do not use
 * this function directly; rte_bswap16() is preferred.
 */
static inline uint16_t
rte_constant_bswap16(uint16_t x)
{
	return (uint16_t)RTE_STATIC_BSWAP16(x);
}

/*
 * An internal function to swap bytes in a 32-bit value.
 *
 * It is used by rte_bswap32() when the value is constant. Do not use
 * this function directly; rte_bswap32() is preferred.
 */
static inline uint32_t
rte_constant_bswap32(uint32_t x)
{
	return (uint32_t)RTE_STATIC_BSWAP32(x);
}

/*
 * An internal function to swap bytes of a 64-bit value.
 *
 * It is used by rte_bswap64() when the value is constant. Do not use
 * this function directly; rte_bswap64() is preferred.
 */
static inline uint64_t
rte_constant_bswap64(uint64_t x)
{
	return (uint64_t)RTE_STATIC_BSWAP64(x);
}

#ifndef RTE_BYTE_ORDER
#define RTE_BYTE_ORDER RTE_LITTLE_ENDIAN
#endif

/*
 * An architecture-optimized byte swap for a 16-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap16().
 */
static inline uint16_t rte_arch_bswap16(uint16_t _x)
{
	uint16_t x = _x;
	asm volatile ("xchgb %b[x1],%h[x2]"
		      : [x1] "=Q" (x)
		      : [x2] "0" (x)
		      );
	return x;
}

/*
 * An architecture-optimized byte swap for a 32-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap32().
 */
static inline uint32_t rte_arch_bswap32(uint32_t _x)
{
	uint32_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}

#ifndef RTE_FORCE_INTRINSICS
#define rte_bswap16(x) ((uint16_t)(__builtin_constant_p(x) ?		\
				   rte_constant_bswap16(x) :		\
				   rte_arch_bswap16(x)))

#define rte_bswap32(x) ((uint32_t)(__builtin_constant_p(x) ?		\
				   rte_constant_bswap32(x) :		\
				   rte_arch_bswap32(x)))

#define rte_bswap64(x) ((uint64_t)(__builtin_constant_p(x) ?		\
				   rte_constant_bswap64(x) :		\
				   rte_arch_bswap64(x)))
#else
/*
 * __builtin_bswap16 is only available gcc 4.8 and upwards
 */
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
#define rte_bswap16(x) ((uint16_t)(__builtin_constant_p(x) ?		\
				   rte_constant_bswap16(x) :		\
				   rte_arch_bswap16(x)))
#endif
#endif

#define rte_cpu_to_le_16(x) (x)
#define rte_cpu_to_le_32(x) (x)
#define rte_cpu_to_le_64(x) (x)

#define rte_cpu_to_be_16(x) rte_bswap16(x)
#define rte_cpu_to_be_32(x) rte_bswap32(x)
#define rte_cpu_to_be_64(x) rte_bswap64(x)

#define rte_le_to_cpu_16(x) (x)
#define rte_le_to_cpu_32(x) (x)
#define rte_le_to_cpu_64(x) (x)

#define rte_be_to_cpu_16(x) rte_bswap16(x)
#define rte_be_to_cpu_32(x) rte_bswap32(x)
#define rte_be_to_cpu_64(x) rte_bswap64(x)

/*
 * An architecture-optimized byte swap for a 64-bit value.
 *
  * Do not use this function directly. The preferred function is rte_bswap64().
 */
/* 64-bit mode */
static inline uint64_t rte_arch_bswap64(uint64_t _x)
{
	uint64_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}

#endif /* _RTE_BYTEORDER_X86_H_ */
