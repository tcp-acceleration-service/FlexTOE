/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2018 Netronome Systems, Inc.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define __USE_GNU
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <poll.h>
#include <asm-generic/ioctl.h>
#include <unistd.h>
#include <linux/limits.h>

#include <rte/pci.h>
#include <rte/io.h>

#include "driver.h"

#include "nfp_ioctl.h"
#include "nfp_cpp.h"
#include "nfp_nsp.h"
#include "nfp_nffw.h"
#include "nfp_cpp_dev.h"

static int nfp_cpp_dev_handle_cmd(struct nfp_cpp_dev_data* data, int fd)
{
    uint64_t tmp;
    size_t count;
    off_t offset;
    unsigned long request;
    int ret;

    ret = read(fd, &tmp, sizeof(tmp));
    if (ret < 0)
        return ret;

    switch(tmp)
    {
    case OP_PREAD:
        ret = read(fd, &tmp, sizeof(tmp));
        if (ret < 0)
            return ret;
        count = (size_t) tmp;

        ret = read(fd, &tmp, sizeof(tmp));
        if (ret < 0)
            return ret;
        offset = (off_t) tmp;

        return nfp_cpp_dev_read(data, fd, count, offset);
    case OP_PWRITE:
        ret = read(fd, &tmp, sizeof(tmp));
        if (ret < 0)
            return ret;
        count = (size_t) tmp;

        ret = read(fd, &tmp, sizeof(tmp));
        if (ret < 0)
            return ret;
        offset = (off_t) tmp;

        return nfp_cpp_dev_write(data, fd, count, offset);
    case OP_IOCTL:
        ret = read(fd, &tmp, sizeof(tmp));
        if (ret < 0)
            return ret;
        request = (unsigned long) tmp;

        return nfp_cpp_dev_ioctl(data, fd, request);
    default:
        return -1;
    }
    return 0;
}

static int nfp_cpp_dev_poll(struct nfp_cpp_dev_data* data)
{
    int ret;
    int i, j;
    int fd;
    int close_count;

    while (1)
    {
        /* Infinite timeout */
        ret = poll(data->connections, data->count, -1);
        if (ret <= 0)
        {
            fprintf(stderr, "%s:%d\n", __func__, __LINE__);
            continue;
        }

        if (data->connections[0].revents & (POLLIN))
        {
            if (data->count < MAX_CONNECTIONS)
            {
                fd = accept(data->listen_fd, NULL, NULL);
                data->connections[data->count].fd = fd;
                data->connections[data->count].events = (POLLIN);
                data->connections[data->count].revents = 0;
                data->count++;
            }
        }

        close_count = 0;
        for (i = 1; i < data->count; i++)
        {
            if (data->connections[i].revents == 0)
                continue;

            if (data->connections[i].revents & (POLLIN))
            {
                ret = nfp_cpp_dev_handle_cmd(data, data->connections[i].fd);
                if (ret < 0)
                {
                    fprintf(stderr, "%s():%d\n", __func__, __LINE__);
                    close(data->connections[i].fd);
                    data->connections[i].fd = -1;
                    close_count++;
                }
            }

            if (data->connections[i].revents & (POLLHUP))
            {
                close(data->connections[i].fd);
                data->connections[i].fd = -1;
                close_count++;
            }
        }

        /* Cleanup list */
        if (close_count)
        {
            j = 1;
            for (i = 1; i < data->count; i++)
            {
                if (data->connections[i].fd != -1)
                {
                    data->connections[j].fd = data->connections[i].fd;
                    data->connections[j].events = (POLLIN);
                    data->connections[j].revents = 0;

                    j++;
                }
            }

            data->count = j;
        }
    }

    return 0;
}

static void* nfp_bar_resource_start(struct nfp_bar *bar)
{
    return ((uint8_t*) bar->resource->addr) + (bar->mask + 1) * (bar->index & 7);
}

static int nfp_bar_write(struct nfp_cpp_dev_data *data, struct nfp_bar *bar,
uint32_t newcfg)
{
    int base, slot;
    int xbar;

    base = bar->index >> 3;
    slot = bar->index & 7;

    if (data->iomem.csr)
    {
        xbar = NFP_PCIE_CPP_BAR_PCIETOCPPEXPANSIONBAR(base, slot);
        rte_write32(newcfg, data->iomem.csr + xbar);
        /* Readback to ensure BAR is flushed */
        rte_read32(data->iomem.csr + xbar);
    }
    else
    {
        xbar = NFP_PCIE_CFG_BAR_PCIETOCPPEXPANSIONBAR(base, slot);
        if (rte_pci_write_config(data->dev, &newcfg, sizeof(newcfg), xbar) < 0)
            return -1;
    }
    bar->barcfg = newcfg;

    return 0;
}

static int bar_cmp(const void *aptr, const void *bptr)
{
	const struct nfp_bar *a = aptr, *b = bptr;

	if (a->bitsize == b->bitsize)
		return a->index - b->index;
	else
		return a->bitsize - b->bitsize;
}

static int nfp_enable_bars(struct nfp_cpp_dev_data *data)
{
    int i, bars_free;
    int expl_groups;
    uint32_t interface;
    struct nfp_bar *bar;
	const uint32_t barcfg_explicit[4] = {
		NFP_PCIE_BAR_PCIE2CPP_MapType(
			NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT0),
		NFP_PCIE_BAR_PCIE2CPP_MapType(
			NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT1),
		NFP_PCIE_BAR_PCIE2CPP_MapType(
			NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT2),
		NFP_PCIE_BAR_PCIE2CPP_MapType(
			NFP_PCIE_BAR_PCIE2CPP_MapType_EXPLICIT3),
	};

    bar = &data->bar[0];
    for (i = 0; i < ARRAY_SIZE(data->bar); i++, bar++)
    {
        struct rte_mem_resource* res;

        res = &data->dev->mem_resource[(i >> 3) * 2];
        if (res->phys_addr == 0) {
            bar--;
            continue;
        }

        bar->barcfg = 0;
        bar->index = i;
        bar->mask = (res->len / 8) - 1;
        bar->bitsize = rte_fls_u64(bar->mask);
        bar->base = 0;
        bar->iomem = NULL;
        bar->resource = res;
    }

    data->bars = bar - &data->bar[0];
    if (data->bars < 8) {
        fprintf(stderr, "No usable BARs found\n");
        return -EINVAL;
    }

    bars_free = data->bars;

    interface = nfp_cpp_interface(data->cpp);
    data->expl.master_id =
                    ((NFP_CPP_INTERFACE_UNIT_of(interface) & 3) + 4) << 4;
    data->expl.signal_ref = 0x10;

    if ((bar->mask + 1) >= NFP_PCI_MIN_MAP_SIZE)
        bar->iomem = nfp_bar_resource_start(bar);
    bars_free--;
    data->expl.data = bar->iomem + NFP_PCIE_SRAM + 0x1000;
    data->iomem.csr = bar->iomem + NFP_PCIE_BAR(0);
    data->iomem.em = bar->iomem + NFP_PCIE_EM;

    /* Skip bar 1 */
    expl_groups = 4;
    bars_free--;

    for (i = 0; i < 4; i++)
    {
        int j;
        if (i >= NFP_PCIE_EXPLICIT_BARS || i >= expl_groups)
        {
            data->expl.group[i].bitsize = 0;
            continue;
        }

        bar = &data->bar[4 + i];
        bar->iomem = nfp_bar_resource_start(bar);
        bars_free--;

        data->expl.group[i].bitsize = bar->bitsize;
        data->expl.group[i].addr = bar->iomem;
        nfp_bar_write(data, bar, barcfg_explicit[i]);

        for (j = 0; j < 4; j++)
            data->expl.group[i].free[j] = 1;

        data->iomem.expl[i] = bar->iomem;
    }

    qsort(&data->bar[0], data->bars, sizeof(data->bar[0]),
            bar_cmp);

    return 0;
}

int nfp_cpp_dev_main(struct nfp_handle_t* handle)
{
    int ret;
    struct sockaddr address;
    struct nfp_cpp_dev_data* data =
            malloc(sizeof(struct nfp_cpp_dev_data));
    struct rte_pci_device* dev = handle->dev;
    struct nfp_cpp* cpp = handle->cpp;

    if (!data)
    {
        fprintf(stderr, "%s():%d\n", __func__, __LINE__);
        return -1;
    }

    unlink("/tmp/nfp_cpp");
    data->cpp = cpp;
    memset(data->connections, 0, sizeof(data->connections));
    data->listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(&address, 0, sizeof(struct sockaddr));
	address.sa_family = AF_UNIX;
	strcpy(address.sa_data, "/tmp/nfp_cpp");
	ret = bind(data->listen_fd, (const struct sockaddr *)&address,
		   sizeof(struct sockaddr));
    if (ret < 0)
    {
        fprintf(stderr, "%s():%d\n", __func__, __LINE__);
        close(data->listen_fd);
        return -1;
    }
    ret = listen(data->listen_fd, LISTEN_BACKLOG);
    if (ret < 0)
    {
        fprintf(stderr, "%s():%d\n", __func__, __LINE__);
        close(data->listen_fd);
        return -1;
    }

    data->connections[0].fd = data->listen_fd;
    data->connections[0].events = (POLLIN);
    data->connections[0].revents = 0;
    data->count = 1;

    INIT_LIST_HEAD(&data->event.list);
    INIT_LIST_HEAD(&data->area.list);
    INIT_LIST_HEAD(&data->req.list);

    data->dev = dev;
    if (nfp_enable_bars(data) < 0)
    {
        fprintf(stderr, "%s():%d\n", __func__, __LINE__);
        close(data->listen_fd);
        return -1;
    }
    return nfp_cpp_dev_poll(data);
}
