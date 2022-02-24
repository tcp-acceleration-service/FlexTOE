/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sysmacros.h>
#include <linux/limits.h>

#include <rte/byteorder.h>
#include <rte/common.h>
#include <rte/pci.h>
#include <rte/string_fns.h>

#include "driver.h"
#include "nfp_cpp.h"
#include "nfp_nffw.h"
#include "nfp_hwinfo.h"
#include "nfp_mip.h"
#include "nfp_rtsym.h"
#include "nfp_nsp.h"

/* Probing Netronome NICs */
#define PCI_VENDOR_ID_NETRONOME         0x19ee
#define PCI_DEVICE_ID_NFP4000_PF_NIC    0x4000
#define PCI_DEVICE_ID_NFP6000_PF_NIC    0x6000

/* split string into tokens */
static int
strsplit(char *string, int stringlen,
         char **tokens, int maxtokens, char delim)
{
    int i, tok = 0;
    int tokstart = 1; /* first token is right at start of string */

    if (string == NULL || tokens == NULL)
        goto einval_error;

    for (i = 0; i < stringlen; i++) {
        if (string[i] == '\0' || tok >= maxtokens)
            break;
        if (tokstart) {
            tokstart = 0;
            tokens[tok++] = &string[i];
        }
        if (string[i] == delim) {
            string[i] = '\0';
            tokstart = 1;
        }
    }
    return tok;

einval_error:
    errno = EINVAL;
    return -1;
}

/*
 * split up a pci address into its constituent parts.
 */
static int
parse_pci_addr_format(const char *buf, int bufsize, struct rte_pci_addr *addr)
{
    /* first split on ':' */
    union splitaddr {
        struct {
            char *domain;
            char *bus;
            char *devid;
            char *function;
        };
        char *str[PCI_FMT_NVAL]; /* last element-separator is "." not ":" */
    } splitaddr;

    char *buf_copy = strndup(buf, bufsize);
    if (buf_copy == NULL)
        return -1;

    if (strsplit(buf_copy, bufsize, splitaddr.str, PCI_FMT_NVAL, ':')
            != PCI_FMT_NVAL - 1)
        goto error;
    /* final split is on '.' between devid and function */
    splitaddr.function = strchr(splitaddr.devid,'.');
    if (splitaddr.function == NULL)
        goto error;
    *splitaddr.function++ = '\0';

    /* now convert to int values */
    errno = 0;
    addr->domain = strtoul(splitaddr.domain, NULL, 16);
    addr->bus = strtoul(splitaddr.bus, NULL, 16);
    addr->devid = strtoul(splitaddr.devid, NULL, 16);
    addr->function = strtoul(splitaddr.function, NULL, 10);
    if (errno != 0)
        goto error;

    free(buf_copy); /* free the copy made with strdup */
    return 0;
error:
    free(buf_copy);
    return -1;
}

/* parse a sysfs (or other) file containing one integer value */
static int
parse_sysfs_value(const char *path, unsigned long *val)
{
    FILE *f;
    char buf[BUFSIZ];
    char *end = NULL;

    if ((f = fopen(path, "r")) == NULL) {
        fprintf(stderr, "%s(): cannot open sysfs value %s\n",
            __func__, path);
        return -1;
    }

    if (fgets(buf, sizeof(buf), f) == NULL) {
        fprintf(stderr, "%s(): cannot read sysfs value %s\n",
            __func__, path);
        fclose(f);
        return -1;
    }
    *val = strtoul(buf, &end, 0);
    if ((buf[0] == '\0') || (end == NULL) || (*end != '\n')) {
        fprintf(stderr, "%s(): cannot parse sysfs value %s\n",
                __func__, path);
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

/* parse the "resource" sysfs file */
static int
pci_parse_sysfs_resource(const char *filename, struct rte_pci_device *dev)
{
    FILE *f;
    char buf[BUFSIZ];
    int i;
    uint64_t phys_addr, end_addr, flags;
    union pci_resource_info
    {
        struct {
            char *phys_addr;
            char *end_addr;
            char *flags;
        };
        char *ptrs[PCI_RESOURCE_FMT_NVAL];
    } res_info;

    f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Cannot open sysfs resource\n");
        return -1;
    }

    for (i = 0; i<PCI_MAX_RESOURCE; i++)
    {

        if (fgets(buf, sizeof(buf), f) == NULL)
        {
            fprintf(stderr,
                "%s(): cannot read resource\n", __func__);
            goto error;
        }

        if (strsplit(buf, sizeof(buf), res_info.ptrs, 3, ' ') != 3)
        {
            fprintf(stderr,
                "%s(): bad resource format\n", __func__);
            goto error;
        }
        errno = 0;
        phys_addr = strtoull(res_info.phys_addr, NULL, 16);
        end_addr = strtoull(res_info.end_addr, NULL, 16);
        flags = strtoull(res_info.flags, NULL, 16);
        if (errno != 0)
        {
            fprintf(stderr,
                "%s(): bad resource format\n", __func__);
            goto error;
        }

        if (flags & IORESOURCE_MEM)
        {
            dev->mem_resource[i].phys_addr = phys_addr;
            dev->mem_resource[i].len = end_addr - phys_addr + 1;
            /* not mapped for now */
            dev->mem_resource[i].addr = NULL;
        }
    }
    fclose(f);
    return 0;

error:
    fclose(f);
    return -1;
}

/**
 * Setup pci_dev entry from a matched PCI device
 */
static struct rte_pci_device*
pci_setup_device(const char *dirname, const struct rte_pci_addr *addr)
{
    char path[PATH_MAX];
    unsigned long tmp;
    struct rte_pci_device *dev;

    dev = malloc(sizeof(struct rte_pci_device));
    if (dev == NULL)
        return NULL;
    memset(dev, 0, sizeof(struct rte_pci_device));
    dev->addr = *addr;

    /* Get vendor ID */
    snprintf(path, sizeof(path), "%s/vendor", dirname);
    if (parse_sysfs_value(path, &tmp) < 0) {
        free(dev);
        return NULL;
    }
    dev->id.vendor_id = (uint16_t)tmp;

    /* Get device id */
    snprintf(path, sizeof(path), "%s/device", dirname);
    if (parse_sysfs_value(path, &tmp) < 0) {
        free(dev);
        return NULL;
    }
    dev->id.device_id = (uint16_t)tmp;

    /* get subsystem_vendor id */
    snprintf(path, sizeof(path), "%s/subsystem_vendor",
         dirname);
    if (parse_sysfs_value(path, &tmp) < 0) {
        free(dev);
        return NULL;
    }
    dev->id.subsystem_vendor_id = (uint16_t)tmp;

    /* get subsystem_device id */
    snprintf(path, sizeof(path), "%s/subsystem_device",
         dirname);
    if (parse_sysfs_value(path, &tmp) < 0) {
        free(dev);
        return NULL;
    }
    dev->id.subsystem_device_id = (uint16_t)tmp;

    /* get class_id */
    snprintf(path, sizeof(path), "%s/class",
         dirname);
    if (parse_sysfs_value(path, &tmp) < 0) {
        free(dev);
        return NULL;
    }
    /* the least 24 bits are valid: class, subclass, program interface */
    dev->id.class_id = (uint32_t)tmp & PCI_CLASS_ANY_ID;

    /* Set max_cfs */
    dev->max_vfs = 0;
    snprintf(path, sizeof(path), "%s/max_vfs", dirname);
    if (!access(path, F_OK) &&
        parse_sysfs_value(path, &tmp) == 0)
        dev->max_vfs = (uint16_t)tmp;

    /* Set device name */
    snprintf(dev->name, sizeof(dev->name), PCI_PRI_FMT,
                addr->domain, addr->bus,
                addr->devid, addr->function);
    dev->device.name = dev->name;

    /* parse resources */
    snprintf(path, sizeof(path), "%s/resource", dirname);
    if (pci_parse_sysfs_resource(path, dev) < 0) {
        fprintf(stderr, "%s(): cannot parse resource\n", __func__);
        free(dev);
        return NULL;
    }

    return dev;
}

/**
 * Scan sysfs for PCI devices to connect with Netronome NIC
 */
static struct rte_pci_device*
pci_scan()
{
    struct dirent *e;
    DIR *dir;
    char path[PATH_MAX];
    struct rte_pci_addr addr;
    unsigned long vendor_id, device_id;

    dir = opendir("/sys/bus/pci/devices/");
    if (dir == NULL)
    {
        fprintf(stderr, "%s(): opendir failed: %s\n",
            __func__, strerror(errno));
        return NULL;
    }

    /* Read every directory in /sys/bus/pci/devices */
    while ((e = readdir(dir)) != NULL)
    {
        if (e->d_name[0] == '.')
            continue;

        if (parse_pci_addr_format(e->d_name, sizeof(e->d_name), &addr) != 0)
            continue;

        /* Read vendor and device id*/
        snprintf(path, sizeof(path), "%s/%s/%s",
            "/sys/bus/pci/devices", e->d_name, "vendor");
        if (parse_sysfs_value(path, &vendor_id) < 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s/%s",
            "/sys/bus/pci/devices", e->d_name, "device");
        if (parse_sysfs_value(path, &device_id) < 0)
            continue;

        if (vendor_id == PCI_VENDOR_ID_NETRONOME &&
                (device_id == PCI_DEVICE_ID_NFP4000_PF_NIC ||
                device_id == PCI_DEVICE_ID_NFP6000_PF_NIC))
        {
            /* Setup NIC */
            closedir(dir);

            snprintf(path, sizeof(path), "%s/%s",
                "/sys/bus/pci/devices", e->d_name);
            return pci_setup_device(path, &addr);
        }
    }

    closedir(dir);
    return NULL;
}

#if 0
static int
pci_mknod_uio_dev(const char *sysfs_uio_path, unsigned uio_num)
{
    FILE *f;
    char filename[PATH_MAX];
    int ret;
    unsigned major, minor;
    dev_t dev;

    /* get the name of the sysfs file that contains the major and minor
     * of the uio device and read its content */
    snprintf(filename, sizeof(filename), "%s/dev", sysfs_uio_path);

    f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "%s(): cannot open sysfs to get major:minor\n",
            __func__);
        return -1;
    }

    ret = fscanf(f, "%u:%u", &major, &minor);
    if (ret != 2) {
        fprintf(stderr, "%s(): cannot parse sysfs to get major:minor\n",
            __func__);
        fclose(f);
        return -1;
    }
    fclose(f);

    /* create the char device "mknod /dev/uioX c major minor" */
    snprintf(filename, sizeof(filename), "/dev/uio%u", uio_num);
    dev = makedev(major, minor);
    ret = mknod(filename, S_IFCHR | S_IRUSR | S_IWUSR, dev);
    if (ret != 0) {
        fprintf(stderr, "%s(): mknod() failed %s\n",
            __func__, strerror(errno));
        return -1;
    }

    return ret;
}
#endif

/* Alloc Interrupt Handlers */
static int
pci_get_device(struct rte_pci_device *dev)
{
    char dirname[PATH_MAX];
    char cfgname[PATH_MAX];
    char devname[PATH_MAX]; /* contains the /dev/uioX */
    int uio_num;
    struct dirent *e;
    DIR *dir;
    struct rte_pci_addr *loc;

    loc = &dev->addr;

    snprintf(dirname, sizeof(dirname),
            "%s/" PCI_PRI_FMT "/uio", "/sys/bus/pci/devices",
            loc->domain, loc->bus, loc->devid, loc->function);
    dir = opendir(dirname);
    if (dir == NULL) {
        /* retry with the parent directory */
        snprintf(dirname, sizeof(dirname),
                "%s/" PCI_PRI_FMT, "/sys/bus/pci/devices",
                loc->domain, loc->bus, loc->devid, loc->function);
        dir = opendir(dirname);

        if (dir == NULL) {
            fprintf(stderr, "Cannot opendir %s\n", dirname);
            return -1;
        }
    }

    /* take the first file starting with "uio" */
    while ((e = readdir(dir)) != NULL) {
        /* format could be uio%d ...*/
        int shortprefix_len = sizeof("uio") - 1;
        /* ... or uio:uio%d */
        int longprefix_len = sizeof("uio:uio") - 1;
        char *endptr;

        if (strncmp(e->d_name, "uio", 3) != 0)
            continue;

        /* first try uio%d */
        errno = 0;
        uio_num = strtoull(e->d_name + shortprefix_len, &endptr, 10);
        if (errno == 0 && endptr != (e->d_name + shortprefix_len)) {
            snprintf(devname, sizeof(devname), "%s/uio%u", dirname, uio_num);
            break;
        }

        /* then try uio:uio%d */
        errno = 0;
        uio_num = strtoull(e->d_name + longprefix_len, &endptr, 10);
        if (errno == 0 && endptr != (e->d_name + longprefix_len)) {
            snprintf(devname, sizeof(devname), "%s/uio:uio%u", dirname, uio_num);
            break;
        }
    }
    closedir(dir);

    /* No uio resource found */
    if (e == NULL)
        return -1;

#if 0
    if (pci_mknod_uio_dev(devname, uio_num) < 0)
        fprintf(stderr, "%s(): Cannot create /dev/uio%u\n",
            __func__, uio_num);
#endif
    snprintf(devname, sizeof(devname), "/dev/uio%u", uio_num);

    /* save fd if in primary process */
    dev->intr_handle.fd = open(devname, O_RDWR);
    if (dev->intr_handle.fd < 0) {
        fprintf(stderr, "Cannot open %s: %s\n",
            devname, strerror(errno));
        return -1;
    }

    snprintf(cfgname, sizeof(cfgname),
            "/sys/class/uio/uio%u/device/config", uio_num);
    dev->intr_handle.uio_cfg_fd = open(cfgname, O_RDWR);
    if (dev->intr_handle.uio_cfg_fd < 0) {
        fprintf(stderr, "Cannot open %s: %s\n",
            cfgname, strerror(errno));
        return -1;
    }

    dev->intr_handle.type = RTE_INTR_HANDLE_UIO;

    return 0;
}

/* Map PCI device */
static int
pci_map_device(struct rte_pci_device *dev)
{
    int ret = -1;
    int fd;
    int i;
    uint64_t phaddr;
    void* mapaddr;
    char devname[PATH_MAX];

    ret = pci_get_device(dev);
    if (ret < 0)
    {
        fprintf(stderr, "%s(): Unable to get device\n",
            __func__);
        return -1;
    }

    /* Map all BARs */
    for (i = 0; i != PCI_MAX_RESOURCE; i++)
    {
        /* skip empty BAR */
        phaddr = dev->mem_resource[i].phys_addr;
        if (phaddr == 0)
            continue;

        snprintf(devname, sizeof(devname),
            "%s/" PCI_PRI_FMT "/resource%d",
            "/sys/bus/pci/devices",
            dev->addr.domain, dev->addr.bus, dev->addr.devid,
            dev->addr.function, i);
        fd = open(devname, O_RDWR);
        if (fd < 0)
        {
            fprintf(stderr, "Cannot open %s: %s\n",
                devname, strerror(errno));
            return -1;
        }

        mapaddr = mmap(NULL, (size_t)dev->mem_resource[i].len,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        if (mapaddr == MAP_FAILED)
        {
            fprintf(stderr,
                "%s(): cannot mmap(%d, %p, 0x%zx, 0x%llx): %s (%p)\n",
                __func__, fd, NULL, dev->mem_resource[i].len,
                (unsigned long long) 0,
                strerror(errno), mapaddr);
            return -1;
        }

        dev->mem_resource[i].addr = mapaddr;
    }

/* TODO: Free other maps in case one fails! */
    return 0;
}

#if 0
#define DEFAULT_FW_PATH       "/lib/firmware/netronome"

static int
nfp_fw_upload(struct rte_pci_device *dev, struct nfp_nsp *nsp, char *card)
{
    struct nfp_cpp *cpp = nsp->cpp;
    int fw_f;
    char *fw_buf;
    char fw_name[125];
    char serial[40];
    struct stat file_stat;
    off_t fsize, bytes;

    // Looking for firmware file in order of priority

    // First try to find a firmware image specific for this device
    snprintf(serial, sizeof(serial),
            "serial-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
        cpp->serial[0], cpp->serial[1], cpp->serial[2], cpp->serial[3],
        cpp->serial[4], cpp->serial[5], cpp->interface >> 8,
        cpp->interface & 0xff);

    snprintf(fw_name, sizeof(fw_name), "%s/%s.nffw", DEFAULT_FW_PATH,
            serial);

    fprintf(stderr, "Trying with fw file: %s\n", fw_name);
    fw_f = open(fw_name, O_RDONLY);
    if (fw_f >= 0)
        goto read_fw;

    // Then try the PCI name
    snprintf(fw_name, sizeof(fw_name), "%s/pci-%s.nffw", DEFAULT_FW_PATH,
            dev->device.name);

    fprintf(stderr, "Trying with fw file: %s\n", fw_name);
    fw_f = open(fw_name, O_RDONLY);
    if (fw_f >= 0)
        goto read_fw;

    // Finally try the card type and media
    snprintf(fw_name, sizeof(fw_name), "%s/%s", DEFAULT_FW_PATH, card);
    fprintf(stderr, "Trying with fw file: %s\n", fw_name);
    fw_f = open(fw_name, O_RDONLY);
    if (fw_f < 0) {
        fprintf(stderr,"Firmware file %s not found.\n", fw_name);
        return -ENOENT;
    }

read_fw:
    if (fstat(fw_f, &file_stat) < 0) {
        fprintf(stderr,"Firmware file %s size is unknown\n", fw_name);
        close(fw_f);
        return -ENOENT;
    }

    fsize = file_stat.st_size;
    fprintf(stderr,"Firmware file found at %s with size: %" PRIu64 "\n",
                fw_name, (uint64_t)fsize);

    fw_buf = malloc((size_t)fsize);
    if (!fw_buf) {
        fprintf(stderr,"malloc failed for fw buffer\n");
        close(fw_f);
        return -ENOMEM;
    }
    memset(fw_buf, 0, fsize);

    bytes = read(fw_f, fw_buf, fsize);
    if (bytes != fsize) {
        fprintf(stderr,"Reading fw to buffer failed."
                   "Just %" PRIu64 " of %" PRIu64 " bytes read\n",
                   (uint64_t)bytes, (uint64_t)fsize);
        free(fw_buf);
        close(fw_f);
        return -EIO;
    }

    fprintf(stderr,"Uploading the firmware ...\n");
    nfp_nsp_load_fw(nsp, fw_buf, bytes);
    fprintf(stderr,"Done\n");

    free(fw_buf);
    close(fw_f);

    return 0;
}

static int
nfp_fw_setup(struct rte_pci_device *dev, struct nfp_cpp *cpp,
         struct nfp_eth_table *nfp_eth_table, struct nfp_hwinfo *hwinfo)
{
    struct nfp_nsp *nsp;
    const char *nfp_fw_model;
    char card_desc[100];
    int err = 0;

    nfp_fw_model = nfp_hwinfo_lookup(hwinfo, "assembly.partno");

    if (nfp_fw_model) {
        fprintf(stderr, "firmware model found: %s", nfp_fw_model);
    } else {
        fprintf(stderr,"firmware model NOT found");
        return -EIO;
    }

    if (nfp_eth_table->count == 0 || nfp_eth_table->count > 8) {
        fprintf(stderr,"NFP ethernet table reports wrong ports: %u\n",
               nfp_eth_table->count);
        return -EIO;
    }

    fprintf(stderr, "NFP ethernet port table reports %u ports\n",
               nfp_eth_table->count);

    fprintf(stderr, "Port speed: %u\n", nfp_eth_table->ports[0].speed);

    snprintf(card_desc, sizeof(card_desc), "nic_%s_%dx%d.nffw",
            nfp_fw_model, nfp_eth_table->count,
            nfp_eth_table->ports[0].speed / 1000);

    nsp = nfp_nsp_open(cpp);
    if (!nsp) {
        fprintf(stderr,"NFP error when obtaining NSP handle\n");
        return -EIO;
    }

    nfp_nsp_device_soft_reset(nsp);
    err = nfp_fw_upload(dev, nsp, card_desc);

    nfp_nsp_close(nsp);
    return err;
}
#endif

static int
pci_probe(struct rte_pci_device *dev, struct nfp_cpp** cppptr)
{
    struct nfp_cpp *cpp;
    struct nfp_hwinfo *hwinfo;
    struct nfp_eth_table *nfp_eth_table = NULL;
    // struct nfp_rtsym_table *sym_tbl;
    // int total_ports;
    // int err;
    int ret = -ENODEV;

    if (!dev)
        return ret;

    ret = pci_map_device(dev);
    if (ret < 0)
    {
        fprintf(stderr, "%s(): Cannot map device\n",
            __func__);
        return -EIO;
    }

    cpp = nfp_cpp_from_device_name(dev, 1);
    if (!cpp)
    {
        fprintf(stderr, "%s(): CPP handle can not be obtained\n",
            __func__);
        ret = -EIO;
        goto error;
    }

    hwinfo = nfp_hwinfo_read(cpp);
    if (!hwinfo) {
        fprintf(stderr, "%s(): Error reading hwinfo table",
            __func__);
        return -EIO;
    }

    nfp_eth_table = nfp_eth_read_ports(cpp);
    if (!nfp_eth_table) {
        fprintf(stderr, "%s(): Error reading NFP ethernet table",
            __func__);

        return -EIO;
    }

#if 0
    if (nfp_fw_setup(dev, cpp, nfp_eth_table, hwinfo)) {
            fprintf(stderr, "Error when uploading firmware");
            ret = -EIO;
            goto error;
    }

    sym_tbl = nfp_rtsym_table_read(cpp);
    if (!sym_tbl) {
        fprintf(stderr, "%s(): Something is wrong with the firmware"
                " symbol table", __func__);
        ret = -EIO;
        goto error;
    }

    total_ports = nfp_rtsym_read_le(sym_tbl, "nfd_cfg_pf0_num_ports", &err);
    if (total_ports != (int)nfp_eth_table->count) {
        fprintf(stderr, "%s(): Inconsistent number of ports",
            __func__);
        ret = -EIO;
        goto error;
    }

    fprintf(stderr, "Total pf ports: %d", total_ports);
#endif

    *cppptr = cpp;
error:
    free(nfp_eth_table);
    return ret;
}

int
nfp_probe_device(struct nfp_handle_t* handle)
{
    struct rte_pci_device* dev;
    int ret;

    dev = pci_scan();

    if (!dev) {
        fprintf(stderr, "Failed to detect compatible NICs\n");
        return -ENODEV;
    }
    handle->dev = dev;

    ret = pci_probe(dev, &handle->cpp);
    if (ret < 0) {
        fprintf(stderr, "Unable to setup NIC\n");
        return ret;
    }

    return 0;
}
