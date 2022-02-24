/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef UTILS_RNG_H_
#define UTILS_RNG_H_

#include <stddef.h>
#include <stdint.h>

struct utils_rng {
    uint64_t seed;
};

void utils_rng_init(struct utils_rng *rng, uint64_t seed);
uint32_t utils_rng_gen32(struct utils_rng *rng);
/** Double uniform between 0 and 1 */
double utils_rng_gend(struct utils_rng *rng);
void utils_rng_gen(struct utils_rng *rng, void *buf, size_t size);

#endif // ndef UTILS_RNG_H_
