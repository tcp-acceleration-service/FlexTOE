/*
 * Copyright (C) 2015-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          apps/wire/config.h
 * @brief         Infrastructure configuration for the wire application.
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "common.h"

/* Each Packet Engine has 128 2-KB buffer credits, in the case that all of the
 * Dcache has been allocated to store packets, and 512 packet credits.
 *
 * We only allocate the entire CTM for packets, hence we use 128 buffer credits.
 */

/* Total number of packet and packet buffers per CTM */
#define TOTAL_CTM_PACKET_CREDITS        512
#define TOTAL_CTM_BUFFER_CREDITS        128

/* NBI pool per CTM */
#define NBI_PACKET_CREDITS              (TOTAL_CTM_PACKET_CREDITS/2)
#define NBI_BUFFER_CREDITS              (TOTAL_CTM_BUFFER_CREDITS/2)

/* ME  pool per CTM */
#define ME_CTM_ALLOC_MAX_PKT_CREDITS    (TOTAL_CTM_PACKET_CREDITS - NBI_PACKET_CREDITS)
#define ME_CTM_ALLOC_MAX_BUF_CREDITS    (TOTAL_CTM_BUFFER_CREDITS - NBI_BUFFER_CREDITS)

/*
 * RX/TX configuration
 * - Configure RX checksum offload so the wire can validate checksums
 */
#define SPLIT_LENGTH            3   // Split offset = 2048
#define PKT_NBI_OFFSET          64
#define PKTBUF_CTM_SIZE         SPLIT_LENGTH
#define PKTBUF_MU_OFFSET        (256 << SPLIT_LENGTH)
#define MAC_PREPEND_BYTES       8

/**
 * NBI configuration
 */
#define NBI_DMA_BP_DROP_ENABLE      0
#define NBI_PKT_PREPEND_BYTES       MAC_PREPEND_BYTES
#define NBI_DMA_BP0_BLQ_TARGET      0,0
#define NBI_DMA_BP1_BLQ_TARGET      0,0
#define NBI_DMA_BP2_BLQ_TARGET      0,0
#define NBI_DMA_BP3_BLQ_TARGET      0,0
#define NBI_DMA_BP4_BLQ_TARGET      0,0
#define NBI_DMA_BP5_BLQ_TARGET      0,0
#define NBI_DMA_BP6_BLQ_TARGET      0,0
#define NBI_DMA_BP7_BLQ_TARGET      0,0
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND0   1,(NBI_PACKET_CREDITS - 1),(NBI_BUFFER_CREDITS - 1)
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND1   1,(NBI_PACKET_CREDITS - 1),(NBI_BUFFER_CREDITS - 1)
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND2   1,(NBI_PACKET_CREDITS - 1),(NBI_BUFFER_CREDITS - 1)
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND3   1,(NBI_PACKET_CREDITS - 1),(NBI_BUFFER_CREDITS - 1)
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND4   0,0,0
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND5   0,0,0
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND6   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND0   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND1   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND2   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND3   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND4   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND5   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND6   0,0,0

/**
 * TM configuration
 */
/* TM Sequencer */
#define NBI_TM_NUM_SEQUENCERS       4       /*> Enables 8 sequencers: See GRO config for sequencer mapping */
#define NBI_TM_ENABLE_SEQUENCER0    0

/* TX Queue configuration */
/* Determine the NBI TM queue depth based on port configuration. */
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 <= 1
    /* 1-port Configuration. */
    #define NS_PLATFORM_NBI_TM_0_QSIZE 9  /* 2^9 packet buffers per queue */
#elif NS_PLATFORM_NUM_PORTS_PER_MAC_0 <= 2
    /* 2-port Configuration. */
    #define NS_PLATFORM_NBI_TM_0_QSIZE 8  /* 2^8 packet buffers per queue */
#elif NS_PLATFORM_NUM_PORTS_PER_MAC_0 <= 4
    /* 3- or 4-port Configuration. */
    #define NS_PLATFORM_NBI_TM_0_QSIZE 7  /* 2^7 packet buffers per queue */
#elif NS_PLATFORM_NUM_PORTS_PER_MAC_0 <= 8
    /* 5- to 8-port Configuration. */
    #define NS_PLATFORM_NBI_TM_0_QSIZE 6  /* 2^6 packet buffers per queue */
#else
    /* Unsupported configuration */
    #error "No support for more than 8 ports per MAC/NBI island"
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 <= 1
    /* 1-port Configuration. */
    #define NS_PLATFORM_NBI_TM_1_QSIZE 9  /* 2^9 packet buffers per queue */
#elif NS_PLATFORM_NUM_PORTS_PER_MAC_1 <= 2
    /* 2-port Configuration. */
    #define NS_PLATFORM_NBI_TM_1_QSIZE 8  /* 2^8 packet buffers per queue */
#elif NS_PLATFORM_NUM_PORTS_PER_MAC_1 <= 4
    /* 3- or 4-port Configuration. */
    #define NS_PLATFORM_NBI_TM_1_QSIZE 7  /* 2^7 packet buffers per queue */
#elif NS_PLATFORM_NUM_PORTS_PER_MAC_1 <= 8
    /* 5- to 8-port Configuration. */
    #define NS_PLATFORM_NBI_TM_1_QSIZE 6  /* 2^6 packet buffers per queue */
#else
    /* Unsupported configuration */
    #error "No support for more than 8 ports per MAC/NBI island"
#endif

/* Initialize the NBI TM queues associated with each port. */
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 0
    #define NBI0_TM_Q_CFG_RANGE0      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(0), \
        NS_PLATFORM_NBI_TM_QID_HI(0), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 1
    #define NBI0_TM_Q_CFG_RANGE1      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(1), \
        NS_PLATFORM_NBI_TM_QID_HI(1), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 2
    #define NBI0_TM_Q_CFG_RANGE2      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(2), \
        NS_PLATFORM_NBI_TM_QID_HI(2), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 3
    #define NBI0_TM_Q_CFG_RANGE3      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(3), \
        NS_PLATFORM_NBI_TM_QID_HI(3), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 4
    #define NBI0_TM_Q_CFG_RANGE4      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(4), \
        NS_PLATFORM_NBI_TM_QID_HI(4), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 5
    #define NBI0_TM_Q_CFG_RANGE5      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(5), \
        NS_PLATFORM_NBI_TM_QID_HI(5), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 6
    #define NBI0_TM_Q_CFG_RANGE6      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(6), \
        NS_PLATFORM_NBI_TM_QID_HI(6), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_0 > 7
    #define NBI0_TM_Q_CFG_RANGE7      \
        1,                            \
        NS_PLATFORM_NBI_TM_QID_LO(7), \
        NS_PLATFORM_NBI_TM_QID_HI(7), \
        NS_PLATFORM_NBI_TM_0_QSIZE
#endif

#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 0
    #define NBI1_TM_Q_CFG_RANGE0                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 0), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 0), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 1
    #define NBI1_TM_Q_CFG_RANGE1                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 1), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 1), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 2
    #define NBI1_TM_Q_CFG_RANGE2                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 2), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 2), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 3
    #define NBI1_TM_Q_CFG_RANGE3                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 3), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 3), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 4
    #define NBI1_TM_Q_CFG_RANGE4                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 4), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 4), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 5
    #define NBI1_TM_Q_CFG_RANGE5                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 5), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 5), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 6
    #define NBI1_TM_Q_CFG_RANGE6                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 6), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 6), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
#if NS_PLATFORM_NUM_PORTS_PER_MAC_1 > 7
    #define NBI1_TM_Q_CFG_RANGE7                                          \
        1,                                                                \
        (NS_PLATFORM_NBI_TM_QID_LO(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 7), \
        (NS_PLATFORM_NBI_TM_QID_HI(NS_PLATFORM_NUM_PORTS_PER_MAC_0) + 7), \
        NS_PLATFORM_NBI_TM_1_QSIZE
#endif
/* NOTE: Shaper not configured! */

/**
 * Buffer List configuration
 */
#include "blm_custom.h"

/**
 * GRO configuration
 *
 * CTX                              [0] SP  (unsequenced)
 * [0-5] assigned by ingress NBI => [1-6]   RX
 * Thus, we need atleast 8 contexts.
 */
#define GRO_NUM_BLOCKS          1
#define GRO_CTX_PER_BLOCK       8
#define GRO_CTX_REORDERQ_SZ     8192
#define GRO_ISL                 48
#define EMEM_ISL                24

#define NBI_INGRESS_SEQR_BASE       (0)
#define NBI_INGRESS_SEQR0           (NBI_INGRESS_SEQR_BASE + 0)
#define NBI_INGRESS_SEQR1           (NBI_INGRESS_SEQR_BASE + 1)
#define NBI_INGRESS_SEQR2           (NBI_INGRESS_SEQR_BASE + 2)
#define NBI_INGRESS_SEQR3           (NBI_INGRESS_SEQR_BASE + 3)
#define NBI_INGRESS_SEQR4           (NBI_INGRESS_SEQR_BASE + 4)
#define NBI_INGRESS_SEQR5           (NBI_INGRESS_SEQR_BASE + 5)
#define NBI_INGRESS_SEQR_MAX        NBI_INGRESS_SEQR5
#define RX_SEQR(INGRESS_SEQR)       (INGRESS_SEQR + 1)              /* We map Ingress [0-5] -> RX [1-6] */
#define RX_SEQR_BASE                RX_SEQR(NBI_INGRESS_SEQR_BASE)
#define RX_SEQR0                    (RX_SEQR_BASE + 0)
#define RX_SEQR1                    (RX_SEQR_BASE + 1)
#define RX_SEQR2                    (RX_SEQR_BASE + 2)
#define RX_SEQR3                    (RX_SEQR_BASE + 3)
#define RX_SEQR4                    (RX_SEQR_BASE + 4)
#define RX_SEQR5                    (RX_SEQR_BASE + 5)
#define RX_SEQR_MAX                 RX_SEQR5
#define TX_SEQR_BASE                (RX_SEQR_MAX + 1)               /* Use 4 sequencers depending on FlowGroup */
#define TX_SEQR(FLOW_GRP)           (TX_SEQR_BASE + FLOW_GRP)
#define TX_SEQR0                    (TX_SEQR_BASE + 0)
#define TX_SEQR1                    (TX_SEQR_BASE + 1)
#define TX_SEQR2                    (TX_SEQR_BASE + 2)
#define TX_SEQR3                    (TX_SEQR_BASE + 3)
#define TX_SEQR_MAX                 TX_SEQR3
#define AC_SEQR_BASE                (TX_SEQR_MAX + 1)               /* Use 4 sequencers depending on FlowGroup */
#define AC_SEQR(FLOW_GRP)           (AC_SEQR_BASE + FLOW_GRP)
#define AC_SEQR0                    (AC_SEQR_BASE + 0)
#define AC_SEQR1                    (AC_SEQR_BASE + 1)
#define AC_SEQR2                    (AC_SEQR_BASE + 2)
#define AC_SEQR3                    (AC_SEQR_BASE + 3)
#define AC_SEQR_MAX                 AC_SEQR3
#define RETX_SEQR_BASE              (0)
#define RETX_SEQR0                  (RETX_SEQR_BASE + 0)
#define RETX_SEQR_MAX               RETX_SEQR0
#define SP_SEQR_BASE                (0)
#define SP_SEQR0                    (SP_SEQR_BASE + 0)
#define SP_SEQR_MAX                 SP_SEQR0

/**
 * Flow steering configuration
 */
#define NUM_FLOW_GROUPS              4       /*> Must be a power of 2 */
#define FLOW_GRP_WQ_SIZE             32768   /*> Should be >= GRO_CTX_REORDERQ_SZ */
#define FLOW_GRP_SP_WQ_SIZE          65536   /*> Slowpath forward WQ size */
#define FLOW_ACTX_BUMP_WQ_SIZE       2048    /*> Application context Bump WQ size */
#define FLOW_QM_SCHED_RING_SIZE      4096    /*> QM ->Preproc schedule ring size */
#define FLOW_QM_BUMP_RING_SIZE       1024    /*> Postproc->QM bump ring size */
#define FLOW_QM_CREDITS              1024    /*> Max in-flight TX operations */
#define FLOW_POSTPROC_RING_SIZE      4096    /*> Proc->Postproc ring size */
#define QM_BYPASS                    0       /*> Bypass QM for uncongested flows */

/**
 * DMA configuration
 */
#define DMA_ATX_DESC_POOL_NUM   512     /*> Must be < 2^16 */
#define DMA_ATX_DESC_POOL_SIZE  (4 * DMA_ATX_DESC_POOL_NUM)
#define DMA_ARX_DESC_POOL_NUM   1024    /*> Must be < 2^16 */
#define DMA_ARX_DESC_POOL_SIZE  (4 * DMA_ARX_DESC_POOL_NUM)
#define DMA_TX_WQ_SIZE          4096    /*> DMA engine queue size (in one direction) = 128; -> 128 * 32 = 4096 */
#define DMA_RX_WQ_SIZE          4096    /*> DMA engine queue size (in one direction) = 128; -> 128 * 32 = 4096 */
#define DMA_ONE_PKT_AT_A_TIME   0

/**
 * PCIe configurations
 */
#define PCIE_CPP2PCIE_NOTIFY    0

/**
 * TCP Protocol configuration
 */
#define ALLOW_FUTURE_ACKS       1     /*> Accept ack > tx_next_seq */
#define SKIP_ACK                0     /*> Skip acknowledgement for well-formed packets (used to emulate delayed ACK) */
#define ALLOW_OOO_RECV          1     /*> Enable out of order receive processing */
#define VALIDATE_BUMP           1     /*> Validate RX & TX bumps from application */
#define TCP_TIMESTAMP_ENABLE    1     /*> Enable TCP timestamp option */
#define FORCE_SEND_WINUPDATE    1     /*> Force send rx window update */
#define TCP_MSS                 1448

/**
 * Miscellaeneous
 */
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) < (B) ? (B) : (A))

#endif /* __APP_CONFIG_H__ */
