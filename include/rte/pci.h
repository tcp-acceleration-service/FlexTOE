/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2015 Intel Corporation.
 * Copyright 2013-2014 6WIND S.A.
 */

#ifndef _RTE_ETHDEV_PCI_H_
#define _RTE_ETHDEV_PCI_H_

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <sys/types.h>

/** Formatting string for PCI device identifier: Ex: 0000:00:01.0 */
#define PCI_PRI_FMT "%.4" PRIx16 ":%.2" PRIx8 ":%.2" PRIx8 ".%" PRIx8
#define PCI_PRI_STR_SIZE sizeof("XXXXXXXX:XX:XX.X")

/** Short formatting string, without domain, for PCI device: Ex: 00:01.0 */
#define PCI_SHORT_PRI_FMT "%.2" PRIx8 ":%.2" PRIx8 ".%" PRIx8

/** Nb. of values in PCI device identifier format string. */
#define PCI_FMT_NVAL 4

/** Nb. of values in PCI resource format. */
#define PCI_RESOURCE_FMT_NVAL 3

/** Maximum number of PCI resources. */
#define PCI_MAX_RESOURCE 6

#define PCI_ANY_ID (0xffff)
#define PCI_CLASS_ANY_ID (0xffffff)

/** IO resource type: */
#define IORESOURCE_IO         0x00000100
#define IORESOURCE_MEM        0x00000200

/**
 * A structure describing an ID for a PCI driver. Each driver provides a
 * table of these IDs for each device that it supports.
 */
struct rte_pci_id {
    uint32_t class_id;            /**< Class ID or RTE_CLASS_ANY_ID. */
    uint16_t vendor_id;           /**< Vendor ID or PCI_ANY_ID. */
    uint16_t device_id;           /**< Device ID or PCI_ANY_ID. */
    uint16_t subsystem_vendor_id; /**< Subsystem vendor ID or PCI_ANY_ID. */
    uint16_t subsystem_device_id; /**< Subsystem device ID or PCI_ANY_ID. */
};

/**
 * A structure describing the location of a PCI device.
 */
struct rte_pci_addr {
    uint32_t domain;                /**< Device domain */
    uint8_t bus;                    /**< Device bus */
    uint8_t devid;                  /**< Device ID */
    uint8_t function;               /**< Device function. */
};

/*
 * Internal identifier length
 * Sufficiently large to allow for UUID or PCI address
 */
#define RTE_DEV_NAME_MAX_LEN 64

/**
 * A structure describing a generic device.
 */
struct rte_device {
    const char *name;             /**< Device name */
};

/**
 * A generic memory resource representation.
 */
struct rte_mem_resource {
    uint64_t phys_addr; /**< Physical address, 0 if not resource. */
    uint64_t len;       /**< Length of the resource. */
    void *addr;         /**< Virtual address, NULL when not mapped. */
};

#define RTE_MAX_RXTX_INTR_VEC_ID      512
#define RTE_INTR_VEC_ZERO_OFFSET      0
#define RTE_INTR_VEC_RXTX_OFFSET      1

/**
 * The interrupt source type, e.g. UIO, VFIO, ALARM etc.
 */
enum rte_intr_handle_type {
    RTE_INTR_HANDLE_UNKNOWN = 0,  /**< generic unknown handle */
    RTE_INTR_HANDLE_UIO,          /**< uio device handle */
    RTE_INTR_HANDLE_UIO_INTX,     /**< uio generic handle */
    RTE_INTR_HANDLE_VFIO_LEGACY,  /**< vfio device handle (legacy) */
    RTE_INTR_HANDLE_VFIO_MSI,     /**< vfio device handle (MSI) */
    RTE_INTR_HANDLE_VFIO_MSIX,    /**< vfio device handle (MSIX) */
    RTE_INTR_HANDLE_ALARM,        /**< alarm handle */
    RTE_INTR_HANDLE_EXT,          /**< external handler */
    RTE_INTR_HANDLE_VDEV,         /**< virtual device */
    RTE_INTR_HANDLE_DEV_EVENT,    /**< device event handle */
    RTE_INTR_HANDLE_VFIO_REQ,     /**< VFIO request handle */
    RTE_INTR_HANDLE_MAX           /**< count of elements */
};

#define RTE_INTR_EVENT_ADD            1UL
#define RTE_INTR_EVENT_DEL            2UL

typedef void (*rte_intr_event_cb_t)(int fd, void *arg);

struct rte_epoll_data {
    uint32_t event;               /**< event type */
    void *data;                   /**< User data */
    rte_intr_event_cb_t cb_fun;   /**< IN: callback fun */
    void *cb_arg;	              /**< IN: callback arg */
};

enum {
    RTE_EPOLL_INVALID = 0,
    RTE_EPOLL_VALID,
    RTE_EPOLL_EXEC,
};

/** interrupt epoll event obj, taken by epoll_event.ptr */
struct rte_epoll_event {
    volatile uint32_t status;  /**< OUT: event status */
    int fd;                    /**< OUT: event fd */
    int epfd;       /**< OUT: epoll instance the ev associated with */
    struct rte_epoll_data epdata;
};

/** Handle for interrupts. */
struct rte_intr_handle {
    union {
        int vfio_dev_fd;  /**< VFIO device file descriptor */
        int uio_cfg_fd;  /**< UIO cfg file desc for uio_pci_generic */
    };
    int fd;	 /**< interrupt event file descriptor */
    enum rte_intr_handle_type type;  /**< handle type */
    uint32_t max_intr;             /**< max interrupt requested */
    uint32_t nb_efd;               /**< number of available efd(event fd) */
    uint8_t efd_counter_size;      /**< size of efd counter, used for vdev */
    int efds[RTE_MAX_RXTX_INTR_VEC_ID];  /**< intr vectors/efds mapping */
    struct rte_epoll_event elist[RTE_MAX_RXTX_INTR_VEC_ID];
                       /**< intr vector epoll event */
    int *intr_vec;                 /**< intr vector number array */
};

/**
 * A structure describing a PCI device.
 */
struct rte_pci_device {
    struct rte_device device;           /**< Inherit core device */
    struct rte_pci_addr addr;           /**< PCI location. */
    struct rte_pci_id id;               /**< PCI ID. */
    struct rte_mem_resource mem_resource[PCI_MAX_RESOURCE];
                        /**< PCI Memory Resource */
    struct rte_intr_handle intr_handle; /**< Interrupt handle */
    uint16_t max_vfs;                   /**< sriov enable if not zero */
    char name[PCI_PRI_STR_SIZE+1];      /**< PCI location (ASCII) */
};

int rte_pci_read_config(const struct rte_pci_device *dev,
        void *buf, size_t len, off_t offset);

int rte_pci_write_config(const struct rte_pci_device *dev,
        const void *buf, size_t len, off_t offset);

#endif /* _RTE_ETHDEV_PCI_H_ */
