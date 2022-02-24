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
#include <poll.h>
#include <asm-generic/ioctl.h>
#include <unistd.h>
#include <linux/limits.h>

#include "nfp_nsp.h"
#include "nfp_nffw.h"
#include "nfp_cpp_dev.h"

static void do_cpp_identification(struct nfp_cpp* cpp,
            struct nfp_cpp_identification* ident)
{
    int total;

    total = offsetof(struct nfp_cpp_identification, size)
        + sizeof(ident->size);

    if (ident->size >= offsetof(struct nfp_cpp_identification, model)
            + sizeof(ident->model)) {
        ident->model = nfp_cpp_model(cpp);
        total = offsetof(struct nfp_cpp_identification, model)
            + sizeof(ident->model);
    }
    if (ident->size >= offsetof(struct nfp_cpp_identification, interface)
            + sizeof(ident->interface)) {
        ident->interface = nfp_cpp_interface(cpp);
        total = offsetof(struct nfp_cpp_identification, interface)
            + sizeof(ident->interface);
    }
    if (ident->size >= offsetof(struct nfp_cpp_identification, serial_hi)
            + sizeof(ident->serial_hi)) {
        const uint8_t *serial;

        nfp_cpp_serial(cpp, &serial);
        ident->serial_hi = (serial[0] <<  8) |
                   (serial[1] <<  0);
        ident->serial_lo = (serial[2] << 24) |
                   (serial[3] << 16) |
                   (serial[4] <<  8) |
                   (serial[5] <<  0);
        total = offsetof(struct nfp_cpp_identification, serial_hi)
            + sizeof(ident->serial_hi);
    }

    /* Modify size to our actual size */
    ident->size = total;
}

static int ioctl_cpp_identification(struct nfp_cpp_dev_data* data,
                int fd)
{
    struct nfp_cpp_identification ident;
    uint64_t temp;
    int ret;

    /* Read size */
    ret = read(fd, &ident.size, sizeof(ident.size));
    if (ret < sizeof(ident.size))
        return -1;

    do_cpp_identification(data->cpp, &ident);

    /* Send return value */
    temp = sizeof(ident);
    ret = write(fd, &temp, sizeof(temp));
    if (ret < sizeof(temp))
        return -1;

    /* Send out params */
    ret = write(fd, &ident, sizeof(ident));
    if (ret < sizeof(ident))
        return -1;

    return 0;
}

#define PAGE_SHIFT	12
#define PAGE_SIZE	(0x1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))

static int nfp_dev_cpp_range_check(struct list_head *req_list,
                   struct nfp_cpp_area_request *area_req)
{
    struct nfp_dev_cpp_area *area;

    /* Look for colliding offsets */
    list_for_each_entry(area, req_list, req_list) {
        if (area_req->offset < (area->req.offset + area->req.size) &&
            (area_req->offset + area_req->size) > area->req.offset) {
            return -ETXTBSY;
        }
    }

    return 0;
}

static int nfp_dev_cpp_area_alloc(struct nfp_cpp_dev_data *cdev, uint16_t interface,
                  struct nfp_cpp_area_request *area_req)
{
    struct nfp_dev_cpp_area *area;
    struct nfp_cpp_area *cpp_area;
    char buff[64];

    area = malloc(sizeof(*area));
    if (!area)
        return -ENOMEM;

    /* Can we allocate the area? */
    snprintf(buff, sizeof(buff), "cdev@0x%lx", area_req->offset);
    cpp_area = nfp_cpp_area_alloc_with_name(
        cdev->cpp, area_req->cpp_id, buff,
        area_req->cpp_addr, area_req->size);
    if (!cpp_area) {
        free(area);
        return -EINVAL;
    }

    area->cdev = cdev;
    area->interface = interface;
    area->area = cpp_area;
    area->req = *area_req;
    list_add_tail(&area->req_list, &cdev->req.list);

    INIT_LIST_HEAD(&area->vma.list);

    /* Poison the area_list entry */
    INIT_LIST_HEAD(&area->area_list);
    list_del(&area->area_list);

    return 0;
}

static void nfp_dev_cpp_area_free(struct nfp_dev_cpp_area *area)
{
    list_del(&area->req_list);
    nfp_cpp_area_free(area->area);
    free(area);
}

static int do_cpp_area_request(struct nfp_cpp_dev_data* data,
            uint16_t interface, struct nfp_cpp_area_request* area_req)
{
    int err = 0;

    if ((area_req->size & ~PAGE_SIZE) != 0)
        return -EINVAL;

    if (area_req->offset != ~0 && (area_req->offset & ~PAGE_MASK) != 0)
        return -EINVAL;

    if (area_req->offset == ~0)
    {
        fprintf(stderr, "IOCTL_AREA_REQ offset=~0\n");

        area_req->offset = 0;
        err = nfp_dev_cpp_range_check(&data->req.list, area_req);
        if (err < 0) {
            struct nfp_dev_cpp_area *area;

            list_for_each_entry(area, &data->req.list, req_list) {
                area_req->offset = area->req.offset +
                           area->req.size;
                err = nfp_dev_cpp_range_check(&data->req.list,
                                  area_req);
                if (err == 0)
                    break;
            }
        }
    }

    if (err >= 0) {
        /* Look for colliding offsets */
        err = nfp_dev_cpp_range_check(&data->req.list, area_req);
        if (err >= 0)
            err = nfp_dev_cpp_area_alloc(data, interface, area_req);
    }

    return err;
}

static int do_cpp_area_release(struct nfp_cpp_dev_data* data,
            uint16_t interface, struct nfp_cpp_area_request* area_req)
{
    struct nfp_dev_cpp_area *area, *atmp;
    int err = -ENOENT;

    list_for_each_entry_safe(area, atmp,
                 &data->req.list, req_list) {
        if (area->req.offset == area_req->offset) {
            err = list_empty(&area->vma.list) ? 0 : -EBUSY;
            if (err == 0)
                nfp_dev_cpp_area_free(area);
            break;
        }
    }

    return err;

}

static int do_cpp_expl_request(struct nfp_cpp_dev_data* data,
                struct nfp_cpp_explicit_request* explicit_req)
{
    return -EINVAL;
}

static int ioctl_area_request(struct nfp_cpp_dev_data* data, int fd)
{
    struct nfp_cpp_area_request area_req;
    int err = 0;

    err = read(fd, &area_req, sizeof(area_req));
    if (err < sizeof(area_req))
        return -1;

    uint16_t interface = nfp_cpp_interface(data->cpp);
    err = do_cpp_area_request(data, interface, &area_req);

    uint64_t temp;
    temp = (uint64_t) err;
    err = write(fd, &temp, sizeof(temp));
    if (err < sizeof(temp))
        return -1;

    err = write(fd, &area_req, sizeof(area_req));
    if (err < sizeof(area_req))
        return -1;

    return 0;
}

static int ioctl_area_release(struct nfp_cpp_dev_data* data, int fd)
{
    struct nfp_cpp_area_request area_req;
    int err = 0;

    err = read(fd, &area_req, sizeof(area_req));
    if (err < sizeof(area_req))
        return -1;

    uint16_t interface = nfp_cpp_interface(data->cpp);
    err = do_cpp_area_release(data, interface, &area_req);

    uint64_t temp;
    temp = (uint64_t) err;
    err = write(fd, &temp, sizeof(temp));
    if (err < sizeof(temp))
        return -1;

    return 0;
}

static int ioctl_expl_request(struct nfp_cpp_dev_data* data, int fd)
{
    struct nfp_cpp_explicit_request explicit_req;
    int err = 0;

    err = read(fd, &explicit_req, sizeof(explicit_req));
    if (err < sizeof(explicit_req))
        return -1;

    err = do_cpp_expl_request(data, &explicit_req);

    uint64_t temp;
    temp = (uint64_t) err;
    err = write(fd, &temp, sizeof(temp));
    if (err < sizeof(temp))
        return -1;

    err = write(fd, &explicit_req, sizeof(explicit_req));
    if (err < sizeof(explicit_req))
        return -1;

    return 0;
}

int nfp_cpp_dev_ioctl(struct nfp_cpp_dev_data* data, int fd,
                unsigned long request)
{
    switch(request)
    {
    case NFP_IOCTL_CPP_IDENTIFICATION:
        return ioctl_cpp_identification(data, fd);

    case NFP_IOCTL_CPP_AREA_REQUEST:
        return ioctl_area_request(data, fd);

    case NFP_IOCTL_CPP_AREA_RELEASE:
        return ioctl_area_release(data, fd);

    case NFP_IOCTL_CPP_EXPL_REQUEST:
        return ioctl_expl_request(data, fd);

    default:
        fprintf(stderr, "%s(): Invalid request type\n", __func__);
        return -1;
    }

    return 0;
}

int nfp_cpp_dev_write(struct nfp_cpp_dev_data* data,
        int fd, size_t count, off_t offset)
{
    char* buf = (char*) malloc(count);
    char* buff = buf;
    struct nfp_cpp_area *area;
    off_t nfp_offset;
    uint32_t cpp_id, pos, len;
    size_t curlen, totlen = 0;
    int err = 0;
    uint64_t tmp;
    uint32_t tmpbuf[16];

    /* Read data from user */
    curlen = 0;
    while (curlen < count)
    {
        err = read(fd, buf + curlen, count - curlen);
        if (err < 0)
        {
            free(buf);
            return -1;
        }
        curlen += err;
    }

    /* Obtain target's CPP ID and offset in target */
    cpp_id = (offset >> 40) << 8;
    nfp_offset = offset & ((1ull << 40) - 1);


    /* Adjust length if not aligned */
    if (((nfp_offset + (off_t)count - 1) & ~(NFP_CPP_MEMIO_BOUNDARY - 1)) !=
        (nfp_offset & ~(NFP_CPP_MEMIO_BOUNDARY - 1))) {
        curlen = NFP_CPP_MEMIO_BOUNDARY -
            (nfp_offset & (NFP_CPP_MEMIO_BOUNDARY - 1));
    }

    curlen = count;
    while (count > 0) {
        /* configure a CPP PCIe2CPP BAR for mapping the CPP target */
        area = nfp_cpp_area_alloc_with_name(data->cpp, cpp_id, "nfp.cdev",
                            nfp_offset, curlen);
        if (!area) {
            err = -EIO;
            goto handle_write_error;
        }

        err = nfp_cpp_area_acquire(area);
        if (err < 0) {
            nfp_cpp_area_free(area);
            err = -EIO;
            goto handle_write_error;
        }

        for (pos = 0; pos < curlen; pos += len) {
            len = curlen - pos;
            if (len > sizeof(tmpbuf))
                len = sizeof(tmpbuf);

            memcpy(tmpbuf, buff + pos, len);

            err = nfp_cpp_area_write(area, pos, tmpbuf, len);
            if (err < 0) {
                nfp_cpp_area_release(area);
                nfp_cpp_area_free(area);
                err = -EIO;
                goto handle_write_error;
            }
        }

        nfp_offset += pos;
        totlen += pos;
        buff += pos;
        nfp_cpp_area_release(area);
        nfp_cpp_area_free(area);

        if (err < 0)
            goto handle_write_error;

        count -= pos;
        curlen = (count > NFP_CPP_MEMIO_BOUNDARY) ?
             NFP_CPP_MEMIO_BOUNDARY : count;
    }

    tmp = (uint64_t) totlen;
    err = write(fd, &tmp, sizeof(tmp));
    if (err < sizeof(tmp))
    {
        free(buf);
        return -1;
    }
    free(buf);
    return 0;

handle_write_error:
    free(buf);
    tmp = (uint64_t) err;
    err = write(fd, &tmp, sizeof(tmp));
    if (err < 0)
    {
        return -1;
    }

    return 0;
}

int nfp_cpp_dev_read(struct nfp_cpp_dev_data* data,
        int fd, size_t count, off_t offset)
{
    char* buf = (char*) malloc(count);
    char* buff = buf;
    struct nfp_cpp_area *area;
    off_t nfp_offset;
    uint32_t cpp_id, pos, len;
    size_t curlen = count, totlen = 0;
    uint32_t tmpbuf[16];
    int err = 0;
    uint64_t tmp;

    /* Obtain target's CPP ID and offset in target */
    cpp_id = (offset >> 40) << 8;
    nfp_offset = offset & ((1ull << 40) - 1);

    /* Adjust length if not aligned */
    if (((nfp_offset + (off_t)count - 1) & ~(NFP_CPP_MEMIO_BOUNDARY - 1)) !=
        (nfp_offset & ~(NFP_CPP_MEMIO_BOUNDARY - 1))) {
        curlen = NFP_CPP_MEMIO_BOUNDARY -
            (nfp_offset & (NFP_CPP_MEMIO_BOUNDARY - 1));
    }

    while (count > 0) {
        area = nfp_cpp_area_alloc_with_name(data->cpp, cpp_id, "nfp.cdev",
                            nfp_offset, curlen);
        if (!area) {
            err = -EIO;
            goto handle_read_error;
        }

        err = nfp_cpp_area_acquire(area);
        if (err < 0) {
            nfp_cpp_area_free(area);
            err = -EIO;
            goto handle_read_error;
        }

        for (pos = 0; pos < curlen; pos += len)
        {
            len = curlen - pos;
            if (len > sizeof(tmpbuf))
                len = sizeof(tmpbuf);

            err = nfp_cpp_area_read(area, pos, tmpbuf, len);
            if (err < 0)
                break;
            memcpy(buff + pos, tmpbuf, len);
        }

        nfp_offset += pos;
        totlen += pos;
        buff += pos;
        nfp_cpp_area_release(area);
        nfp_cpp_area_free(area);

        if (err < 0)
            goto handle_read_error;

        count -= pos;
        curlen = (count > NFP_CPP_MEMIO_BOUNDARY) ?
            NFP_CPP_MEMIO_BOUNDARY : count;
    }

    /* Send return value */
    tmp = (uint64_t) totlen;
    err = write(fd, &tmp, sizeof(tmp));
    if (err < sizeof(tmp))
    {
        free(buf);
        return -1;
    }

    curlen = 0;
    while (curlen < totlen)
    {
        err = write(fd, buf + curlen, totlen - curlen);
        if (err < 0)
        {
            free(buf);
            return -1;
        }

        curlen += err;
    }
    free(buf);
    return 0;

handle_read_error:
    free(buf);
    tmp = (uint64_t) err;
    err = write(fd, &tmp, sizeof(tmp));
    if (err < 0)
    {
        return -1;
    }

    return 0;
}
