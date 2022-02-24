/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <stdint.h>
#include <stdlib.h>

#include <nfp.h>
#include <nfp/me.h>
#include <nfp/mem_ring.h>

/* Scheduler input type */
struct schedule_t {
  union {
    __packed struct {
      uint32_t force:1;         /*> Force transmission            */
      uint32_t rsvd:15;
      uint32_t flow_id:16;      /*> Flow_id                       */
    };

    uint32_t __raw;
  };
};

/* ACTX input type */
struct actx_bump_t {
  union {
    __packed struct {
      uint32_t desc_idx:16;     /*> Descriptor Index */
      uint32_t seq:16;          /*> Sequencer number */
    };

    uint32_t __raw;
  };
};

/**
 * AC INPUT buffers
 */
MEM_RING_INIT(actx_bump_ring, FLOW_ACTX_BUMP_WQ_SIZE);

/**
 * Flow Ring buffers
 */
MEM_RING_INIT(flow_ring_0,  FLOW_GRP_WQ_SIZE);
MEM_RING_INIT(flow_ring_1,  FLOW_GRP_WQ_SIZE);
MEM_RING_INIT(flow_ring_2,  FLOW_GRP_WQ_SIZE);
MEM_RING_INIT(flow_ring_3,  FLOW_GRP_WQ_SIZE);
MEM_RING_INIT(flow_ring_sp, FLOW_GRP_SP_WQ_SIZE);

/**
 * Work types
 */
enum {
  WORK_TYPE_RX = 0,
  WORK_TYPE_TX = 1,
  WORK_TYPE_AC = 2,
  WORK_TYPE_RETX = 3,
};

struct work_t
{
  union {
    __packed struct {
      uint32_t type:2;      /*> WORK_TYPE_ */
      uint32_t rsvd0:30;

      uint32_t rsvd1;

      uint32_t flow_id:16;
      uint32_t rsvd2:16;
    };

    __packed struct {
      uint32_t type:2;      /*> WORK_TYPE_ */
      uint32_t cbs:2;       /*> CTM buffer size as a multiple of 256 */
      uint32_t isl:2;       /*> NOTE: Valid island numbers are 32, 33, 34, 35. Add +32 to use in NFP APIs */
      uint32_t pnum:10;
      uint32_t seq:16;

      uint32_t force:1;     /*> Force transmission */
      uint32_t bls:2;       /*> Buffer list of the MU buffer */
      uint32_t muptr:29;    /*> Pointer to the MU buffer >>11 */

      uint32_t flow_id:16;
      uint32_t seqr:5;
      uint32_t plen:11;     /*> Payload length */
    } io;

    __packed struct {
      uint32_t type:2;      /*> WORK_TYPE_ */
      uint32_t fin:1;
      uint32_t rx_bump:29;

      uint32_t rsvd:3;     /*> May be used for higher bits of flow_id */
      uint32_t tx_bump:29;

      uint32_t flow_id:16;
      uint32_t desc_idx:16;
    } ac;

    uint32_t __raw[3];
  };
};

/**
 * Work Result flags
 */

#define  WORK_RESULT_TX                   (1 << 0)     /*> Send ACK/SEG on MAC               */
#define  WORK_RESULT_DMA_ACDESC           (1 << 1)     /*> DMA appctx descriptor to host     */
#define  WORK_RESULT_DMA_PAYLOAD          (1 << 2)     /*> DMA payload to/from host          */
#define  WORK_RESULT_RETX                 (1 << 3)     /*> Trigger Retransmissions           */
#define  WORK_RESULT_FIN                  (1 << 4)     /*> Marked connection as FIN          */
#define  WORK_RESULT_QM                   (1 << 5)     /*> Update QueueManager state         */
#define  WORK_RESULT_QM_FORCE             (1 << 6)     /*> Force QM to schedule transmission */
#define  WORK_RESULT_ECE                  (1 << 7)     /*> Generate ECN echo                 */
#define  WORK_RESULT_TXP_ZERO             (1 << 8)     /*> If fs->tx_sent == 0               */
#define  WORK_RESULT_ECNB                 (1 << 9)     /*> ECN echo received                 */

#define WORK_TCPH_FLAGS_WIN_SET(_RSLT, _FLAGS, _WIN)    \
          (_RSLT)->__raw[5]  = ((NET_TCPPADTS_LEN32 <<  28) | ((_FLAGS) << 16) | (_WIN))
#define WORK_DMA_OFFSET_LEN_SET(_RSLT, _LEN, _OFFSET)   \
          (_RSLT)->__raw[10]  =  (((_LEN) << 16) | (_OFFSET))

/** Work Result */
struct work_result_t
{
  union {
    __packed struct {
      struct work_t work;     /*> Work information */

      /* Protocol snapshot */
      uint32_t seq;
      uint32_t ack;

      uint32_t win;

      uint32_t ts_val;        /*> TS echo by remote peer */
      uint32_t ts_ecr;        /*> TS echo to remote peer */

      uint32_t qm_bump;       /*> New bytes available for scheduling in QueueManager */
      uint32_t dma_pos;       /*> Position in the TX/RX buffer to DMA the payload to/from */

      uint16_t dma_len;       /*> Payload length */
      uint16_t dma_offset;    /*> Offset in packet buffer to DMA to/from */

      uint32_t ac_rx_bump;    /*> RX Bump to AC descriptor */
      uint32_t ac_tx_bump;    /*> TX Bump to AC descriptor */
      uint32_t flags;         /*> Refer to WORK_RESULT flags */
    };

    uint32_t __raw[14];
  };
};

#endif /* PIPELINE_H_ */
