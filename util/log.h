/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_

#include <stdio.h>

static inline void hexdump(const unsigned char* addr, const unsigned len)
{
  unsigned idx, i;
  unsigned char buf[16];

  for (idx = 0; (idx + 16) < len; idx += 16) {
    for (i = 0; i < 16; i++) {
      if (i + idx >= len)
        break;

      buf[i] = addr[i + idx];
    }

  printf("%08x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
      idx, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
      buf[8 + 0], buf[8 + 1], buf[8 + 2], buf[8 + 3], buf[8 + 4], buf[8 + 5], buf[8 + 6], buf[8 + 7]);
}

  if (idx < len) {
    printf("%08x:", idx);
    for (; idx < len; idx++) {
      printf(" %02x", addr[idx]);
    }
    printf("\n");
  }
}

#endif /* UTILS_LOG_H_ */
