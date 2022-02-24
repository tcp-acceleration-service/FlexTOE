/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/limits.h>

#include "connect.h"
#include "shm.h"

#define PFN_MASK_SIZE           8

static void destroy_shm(const char* path, size_t size, void* addr)
{
  if (munmap(addr, size) != 0) {
    fprintf(stderr, "Warning: munmap failed (%s)\n", strerror(errno));
  }
  shm_unlink(path);
}

void util_destroy_shm(const char *name, size_t size, void *addr)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", FLEXNIC_SHM_PREFIX, name);

  destroy_shm(path, size, addr);
}

void util_destroy_shm_huge(const char *name, size_t size, void *addr)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", FLEXNIC_HUGE_PREFIX, name);

  destroy_shm(path, size, addr);
}

static void* create_shm(const char* path, size_t size, void* addr)
{
  int fd;
  void *p;

  if ((fd = open(path, O_CREAT | O_RDWR, 0666)) == -1) {
    perror("open failed");
    goto error_out;
  }
  if (ftruncate(fd, size) != 0) {
    perror("ftruncate failed");
    goto error_remove;
  }

  if ((p = mmap(addr, size, PROT_READ | PROT_WRITE,
      MAP_SHARED | (addr == NULL ? 0 : MAP_FIXED) | MAP_POPULATE, fd, 0)) ==
      (void *) -1)
  {
    perror("mmap failed");
    goto error_remove;
  }

  memset(p, 0, size);

  close(fd);
  return p;

error_remove:
  close(fd);
  shm_unlink(path);
error_out:
  return NULL;
}

void* util_create_shm(const char *name, size_t size, void *addr)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", FLEXNIC_SHM_PREFIX, name);

  return create_shm(path, size, addr);
}


void* util_create_shm_huge(const char *name, size_t size, void *addr)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", FLEXNIC_HUGE_PREFIX, name);

  return create_shm(path, size, addr);
}

void* map_region(const char* path, size_t size)
{
  int fd;
  void *m;

  if ((fd = open(path, O_RDWR)) == -1) {
    perror("map_region: shm_open memory failed");
    return NULL;
  }
  m = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
  close(fd);
  if (m == (void *) -1) {
    perror("map_region: mmap failed");
    return NULL;
  }

  return m;
}

void* util_map_region(const char *name, size_t size)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", FLEXNIC_SHM_PREFIX, name);

  return map_region(path, size);
}

void* util_map_region_huge(const char *name, size_t size)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", FLEXNIC_HUGE_PREFIX, name);

  return map_region(path, size);
}

void util_unmap_region(void* addr, size_t size)
{
  munmap(addr, size);
}

/**
 * Convert virtual address to physical address.
 * Hacky solution used by DPDK!
 *
 * @note
 * Refer http://man7.org/linux/man-pages/man5/proc.5.html
 */
uint64_t
util_virt2phy(const void* virtaddr)
{
  int fd, retval;
  uint64_t page, paddr;
  unsigned long virt_pfn;
  off_t offset;

  fd = open("/proc/self/pagemap", O_RDONLY);
  if (fd < 0)
  {
    fprintf(stderr, "%s(): Cannot open %s: %s\n",
        __func__, "/proc/self/pagemap", strerror(errno));
    return BAD_IOVA;
  }

  virt_pfn = (unsigned long)virtaddr / PAGE_SIZE;
  offset = sizeof(uint64_t) * virt_pfn;   // Offset into pagemap file
  if (lseek(fd, offset, SEEK_SET) == (off_t) -1)
  {
    fprintf(stderr, "%s(): Cannot seek in %s: %s\n",
        __func__, "/proc/self/pagemap", strerror(errno));
    close(fd);
    return BAD_IOVA;
  }

  retval = read(fd, &page, PFN_MASK_SIZE);
  close(fd);
  if (retval < 0)
  {
    fprintf(stderr, "%s(): cannot read %s: %s\n",
            __func__, "/proc/self/pagemap", strerror(errno));
    return BAD_IOVA;
  }
  else if (retval != PFN_MASK_SIZE)
  {
    fprintf(stderr, "%s(): read %d bytes from %s"
            "but expected %d:\n",
            __func__, retval, "/proc/self/pagemap", PFN_MASK_SIZE);
    return BAD_IOVA;
  }

  /*
    * the pfn (page frame number) are bits 0-54 (see
    * pagemap.txt in linux Documentation)
    */
  if ((page & 0x7fffffffffffffULL) == 0)
    return BAD_IOVA;

  paddr = ((page & 0x7fffffffffffffULL) * PAGE_SIZE)
      + ((unsigned long)virtaddr % PAGE_SIZE);

  return paddr;
}
