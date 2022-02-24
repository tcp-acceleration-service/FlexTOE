/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef MSIX_H_
#define MSIX_H_

#include <nfp.h>

struct msix_tbl_entry_t {
  uint32_t addr_lo;
  uint32_t addr_hi;
  uint32_t msg_data;
  uint32_t msg_flags;
};

#define PCI_MSIX_TBL_ENTRY_SZ           sizeof(struct msix_tbl_entry_t)
#define PCI_MSIX_TBL_ENTRY_SZ32         (PCI_MSIX_TBL_ENTRY_SZ / 4)
#define PCI_MSIX_TBL_ENTRY_OFF(_x)      (PCI_MSIX_TBL_ENTRY_SZ * (_x))

/**
 * Init MSI-X interrupts.
 *
 */
__intrinsic void msix_init();

/**
 * Send MSI-X interrupt to an appctx
 *
 * @param db_id       APPCTX doorbell id
 */
__intrinsic void msix_appctx_intr(unsigned int db_id);

#endif /* MSIX_H_ */
