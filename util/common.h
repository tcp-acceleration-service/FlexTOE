/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <arpa/inet.h>

#include <rte/io.h>

#define MIN(a,b) ((b) < (a) ? (b) : (a))
#define MAX(a,b) ((b) > (a) ? (b) : (a))
#define MEM_BARRIER() __asm__ volatile("" ::: "memory")
#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
#define LIKELY(x) __builtin_expect((x),1)
#define UNLIKELY(x) __builtin_expect((x),0)

int util_parse_ipv4(const char *s, uint32_t *ip);
int util_parse_mac(const char *s, uint64_t *mac);

/* types for big endian integers to catch those errors with types */
struct beui16 { uint16_t x; } __attribute__((packed));
struct beui32 { uint32_t x; } __attribute__((packed));
struct beui64 { uint64_t x; } __attribute__((packed));
typedef struct beui16 beui16_t;
typedef struct beui32 beui32_t;
typedef struct beui64 beui64_t;

static inline uint16_t f_beui16(beui16_t x) { return __builtin_bswap16(x.x); }
static inline uint32_t f_beui32(beui32_t x) { return __builtin_bswap32(x.x); }
static inline uint64_t f_beui64(beui64_t x) { return __builtin_bswap64(x.x); }

static inline beui16_t t_beui16(uint16_t x)
{
  beui16_t b;
  b.x = __builtin_bswap16(x);
  return b;
}

static inline beui32_t t_beui32(uint32_t x)
{
  beui32_t b;
  b.x = __builtin_bswap32(x);
  return b;
}

static inline beui64_t t_beui64(uint64_t x)
{
  beui64_t b;
  b.x = __builtin_bswap64(x);
  return b;
}

static inline uint64_t util_rdtsc(void)
{
  uint32_t eax, edx;
  asm volatile ("rdtsc" : "=a" (eax), "=d" (edx));
  return ((uint64_t) edx << 32) | eax;
}

static inline uint32_t util_killcycles(uint32_t cyc, uint32_t opaque)
{
  uint64_t start = util_rdtsc();
  uint64_t end = start + cyc;

  if (end >= start) {
    while (util_rdtsc() < end) {
      opaque = opaque * 42 + 37;
      opaque ^= 0x12345678;
      opaque = opaque * 42 + 37;
      opaque ^= 0x87654321;
    }
  } else {
    while (util_rdtsc() >= start || util_rdtsc() < end) {
      opaque = opaque * 42 + 37;
      opaque ^= 0x12345678;
      opaque = opaque * 42 + 37;
      opaque ^= 0x87654321;
    }
  }
  return opaque;
}

static inline void util_prefetch0(const volatile void *p)
{
  asm volatile ("prefetcht0 %[p]" : : [p] "m" (*(const volatile char *)p));
}

static inline uint8_t nn_readb(volatile const void *addr)
{
	return rte_read8(addr);
}

static inline void nn_writeb(uint8_t val, volatile void *addr)
{
	rte_write8(val, addr);
}

static inline uint16_t nn_readw(volatile const void *addr)
{
  return rte_read16(addr);
}

static inline void nn_writew(uint16_t val, volatile void *addr)
{
	rte_write16(val, addr);
}

static inline uint32_t nn_readl(volatile const void *addr)
{
	return rte_read32(addr);
}

static inline void nn_writel(uint32_t val, volatile void *addr)
{
	rte_write32(val, addr);
}

static inline uint64_t nn_readq(volatile void *addr)
{
	const volatile uint32_t *p = (uint32_t *)addr;
	uint32_t low, high;

	high = nn_readl((volatile const void *)p);
	low = nn_readl((volatile const void *)(p + 1));
#if 0
	high = nn_readl((volatile const void *)(p + 1));
	low = nn_readl((volatile const void *)p);
#endif

	return low + ((uint64_t)high << 32);
}

static inline void nn_writeq(uint64_t val, volatile void *addr)
{
	nn_writel(val >> 32, addr);
	nn_writel(val, (volatile char *)addr + 4);
#if 0
  nn_writel(val >> 32, (volatile char *)addr + 4);
  nn_writel(val, addr);
#endif
}

#endif /* UTILS_H_ */
