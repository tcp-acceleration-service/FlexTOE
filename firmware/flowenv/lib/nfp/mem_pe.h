/*
 * Copyright (C) 2012-2018,  Netronome Systems, Inc.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          lib/nfp/mem_pe.h
 * @brief         NFP memory packet engine interface
 */
#ifndef _NFP__MEM_PE_H_
#define _NFP__MEM_PE_H_

#include <nfp.h>
#include <types.h>

/**
 * DMA from MU to CTM.
 * @param ctm_addr  32-bit pointer to the CTM start address (destination)
 * @param mem_addr  40-bit pointer to the MU start address (source)
 * @param size      Size of the DMA, must be a multiple of 64B and <= 2048B
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note You *must not* have more than 16 outstanding DMA commands per
 *       CTM. It is the caller's responsibility to enforce
 *       this. libpktdma provides safe functions which enforce this
 *       constraint.
 * @note @ctm_addr must be a real address not a packet mode address.
 */
__intrinsic void
__mem_pe_dma_mu_to_ctm(__ctm40 void *ctm_addr, __mem40 void *mem_addr,
                       size_t size, sync_t sync, SIGNAL *sig);

__intrinsic void
mem_pe_dma_mu_to_ctm(__ctm40 void *ctm_addr, __mem40 void *mem_addr,
                     size_t size);

/**
 * DMA from CTM to MU.
 * @param mem_addr  40-bit pointer to the MU start address (destination)
 * @param ctm_addr  32-bit pointer to the CTM start address (source)
 * @param size      Size of the DMA, must be a multiple of 64B and <= 2048B
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note You *must not* have more than 16 outstanding DMA commands per
 *       CTM. It is the caller's responsibility to enforce
 *       this. libpktdma provides safe functions which enforce this
 *       constraint.
 * @note @ctm_addr must be a real address not a packet mode address.
 */
__intrinsic void
__mem_pe_dma_ctm_to_mu(__mem40 void *mem_addr, __ctm40 void *ctm_addr,
                       size_t size, sync_t sync, SIGNAL *sig);

__intrinsic void
mem_pe_dma_ctm_to_mu(__mem40 void *mem_addr, __ctm40 void *ctm_addr,
                     size_t size);

/**
 * DMA CTM packet buffer to MU.
 * @param mem_addr  40-bit pointer to the MU start address (destination)
 * @param pnum      CTM packet number
 * @param offset    11-but offset within packet buffer to copy from
 * @param sync      Type of synchronisation (sig_done or ctx_swap)
 * @param sig       Signal to use
 *
 * @note You *must not* have more than 16 outstanding DMA commands per
 *       CTM. It is the caller's responsibility to enforce
 *       this. libpktdma provides safe functions which enforce this
 *       constraint.
 */
__intrinsic void
__mem_pe_dma_ctm_to_mu_packet(__mem40 void *mem_addr, unsigned int pnum,
                              unsigned int offset, sync_t sync, SIGNAL *sig);
__intrinsic void
mem_pe_dma_ctm_to_mu_packet(__mem40 void *mem_addr, unsigned int pnum,
                            unsigned int offset);
#endif /* !_NFP__MEM_BULK_H_ */
