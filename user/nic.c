/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include "nfp_cpp.h"
#include "nfp_nsp.h"
#include "nfp_rtsym.h"

#include "nfp6000/nfp6000.h"
#include "nfp-common/nfp_resid.h"

#include <rte/cycles.h>

#include "util/common.h"

#include "common.h"
#include "connect.h"
#include "flextoe.h"
#include "driver.h"

struct nfp_handle_t nic_handle;
static struct nfp_cpp_area* fp_state_area = NULL;
static struct nfp_cpp_area* fp_debug_area = NULL;
static struct nfp_cpp_area* fp_jrnl_area  = NULL;
struct flextcp_pl_mem *fp_state = NULL;
struct flextcp_pl_debug *fp_debug = NULL;
unsigned int *fp_journal = NULL;


#define SYMBOL_FP_STATE "_fp_state"
#define SYMBOL_FP_DEBUG "_fp_debug"
#define SYMBOL_FP_JRNL  "fp_dbg_journal"
#define BAR_RESOURCE_IDX 0

__rte_unused static int nic_reset_pcie_queues(struct nfp_cpp* handle)
{
  uint32_t cpp_id, dma_status;
  int ret;

  /* Reset ToPCIe queues */
  cpp_id = NFP_CPP_ISLAND_ID(NFP_CPP_TARGET_PCIE, NFP_CPP_ACTION_RW, 0, 4); // PCIe Island 4
  if ((ret = nfp_cpp_readl(handle, cpp_id, NFP_PCIE_DMA_QSTS0_TOPCI, &dma_status)) < 0)
    goto error_cpp;
  // Clear QUEUE_STOP & ERROR bits
  dma_status &= ~NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP;
  dma_status &= ~NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP;
  dma_status &= ~NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP;
  dma_status &= ~NFP_PCIE_DMA_QSTS0_DMA_ERROR(NFP_PCIE_DMA_QSTS0_DMA_ERROR_msk);
  if ((ret = nfp_cpp_writel(handle, cpp_id, NFP_PCIE_DMA_QSTS0_TOPCI, dma_status)) < 0)
    goto error_cpp;

  /* Reset FromPCIe queues */
  cpp_id = NFP_CPP_ISLAND_ID(NFP_CPP_TARGET_PCIE, NFP_CPP_ACTION_RW, 0, 4); // PCIe Island 4
  if ((ret = nfp_cpp_readl(handle, cpp_id, NFP_PCIE_DMA_QSTS0_FROMPCI, &dma_status)) < 0)
    goto error_cpp;
  // Clear QUEUE_STOP & ERROR bits
  dma_status &= ~NFP_PCIE_DMA_QSTS0_HI_QUEUE_STOP;
  dma_status &= ~NFP_PCIE_DMA_QSTS0_MED_QUEUE_STOP;
  dma_status &= ~NFP_PCIE_DMA_QSTS0_LO_QUEUE_STOP;
  dma_status &= ~NFP_PCIE_DMA_QSTS0_DMA_ERROR(NFP_PCIE_DMA_QSTS0_DMA_ERROR_msk);
  if ((ret = nfp_cpp_writel(handle, cpp_id, NFP_PCIE_DMA_QSTS0_FROMPCI, dma_status)) < 0)
    goto error_cpp;

  return 0;

error_cpp:
  return ret;
}

static int nic_load_fw(struct nfp_nsp* handle)
{
  int fd;
  char* fw_buf;
  struct stat fw_fstat;
  int ret;

  fd = open(config.firmware_path, O_RDONLY);
  if (fd < 0) {
    ret = -errno;
    goto error_fd;
  }

  if (fstat(fd, &fw_fstat) < 0) {
    ret = -errno;
    goto error_fstat;
  }

  if ((fw_buf = mmap(0, fw_fstat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
    ret = -errno;
    goto error_mmap;
  }

  if ((ret = nfp_nsp_load_fw(handle, fw_buf, fw_fstat.st_size)) < 0)
    goto error_nsp;

  munmap(fw_buf, fw_fstat.st_size);
  close(fd);

  return 0;

error_nsp:
  munmap(fw_buf, fw_fstat.st_size);
error_mmap:
error_fstat:
  close(fd);
error_fd:
  return ret;
}

static int nic_configure_eth_if(struct nfp_cpp* handle)
{
  struct nfp_eth_table *eth_table;
  static_assert(PORT_IN_USE < NS_PLATFORM_NUM_PORTS, "Invalid PORT configuration");

  /* Read MAC address */
  eth_table = nfp_eth_read_ports(handle);
  if (eth_table == NULL)
    return -EIO;

  if (eth_table->count <= PORT_IN_USE) {
    free(eth_table);
    return -ENOENT;
  }

  memcpy(&flextoe_info->mac_address, eth_table->ports[PORT_IN_USE].mac_addr, ETH_ALEN);
  free(eth_table);

  /* Enable RX/TX on chosen port */
  return nfp_eth_set_mod_enable(handle, NS_PLATFORM_MAC_CORE_SERDES_LO(PORT_IN_USE), 1, 1, 1);
}

static int nic_mmap_symbols(struct nfp_cpp* handle)
{
  struct nfp_rtsym_table* symbol_table = nfp_rtsym_table_read(handle);
  if (symbol_table == NULL)
    return -EIO;

  /* Flow state */
  fp_state = (struct flextcp_pl_mem*) nfp_rtsym_map(symbol_table, SYMBOL_FP_STATE, sizeof(*fp_state), &fp_state_area);
  if (fp_state == NULL)
    goto error_mmap;

  /* Debug */
  RTE_SET_USED(fp_debug);
  RTE_SET_USED(fp_debug_area);
#if FP_LOG_ENABLE || FP_STAT_ENABLE || FP_PROF_ENABLE
  fp_debug = (struct flextcp_pl_debug*) nfp_rtsym_map(symbol_table, SYMBOL_FP_DEBUG, sizeof(*fp_debug), &fp_debug_area);
  if (fp_debug == NULL)
    goto error_mmap;
#endif

  /* Journal */
  RTE_SET_USED(fp_journal);
  RTE_SET_USED(fp_jrnl_area);
#if FP_JRNL_ENABLE
  fp_journal = (unsigned int*) nfp_rtsym_map(symbol_table, SYMBOL_FP_JRNL, JOURNAL_SIZE, &fp_jrnl_area);
  if (fp_journal == NULL)
    goto error_mmap;
#endif

  free(symbol_table);
  return 0;

error_mmap:
  free(symbol_table);
  return -ENOENT;
}

static int nic_init_config()
{
  /* Init BAR info */
  snprintf(flextoe_info->bar_resource_path, PATH_MAX,
            "%s/" PCI_PRI_FMT "/resource%d",
            "/sys/bus/pci/devices",
            nic_handle.dev->addr.domain,
            nic_handle.dev->addr.bus,
            nic_handle.dev->addr.devid,
            nic_handle.dev->addr.function, BAR_RESOURCE_IDX);
  flextoe_info->internal_mem_offset = ((uint8_t*) fp_state) - ((uint8_t*) nic_handle.dev->mem_resource[BAR_RESOURCE_IDX].addr);
  flextoe_info->internal_mem_size = nic_handle.dev->mem_resource[BAR_RESOURCE_IDX].len;

  /* Write MAC address to fastpath */
  uint64_t local_mac = 0;
  memcpy(&local_mac, &flextoe_info->mac_address, ETH_ALEN);
  nn_writeq(htobe64(local_mac), &fp_state->cfg.local_mac_1);
  nn_writeq(nic_us_to_cyc(config.fp_poll_interval_app), &fp_state->cfg.poll_cycle_app);

  return 0;
}

uint64_t nic_us_to_cyc(uint64_t us)
{
  /* NFP tsc increments every 16 cycles */
  uint64_t tsc_freq = (NS_PLATFORM_PCLK * 1000000ull)/16;
  uint64_t tsc_per_us = tsc_freq/US_PER_S;

  return us * tsc_per_us;
}

int nic_init()
{
  struct nfp_nsp *nsp_handle;
  int ret;

  printf(" - Probe... \n");
  if ((ret = nfp_probe_device(&nic_handle)) < 0) {
    fprintf(stderr, "failed to setup network device: %d\n", ret);
    return EXIT_FAILURE;
  }
  nsp_handle = nfp_nsp_open(nic_handle.cpp);
  if (!nsp_handle) {
    fprintf(stderr, "failed to setup network device: %d\n", EIO);
    return EXIT_FAILURE;
  }

  printf(" - Reset... \n");
  /* MAC re-initialization */
  if ((ret = nfp_nsp_mac_reinit(nsp_handle)) < 0) {
    fprintf(stderr, "failed to initialize MAC: %d\n", ret);
    nfp_nsp_close(nsp_handle);
    return EXIT_FAILURE;
  }
  /* Soft reset device */
  if ((ret = nfp_nsp_device_soft_reset(nsp_handle)) < 0) {
    fprintf(stderr, "failed to reset device: %d\n", ret);
    nfp_nsp_close(nsp_handle);
    return EXIT_FAILURE;
  }
#if 0
  /* Temporary Fix: Reset PCIe queues */
  if (nic_reset_pcie_queues(nic_handle.cpp) < 0) {
    fprintf(stderr, "failed to reset pcie queues: %d\n", ret);
    nfp_nsp_close(nsp_handle);
    return EXIT_FAILURE;
  }
#endif

  printf(" - Load... \n");
  if ((ret = nic_load_fw(nsp_handle)) < 0) {
    fprintf(stderr, "failed to load firmware: %d\n", ret);
    nfp_nsp_close(nsp_handle);
    return EXIT_FAILURE;
  }
  nfp_nsp_close(nsp_handle);

  printf(" - Init... \n");
  if ((ret = nic_configure_eth_if(nic_handle.cpp)) < 0) {
    fprintf(stderr, "failed to configure ethernet interface: %d\n", ret);
    return EXIT_FAILURE;
  }

  if ((ret = nic_mmap_symbols(nic_handle.cpp)) < 0) {
    fprintf(stderr, "failed to map fastpath symbols: %d\n", ret);
    return EXIT_FAILURE;
  }

  if ((ret = nic_init_config()) < 0) {
    fprintf(stderr, "failed to init config: %d\n", ret);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void nic_cleanup(void)
{
  fprintf(stderr, "%s: Not yet implemented!\n", __func__);
}
