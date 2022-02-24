/*
 * Copyright (C) 2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          me/lib/gro/gro_cli.h
 */

#ifndef __GRO_CLI_H
#define __GRO_CLI_H

#include <nfp.h>
#include "gro.h"

/* TODO: these can be loaded at run-time from the global config */
/* should they be?  It will come at the cost of some global state. */

#ifndef GRO_NUM_BLOCKS
#error "GRO_NUM_BLOCKS must be #defined"
#endif

#ifndef GRO_CTX_PER_BLOCK
#error "GRO_CTX_PER_BLOCK must be #defined"
#endif


#if ((GRO_CTX_PER_BLOCK & (GRO_CTX_PER_BLOCK - 1)) != 0)
#error GRO_CTX_PER_BLOCK must be a power of 2
#endif

#define GRO_CTX_BLOCK_MASK (GRO_CTX_PER_BLOCK-1)

#define GRO_TOTAL_CTX (GRO_NUM_BLOCKS * GRO_CTX_PER_BLOCK)


/**
 * gro_cli_init()
 *
 * Initialize local state for a GRO client.  Must be called once by
 * one context before any other context in the ME calls gro_cli_send().
 *
 */
__intrinsic void gro_cli_init(void);


/**
 * gro_cli_build_drop_seq_meta()
 *
 * Drop a sequence number from GRO.
 *
 * @param meta          The location to put the GRO metadata
 *
 */
__intrinsic void gro_cli_build_drop_seq_meta(__xwrite
                                             struct gro_meta_drop *meta);


/**
 * gro_cli_build_drop_ctm_buf_meta()
 *
 * Drop a sequence number and its associated CTM buffer.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island that owns the CTM buffer
 * @param pkt_num       The packet number of the CTM buffer
 */
__intrinsic void gro_cli_build_drop_ctm_buf_meta(__xwrite
                                                struct gro_meta_drop *meta,
                                                unsigned int isl,
                                                unsigned int pkt_num);



/**
 * gro_cli_build_drop_mu_buf_meta()
 *
 * Drop a sequence number and its associated MU buffer.
 *
 * @param meta          The location to put the GRO metadata
 */
__intrinsic void gro_cli_build_drop_mu_buf_meta(__xwrite
                                                struct gro_meta_drop *meta,
                                                unsigned int ring_addr_hi,
                                                unsigned int ring_num,
                                                unsigned int mu_handle);


/**
 * gro_cli_build_workq_meta1()
 *
 * Build metadata to send one word to an arbitrary work queue.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island of the EMU to send to
 * @param qnum          The queue number of the work queue
 * @param w0            The word to put on the work queue
 */
__intrinsic void gro_cli_build_workq_meta1(__xwrite struct gro_meta_memq *meta,
                                           unsigned int isl, unsigned int qnum,
                                           int w0);


/**
 * gro_cli_build_workq_meta2()
 *
 * Build metadata to send two words to an arbitrary work queue.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island of the EMU to send to
 * @param qnum          The queue number of the work queue
 * @param w0            The first word to put on the work queue
 * @param w1            The second word to put on the work queue
 */
__intrinsic void gro_cli_build_workq_meta2(__xwrite struct gro_meta_memq *meta,
                                           unsigned int isl, unsigned int qnum,
                                           int w0, int w1);


/**
 * gro_cli_build_workq_meta3()
 *
 * Build metadata to send three words to an arbitrary work queue.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island of the EMU to send to
 * @param qnum          The queue number of the work queue
 * @param w0            The first word to put on the work queue
 * @param w1            The second word to put on the work queue
 * @param w2            The third word to put on the work queue
 */
__intrinsic void gro_cli_build_workq_meta3(__xwrite struct gro_meta_memq *meta,
                                           unsigned int isl, unsigned int qnum,
                                           int w0, int w1, int w2);


/**
 * gro_cli_build_ring_meta1()
 *
 * Build metadata to send one word to an arbitrary ring.  Note that
 * GRO sends the ring data using mem[journal...] and no error checking/
 * flow control occurs.  So the caller must ensure that there is space
 * on the ring to receive the data.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island of the EMU to send to
 * @param qnum          The queue number of the ring
 * @param w0            The word to put on the ring
 */
__intrinsic void gro_cli_build_mem_ring_meta1(
                             __xwrite struct gro_meta_memq *meta,
                             unsigned int isl, unsigned int qnum, int w0);


/**
 * gro_cli_build_ring_meta2()
 *
 * Build metadata to send two words to an arbitrary ring.  Note that
 * GRO sends the ring data using mem[journal...] and no error checking/
 * flow control occurs.  So the caller must ensure that there is space
 * on the ring to receive the data.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island of the EMU to send to
 * @param qnum          The queue number of the ring
 * @param w0            The first word to put on the ring
 * @param w1            The second word to put on the ring
 * @param w2            The third word to put on the ring
 */
__intrinsic void gro_cli_build_mem_ring_meta2(
                             __xwrite struct gro_meta_memq *meta,
                             unsigned int isl, unsigned int qnum, int w0,
                             int w1);


/**
 * gro_cli_build_ring_meta3()
 *
 * Build metadata to send three words to an arbitrary ring.  Note that
 * GRO sends the ring data using mem[journal...] and no error checking/
 * flow control occurs.  So the caller must ensure that there is space
 * on the ring to receive the data.
 *
 * @param meta          The location to put the GRO metadata
 * @param isl           The island of the EMU to send to
 * @param qnum          The queue number of the ring
 * @param w0            The first word to put on the ring
 * @param w1            The second word to put on the ring
 * @param w2            The third word to put on the ring
 */
__intrinsic void gro_cli_build_mem_ring_meta3(
                             __xwrite struct gro_meta_memq *meta,
                             unsigned int isl, unsigned int qnum, int w0,
                             int w1, int w2);


/**
 * gro_cli_build_memq_meta()
 *
 * General routine to build GRO metadata for sending to a ring or work queue.
 * Note that when sending to an EMU ring, GRO sends the ring data using
 * mem[journal...] and no error checking/flow control occurs.  So the
 * caller must ensure that there is space on the ring to receive the data.
 *
 * @param meta          The location to store the GRO metadata
 * @param type          GRO_DTYPE_WORKQ or GRO_DTYPE_RING
 * @param isl           The island of the EMU with the ring or workq
 * @param nwords        The number of words to send (1, 2, or 3)
 * @param w0            Word 0 to send
 * @param w1            Word 1 to send (ignored if nwords < 2)
 * @param w2            Word 2 to send (ignored if nwords < 3)
 */
__intrinsic void gro_cli_build_memq_meta(__xwrite struct gro_meta_memq *meta,
                                         int type, unsigned int isl,
                                         unsigned int qnum, int nwords,
                                         int w0, int w1, int w2);


/**
 * gro_cli_build_nbi_meta()
 *
 * Generate GRO metadata for sending a packet to a TX queue on an NBI.
 *
 * @param meta          The location to store the GRO metadata
 * @param pkt_isl       The island of the CTM buffer holding the packet
 * @param ctm_sz        The size of the CTM buffer (2^(8+sz) bytes).
 * @param ms_off        The byte offset of the modifier script
 * @param eoff          The ending offset of the packet
 * @param nbi           The NBI to send the packet queue
 * @param txq           The transmit queue in the NBi to send the packet out
 *
 * Note that the 'eoff' parameter is a bit subtle due to the NFP6000
 * packet_processing_complete command.  It specifies the number of bytes
 * in the packet starting from the start of the CTM buffer.  It should equal
 * ms_off + Length_of_modifier_script + packet length.
 */
__intrinsic void gro_cli_build_nbi_meta(__xwrite struct gro_meta_nbi *meta,
                                        unsigned int pkt_isl,
                                        unsigned int pkt_num,
                                        unsigned int ctm_sz,
                                        unsigned int ms_off,
                                        unsigned int eoff,
                                        unsigned int nbi,
                                        unsigned int txq);


/**
 * gro_cli_nfd_desc2meta()
 *
 * Build a GRO descriptor to send to NFD based on an PCI.OUT descriptor.
 *
 * @param meta          The location to store the GRO metadata.
 * @param idescp        The input NFD PCI.OUT descriptor.
 * @param pci           The PCI unit to send to.
 * @param qid           The NFD queue to send to in "bitmask form".
 */
__intrinsic void gro_cli_nfd_desc2meta(__xwrite struct gro_meta_nfd3 *meta,
                                       void *idescp, unsigned int pci,
                                       unsigned int qid);



/**
 * gro_cli_send()
 *
 * Enqueue a packet/block of data to GRO to send.
 *
 * @param meta          The GRO metadata to send
 * @param ctx           The GRO reorder context
 * @param seq           The GRO sequence number
 */
__intrinsic void gro_cli_send(__xwrite void *meta, unsigned int ctx,
                              unsigned int seq);

#endif /* __GRO_CLI_H */
