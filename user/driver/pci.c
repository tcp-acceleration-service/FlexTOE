/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <rte/pci.h>

/* Read PCI config space. */
int rte_pci_read_config(const struct rte_pci_device *dev,
        void *buf, size_t len, off_t offset)
{
    const struct rte_intr_handle *intr_handle = &dev->intr_handle;
    return pread(intr_handle->uio_cfg_fd, buf, len, offset);
}

/* Write PCI config space. */
int rte_pci_write_config(const struct rte_pci_device *dev,
        const void *buf, size_t len, off_t offset)
{
    const struct rte_intr_handle *intr_handle = &dev->intr_handle;
    return pwrite(intr_handle->uio_cfg_fd, buf, len, offset);
}
