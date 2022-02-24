/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2018 Netronome Systems, Inc.
 * All rights reserved.
 */

#ifndef _NFP_CPP_DEV_OPS_H_
#define _NFP_CPP_DEV_OPS_H_

#include <poll.h>

#include "list.h"
#include "nfp_cpp.h"
#include "nfp_ioctl.h"
#include <rte/pci.h>

#define LISTEN_BACKLOG 8
#define MAX_CONNECTIONS 64

#define NFP_CPP_MEMIO_BOUNDARY		(1 << 20)
#define PCI_64BIT_BAR_COUNT             3
#define NFP_PCI_BAR_MAX    (PCI_64BIT_BAR_COUNT * 8)
#define NFP_PCI_MIN_MAP_SIZE		0x080000
#define NFP_PCIE_SRAM               0x000000
#define NFP_PCIE_EM                 0x020000
#define NFP_PCIE_BAR(_pf)       (0x30000 + ((_pf) & 7) * 0xc0)
#define NFP_PCIE_EXPLICIT_BARS	2
#define NFP_PCIE_CFG_BAR_PCIETOCPPEXPANSIONBAR(bar, slot) \
	(0x400 + ((bar) * 8 + (slot)) * 4)
#define NFP_PCIE_CPP_BAR_PCIETOCPPEXPANSIONBAR(bar, slot) \
	(((bar) * 8 + (slot)) * 4)
#define   NFP_PCIE_BAR_PCIE2CPP_MapType(_x)     (((_x) & 0x7) << 29)
#define     NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT0     4
#define     NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT1     5
#define     NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT2     6
#define     NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT3     7

enum op_type {
    OP_UNUSED,
    OP_PREAD,
    OP_PWRITE,
    OP_IOCTL
};

struct nfp_bar {
	uint32_t barcfg;
	uint64_t base;          /* CPP address base */
	uint64_t mask;          /* Bit mask of the bar */
	uint32_t bitsize;       /* Bit size of the bar */
	int index;

	void *iomem;
	struct rte_mem_resource *resource;
};

struct nfp_cpp_dev_data
{
    struct nfp_cpp* cpp;
    struct rte_pci_device* dev;
    int listen_fd;
    struct pollfd connections[MAX_CONNECTIONS];
    int count;
    char firmware[NFP_FIRMWARE_MAX];
    int bars;
    struct nfp_bar bar[NFP_PCI_BAR_MAX];
    struct {
        void *csr;
        void *em;
        void *expl[4];
    } iomem;
    struct {
        uint8_t master_id;
        uint8_t signal_ref;
        void *data;
        struct {
            void *addr;
            int bitsize;
            int free[4];
        } group[4];
    } expl;
    struct {
        struct list_head list;
    } event;
    struct {
        struct list_head list;
    } area;
    struct {
        struct list_head list;
    } req;
};

/* Acquired areas */
struct nfp_dev_cpp_area {
	struct nfp_cpp_dev_data *cdev;
	uint32_t interface;
	struct nfp_cpp_area_request req;
	struct list_head req_list; /* protected by cdev->req.lock */
	struct nfp_cpp_area *area;
	struct list_head area_list; /* protected by cdev->area.lock */
	struct {
		struct list_head list;	/* protected by cdev->area.lock */
	} vma;
};

int nfp_cpp_dev_ioctl(struct nfp_cpp_dev_data* data, int fd,
                unsigned long request);
int nfp_cpp_dev_write(struct nfp_cpp_dev_data* data,
        int fd, size_t count, off_t offset);
int nfp_cpp_dev_read(struct nfp_cpp_dev_data* data,
        int fd, size_t count, off_t offset);

#endif /* _NFP_CPP_DEV_OPS_H_ */
