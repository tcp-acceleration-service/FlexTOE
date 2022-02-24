// SPDX-License-Identifier: GPL-2.0
/*-
 * Copyright(c) 2010-2017 Intel Corporation. All rights reserved.
 */

#include "../include/params.h"

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/device.h>
#include <linux/module.h>
#include <linux/eventfd.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/msi.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

static inline bool flextoeuio_kernel_is_locked_down(void)
{
#ifdef CONFIG_LOCK_DOWN_KERNEL
#ifdef CONFIG_LOCK_DOWN_IN_EFI_SECURE_BOOT
  return kernel_is_locked_down(NULL);
#elif defined(CONFIG_EFI_SECURE_BOOT_LOCK_DOWN)
  return kernel_is_locked_down();
#else
  return false;
#endif
#else
  return false;
#endif
}

/** NOTE: This driver only supports MSI-X style interrupts. */

#define MAX_MSIX_VECTORS        64
#define DRIVER_VERSION          "0.1"

/**
 * A structure describing the private information for a uio device.
 */
struct flextoe_uio_msix_irq_ctx {
  int evfd;											/*> eventfd mapped to interrupt */
  struct eventfd_ctx *trigger;  /*> vector to eventfd */
  char *name;                   /*> name in /proc/interrupts */
};
struct flextoe_uio_pci_dev {
  struct uio_info info;       /*> UIO driver info */
  struct pci_dev *pdev;       /*> PCI device */
  struct mutex    mutex;      /*> open/release mutex */
  int refcnt;                 /*> references to device */
  unsigned int max_vectors;   /*> MSI-X slots available */
  struct flextoe_uio_msix_irq_ctx *ctx;  /*> MSI-X eventfd ctx */
};

static int wc_activate;

/**
 * This is interrupt handler which will check if the interrupt is for the right device.
 * If yes, disable it here and will be enable later.
 */
static irqreturn_t
flextoeuio_msi_irqhandler(int irq, void *arg)
{
  struct eventfd_ctx *trigger = arg;

  eventfd_signal(trigger, 1);
  return IRQ_HANDLED;
}

/* set the mapping between vector # and existing eventfd. */
static int set_irq_eventfd(struct flextoe_uio_pci_dev *udev, u32 vec, int fd)
{
  struct eventfd_ctx *trigger;
  int irq, err;

  if (vec >= udev->max_vectors) {
    dev_notice(&udev->pdev->dev, "vec %u >= num_vec %u\n",
              vec, udev->max_vectors);
    return -ERANGE;
  }

  irq = pci_irq_vector(udev->pdev, vec);
  trigger = udev->ctx[vec].trigger;
  if (trigger) {
    /* Clearup existing irq mapping */
    free_irq(irq, trigger);
    eventfd_ctx_put(trigger);
    udev->ctx[vec].trigger = NULL;
    udev->ctx[vec].evfd = -1;
  }

  /* Passing -1 is used to disable interrupt */
  if (fd < 0)
    return 0;

  trigger = eventfd_ctx_fdget(fd);
  if (IS_ERR(trigger)) {
    err = PTR_ERR(trigger);
    dev_notice(&udev->pdev->dev,
              "eventfd ctx get failed: %d\n", err);
    return err;
  }

  err = request_irq(irq, flextoeuio_msi_irqhandler, 0,
                udev->ctx[vec].name, trigger);
  if (err) {
    dev_notice(&udev->pdev->dev,
              "request irq failed: %d\n", err);
    eventfd_ctx_put(trigger);
    return err;
  }
  udev->ctx[vec].trigger = trigger;
  udev->ctx[vec].evfd = fd;
  return 0;
}

/* Map interrupts to eventfds */
static ssize_t
show_irq_fds(struct device *dev, struct device_attribute *attr,
            char *buf)
{
  struct pci_dev *pdev = to_pci_dev(dev);
  struct uio_info *info = pci_get_drvdata(pdev);
  struct flextoe_uio_pci_dev *udev = info->priv;

  int i;
  ssize_t offset = 0;

  mutex_lock(&udev->mutex);
  for (i = 0; i < udev->max_vectors; i++) {
    offset += scnprintf(buf + offset, PAGE_SIZE - offset, "%d ", udev->ctx[i].evfd);
  }
  mutex_unlock(&udev->mutex);

  offset += scnprintf(buf + offset, PAGE_SIZE - offset, "\n");
  return offset;
}

/* write format is different from read. Input: <idx, fd> */
static ssize_t
store_irq_fds(struct device *dev, struct device_attribute *attr,
              const char *buf, size_t count)
{
  struct pci_dev *pdev = to_pci_dev(dev);
  struct uio_info *info = pci_get_drvdata(pdev);
  struct flextoe_uio_pci_dev *udev = info->priv;
  int vec, fd, err;

  if (sscanf(buf, "%u %d", &vec, &fd) != 2)
    return -EINVAL;

  if (vec >= udev->max_vectors)
    return -EINVAL;

  mutex_lock(&udev->mutex);
  err = set_irq_eventfd(udev, vec, fd);
  mutex_unlock(&udev->mutex);

  return err ? err : count;
}

static DEVICE_ATTR(irq_fds, S_IRUGO | S_IWUSR, show_irq_fds, store_irq_fds);

static struct attribute *dev_attrs[] = {
  &dev_attr_irq_fds.attr,
  NULL,
};

static const struct attribute_group dev_attr_grp = {
  .attrs = dev_attrs,
};

/**
 * This gets called while opening uio device file.
 */
static int
flextoeuio_pci_open(struct uio_info *info, struct inode *inode)
{
  struct flextoe_uio_pci_dev *udev = info->priv;
  struct pci_dev *dev = udev->pdev;
  int ret = 0;

  /* Enable interrupts on first open */
  mutex_lock(&udev->mutex);
  if (udev->refcnt++ == 0) {
    ret = pci_alloc_irq_vectors(udev->pdev, udev->max_vectors, udev->max_vectors, PCI_IRQ_MSIX);
  }
  mutex_unlock(&udev->mutex);

  if (ret != udev->max_vectors) {
    dev_err(&dev->dev, "Enable interrupt fails err=%u\n", ret);
    return -ENOTSUPP;
  }
  return 0;
}

static int
flextoeuio_pci_release(struct uio_info *info, struct inode *inode)
{
  struct flextoe_uio_pci_dev *udev = info->priv;
  struct pci_dev *dev = udev->pdev;
  int i;

  /* Disable interrupts on last release */
  mutex_lock(&udev->mutex);
  if (--udev->refcnt == 0) {
    for (i = 0; i < udev->max_vectors; i++) {
      struct eventfd_ctx *trigger = udev->ctx[i].trigger;

      if (!trigger)
        continue;

      eventfd_ctx_put(trigger);
      udev->ctx[i].trigger = NULL;
    }
    pci_free_irq_vectors(udev->pdev);

    /* stop the device from further DMA */
    pci_clear_master(dev);
  }
  mutex_unlock(&udev->mutex);

  return 0;
}

/* Remap pci resources described by bar #pci_bar in uio resource n. */
static int
flextoeuio_pci_setup_iomem(struct pci_dev *dev, struct uio_info *info,
           int n, int pci_bar, const char *name)
{
  unsigned long addr, len;
  void *internal_addr;

  if (n >= ARRAY_SIZE(info->mem))
    return -EINVAL;

  addr = pci_resource_start(dev, pci_bar);
  len = pci_resource_len(dev, pci_bar);
  if (addr == 0 || len == 0)
    return -1;
  if (wc_activate == 0) {
    internal_addr = ioremap(addr, len);
    if (internal_addr == NULL)
      return -1;
  } else {
    internal_addr = NULL;
  }
  info->mem[n].name = name;
  info->mem[n].addr = addr;
  info->mem[n].internal_addr = internal_addr;
  info->mem[n].size = len;
  info->mem[n].memtype = UIO_MEM_PHYS;
  return 0;
}

/* Get pci port io resources described by bar #pci_bar in uio resource n. */
static int
flextoeuio_pci_setup_ioport(struct pci_dev *dev, struct uio_info *info,
    int n, int pci_bar, const char *name)
{
  unsigned long addr, len;

  if (n >= ARRAY_SIZE(info->port))
    return -EINVAL;

  addr = pci_resource_start(dev, pci_bar);
  len = pci_resource_len(dev, pci_bar);
  if (addr == 0 || len == 0)
    return -EINVAL;

  info->port[n].name = name;
  info->port[n].start = addr;
  info->port[n].size = len;
  info->port[n].porttype = UIO_PORT_X86;

  return 0;
}

/* Unmap previously ioremap'd resources */
static void
flextoeuio_pci_release_iomem(struct uio_info *info)
{
  int i;

  for (i = 0; i < MAX_UIO_MAPS; i++) {
    if (info->mem[i].internal_addr)
      iounmap(info->mem[i].internal_addr);
  }
}

static int
flextoeuio_setup_bars(struct pci_dev *dev, struct uio_info *info)
{
  int i, iom, iop, ret;
  unsigned long flags;
  static const char *bar_names[PCI_STD_RESOURCE_END + 1]  = {
    "BAR0",
    "BAR1",
    "BAR2",
    "BAR3",
    "BAR4",
    "BAR5",
  };

  iom = 0;
  iop = 0;

  for (i = 0; i < ARRAY_SIZE(bar_names); i++) {
    if (pci_resource_len(dev, i) != 0 &&
        pci_resource_start(dev, i) != 0) {
      flags = pci_resource_flags(dev, i);
      if (flags & IORESOURCE_MEM) {
        ret = flextoeuio_pci_setup_iomem(dev, info, iom,
                   i, bar_names[i]);
        if (ret != 0)
          return ret;
        iom++;
      } else if (flags & IORESOURCE_IO) {
        ret = flextoeuio_pci_setup_ioport(dev, info, iop,
                    i, bar_names[i]);
        if (ret != 0)
          return ret;
        iop++;
      }
    }
  }

  return (iom != 0 || iop != 0) ? ret : -ENOENT;
}

static int
flextoeuio_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
  struct flextoe_uio_pci_dev *udev;
  dma_addr_t map_dma_addr;
  void *map_addr;
  int err, i, vectors;

  if (pci_is_bridge(dev)) {
    dev_warn(&dev->dev, "Ignoring PCI bridge device\n");
    return -ENODEV;
  }

  udev = kzalloc(sizeof(struct flextoe_uio_pci_dev), GFP_KERNEL);
  if (!udev)
    return -ENOMEM;

  /*
   * enable device: ask low-level code to enable I/O and
   * memory
   */
  err = pci_enable_device(dev);
  if (err != 0) {
    dev_err(&dev->dev, "Cannot enable PCI device\n");
    goto fail_free;
  }

  /* enable bus mastering on the device */
  pci_set_master(dev);

  /* remap IO memory */
  err = flextoeuio_setup_bars(dev, &udev->info);
  if (err != 0)
    goto fail_release_iomem;

  /* set 64-bit DMA mask */
  err = pci_set_dma_mask(dev,  DMA_BIT_MASK(64));
  if (err != 0) {
    dev_err(&dev->dev, "Cannot set DMA mask\n");
    goto fail_release_iomem;
  }

  err = pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(64));
  if (err != 0) {
    dev_err(&dev->dev, "Cannot set consistent DMA mask\n");
    goto fail_release_iomem;
  }

  /* fill uio infos */
  /* TODO: implement irqcontrol */
  udev->info.name = "flextoe_uio";
  udev->info.version = DRIVER_VERSION;
  udev->info.open = flextoeuio_pci_open;
  udev->info.release = flextoeuio_pci_release;
  udev->info.irq = UIO_IRQ_CUSTOM;
  udev->info.priv = udev;
  udev->pdev = dev;
  udev->refcnt = 0;
  mutex_init(&udev->mutex);

  vectors = pci_msix_vec_count(dev);
  if (vectors < FLEXNIC_PL_APPCTX_NUM) {
    dev_info(&dev->dev, "device does not support MSI-X or supports \
                      fewer vectors=%d\n", vectors);
    err = -EINVAL;
    goto fail_release_iomem;
  }

  udev->max_vectors = FLEXNIC_PL_APPCTX_NUM;
  dev_info(&dev->dev, "using up to %u MSI-X vectors\n",
            udev->max_vectors);

  udev->ctx = kcalloc(udev->max_vectors,
                      sizeof(struct flextoe_uio_msix_irq_ctx), GFP_KERNEL);
  if (!udev->ctx) {
    err = -ENOMEM;
    goto fail_release_iomem;
  }

  for (i = 0; i < udev->max_vectors; i++) {
    udev->ctx[i].evfd = -1;
    udev->ctx[i].name = kasprintf(GFP_KERNEL,
                                  KBUILD_MODNAME "[%d](%s)",
                                  i, pci_name(dev));
    if (!udev->ctx[i].name) {
      err = -ENOMEM;
      goto fail_free_ctx;
    }
  }

  err = sysfs_create_group(&dev->dev.kobj, &dev_attr_grp);
  if (err != 0)
    goto fail_free_ctx;

  /* register uio driver */
  err = uio_register_device(&dev->dev, &udev->info);
  if (err != 0)
    goto fail_remove_group;

  pci_set_drvdata(dev, udev);

  /*
   * Doing a harmless dma mapping for attaching the device to
   * the iommu identity mapping if kernel boots with iommu=pt.
   * Note this is not a problem if no IOMMU at all.
   */
  map_addr = dma_alloc_coherent(&dev->dev, 1024, &map_dma_addr,
      GFP_KERNEL);
  if (map_addr)
    memset(map_addr, 0, 1024);

  if (!map_addr)
    dev_info(&dev->dev, "dma mapping failed\n");
  else {
    dev_info(&dev->dev, "mapping 1K dma=%#llx host=%p\n",
       (unsigned long long)map_dma_addr, map_addr);

    dma_free_coherent(&dev->dev, 1024, map_addr, map_dma_addr);
    dev_info(&dev->dev, "unmapping 1K dma=%#llx host=%p\n",
       (unsigned long long)map_dma_addr, map_addr);
  }

  return 0;

fail_remove_group:
  sysfs_remove_group(&dev->dev.kobj, &dev_attr_grp);
fail_free_ctx:
  for (i = 0; i < udev->max_vectors; i++)
    kfree(udev->ctx[i].name);
  kfree(udev->ctx);
fail_release_iomem:
  flextoeuio_pci_release_iomem(&udev->info);
  pci_disable_device(dev);
fail_free:
  kfree(udev);

  return err;
}

static void
flextoeuio_pci_remove(struct pci_dev *dev)
{
  struct flextoe_uio_pci_dev *udev = pci_get_drvdata(dev);
  int i;

  flextoeuio_pci_release(&udev->info, NULL);

  sysfs_remove_group(&dev->dev.kobj, &dev_attr_grp);
  uio_unregister_device(&udev->info);
  flextoeuio_pci_release_iomem(&udev->info);
  for (i = 0; i < udev->max_vectors; i++)
    kfree(udev->ctx[i].name);
  kfree(udev->ctx);
  pci_disable_device(dev);
  pci_set_drvdata(dev, NULL);
  kfree(udev);
}

static struct pci_driver flextoeuio_pci_driver = {
  .name = "flextoe_uio",
  .id_table = NULL,
  .probe = flextoeuio_pci_probe,
  .remove = flextoeuio_pci_remove,
};

static int __init
flextoeuio_pci_init_module(void)
{
  BUILD_BUG_ON(LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0));
  BUILD_BUG_ON(FLEXNIC_PL_APPCTX_NUM > MAX_MSIX_VECTORS);

  if (flextoeuio_kernel_is_locked_down()) {
    pr_err("Not able to use module, kernel lock down is enabled\n");
    return -EINVAL;
  }

  if (wc_activate != 0)
    pr_info("wc_activate is set\n");

  return pci_register_driver(&flextoeuio_pci_driver);
}

static void __exit
flextoeuio_pci_exit_module(void)
{
  pci_unregister_driver(&flextoeuio_pci_driver);
}

module_init(flextoeuio_pci_init_module);
module_exit(flextoeuio_pci_exit_module);

module_param(wc_activate, int, 0);
MODULE_PARM_DESC(wc_activate,
"Activate support for write combining (WC) (default=0)\n"
"    0 - disable\n"
"    other - enable\n");

MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Rajath Shashidhara <rajaths@utexas.edu>");
MODULE_DESCRIPTION("UIO driver for FlexTOE on Netronome Agilio PCI cards");
MODULE_SOFTDEP("pre: uio");
