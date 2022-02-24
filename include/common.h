/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef _FLEXTOE_COMMON_H_
#define _FLEXTOE_COMMON_H_

#include "params.h"

#if defined(__NFP_LANG_MICROC)
  #define FIRMWARE 1
#else
  #define FIRMWARE 0
#endif

#if FIRMWARE
  #define PACKED_STRUCT(name)                 __packed struct name
  #define PACKED_ALIGN_STRUCT(name, align)    __packed __align##align struct name
  #define PACKED_UNION(name)                  __packed union name
#else
  #define PACKED_STRUCT(name)                 struct __attribute__((packed)) name
  #define PACKED_ALIGN_STRUCT(name, align)    struct __attribute__((packed, aligned(align))) name
  #define PACKED_UNION(name)                  union __attribute__((packed)) name
#endif

#endif /* _FLEXTOE_COMMON_H_ */
