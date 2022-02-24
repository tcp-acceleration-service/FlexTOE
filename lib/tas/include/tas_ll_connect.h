/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLEXNIC_DRIVER_H_
#define FLEXNIC_DRIVER_H_

#include <stddef.h>

#include "connect.h"

/**
 * Connect to flexnic. Returns 0 on success, < 0 on error, > 0 if flexnic is not
 * ready yet.
 */
int flexnic_driver_connect(struct flexnic_info_t **info, void **mem_start);

/** Connect to flexnic internal memory. */
int flexnic_driver_internal(void **int_mem_start);

#endif /* FLEXNIC_DRIVER_H_ */
