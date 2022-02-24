/*
 * Copyright (C) 2014-2015-2015,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          /lib/nfp/pcie.h
 * @brief         NFP PCIe interface
 */

#ifndef _NFP__PCIE_H_
#define _NFP__PCIE_H_

#include <nfp6000/nfp_pcie.h>

/*
 * Interface for the direct PCIe read/writes through a BAR
 */


/**
 * Calculate the CPP2PCIe bar value.
 * @param addr_hi   High 32bit of host DMA address
 * @param addr_lo   Low 32 bit of host DMA address
 * @param req_id    Requester ID to use
 *
 * This computes the value to program a CPP2PCIe BAR for the  host
 * address composed of @addr_hi and @addr_lo.
 *
 * Only the top 5 bits of addr_lo are used for the bar base.
 *
 * If @req_id is constant and 0, don't configure the Requester ID and
 * use the device default instead.
 */
__intrinsic unsigned int pcie_c2p_barcfg_val(unsigned int addr_hi,
                                             unsigned int addr_lo,
                                             unsigned int req_id);


/**
 * Set a CPP2PCIe BAR to an explicit value.
 * @param pcie_isl  PCIe Island to access
 * @param bar_idx   CPP2PCIe BAR to configure
 * @param bar_val   Value to program into the BAR
 *
 * This functions programs the CPP2PCIe BAR corresponding to @bar_idx
 * with @bar_val.  @bar_val should be obtained by calling
 * @pcie_c2p_barcfg_val().  It also performs a read-back of the CSR in
 * order to ensure that the write completed.
 */
__intrinsic void pcie_c2p_barcfg_set_expl(unsigned int pcie_isl,
                                          unsigned char bar_idx,
                                          unsigned int bar_val);


/**
 * Set a CPP2PCIe BAR.
 * @param pcie_isl  PCIe Island to access
 * @param bar_idx   CPP2PCIe BAR to configure
 * @param addr_hi   High 32bit of host DMA address
 * @param addr_lo   Low 32 bit of host DMA address
 * @param req_id    Requester ID to use
 *
 * This functions programs the CPP2PCIe BAR corresponding to @bar_idx
 * based on the host address composed of @addr_hi and @addr_lo. It
 * also performs a read-back of the CSR in order to ensure that the
 * write completed.
 *
 * Only the top 5 bits of addr_lo are used for the bar base.
 *
 * If @req_id is constant and 0, don't configure the Requester ID and
 * use the device default instead.
 */
__intrinsic void pcie_c2p_barcfg_set(unsigned int pcie_isl,
                                     unsigned char bar_idx,
                                     unsigned int addr_hi, unsigned int addr_lo,
                                     unsigned char req_id);

/**
 * Read data from the host through a CPP2PCIe BAR.
 * @param data      xfer registers to return the data in
 * @param pcie_isl  PCIe island (0-3) to address
 * @param bar_idx   CPP2PCIe BAR to use (expected to be configured)
 * @param addr_hi   Bits 35:32 of the host address to write to
 * @param addr_lo   Bottom 32 bits of the Host address to read from
 * @param size      Number of bytes to read
 *
 * Maximum size supported is 128B.
 */
__intrinsic void __pcie_read(__xread void *data, unsigned int pcie_isl,
                             unsigned char bar_idx, unsigned int addr_hi,
                             unsigned int addr_lo, size_t size,
                             size_t max_size, sync_t sync, SIGNAL *sig);

__intrinsic void pcie_read(__xread void *data, unsigned int pcie_isl,
                           unsigned char bar_idx, unsigned int addr_hi,
                           unsigned int addr_lo, size_t size);

/**
 * Write data to the host through a CPP2PCIe BAR.
 * @param data      xfer registers containing data to write to the host
 * @param pcie_isl  PCIe island (0-3) to address
 * @param bar_idx   Which CPP2PCIe BAR to use (expected to be configured)
 * @param addr_hi   Bits 35:32 of the host address to write to
 * @param addr_lo   Bottom 32bits of the host address to write to
 * @param size      Number of bytes to write
 *
 * Maximum size supported is 128B.
 */
__intrinsic void __pcie_write(__xwrite void *data, unsigned int pcie_isl,
                              unsigned char bar_idx, unsigned int addr_hi,
                              unsigned int addr_lo, size_t size,
                              size_t max_size, sync_t sync, SIGNAL *sig);

__intrinsic void pcie_write(__xwrite void *data, unsigned int pcie_isl,
                            unsigned char bar_idx, unsigned int addr_hi,
                            unsigned int addr_lo, size_t size);

/*
 * Interface to the DMA engines
 */
#define PCIE_DMA_MAX_SZ     4096

/**
 * Structure of a DMADescrConfig register
 *
 *  Refer to the data book for a description of the fields.
 */
struct pcie_dma_cfg_one {
    union {
        struct {
            unsigned int __reserved:3;
            unsigned int signal_only:1;
            unsigned int end_pad:2;
            unsigned int start_pad:2;
            unsigned int id_based_order:1;
            unsigned int relaxed_order:1;
            unsigned int no_snoop:1;
            unsigned int target_64:1;
            unsigned int cpp_target:4;
        };
        unsigned short __raw;
    };
};

/**
 * Configure a DMADescrConfig register.
 * @param pcie_isl          PCIe island (0-3) to address
 * @param index             DmaConfigRegIndex to configure
 * @param new_cfg           Configuration to apply
 *
 * This function accepts an index in the same format as the DMA descriptor,
 * and applies the given configuration to that index only.  A
 * read-modify-write is performed to ensure the other configuration in the
 * pair is unchanged.  It is the caller's responsibility to ensure that calls
 * for neighbors in a pair do not overlap.
 */
__intrinsic void pcie_dma_cfg_set_one(unsigned int pcie_isl,
                                      unsigned int index,
                                      struct pcie_dma_cfg_one new_cfg);

/**
 * Configure a pair of DMADescrConfig registers.
 * @param pcie_isl          PCIe island (0-3) to address
 * @param index             DmaConfigRegIndex to configure
 * @param new_cfg           Configuration to apply
 * @param sync              Type of synchronization
 * @param sig               Signal to use
 *
 * This function accepts an index in the same format as the DMA descriptor,
 * and applies the configuration to that index and it's pair. sig_done and
 * ctx_swap sync options are supported.
 */
__intrinsic void __pcie_dma_cfg_set_pair(unsigned int pcie_isl,
                                         unsigned int index, __xwrite struct
                                         nfp_pcie_dma_cfg *new_cfg,
                                         sync_t sync, SIGNAL *sig);

__intrinsic void pcie_dma_cfg_set_pair(unsigned int pcie_isl,
                                       unsigned int index, __xwrite struct
                                       nfp_pcie_dma_cfg *new_cfg);

/**
 * Populate the mode_sel and dma_mode fields for a DMA completion signal.
 * @param cmd           struct nfp_pcie_dma_cmd containing partial descriptor
 * @param meid          ME to receive signal (formatted as from __MEID)
 * @param ctx           Context to receive signal
 * @param sig_no        Signal number to return
 *
 * Either 'cmd' must be held in R/W accessible registers, preferably GPRs, or
 * this function must operate on compile time constants only.
 */
__intrinsic void pcie_dma_set_sig(void *cmd,
                                  unsigned int meid, unsigned int ctx,
                                  unsigned int sig_no);

/**
 * Populate the mode_sel and dma_mode fields for a DMA completion event.
 * @param cmd           struct nfp_pcie_dma_cmd containing partial descriptor
 * @param type          Event type
 * @param source        Event source
 *
 * Either 'cmd' must be held in R/W accessible registers, preferably GPRs, or
 * this function must operate on compile time constants only.
 */
__intrinsic void pcie_dma_set_event(void *cmd, unsigned int type,
                                    unsigned int source);

/**
 * Enqueue a DMA descriptor.
 * @param pcie_isl          PCIe island (0-3) to address
 * @param cmd               DMA command to send
 * @param queue             Queue to use, e.g. NFP_PCIE_DMA_TOPCI_HI
 */
__intrinsic void __pcie_dma_enq(unsigned int pcie_isl,
                                __xwrite struct nfp_pcie_dma_cmd *cmd,
                                unsigned int queue, sync_t sync, SIGNAL *sig);

__intrinsic void pcie_dma_enq(unsigned int pcie_isl,
                              __xwrite struct nfp_pcie_dma_cmd *cmd,
                              unsigned int queue);

__intrinsic void pcie_dma_enq_no_sig(unsigned int pcie_isl,
                                     __xwrite struct nfp_pcie_dma_cmd *cmd,
                                     unsigned int queue);


/**
 * Read DMA Qstatus
 * @param pcie_isl            PCIe island (0-3) to address
 * @param direction           0: TOPCI 1: FROMPCI
 * @param status0             DMA status word0
 * @param status1             DMA status word1
 */
__intrinsic void pcie_dmaq_status(unsigned int pcie_isl, int direction,
                                 __xread struct nfp_pcie_dma_qsts0* status0,
                                 __xread struct nfp_pcie_dma_qsts1* status1);

#endif /* _NFP__PCIE_H_ */
