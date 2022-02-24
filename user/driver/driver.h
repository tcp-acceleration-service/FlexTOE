/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef _USERSPACE_DRIVER_H
#define _USERSPACE_DRIVER_H

#include <rte/pci.h>
#include <nfp_nsp.h>

struct nfp_handle_t {
    struct rte_pci_device* dev;
    struct nfp_cpp* cpp;
};


/**
 * Scan sysfs for PCI devices to connect with Netronome NIC
 * Create a NFP CPP object handle to use with NFP libs
 */
int nfp_probe_device(struct nfp_handle_t* handle);

/**
 * Start userspace driver server to connect to BSP tools
 */
int nfp_cpp_dev_main(struct nfp_handle_t* handle);

#endif /* _USERSPACE_DRIVER_H */
