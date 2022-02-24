/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation.
 * Copyright(c) 2013 6WIND S.A.
 */

#ifndef _RTE_CYCLES_H_
#define _RTE_CYCLES_H_

/**
 * @file
 *
 * Simple Time Reference Functions.
 */

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <cpuid.h>

#include <rte/common.h>
#include <rte/atomic.h>

#define MS_PER_S 1000
#define US_PER_S 1000000
#define NS_PER_S 1000000000

__rte_unused
static inline uint64_t
rte_rdtsc(void)
{
	union {
		uint64_t tsc_64;
		RTE_STD_C11
		struct {
			uint32_t lo_32;
			uint32_t hi_32;
		};
	} tsc;

	asm volatile("rdtsc" :
		     "=a" (tsc.lo_32),
		     "=d" (tsc.hi_32));
	return tsc.tsc_64;
}

__rte_unused
static unsigned int
rte_cpu_get_model(uint32_t fam_mod_step)
{
	uint32_t family, model, ext_model;

	family = (fam_mod_step >> 8) & 0xf;
	model = (fam_mod_step >> 4) & 0xf;

	if (family == 6 || family == 15) {
		ext_model = (fam_mod_step >> 16) & 0xf;
		model += (ext_model << 4);
	}

	return model;
}

__rte_unused
static int32_t
rdmsr(int msr, uint64_t *val)
{
	int fd;
	int ret;

	fd = open("/dev/cpu/0/msr", O_RDONLY);
	if (fd < 0)
		return fd;

	ret = pread(fd, val, sizeof(uint64_t), msr);

	close(fd);

	return ret;
}

__rte_unused
static uint32_t
check_model_wsm_nhm(uint8_t model)
{
	switch (model) {
	/* Westmere */
	case 0x25:
	case 0x2C:
	case 0x2F:
	/* Nehalem */
	case 0x1E:
	case 0x1F:
	case 0x1A:
	case 0x2E:
		return 1;
	}

	return 0;
}

__rte_unused
static uint32_t
check_model_gdm_dnv(uint8_t model)
{
	switch (model) {
	/* Goldmont */
	case 0x5C:
	/* Denverton */
	case 0x5F:
		return 1;
	}

	return 0;
}

__rte_unused
static uint64_t
get_tsc_freq_arch(void)
{
	uint64_t tsc_hz = 0;
	uint32_t a, b, c, d, maxleaf;
	uint8_t mult, model;
	int32_t ret;

	/*
	 * Time Stamp Counter and Nominal Core Crystal Clock
	 * Information Leaf
	 */
	maxleaf = __get_cpuid_max(0, NULL);

	if (maxleaf >= 0x15) {
		__cpuid(0x15, a, b, c, d);

		/* EBX : TSC/Crystal ratio, ECX : Crystal Hz */
		if (b && c)
			return c * (b / a);
	}

	__cpuid(0x1, a, b, c, d);
	model = rte_cpu_get_model(a);

	if (check_model_wsm_nhm(model))
		mult = 133;
	else if ((c & bit_AVX) || check_model_gdm_dnv(model))
		mult = 100;
	else
		return 0;

	ret = rdmsr(0xCE, &tsc_hz);
	if (ret < 0)
		return 0;

	return ((tsc_hz >> 8) & 0xff) * mult * 1E6;
}

/**
 * Get the measured frequency of the RDTSC counter
 *
 * @return
 *   The TSC frequency for this lcore
 */
__rte_unused
static uint64_t
rte_get_tsc_hz(void) { return get_tsc_freq_arch(); }

/**
 * Return the number of TSC cycles since boot
 *
  * @return
 *   the number of cycles
 */
__rte_unused
static inline uint64_t
rte_get_tsc_cycles(void) { return rte_rdtsc(); }

#endif /* _RTE_CYCLES_H_ */
