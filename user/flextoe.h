/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef TAS_H_
#define TAS_H_

#include "driver.h"

#include "connect.h"
#include "fp_mem.h"
#include "fp_debug.h"
#include "config.h"
#include "packet_defs.h"

extern int exited;

extern struct configuration config;
extern void *flextoe_dma_mem;
extern struct flextcp_pl_mem *fp_state; /* MMIO */
extern struct flexnic_info_t *flextoe_info;
extern struct flextcp_pl_debug *fp_debug;
extern unsigned int *fp_journal;
extern struct nfp_handle_t nic_handle;
extern struct eth_addr eth_addr;

int slowpath_main(void);

int shm_init(void);
void shm_cleanup(void);
void shm_set_ready(void);

int nic_init(void);
uint64_t nic_us_to_cyc(uint64_t us);
void nic_cleanup(void);

#endif /* TAS_H_ */
