/*
 * Copyright (C) 2014-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/pkt/pkt.h
 * @brief         Interface for ME packet manipulation routines
 */
#ifndef _PKT__PKT_H_
#define _PKT__PKT_H_

#include <nfp.h>
#include <stdint.h>
#include <types.h>


/*
 * Packet Reception and Transmission over Ethernet in the NFP6000
 *
 * The NFP 6000 series of chips has multiple subsystems for packet reception
 * and transmission over Ethernet:
 *  - the MAC block that manages the physical interconnection and perform
 *    timestamping and checksum offload
 *  - the NBI preclassifier that performs basic packet parsing and metadata
 *    construction
 *  - the NBI DMA unit which load balances incoming packets to CTM islands
 *    and transfers the packet to CTM and MU buffers
 *  - the CTM packet engine (PE) that manages the CTM packet buffers
 *  - the NBI traffic manager (TM) which performs packet reordering, shaping
 *    and modification
 *  - the MEs that perform the forwarding function plus any intelligent
 *    processing.
 * There are also various interdependencies between these systems all of
 * which make basic packet operations rather complex.  The functions in
 * this library attempt to abstract away most of the details of these
 * operations from the perspective of the ME programmer.  What follows is
 * a high level description of the packet recieve and transmit processes.
 *
 *
 * Packet reception proceeds roughly as follows.  First, the MAC block
 * pulls a packet in from a physical interface and transfers it over
 * a "minipacket bus" to internal memory in the NBI preclassifier block.
 * During this process it may prepend timestamp and checksum validation
 * information to the beginning of the packet.  Next, the NBI preclassifier
 * performs (very) basic packet classification and forwarding and constructs
 * 16 bytes of packet metadata for the incoming packet.  It also selects
 * a group of CTM islands and a pair of buffer lists for the packet.
 *
 * The NBI DMA unit then selects a CTM island to send the packet to and an
 * MU buffer for the packet based on the NBI preclassifier output.  It then
 * requests a CTM buffer for the packet from the selected island's CTM PE.
 * The CTM island provides the NBI with the CTM buffer to hold the front of
 * the packet.  The NBI DMA block then transfers the front of the packet to
 * the CTM buffer and the remainder of the packet to the MU buffer.  It also
 * transfers 8 bytes of fixed buffer metadata along with the 16 bytes of
 * preclassifier metadata to the beginning of the CTM buffer.  Finally, it
 * notifies the CTM packet engine that the packet is ready.
 *
 * During this time, MEs can signal the CTM packet engine that they are
 * ready to receive a packet.  If there are no packets available for
 * reception, the CTM enqueues the ME thread information in its internal
 * work queue.  When the NBI TM delivers a packet to the CTM PE, if there
 * are threads on the work queue, the CTM PE dequeues the first and delivers
 * the packet to it.  If the work queue contains no ME thread requests then
 * the CTM PE instead enqueues the packet onto the work queue so that when
 * an ME requests a packet, it will be able to deliver it to the ME
 * immediately.
 *
 *
 * It is worth understanding the relationship between CTM and MU buffers for
 * holding packets.  Each CTM buffer must be 256, 512, 1K or 2K bytes in
 * length.  The NBI DMA configuration determines the maximum size from among
 * those possible sizes.  The CTM buffer contains packet metadata followed
 * by a padding gap followed by beginning of the packet data itself as
 * delivered by the MAC.  The MU buffer (which must be 2K aligned) contains
 * the remainder of the packet data.  However, this remaining packet data
 * will not start until 'S' bytes into the MU buffer where 'S' is the size
 * of the CTM buffer.  If the packet can fit entirely in the CTM buffer
 * (when factoring in the metadata and starting packet headeroom) then the
 * packet will still have an associated MU buffer, but none of the packet
 * data will reside within it.  The figure below shows the general layout of
 * the NFP 6000 packet buffers.
 *
 * CTM buffer
 *      8 or 40 bytes                  CTM_SIZE =
 *             |   0-12 bytes of MAC   256/512/1024/2048 bytes
 *    24 bytes |   prepend data        /
 *       |     |     /                /
 *    +----+-------+---+-------------+
 *    |meta|padding|MPP|packet ...   |
 *    +----+-------+---+-------------+
 *                 |
 *               32/64
 * MU buffer
 *    +------------------------------+----------------------------------+
 *    | CTM_SIZE uninitialized bytes | ... packet data ....             |
 *    +------------------------------+----------------------------------+
 *    |                              |                                  |
 * Must be 2K aligned        256/512/1024/2048                       16K max
 *                          depending on CTM_SIZE
 *
 *
 * Packet transmission is also a multistep process.  The ME must first
 * determine where the packet to transmit begins in the CTM buffer and how
 * long it is.  (This, of course may be different from the starting offset
 * and length delivered to the ME on packet reception.)  Next, the ME must
 * determine if it wants the egress MAC block to perform any TCP or UDP
 * checksum updates.  If so, it must prepend the packet with a special
 * command for the MAC block.  The ME must next prepend a "modification
 * script" to the front of the packet to inform the TM of how to modify the
 * packet before transmission.  The ME must do this even if no modifications
 * are required.  The ME must then select a sequence number and TM sequencer
 * for transmission.  Packets will leave the TM block roughly in order of
 * sequence number among the other packets assigned to the same sequencer.
 * However, the TMs reorder block operates heuristically and may send the
 * packets out of order.  The ME must also select the NBI and the transmit
 * queue on that NBI to send the packet out.  Each transmit queue
 * corresponds (ultimately) to a single physical port, but multiple transmit
 * queues may map to the same port.  The transmit queue determines the
 * traffic shaping applied to the packet during transmisison.  Finally, the
 * ME issues a "packet processing complete" command to the CTM PE to send
 * the packet.
 *
 * The CTM PE receives the PPC comand and issues its own "packet ready"
 * command to the NBI TM to send the packet.  The CTM PE builds much of the
 * information in this command from the information that the ME supplied.
 * The NBI TM then enqueues the packet to its reorder subsystem while
 * simultaneously reserving space for the packet in the requested transmit
 * queue.  If the transmit queue is full, the TM may drop the packet or
 * request the CTM PE to retry sending the packet later.  Either way, the ME
 * is no longer involved with this packet.
 *
 * The TM's next step is to await release from the reorder subsystem.  The
 * TM does this according to the sequence number and sequencer that the ME
 * supplied with the original "packet processing complete command".  After
 * release from reorder, the TM may, hold onto the packet further according
 * to scheduling parameters configured for the given TX queue.  Finally,
 * when the packet is ready for transmission, the TM copies the packet into
 * internal TM memory from the CTM and MU buffers and notifies the CTM PE
 * that the CTM buffer is free.  It also enqueues the MU buffer onto an
 * internal queue of free MU buffer handles.
 *
 * Once the packet resides completely in NBI TM internal memory, the TM
 * passes the packet to the packet modifier subsystem which then sends it
 * onto the egress MAC block.  The packet modifier reads and strips the
 * modification script prepended to the packet and performs the requested
 * modifications.  The packet then goes to the egress MAC block which may
 * perform TCP checksum updates if the ME prepended the packet with a
 * command to do so.  Finally, the egress MAC block will transmit the packet
 * onto the physicial media.
 *
 * In order to recycle MU buffers back into use by ingress processing,
 * software must include a special purpose ME that drains the egress MU
 * buffer handle queues and re-enqueues the buffer handles to the NBI DMA
 * engines.  This "buffer list manager" software might alternately make
 * the MU buffers available for other types of packet operations such as
 * PCIe reception, packet copying, etc...
 */

#if defined(__NFP_LANG_MICROC)

/**
 * Structure describing the invariant part of the packet metadata that
 * the NBI/CTM Packet Engine delivers to the ME.  The NBI traffic manager
 * also requires this data at the beginning of the CTM buffer in the
 * same format for packet transmission.  However, the TM ignores the
 * Packet Length field and uses an encoded address (see below) to determine
 * the the transmission length.
 */
struct nbi_meta_pkt_info {
    union {
        struct {
            unsigned int isl:6;    /**< Island of the CTM of the packet */
            unsigned int pnum:10;  /**< Packet number of the packet */
            unsigned int bls:2;    /**< Buffer list of the MU buffer */
            unsigned int len:14;   /**< Length of the packet in bytes
                                        (includes possible MAC prepend bytes) */

            unsigned int split:1;  /**< Set if pkt split between CTM/MU */
            unsigned int resv0:2;  /**< Reserved */
            unsigned int muptr:29; /**< Pointer to the MU buffer >>11 */
        };
        uint32_t __raw[2];
    };
};


/**
 * Structure describing the NBI/CTM PE metadata for a packet delivered by
 * the "null" picoengine load.  This load immediately sends the packet
 * without inspection.  For IPv4 traffic, this causes the sequencer and
 * sequence number to be loaded as shown below.  For IPv6 and non-IP
 * packets this is not as well defined.  Basically, only use the
 * "null" load for basic first try testing.
 *
 * Packet Metadata Format from the NBI
 *
 * Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 * -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * Word  +-----------+-------------------+---+---------------------------+
 *    0  | CTM Island|   Packet Number   |Bls|      Packet Length        |
 *       +-+---+-----+-------------------+---+---------------------------+
 *    1  |S|Rsv|                   MU Pointer                            |
 *       +-+---+---------------------------------------------------------+
 *    2  |                      Preclass Meta 0                          |
 *       +---------------------------------------------------------------+
 *    3  |                      Preclass Meta 1                          |
 *       +---------------------------------------------------------------+
 *    4  |                      Preclass Meta 2                          |
 *       +---------------------------------------------------------------+
 *    5  |                      Preclass Meta 3                          |
 *       +---------------------------------------------------------------+
 *
 *      S -> 1 if packet is split between CTM and MU data
 *      Bls -> Buffer List
 */
struct nbi_meta_null {
    union {
        struct {
            struct nbi_meta_pkt_info pkt_info;
            unsigned int resv1:6;       /**< Reserved */
            unsigned int seq:10;        /**< Packet sequence number */
            unsigned int resv2:3;       /**< Reserved */
            unsigned int seqr:5;        /**< Packet sequencer */
            unsigned int resv3:8;       /**< Reserved */
            unsigned int preclass[3];   /**< Unknown preclassifier metadata */
        };
        uint32_t __raw[6];
    };
};

/**
* Structure describing the NBI/CTM PE metadata for a packet delivered by
* the "catamaran" picoengine load.  This load provides packet
* pre-classification metadata and should be the NBI load used for flow
* processing of packets.
* See EDD-Catamaran 1.0 NPFW for details. PS5 build 1908 and later.
*
* Packet Metadata Format Catamaran
* Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
* -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
* Word  +---------------------------------------------------------------+
*    0  | CTM Island| Packet Number     |Bls|     Packet Length         |
*       +---------------------------------------------------------------+
*    1  |S|Rsv|                   MU Pointer                            |
*       +---------------------------------------------------------------+
*    2  |      Sequence Number          |NFP|  R  | Seq |P|Mtype|V| BP  |
*       +---------------------------------------------------------------+
*    3  |    Reserved             |E|TLD| OL4   | OL3  |  OL2     | R   |
*       +---------------------------------------------------------------+
*    4  |    Port        |     HP-Off1  |     HP-Off0    |    Misc      |
*       +---------------------------------------------------------------+
*    5  |P|I|W|S| VC |I|T|      O-Off   |     LIF+Mode / Match          |
*       +---------------------------------------------------------------+
*/
struct nbi_meta_catamaran {
   union {
       struct {
           struct nbi_meta_pkt_info pkt_info;
           unsigned int seq:16;               /**< Packet sequence number */
           unsigned int nfp:2;                /**< Packet NFP number */
           unsigned int __reserved_12:3;      /**< Reserved */
           unsigned int seqr:3;               /**< Packet sequencer */
           unsigned int prot_err:1;           /**< Packet protocol error */
           unsigned int meta_type:3;          /**< Packet meta type nbi_id */
           unsigned int meta_valid:1;         /**< Packet meta valid */
           unsigned int buffer_pool:3;        /**< Buffer pool */

           unsigned int __reserved_19:13;     /**< Reserved */
           unsigned int e_term:1;             /**< Early termination of pkt */
           unsigned int tunnel_depth:2;       /**< Packet tunnel layer depth */
           unsigned int outer_l4_prot_type:4; /**< Packet outer L4 protocol */
           unsigned int outer_l3_prot_type:4; /**< Packet outer L3 protocol */
           unsigned int outer_l2_prot_type:5; /**< Packet outer L2 protocol */
           unsigned int __reserved_1:3;       /**< Reserved */

           unsigned int port:8;               /**< Port number */
           unsigned int hp_off1:8;            /**< Packet header parse offset
                                                * 1 Start of L4 payload
                                                * relative to PKT_NBI_OFFSET
                                                * */
           unsigned int hp_off0:8;            /**< Packet header parse offset
                                                * 2 Start of L4 relative to
                                                * PKT_NBI_OFFSET */
           unsigned int misc:8;               /**< Miscellaneous packet flags */

           unsigned int pe:1;                 /**< Packet error */
           unsigned int ie:1;                 /**< Interface error */
           unsigned int pw:1;                 /**< Packet warning */
           unsigned int sp:1;                 /**< Special handling indicator */
           unsigned int vlan_cnt:2;           /**< VLAN tag count */
           unsigned int it:1;                 /**< TCAM tag indicator */
           unsigned int ts:1;                 /**< Table set indicator */
           unsigned int outer_off:8;          /**< Packet outer offset */
           unsigned int lif:16;               /**< Logical interface id */
       };
       uint32_t __raw[6];
   };
};


/* TODO: Add "wire" and other default "comes-with-NFP" NBI loads */


/**
 * Sending or freeing a packet is an involved process in the NFP 6xxx.
 * There are numerous fields that one must override in the DSF CPP
 * command, some in unconventional ways.  This requires using both the
 * previous ALU instruction and the CmdIndirectRev0 CSR in the ME to
 * build the command.  It also requires building the address of the
 * packet in an unusual way.
 *
 * References:
 *  - NFP 6xxx PRM Section 2.1.6.4.6 "NFP-6xxx (Normal) Indirect
 *    Reference Formats"
 *  - NFP 6xxx DB Section 9.2.2.7.9 "Packet Processing Complete Target
 *    Command and Packet Ready Master Command"
 *
 * Users of this API need not worry about these details.  The functions
 * provided handle all of these steps.  The pkt_iref_* functions and
 * the TM_MS_* constants and macros below are thus provided for the
 * user who wants to write their own packet transmit operations.
 */


/**
 * The bitfields for the CmdIndirectRef0 CSR when sending/freeing CTM
 * packets.  This structure defines the fields as the CTM packet engine
 * sees them instead of how the ME and the DSF CPP bus sees the fields.
 * For example, the sequencer number field goes in the CPP bytemask
 * field.  The sequence number overlaps the signal master, data master
 * and data master island fields.  The reserved fields should be left
 * as zero.
 */
struct pkt_iref_csr0 {
    union {
        struct {
            unsigned int resv0:2;       /**< Reserved, set to 0 */
            unsigned int seq:14;        /**< NBI TM sequence number */
            unsigned int resv1:8;       /**< Reserved, set to 0 */
            unsigned int seqr:8;        /**< NBI TM sequencer number */
        };
        uint32_t __raw;
    };
};


/**
 * Packet Ready address fields for directly sending the Packet Ready to the NBI
 *  must overide:
 *  - Total length                              (len)
 *  - Packet number                             (pnum << 16)
 *  - Data master (based on ME ID)              ((meid & 0x1 | 0x2) << 28)
 *  - Data master island (island ID)            (iid << 32)
 *  - NBI ID                                    (nbi << 38)
 *
 * Packet Ready address upper fields for directly sending the Packet Ready to
 * the NBI is shifted before being sent:
 * - Packet Ready address left shift amount    8
 */
#define PKT_READY_ADDR_LO_PNUM_shf        16

#define PKT_READY_ADDR_HI_LSHIFT          8
#define PKT_READY_ADDR_HI_DMASTER_shf     (28 - PKT_READY_ADDR_HI_LSHIFT)
#define PKT_READY_ADDR_HI_DMASTER_ISL_shf (32 - PKT_READY_ADDR_HI_LSHIFT)
#define PKT_READY_ADDR_HI_NBI_shf         (38 - PKT_READY_ADDR_HI_LSHIFT)

#define PKT_READY_ADDR_LO_FIELDS(_pnum, _len)      \
    ((_pnum << PKT_READY_ADDR_LO_PNUM_shf) | (_len))

#define PKT_READY_ADDR_HI_DMASTER(_meid)  ((_meid & 0x1) | 0x2)
#define PKT_READY_ADDR_HI_FIELDS(_nbi, _iid, _meid)                         \
    ((_nbi << PKT_READY_ADDR_HI_NBI_shf)                                    \
     | (_iid << PKT_READY_ADDR_HI_DMASTER_ISL_shf)                          \
     | (PKT_READY_ADDR_HI_DMASTER(_meid) << PKT_READY_ADDR_HI_DMASTER_shf))

/**
 * Bitfield format of the previous ALU instruction result for the
 * indirect reference required to send or drop a CTM packet.  This
 * structure defines the fields as the CTM packet engine or NBI TM
 * understands them rather than how the ME and DSF CPP bus sees them.
 */
struct pkt_iref_palu {
    union {
        struct {
            unsigned int resv0:2;       /**< Reserved: set to 0 */
            unsigned int nbi:2;         /**< NBI to send the packet to */
            unsigned int resv1:2;       /**< Never set retry bit */
            unsigned int txq:10;        /**< TX queue to send the packet to */
            unsigned int resv2:3;       /**< No override sig num or sig ctx */
            unsigned int ms_off:5;      /**< Encoded mod script offset */
            unsigned int magic:8;       /**< PKT_IREF_PALU_MAGIC: see below */
        };
        uint32_t __raw;
    };
};


/**
 * Previous ALU for Packet Processing Complete must override:
 * - Signal master from CSR0 (seq number)  (1 << 0)
 * - Data master from CSR0 (seq number)    (1 << 1)
 *   Data master island from CSR0 (seq number)
 * - Data reference (from prev alu DATA16, for TXQ and NBI) (1 << 3)
 * - Bytemask from CSR0 (TM sequencer)  (1 << 6)
 * - Length (from prev ALU length, for offset of rewrite script) (1 << 7)
 */
#define PKT_IREF_PALU_MAGIC     0xcb


/**
 * Holds information pertaining to the addition of a modification script to
 * the beginning of a CTM buffer.
 */
struct pkt_ms_info {
    uint16_t off_enc;   /**< encoded MS offset */
    uint16_t len_adj;   /**< # of bytes to add to the length on PPC */
};


/**
 * NBI Packet Modify Modification Script format
 *
 * In order to send a packet in the NFP 6xxx A0/A1 chip, one
 * must prepend the packet with a "modification script" for the NBI
 * TM packet modifier.  One must do this even if one does not want
 * any modifications to the packet.
 *
 * The script must be placed at an 8-byte aligned location in
 * CTM memory and must be a multiple of 8 bytes in length.  The
 * packet must also have the first 8 bytes of CTM memory containing
 * crucial TM metadata.  So, the script can not go at offset 0.
 *
 * It is also important to note that TM calculates the starting
 * offset of the packet data by adding the starting offset of
 * the modification script to the length of the modification script.
 * This implies that a packet can only start at an 8-byte aligned
 * address, by default.
 *
 * The way to get around this is to create an 8-byte modification
 * script starting at the nearest 8-byte address boundary 8 or more
 * bytes below the start of the packet that deletes the intervening
 * bytes between the modification script and the start of the packet.
 * It is extremely annoying and a waste of time and I/O bandwidth all
 * to "do nothing".
 *
 * NFP 6xxx B0 and later chips will allow NOT using a modification
 * script as well as arbitrary byte offsets for the start of packet
 * when the modification script is disabled.  (The packet must still
 * start at start at byte 8 or later in the CTM buffer).
 *
 * See:
 *  - NFP 6xxx Databook Section 7.2.4.1 "Modification Script"
 */

#define NBI_PM_TYPE_shf      31
#define NBI_PM_TYPE_DIRECT   1
#define NBI_PM_TYPE_INDIRECT 0
#define NBI_PM_TYPE(_x)      (_x << NBI_PM_TYPE_shf)

#define NBI_PM_OPCODE_shf    16

/*
 *  ModInstructOp opcode
 *   * See:
 *  - NFP 6xxx Databook Section 7.3.4.2.2 "NbiPmOpcode32Cnfg Register"
 */
/* NBI packet modifier modification script instruction values */
enum nbi_pkt_ms_instruct {
    NBI_PKT_MS_INSTRUCT_DELETE,
    NBI_PKT_MS_INSTRUCT_INSERT,
    NBI_PKT_MS_INSTRUCT_REPLACE,
    NBI_PKT_MS_INSTRUCT_INCREMENT,
    NBI_PKT_MS_INSTRUCT_DECREMENT,
    NBI_PKT_MS_INSTRUCT_MASK_REPLACE,
    NBI_PKT_MS_INSTRUCT_SHORT_PAD,
    NBI_PKT_MS_INSTRUCT_NOOP
};

/**
 * Build a NBI packet modify modification script opcode from parameters.
 * @param _instruct  3-bit value defining the modification instruction:
 *                       See enum nbi_pkt_ms_instruct.
 * @param _bytes     4-bit value defining the number of bytes to be modified:
 *                       0x0: modify 1 byte / pad 60 bytes
 *                       0x1: modify 2 bytes / pad 62 bytes
 *                       ...              ....
 *                       0xe: modify 15 bytes / pad 88 bytes
 *                       0xf: modify 16 bytes / reserved
 * @param _rd_loc     1-bit indicating where to get the replacement data from
 *                    (when 'rdata'=b'10'):
 *                       0x0: from script        0x1: from config ram
 */
#define NBI_PM_OPCODE(_instruct, _bytes, _rd_loc) \
              (((_bytes) > 0 ? \
              (((_instruct & 0x7) << 5) | (((_bytes - 1) & 0xF) << 1) | \
              (_rd_loc & 1)) : \
              (((_instruct & 0x7) << 5) | (_rd_loc & 1))) << \
              NBI_PM_OPCODE_shf)

/* 2-bit encoded various CTM buffer sizes */
enum PKT_CTM_SIZE {
    PKT_CTM_SIZE_256,
    PKT_CTM_SIZE_512,
    PKT_CTM_SIZE_1024,
    PKT_CTM_SIZE_2048,
};

/**
 * Packets and Buffers credits structure.
 * Used for CTM packets and buffers credits management.
 */
struct ctm_pkt_credits {
    unsigned int pkts;
    unsigned int bufs;
};

/* CTM credit allocation fail sleep */
#ifndef PKT_CTM_CRED_ALLOC_FAIL_SLEEP
#define PKT_CTM_CRED_ALLOC_FAIL_SLEEP 1000
#endif

/**
 * Packet Engine response for packet_alloc commands.
 */
struct pe_pkt_alloc_res {
    union {
        struct {
            unsigned int resv:2;        /**< Reserved */
            unsigned int pnum:10;       /**< Packet number */
            unsigned int pkt_credit:11; /**< Packet credits */
            unsigned int buf_credit:9;  /**< Buffer credits */
        };
        unsigned int __raw;
    };
};

/**
 * Packet Engine response for packet_credit_get command.
 */
struct pe_credit_get_res {
    union {
        struct {
            unsigned int resv:12;       /**< Reserved */
            unsigned int pkt_credit:11; /**< Packet Credit */
            unsigned int buf_credit:9;  /**< Buffer Credit */
        };
        unsigned int __raw;
    };
};


/**
 * Receive a packet from a the local island's CTM workq receiving NBI
 * metadata and/or MAC prepend data in the same push.  The NBI metadata
 * goes in the first 6 words of the 'meta' parameter while the MAC
 * prepend and/or header data goes in the subsequent words.  The user
 * is not required to supply transfer-register space to this call
 * beyond the first 6 words.
 *
 * @param meta  Pointer to the location to store the packet metadata
 *              followed by (optional) space for prepend/packet header data
 * @param msize The size of the metadata/prepend/header buffer (in bytes)
 * @param off   The offset in bytes to receive data from in the CTM buffer;
 *              this is the packet offset from which to receive data after
 *              the first 24 bytes of metadata (must be a multiple of 4)
 * @param sync  The type of synchronization (sig_done or ctx_swap)
 * @param sig   The signal to use
 */
__intrinsic void __pkt_nbi_recv_with_hdrs(__xread void *meta, size_t msize,
                                          uint32_t off, sync_t sync,
                                          SIGNAL *sig);

__intrinsic void pkt_nbi_recv_with_hdrs(__xread void *meta, size_t msize,
                                        uint32_t off);

/**
 * Receive a packet from a the local island's CTM workq.
 *
 * @param meta  Pointer to the location to store the packet metadata
 * @param msize The size of the metadata buffer (in bytes)
 * @param sync  The type of synchronization (sig_done or ctx_swap)
 * @param sig   The signal to use
 */
__intrinsic void __pkt_nbi_recv(__xread void *meta, size_t msize, sync_t sync,
                                SIGNAL *sig);

__intrinsic void pkt_nbi_recv(__xread void *meta, size_t msize);


/**
 * Build a 40-bit pointer to the start of a CTM packet from the CTM
 * island and packet number.
 *
 * @param isl   The CTM buffer island
 * @param pnum  The CTM packet number
 * @param off   The offset within the CTM buffer
 */
__intrinsic __mem40 void *pkt_ctm_ptr40(unsigned char isl, unsigned int pnum,
                                         unsigned int off);

/**
 * Build a 32-bit pointer to the start of a CTM packet buffer from the CTM
 * island and packet number.  Can only refer to a packet in the current
 * island.
 *
 * @param pnum  The CTM packet number
 * @param off   The offset within the CTM buffer
 */
__intrinsic __mem32 void *pkt_ctm_ptr32(unsigned int pnum, unsigned int off);


/**
 * Packet engine packet wait/read status response structure
 */
typedef union pkt_status_t {
    struct {
        unsigned int error:1;             /**< Error. */
        unsigned int last_seg_rcvd:1;     /**< Last segment received. */
        unsigned int first_seg_rcvd:1;    /**< First segment received. */
        unsigned int sent_to_me:1;        /**< Packet sent to ME. */
        unsigned int not_valid:1;         /**< Packet not valid, error. */
        unsigned int owned_by_me:1;       /**< Packet owned by
                                           * 0 = packet owned by NBI
                                           * 1 = packet owned by ME. */
        unsigned int owner:2;             /**< Packet owner.
                                           * 0 = owned by ME
                                           * 1 = owned by NBI-0
                                           * 2 = owned by NBI-1
                                           * 3 = owned by ME. */
        unsigned int resv_0:6;            /**< Reserved. */
        unsigned int size:2;              /**< Packet size. */
        unsigned int resv_1:6;            /**< Reserved. */
        unsigned int ctm_addr_div256:10;  /**< CTM DCACHE address.
                                           * Multiply by 256 to get
                                           * actual CTM address. */
    };
    unsigned int __raw;
} pkt_status_t;

#define MAX_PKT_NUM_mask   0x1ff  /* max bits for packet number is 9 but in
                                   * some cases documentation refers to 10 */
#define MAX_PKT_NUM_of(_x) (_x & MAX_PKT_NUM_mask)


/**
 * Extract the checksum-prepend word from a buffer.
 * @param src_buf  Input buffer
 * @param off      Byte offset within the buffer where the Ethernet
 *                 header starts
 * @return         Offset to the next protocol header
 *
 * The NFP-6000 MAC can be configured to prepend a word to a packet
 * contain the results of checksum checks.  This function extracts
 * this words from the front of the buffer. This function expects @off
 * to be a compile time constant (which should be fine given that the
 * prepend word is at the start of the buffer).
 */
__intrinsic unsigned int pkt_csum_read(void *src_buf, int off);

/**
 * Get the status response of a CTM packet buffer.
 * @param isl         CTM buffer island
 * @param pnum        CTM packet number
 * @param pkt_status  Packet status response
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 */
__intrinsic void __pkt_status_read(unsigned char isl, unsigned int pnum,
                                   __xread pkt_status_t *pkt_status,
                                   sync_t sync, SIGNAL *sig_ptr);

__intrinsic void pkt_status_read(unsigned char isl, unsigned int pnum,
                                 __xread pkt_status_t *pkt_status);


/**
 * Get the number of packet bytes sitting in the CTM packet buffer.
 * @param len            Total CTM+EMU packet data bytes
 * @param offset         Start-of-packet offset in CTM buffer
 * @param ctm_buf_size   Encoded CTM buffer size
 *                       [0 = 256b, 1 = 512b, 2 = 1024b or 3 = 2048b]
 */
__intrinsic size_t pkt_ctm_data_size(unsigned int pkt_len,
                                     unsigned int pkt_offset,
                                     enum PKT_CTM_SIZE ctm_buf_size);

/**
 * Get the number of packets bytes sitting in the EMU packet buffer.
 * @param len            Total CTM+EMU packet data bytes
 * @param offset         Start-of-packet offset in CTM buffer
 * @param ctm_buf_size   Encoded CTM buffer size
 *                       [0 = 256b, 1 = 512b, 2 = 1024b or 3 = 2048b]
 */
__intrinsic size_t pkt_emem_data_size(unsigned int pkt_len,
                                      unsigned int pkt_offset,
                                      enum PKT_CTM_SIZE ctm_buf_size);


/*
 * MAC Egress L3 and/or L4 Checksum Insertion Functionality
 *
 * In order to take advantage of the L3 and/or L4 checksum insertion
 * functionality in the NFP 6xxx MAC blocks, one must prepend the packet
 * with a 4-byte MAC egress command for the MAC.  One must do this if
 * the MAC egress command functionality is enabled for the egress port,
 * regardless of whether any L3 or L4 checksum insertion is desired for
 * the given packet.  This 4-byte MAC egress command would then be
 * stripped off by the MAC prior to being transmitted out the port.
 *
 * It is important to note that the NBI TM and PM treat this MAC egress
 * command word as part of the packet data.  Therefore, the starting
 * offset of the packet data becomes the offset of the MAC egress
 * command, if present.
 *
 * If the MAC egress command functionality is NOT enabled for the given
 * egress port, a MAC egress command should NOT be prepended to the
 * packet.
 *
 * See:
 *  - NFP 6xxx Databook Section 10.2.9.3 "Egress Command Prepend"
 */

/**
 * Write a MAC egress command to the beginning of a packet.
 * @param pbuf         Pointer to the start of the packet buffer
 * @param off          Starting offset of the packet data
 * @param l3_csum_ins  Enables insertion of L3 checksum
 * @param l4_csum_ins  Enables insertion of L4 checksum
 * @param xcmd         Transfer registers to hold the command
 * @param sync         Type of synchronization (sig_done or ctx_swap)
 * @param sig          Signal to use.
 *
 * @note 'off' must be set to 20 or higher.
 * @note After calling this function, the 4-byte MAC egress command is
 *       prepended to the packet data; therefore, when calling the packet
 *       modifier modification script write function, the starting offset of
 *       the packet data becomes 'off' - 4, in order to include the MAC egress
 *       command word as part of the packet data.
 */
__intrinsic void __pkt_mac_egress_cmd_write(
    __mem40 void *pbuf, unsigned char off, int l3_csum_ins, int l4_csum_ins,
    __xwrite uint32_t *xcmd, sync_t sync, SIGNAL *sig);

__intrinsic void pkt_mac_egress_cmd_write(__mem40 void *pbuf,
                                          unsigned char off, int l3_csum_ins,
                                          int l4_csum_ins);


__intrinsic struct pkt_ms_info pkt_gen_msi_info(unsigned char off);

/**
 * Write a direct packet modifier modification script to the beginning of a
 * packet and return the metadata required to send the packet.
 * @param pbuf  Pointer to the start of the packet buffer
 * @param off   Starting offset of the packet data
 * @param xms   Transfer registers to hold the script
 * @param size  Size of the xms.
 * @param sync  Type of synchronization (sig_done or ctx_swap)
 * @param sig   Signal to use.
 *
 * The pointer must point to the first byte of the packet to go over
 * the wire (or the MAC egress command that may precede it).  There
 * must be at least 16 bytes between this pointer and the beginning of
 * the CTM buffer.
 */
__intrinsic struct pkt_ms_info __pkt_msd_write(__mem40 void *pbuf,
                                               unsigned char off,
                                               __xwrite uint32_t xms[2],
                                               size_t size, sync_t sync,
                                               SIGNAL *sig);

__intrinsic struct pkt_ms_info pkt_msd_write(__mem40 void *pbuf,
                                             unsigned char off);


/**
 * Send a packet to an NBI port. Notifies sequencer and frees the packet.
 * @param isl           Island of the CTM packet
 * @param pnum          Packet number of the CTM packet
 * @param msi           Modification script info required for transmission
 * @param len           Length of the packet from the start of the packet data
 *                      (which immediately follows the rewrite script
 *                      plus padding)
 * @param nbi           NBI TM to send the packet to
 * @param txq           NBI TM TX queue to send the packet to
 * @param seqr          NBI TM sequencer to send the packet to
 * @param seq           NBI TM sequence number of the packet
 * @param ctm_buf_size  Encoded CTM buffer size
 */
__intrinsic void pkt_nbi_send(unsigned char isl, unsigned int pnum,
                              __gpr const struct pkt_ms_info *msi,
                              unsigned int len, unsigned int nbi,
                              unsigned int txq, unsigned int seqr,
                              unsigned int seq,
                              enum PKT_CTM_SIZE ctm_buf_size);

/**
 * Send a packet to an NBI port without freeing it. Does not notify sequencer.
 * @param isl           Island of the CTM packet
 * @param pnum          Packet number of the CTM packet
 * @param msi           Modification script info required for transmission
 * @param len           Length of the packet from the start of the
 *                      packet data (which immediately follows the rewrite
 *                      script plus padding)
 * @param nbi           NBI TM to send the packet to
 * @param txq           NBI TM TX queue to send the packet to
 * @param seqr          NBI TM sequencer to send the packet to
 * @param seq           NBI TM sequence number of the packet
 * @param ctm_buf_size  Encoded CTM buffer size
 */
__intrinsic void pkt_nbi_send_dont_free(unsigned char isl, unsigned int pnum,
                                        __gpr const struct pkt_ms_info *msi,
                                        unsigned int len, unsigned int nbi,
                                        unsigned int txq, unsigned int seqr,
                                        unsigned int seq,
                                        enum PKT_CTM_SIZE ctm_buf_size);

/**
 * Drop a packet sequence from an NBI port. Needed to keep sequencer happy.
 * @param isl           Island of the CTM packet
 * @param pnum          Packet number of the CTM packet
 * @param msi           Modification script info required for transmission
 * @param len           Length of the packet from the start of the
 *                      packet data (which immediately follows the rewrite
 *                      script plus padding)
 * @param nbi           NBI TM to send the packet to
 * @param txq           NBI TM TX queue to send the packet to
 * @param seqr          NBI TM sequencer to send the packet to
 * @param seq           NBI TM sequence number of the packet
 * @param ctm_buf_size  Encoded CTM buffer size
 */
__intrinsic void pkt_nbi_drop_seq(unsigned char isl, unsigned int pnum,
                                  __gpr const struct pkt_ms_info *msi,
                                  unsigned int len, unsigned int nbi,
                                  unsigned int txq, unsigned int seqr,
                                  unsigned int seq,
                                  enum PKT_CTM_SIZE ctm_buf_size);


/*
 *  The following sequence of actions is expected to properly allocate and
 *  free CTM packets:
 *
 *  1. Allocate a credits management structure in CLS memory.
 *      __export __shared __cls struct ctm_pkt_credits my_credits;
 *
 *  2. Initialize the credits structure with the packets and buffers credits.
 *      pkt_ctm_init_credits(&my_credits, INIT_PKT_CREDITS, INIT_BUF_CREDITS);
 *
 *  3. Get credits for the packets to be allocated (could be skipped if done
 *     internally in the pkt_ctm_alloc function).
 *      pkt_ctm_get_credits(&my_credits, NUM_PKTS, NUM_BUFS, 0);
 *     or
 *      pkt_ctm_get_credits(&my_credits, NUM_PKTS, NUM_BUFS, 1);
 *
 *  4. Allocate packets (one at a time, in this example 256 bytes)
 *     (do not allocate or replenish credits; already performed):
 *      pkt_num = pkt_ctm_alloc(&my_credits, isl_num, PKT_CTM_SIZE_256, 0, 0);
 *     or (allocate but do not replenish credits if low):
 *      pkt_num = pkt_ctm_alloc(&my_credits, isl_num, PKT_CTM_SIZE_256, 1, 0);
 *     or (allocate and replenish credits if low):
 *      pkt_num = pkt_ctm_alloc(&my_credits, isl_num, PKT_CTM_SIZE_256, 1, 1);
 *
 *  5. (optional) Periodically replenish the credit buckets.  One can omit
 *     this if the firmware invokes pkt_ctm_get_credits() or
 *     pkt_ctm_alloc() with the 'replenish_credits' argument as non-zero.
 *      pkt_ctm_poll_pe_credit(&my_credits);
 *
 *  6. Free CTM packets (using pkt_num from stage 4).
 *      pkt_ctm_free(isl_num, pkt_num);
 */

/**
 * Free a packet from CTM. Does not notify sequencer.
 * @param isl   Island of the CTM packet
 * @param pnum  Packet number of the CTM packet
 */
__intrinsic void pkt_ctm_free(unsigned int isl, unsigned int pnum);

/**
 * Allocate a CTM packet buffer.
 * @param credits           Credits management struct
 * @param isl               Island of the CTM packet
 * @param size              CTM buffer size (PKT_CTM_SIZE_*)
 * @param alloc_internal    If credits are to be allocated internally
 * @param replenish_credits Try and replenish credits internally
 * @return the allocated packet number on success, 0xffffffff on failure
 *
 * If the "alloc_internal" param is 0 - this function does not check for
 * available credits in the given credits structure.
 * It is assumed that the user has already called "pkt_ctm_get_credits"
 * and has successfully acquired the needed credits.
 * Returned credits, upon successful packet allocation, will update the
 * credits structure.
 *
 * A note of caution:
 * Since the PE "assumes" that the ME has allocated 1 buffer credit
 * before calling the "packet_alloc_poll" command, it will always
 * return 1 buffer credit if no new buffer was allocated. This can
 * overflow the initial credits (since the credits management
 * structure is always updates with the returned credits). If the user
 * prefers to allocate packets and credits once (for efficiency
 * reasons) and skip the credits allocation done internally, he/she
 * must make sure one buffer credit is allocated for every packet
 * allocation request.
 * For more details see Data Book section 9.2.2.7.1 Packet Allocation Request.
 *
 * No overflow checking is done internally (for efficiency reasons).
 */
__intrinsic unsigned int pkt_ctm_alloc(__cls struct ctm_pkt_credits *credits,
                                       unsigned char isl,
                                       enum PKT_CTM_SIZE size,
                                       unsigned char alloc_internal,
                                       int replenish_credits);

/**
 * Initialise the credit management structure.
 * @param credits       Credits management structure to initialize
 * @param pkt_credits   Number of packet credits
 * @param buf_credits   Number of buffer credits
 *
 * The user is expected to allocate the credits management structure and call
 * this function to initialize the required packets and buffers credits.
 * The user must not update the credits structure manualy since it is used
 * for an entire island's credit management.
 */
__intrinsic void pkt_ctm_init_credits(__cls struct ctm_pkt_credits *credits,
                                      unsigned int pkt_credits,
                                      unsigned int buf_credits);

/**
 * Poll for credits from the Packet Engine.
 * @param credits   Credits management structure to update
 *
 * Updates the credits management structure with the new polled credits.
 * This function must be run periodically to replenish the credit buckets.
 */
__intrinsic void pkt_ctm_poll_pe_credit(__cls struct ctm_pkt_credits *credits);

/**
 * Get credits for allocation of packet(s) in CTM.
 * @param pkt_credits       Desired amount of packet credits
 * @param buf_credits       Desired amount of buffer credits
 * @param replenish_credits Try and replenish credits internally
 *
 * This function gets credits from the credits mangment structure
 * in a safe way.
 * Will block until the requested packet and buffer credits could be acquired.
 */
__intrinsic void pkt_ctm_get_credits(__cls struct ctm_pkt_credits *credits,
                                     unsigned int pkt_credits,
                                     unsigned int buf_credits,
                                     int replenish_credits);

#endif /* __NFP_LANG_MICROC */

#endif /* !_PKT__PKT_H_ */
