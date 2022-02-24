/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef FLOWS_H_
#define FLOWS_H_

#include <nfp.h>

#include "packet_defs.h"
#include "fp_mem.h"
#include "pipeline.h"

__intrinsic void flows_tx(
                          struct work_t*        work,
                   __lmem struct flowst_tcp_t*  fs,
                 __xwrite struct work_result_t* result);

__intrinsic void flows_ac(
                          struct work_t*         work,
                   __lmem struct flowst_tcp_t*   fs,
                 __xwrite struct work_result_t*  result);

__intrinsic void flows_retx(
                            struct work_t*         work,
                     __lmem struct flowst_tcp_t*   fs,
                   __xwrite struct work_result_t*  result);

__intrinsic void flows_ack(
                          struct work_t*         work,
                  __xread struct pkt_summary_t*  pkt,
                   __lmem struct flowst_tcp_t*   fs,
                 __xwrite struct work_result_t*  result);

__intrinsic void flows_seg(
                          struct work_t*         work,
                  __xread struct pkt_summary_t*  pkt,
                   __lmem struct flowst_tcp_t*   fs,
                 __xwrite struct work_result_t*  result);

#endif /* FLOWS_H_ */
