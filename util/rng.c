/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include "rng.h"

static const uint64_t a = 0x5deece66dULL;
static const uint64_t c = 0xb;
static const uint64_t m = 1ULL << 48;

void utils_rng_init(struct utils_rng *rng, uint64_t seed)
{
    rng->seed = (seed ^ a) % m;
}

uint32_t utils_rng_gen32(struct utils_rng *rng)
{
    uint64_t next;
    next = (a * rng->seed + c) % m;
    rng->seed = next;
    return next >> 16;
}

double utils_rng_gend(struct utils_rng *rng)
{
    // This is what Java seems to do
    uint64_t x =
            (((uint64_t) utils_rng_gen32(rng) >> 6) << 27) +
            (utils_rng_gen32(rng) >> 5);
    return x / ((double) (1ULL << 53));
}

void utils_rng_gen(struct utils_rng *rng, void *buf, size_t size)
{
    uint32_t x;
    while (size >= 4) {
        * ((uint32_t *) buf) = utils_rng_gen32(rng);
        buf = (void*) ((uintptr_t) buf + 4);
        size -= 4;
    }

    x = utils_rng_gen32(rng);
    while (size > 0) {
        * ((uint8_t *) buf) = x >> 24;
        x <<= 8;
        buf = (void*) ((uintptr_t) buf + 1);
        size--;
    }

}

