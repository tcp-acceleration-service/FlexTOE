/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/pcie.h>

#include "msix.h"

#include "global.h"
#include "debug.h"

/* Offset of the MSI-X table in PCIe SRAM */
#define MSIX_TABLE_OFF    0x0000    // NOTE: How is this decided?

/* NOTE: Upto 1024 MSI-X interrupts are supported. */
__shared __lmem struct msix_tbl_entry_t msix_table[FLEXNIC_PL_APPCTX_NUM];

/* Global variable to cache the current CPP2PCIe BAR */
__shared __gpr unsigned int msix_cur_c2p_addr;

PCIE_C2P_BAR_ALLOC(msix_notify_bar, me, 0, PCIE_CPP2PCIE_NOTIFY);

__intrinsic void msix_update_msix_table_entry(int idx)
{
  __xread struct msix_tbl_entry_t entry;
  unsigned int addr_lo, addr_hi;
  SIGNAL msix_sig;

  addr_hi = 0;  // PCIE Isl: 0
  addr_lo = NFP_PCIE_SRAM_BASE + MSIX_TABLE_OFF + PCI_MSIX_TBL_ENTRY_OFF(idx);

  __asm {
    pcie[read_pci, entry, addr_hi, <<8, addr_lo, \
        __ct_const_val(PCI_MSIX_TBL_ENTRY_SZ32)], ctx_swap[msix_sig];
  }

  msix_table[idx] = entry;
}

__intrinsic void msix_init()
{
  int idx;

  unsigned int bar_addr;

  /* Read the MSI-X table */
  for (idx = 0; idx < FLEXNIC_PL_APPCTX_NUM; idx++)
  {
    msix_update_msix_table_entry(idx);
  }

  /* Configure CPP2PCIe BAR for APPCTX 0 */
  bar_addr = pcie_c2p_barcfg_val(msix_table[0].addr_hi, msix_table[0].addr_lo, 0);
  pcie_c2p_barcfg_set_expl(0, PCIE_CPP2PCIE_NOTIFY, bar_addr);
  msix_cur_c2p_addr = bar_addr;
}

/* Do not run in multiple contexts! */
__intrinsic void msix_appctx_intr(unsigned int db_id)
{
  unsigned int bar_addr;
  struct msix_tbl_entry_t entry;
  __xwrite unsigned int data;

  entry = msix_table[db_id];
  bar_addr = pcie_c2p_barcfg_val(entry.addr_hi, entry.addr_lo, 0);

  if (bar_addr != msix_cur_c2p_addr) {
    pcie_c2p_barcfg_set_expl(0, PCIE_CPP2PCIE_NOTIFY, bar_addr);
    msix_cur_c2p_addr = bar_addr;
  }

  /* Generate interrupt */
  data = entry.msg_data;
  pcie_write(&data, 0, PCIE_CPP2PCIE_NOTIFY,
            entry.addr_hi, entry.addr_lo, sizeof(data));
}

