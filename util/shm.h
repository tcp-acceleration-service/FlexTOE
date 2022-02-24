/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef UTILS_SHM_H_
#define UTILS_SHM_H_

#include <stdlib.h>
#include <stdint.h>

#define CACHE_LINE_SIZE         64
#define PAGE_SIZE               (1 << 12)
#define HUGE_PAGE_SIZE          (1 << 30)

#define BAD_IOVA        -1

void* util_create_shm(const char *name, size_t size, void *addr);
void* util_create_shm_huge(const char *name, size_t size, void *addr);
void* util_map_region(const char *name, size_t size);
void* util_map_region_huge(const char *name, size_t size);
void util_destroy_shm(const char *name, size_t size, void *addr);
void util_destroy_shm_huge(const char *name, size_t size, void *addr);
void util_unmap_region(void* addr, size_t size);
uint64_t util_virt2phy(const void* virtaddr);

#endif /* UTILS_SHM_H_ */
