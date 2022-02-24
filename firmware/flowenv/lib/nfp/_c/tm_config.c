/*
 * Copyright (C) 2017-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/_c/tm_config.c
 * @brief         NBI Traffic Manager Initialisation library
 */


#include <nfp6000/nfp_me.h>
#include <nfp6000/nfp_nbi_tm.h>
#include <nfp/xpb.h>
#include <nfp.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <std/reg_utils.h>
#include <assert.h>
#include <nfp/tm_config.h>

#define MAX_NBI_NUMBER          1
#define MAX_TM_QUEUE_NUM        1023
#define MAX_TM_L1_L2_SCHED_NUM  143
#define MAX_TM_SCHED_NUM        144
#define NUM_L1_SCHEDS           16
#define NUM_L2_SCHEDS           128
#define HEAD_TAIL_SRAM_MAX      16384
#define QUEUE_SIZE_MIN          3
#define TM_L0_SCHED_NUM         144

/* Number of registers to modify per command (must be power of 2). */
#define CMD_BURST_SZ            16
#define CMD_BURST_BYTES         ((CMD_BURST_SZ) << 2)

/* Number of L1/L2 DWRR register to modify per command. */
#define L1L2_DWRR_BURST_SZ      L1L2_SCHEDULER_INPUTS
#define L1L2_DWRR_BURST_BYTES   ((L1L2_DWRR_BURST_SZ) << 2)

/**
 * NBI Traffic Manager CPP bus offset for a given NBI island.
 */
#ifndef NFP_NBI_TM_CPP_OFF
#define NFP_NBI_TM_CPP_OFF(_isl) ((_isl) << 24)
#endif


/**
 * NBI Traffic Manager Head Tail SRAM CPP bus offset for a given NBI island
 */
#ifndef NBI_TM_HEAD_TAIL_SRAM_OFFS
#define NBI_TM_HEAD_TAIL_SRAM_OFFS  0x268000
#endif

#ifndef NBI_TM_HEAD_TAIL_SRAM
#define NBI_TM_HEAD_TAIL_SRAM(_isl) ((NFP_NBI_TM_CPP_OFF(_isl)) | \
    NBI_TM_HEAD_TAIL_SRAM_OFFS)
#endif

/** Base addresses for the NBI TM scheduler config registers. */
#define TM_SCHED_CONFIG_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + \
    NFP_NBI_TM_SCHEDULER_REG)

/** Base addresses for the NBI TM scheduler L1 weight registers. */
#define TM_SCHED_L1_WEIGHT_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + \
    NFP_NBI_TM_SCHEDULER_L1_WEIGHT)

/** Base addresses for the NBI TM scheduler L2 weight registers. */
#define TM_SCHED_L2_WEIGHT_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + \
    NFP_NBI_TM_SCHEDULER_L2_WEIGHT)

/** Base addresses for the NBI TM scheduler L1 deficit registers. */
#define TM_SCHED_L1_DEFICIT_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + \
    NFP_NBI_TM_SCHEDULER_L1_DEFICIT)

/** Base addresses for the NBI TM scheduler L2 deficit registers. */
#define TM_SCHED_L2_DEFICIT_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + \
    NFP_NBI_TM_SCHEDULER_L2_DEFICIT)

/** Base addresses for the NBI TM shaper registers. */
#define TM_SHAPER_XPB_BASE(_nbi)  (NFP_NBI_TM_XPB_OFF(_nbi) + \
    NFP_NBI_TM_SHAPER_REG)

/** Address of the TM queue config register. */
#define TM_QUEUE_CONFIG_ADDR(_nbi, _qnum)                       \
    (TMQ_XPB_BASE(_nbi) + NFP_NBI_TM_QUEUE_CONFIG(_qnum))

/** Address of the TM config register. */
#define TM_CONFIG_REG_ADDR(_nbi)                       \
    (NFP_NBI_TM_XPB_OFF(_nbi) + NFP_NBI_TM_TRAFFIC_MANAGER_REG)

/** Address of the TM scheduler config register. */
#define TM_SCHED_CONFIG_ADDR(_nbi, _sched)                       \
    (TM_SCHED_CONFIG_XPB_BASE(_nbi) + NFP_NBI_TM_SCHEDULER_CONFIG(_sched))

/** Address of the TM L0 scheduler weight register. */
#define TM_SCHED_L0_WEIGHT_ADDR(_nbi, _sch_port)                              \
    (TM_SCHED_CONFIG_XPB_BASE(_nbi) + NFP_NBI_TM_SCHEDULER_WEIGHT(_sch_port))

/** Address of the TM L1 scheduler weight register. */
#define TM_SCHED_L1_WEIGHT_ADDR(_nbi, _sched, _sch_port)                    \
    (TM_SCHED_L1_WEIGHT_XPB_BASE(_nbi) +                                    \
     NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT(((_sched) * L1L2_SCHEDULER_INPUTS) + \
                                       (_sch_port)))

/** Address of the TM L2 scheduler weight register. */
#define TM_SCHED_L2_WEIGHT_ADDR(_nbi, _sched, _sch_port)                    \
    (TM_SCHED_L2_WEIGHT_XPB_BASE(_nbi) +                                    \
     NFP_NBI_TM_L1_L2_SCHEDULER_WEIGHT(((_sched) * L1L2_SCHEDULER_INPUTS) + \
                                       (_sch_port)))

/** Base address of the weight registers for a TM L1 scheduler. */
#define TM_SCHED_L1_WEIGHT_ADDR_BASE(_nbi, _sched) \
    TM_SCHED_L1_WEIGHT_ADDR(_nbi, _sched, 0)

/** Base address of the weight registers for a TM L2 scheduler. */
#define TM_SCHED_L2_WEIGHT_ADDR_BASE(_nbi, _sched) \
    TM_SCHED_L2_WEIGHT_ADDR(_nbi, _sched, 0)

/** Address of the TM L0 scheduler deficit register. */
#define TM_SCHED_L0_DEFICIT_ADDR(_nbi, _sch_port) \
    (TM_SCHED_CONFIG_XPB_BASE(_nbi) +             \
     NFP_NBI_TM_SCHEDULER_DEFICIT(_sch_port))

/** Address of the TM L1 scheduler deficit register. */
#define TM_SCHED_L1_DEFICIT_ADDR(_nbi, _sched, _sch_port)                    \
    (TM_SCHED_L1_DEFICIT_XPB_BASE(_nbi) +                                    \
     NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT(((_sched) * L1L2_SCHEDULER_INPUTS) + \
                                        (_sch_port)))

/** Address of the TM L2 scheduler deficit register. */
#define TM_SCHED_L2_DEFICIT_ADDR(_nbi, _sched, _sch_port)                    \
    (TM_SCHED_L2_DEFICIT_XPB_BASE(_nbi) +                                    \
     NFP_NBI_TM_L1_L2_SCHEDULER_DEFICIT(((_sched) * L1L2_SCHEDULER_INPUTS) + \
                                        (_sch_port)))

/** Base address of the deficit registers for a TM L1 scheduler. */
#define TM_SCHED_L1_DEFICIT_ADDR_BASE(_nbi, _sched) \
    TM_SCHED_L1_DEFICIT_ADDR(_nbi, _sched, 0)

/** Base address of the deficit registers for a TM L2 scheduler. */
#define TM_SCHED_L2_DEFICIT_ADDR_BASE(_nbi, _sched) \
    TM_SCHED_L2_DEFICIT_ADDR(_nbi, _sched, 0)

/** Address of the TM shaper rate register. */
#define TM_SHAPER_RATE_ADDR(_nbi, _shaper)                       \
    (TM_SHAPER_XPB_BASE(_nbi) + NFP_NBI_TM_SHAPER_RATE(_shaper))

/** Address of the TM shaper threshold register. */
#define TM_SHAPER_THRESHOLD_ADDR(_nbi, _shaper)                       \
    (TM_SHAPER_XPB_BASE(_nbi) + NFP_NBI_TM_SHAPER_THRESHOLD(_shaper))

/** Address of the TM shaper max overshoot register. */
#define TM_SHAPER_MAX_OVERSHOOT_ADDR(_nbi, _shaper)                       \
    (TM_SHAPER_XPB_BASE(_nbi) + NFP_NBI_TM_SHAPER_MAX_OVERSHOOT(_shaper))

/** Address of the TM shaper rate adjust register. */
#define TM_SHAPER_RATE_ADJUST_ADDR(_nbi, _shaper)                       \
    (TM_SHAPER_XPB_BASE(_nbi) + NFP_NBI_TM_SHAPER_RATE_ADJUST(_shaper))

__intrinsic void
__tm_config_l0_dwrr_weight_read(__xread void *weight, unsigned int nbi,
                                unsigned int sched_port,
                                unsigned int num_sch_ports, sync_t sync,
                                SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_L0_WEIGHT_ADDR(nbi, sched_port);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_port < L0_SCHEDULER_INPUTS);
    try_ctassert((num_sch_ports >= 1) && (num_sch_ports <= CMD_BURST_SZ));
    try_ctassert((sched_port + num_sch_ports) <= L0_SCHEDULER_INPUTS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(weight, addr, num_sch_ports << 2, CMD_BURST_BYTES, sync, sig);
}

__intrinsic void
tm_config_l0_dwrr_weight_read(__xread void *weight, unsigned int nbi,
                              unsigned int sched_port,
                              unsigned int num_sch_ports)
{
    SIGNAL sig;

    __tm_config_l0_dwrr_weight_read(weight, nbi, sched_port, num_sch_ports,
                                    ctx_swap, &sig);
}

__intrinsic void
__tm_config_l0_dwrr_weight_write(__xwrite void *weight, __xwrite void *deficit,
                                 unsigned int nbi, unsigned int sched_port,
                                 unsigned int num_sch_ports,
                                 sync_t sync, SIGNAL *sig_weight,
                                 SIGNAL *sig_deficit)
{
    uint32_t deficit_addr = TM_SCHED_L0_DEFICIT_ADDR(nbi, sched_port);
    uint32_t weight_addr  = TM_SCHED_L0_WEIGHT_ADDR(nbi, sched_port);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_port < L0_SCHEDULER_INPUTS);
    try_ctassert((num_sch_ports >= 1) && (num_sch_ports <= CMD_BURST_SZ));
    try_ctassert((sched_port + num_sch_ports) <= L0_SCHEDULER_INPUTS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(weight, weight_addr, num_sch_ports << 2, CMD_BURST_BYTES, sync,
                sig_weight);
    __xpb_write(deficit, deficit_addr, num_sch_ports << 2, CMD_BURST_BYTES,
                sync, sig_deficit);
}


__intrinsic void
tm_config_l0_dwrr_weight_write(__xwrite void *weight, unsigned int nbi,
                               unsigned int sched_port,
                               unsigned int num_sch_ports)
{
    SIGNAL sig;
    __xwrite struct nfp_nbi_tm_scheduler_deficit deficit[CMD_BURST_SZ];

    reg_zero(deficit, sizeof(deficit));

    __tm_config_l0_dwrr_weight_write(weight, deficit, nbi, sched_port,
                                     num_sch_ports, ctx_swap, &sig, &sig);
}

__intrinsic void
__tm_config_l0_sched_read(__xread void *config, unsigned int nbi, sync_t sync,
                          SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_CONFIG_ADDR(nbi, TM_L0_SCHED_NUM);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(config, addr, 4, 4, sync, sig);
}

__intrinsic void
tm_config_l0_sched_read(__xread void *config, unsigned int nbi)
{
    SIGNAL sig;

    __tm_config_l0_sched_read(config, nbi, ctx_swap, &sig);
}

__intrinsic void
__tm_config_l0_sched_write(__xwrite void *config, uint32_t nbi, sync_t sync,
                           SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_CONFIG_ADDR(nbi, TM_L0_SCHED_NUM);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(config, addr, 4, 4, sync, sig);
}

__intrinsic void
tm_config_l0_sched_write(__xwrite void *config, uint32_t nbi)
{
    SIGNAL sig;

    __tm_config_l0_sched_write(config, nbi, ctx_swap, &sig);
}

__intrinsic void
__tm_config_l0_shaper_read(__xread void *rate, __xread void *threshold,
                           __xread void *overshoot, __xread void *adjust,
                           unsigned int nbi, sync_t sync, SIGNAL *sig_rate,
                           SIGNAL *sig_thresh, SIGNAL *sig_over,
                           SIGNAL *sig_adj)
{
    uint32_t addr_adj    = TM_SHAPER_RATE_ADJUST_ADDR(nbi, TM_L0_SCHED_NUM);
    uint32_t addr_over   = TM_SHAPER_MAX_OVERSHOOT_ADDR(nbi, TM_L0_SCHED_NUM);
    uint32_t addr_rate   = TM_SHAPER_RATE_ADDR(nbi, TM_L0_SCHED_NUM);
    uint32_t addr_thresh = TM_SHAPER_THRESHOLD_ADDR(nbi, TM_L0_SCHED_NUM);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(adjust, addr_adj, 4, 4, sync, sig_adj);
    __xpb_read(overshoot, addr_over, 4, 4, sync, sig_over);
    __xpb_read(rate, addr_rate, 4, 4, sync, sig_rate);
    __xpb_read(threshold, addr_thresh, 4, 4, sync, sig_thresh);
}

__intrinsic void
tm_config_l0_shaper_read(__xread void *rate, __xread void *threshold,
                         __xread void *overshoot, __xread void *adjust,
                         unsigned int nbi)
{
    SIGNAL sig;

    __tm_config_l0_shaper_read(rate, threshold, overshoot, adjust, nbi,
                               ctx_swap, &sig, &sig, &sig, &sig);
}

__intrinsic void
__tm_config_l0_shaper_write(__xwrite void *rate, __xwrite void *threshold,
                            __xwrite void *overshoot, __xwrite void *adjust,
                            unsigned int nbi, sync_t sync, SIGNAL *sig_rate,
                            SIGNAL *sig_thresh, SIGNAL *sig_over,
                            SIGNAL *sig_adj)
{
    uint32_t addr_adj    = TM_SHAPER_RATE_ADJUST_ADDR(nbi, TM_L0_SCHED_NUM);
    uint32_t addr_over   = TM_SHAPER_MAX_OVERSHOOT_ADDR(nbi, TM_L0_SCHED_NUM);
    uint32_t addr_rate   = TM_SHAPER_RATE_ADDR(nbi, TM_L0_SCHED_NUM);
    uint32_t addr_thresh = TM_SHAPER_THRESHOLD_ADDR(nbi, TM_L0_SCHED_NUM);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(adjust, addr_adj, 4, 4, sync, sig_adj);
    __xpb_write(overshoot, addr_over, 4, 4, sync, sig_over);
    __xpb_write(rate, addr_rate, 4, 4, sync, sig_rate);
    __xpb_write(threshold, addr_thresh, 4, 4, sync, sig_thresh);
}

__intrinsic void
tm_config_l0_shaper_write(__xwrite void *rate, __xwrite void *threshold,
                          __xwrite void *overshoot, __xwrite void *adjust,
                          unsigned int nbi)
{
    SIGNAL sig;

    __tm_config_l0_shaper_write(rate, threshold, overshoot, adjust, nbi,
                                ctx_swap, &sig, &sig, &sig, &sig);
}

__intrinsic void
__tm_config_l1_dwrr_weight_read(__xread void *weight, unsigned int nbi,
                                unsigned int sched, unsigned int sched_port,
                                unsigned int num_sch_ports, sync_t sync,
                                SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_L1_WEIGHT_ADDR(nbi, sched, sched_port);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched < NUM_L1_SCHEDS);
    try_ctassert(sched_port < L1L2_SCHEDULER_INPUTS);
    try_ctassert((num_sch_ports >= 1) &&
                 (num_sch_ports <= L1L2_DWRR_BURST_SZ));
    try_ctassert((sched_port + num_sch_ports) <= L1L2_SCHEDULER_INPUTS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(weight, addr, num_sch_ports << 2, L1L2_DWRR_BURST_BYTES, sync,
               sig);
}

__intrinsic void
tm_config_l1_dwrr_weight_read(__xread void *weight, unsigned int nbi,
                              unsigned int sched, unsigned int sched_port,
                              unsigned int num_sch_ports)
{
    SIGNAL sig;

    __tm_config_l1_dwrr_weight_read(weight, nbi, sched, sched_port,
                                    num_sch_ports, ctx_swap, &sig);
}

__intrinsic void
__tm_config_l1_dwrr_weight_write(__xwrite void *weight, __xwrite void *deficit,
                                 unsigned int nbi, unsigned int sched,
                                 unsigned int sched_port,
                                 unsigned int num_sch_ports, sync_t sync,
                                 SIGNAL *sig_weight, SIGNAL *sig_deficit)
{
    uint32_t deficit_addr = TM_SCHED_L1_DEFICIT_ADDR(nbi, sched, sched_port);
    uint32_t weight_addr  = TM_SCHED_L1_WEIGHT_ADDR(nbi, sched, sched_port);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched < NUM_L1_SCHEDS);
    try_ctassert(sched_port < L1L2_SCHEDULER_INPUTS);
    try_ctassert((num_sch_ports >= 1) &&
                 (num_sch_ports <= L1L2_DWRR_BURST_SZ));
    try_ctassert((sched_port + num_sch_ports) <= L1L2_SCHEDULER_INPUTS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(weight, weight_addr, num_sch_ports << 2, L1L2_DWRR_BURST_BYTES,
                sync, sig_weight);
    __xpb_write(deficit, deficit_addr, num_sch_ports << 2,
                L1L2_DWRR_BURST_BYTES, sync, sig_deficit);
}

__intrinsic void
tm_config_l1_dwrr_weight_write(__xwrite void *weight, unsigned int nbi,
                               unsigned int sched, unsigned int sched_port,
                               unsigned int num_sch_ports)
{
    SIGNAL sig;
    __xwrite struct nfp_nbi_tm_scheduler_deficit deficit[L1L2_DWRR_BURST_SZ];

    reg_zero(deficit, sizeof(deficit));

    __tm_config_l1_dwrr_weight_write(weight, deficit, nbi, sched, sched_port,
                                     num_sch_ports, ctx_swap, &sig, &sig);
}

__intrinsic void
__tm_config_l1_sched_read(__xread void *config, unsigned int nbi,
                          unsigned int sched_num, unsigned int num_scheds,
                          sync_t sync, SIGNAL *sig)
{
    unsigned int l1_sched = sched_num + NUM_L2_SCHEDS;
    uint32_t addr         = TM_SCHED_CONFIG_ADDR(nbi, l1_sched);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L1_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L1_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(config, addr, num_scheds << 2, CMD_BURST_BYTES, sync, sig);
}

__intrinsic void
tm_config_l1_sched_read(__xread void *config, unsigned int nbi,
                        unsigned int sched_num, unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l1_sched_read(config, nbi, sched_num, num_scheds, ctx_swap,
                              &sig);
}

__intrinsic void
__tm_config_l1_sched_write(__xwrite void *config, unsigned int nbi,
                           unsigned int sched_num, unsigned int num_scheds,
                           sync_t sync, SIGNAL *sig)
{
    unsigned int l1_sched = sched_num + NUM_L2_SCHEDS;
    uint32_t addr         = TM_SCHED_CONFIG_ADDR(nbi, l1_sched);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L1_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L1_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(config, addr, num_scheds << 2, CMD_BURST_BYTES, sync, sig);
}

__intrinsic void
tm_config_l1_sched_write(__xwrite void *config, unsigned int nbi,
                         unsigned int sched_num, unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l1_sched_write(config, nbi, sched_num, num_scheds, ctx_swap,
                               &sig);
}

__intrinsic void
__tm_config_l1_shaper_read(__xread void *rate, __xread void *threshold,
                           __xread void *overshoot, __xread void *adjust,
                           unsigned int nbi, unsigned int sched_num,
                           unsigned int num_scheds, sync_t sync,
                           SIGNAL *sig_rate, SIGNAL *sig_thresh,
                           SIGNAL *sig_over, SIGNAL *sig_adj)
{
    unsigned int l1_sched = sched_num + NUM_L2_SCHEDS;
    uint32_t addr_adj     = TM_SHAPER_RATE_ADJUST_ADDR(nbi, l1_sched);
    uint32_t addr_over    = TM_SHAPER_MAX_OVERSHOOT_ADDR(nbi, l1_sched);
    uint32_t addr_rate    = TM_SHAPER_RATE_ADDR(nbi, l1_sched);
    uint32_t addr_thresh  = TM_SHAPER_THRESHOLD_ADDR(nbi, l1_sched);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L1_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L1_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(adjust, addr_adj, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_adj);
    __xpb_read(overshoot, addr_over, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_over);
    __xpb_read(rate, addr_rate, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_rate);
    __xpb_read(threshold, addr_thresh, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_thresh);
}

__intrinsic void
tm_config_l1_shaper_read(__xread void *rate, __xread void *threshold,
                         __xread void *overshoot, __xread void *adjust,
                         unsigned int nbi, unsigned int sched_num,
                         unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l1_shaper_read(rate, threshold, overshoot, adjust, nbi,
                               sched_num, num_scheds, ctx_swap, &sig, &sig,
                               &sig, &sig);
}

__intrinsic void
__tm_config_l1_shaper_write(__xwrite void *rate, __xwrite void *threshold,
                            __xwrite void *overshoot, __xwrite void *adjust,
                            unsigned int nbi, unsigned int sched_num,
                            unsigned int num_scheds, sync_t sync,
                            SIGNAL *sig_rate, SIGNAL *sig_thresh,
                            SIGNAL *sig_over, SIGNAL *sig_adj)
{
    unsigned int l1_sched = sched_num + NUM_L2_SCHEDS;
    uint32_t addr_adj     = TM_SHAPER_RATE_ADJUST_ADDR(nbi, l1_sched);
    uint32_t addr_over    = TM_SHAPER_MAX_OVERSHOOT_ADDR(nbi, l1_sched);
    uint32_t addr_rate    = TM_SHAPER_RATE_ADDR(nbi, l1_sched);
    uint32_t addr_thresh  = TM_SHAPER_THRESHOLD_ADDR(nbi, l1_sched);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L1_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L1_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(adjust, addr_adj, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_adj);
    __xpb_write(overshoot, addr_over, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_over);
    __xpb_write(rate, addr_rate, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_rate);
    __xpb_write(threshold, addr_thresh, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_thresh);
}

__intrinsic void
tm_config_l1_shaper_write(__xwrite void *rate, __xwrite void *threshold,
                          __xwrite void *overshoot, __xwrite void *adjust,
                          unsigned int nbi, unsigned int sched_num,
                          unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l1_shaper_write(rate, threshold, overshoot, adjust, nbi,
                                sched_num, num_scheds, ctx_swap, &sig, &sig,
                                &sig, &sig);
}

__intrinsic void
__tm_config_l2_dwrr_weight_read(__xread void *weight, unsigned int nbi,
                                unsigned int sched, unsigned int sched_port,
                                unsigned int num_sch_ports, sync_t sync,
                                SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_L2_WEIGHT_ADDR(nbi, sched, sched_port);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched < NUM_L2_SCHEDS);
    try_ctassert(sched_port < L1L2_SCHEDULER_INPUTS);
    try_ctassert((num_sch_ports >= 1) &&
                 (num_sch_ports <= L1L2_DWRR_BURST_SZ));
    try_ctassert((sched_port + num_sch_ports) <= L1L2_SCHEDULER_INPUTS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(weight, addr, num_sch_ports << 2, L1L2_DWRR_BURST_BYTES, sync,
               sig);
}

__intrinsic void
tm_config_l2_dwrr_weight_read(__xread void *weight, unsigned int nbi,
                              unsigned int sched, unsigned int sched_port,
                              unsigned int num_sch_ports)
{
    SIGNAL sig;

    __tm_config_l2_dwrr_weight_read(weight, nbi, sched, sched_port,
                                    num_sch_ports, ctx_swap, &sig);
}

__intrinsic void
__tm_config_l2_dwrr_weight_write(__xwrite void *weight, __xwrite void *deficit,
                                 unsigned int nbi, unsigned int sched,
                                 unsigned int sched_port,
                                 unsigned int num_sch_ports, sync_t sync,
                                 SIGNAL *sig_weight, SIGNAL *sig_deficit)
{
    uint32_t deficit_addr = TM_SCHED_L2_DEFICIT_ADDR(nbi, sched, sched_port);
    uint32_t weight_addr  = TM_SCHED_L2_WEIGHT_ADDR(nbi, sched, sched_port);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched < NUM_L2_SCHEDS);
    try_ctassert(sched_port < L1L2_SCHEDULER_INPUTS);
    try_ctassert((num_sch_ports >= 1) &&
                 (num_sch_ports <= L1L2_DWRR_BURST_SZ));
    try_ctassert((sched_port + num_sch_ports) <= L1L2_SCHEDULER_INPUTS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(weight, weight_addr, num_sch_ports << 2, L1L2_DWRR_BURST_BYTES,
                sync, sig_weight);
    __xpb_write(deficit, deficit_addr, num_sch_ports << 2,
                L1L2_DWRR_BURST_BYTES, sync, sig_deficit);
}

__intrinsic void
tm_config_l2_dwrr_weight_write(__xwrite void *weight, unsigned int nbi,
                               unsigned int sched, unsigned int sched_port,
                               unsigned int num_sch_ports)
{
    SIGNAL sig;
    __xwrite struct nfp_nbi_tm_scheduler_deficit deficit[L1L2_DWRR_BURST_SZ];

    reg_zero(deficit, sizeof(deficit));

    __tm_config_l2_dwrr_weight_write(weight, deficit, nbi, sched, sched_port,
                                     num_sch_ports, ctx_swap, &sig, &sig);
}

__intrinsic void
__tm_config_l2_sched_read(__xread void *config, unsigned int nbi,
                          unsigned int sched_num, unsigned int num_scheds,
                          sync_t sync, SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_CONFIG_ADDR(nbi, sched_num);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L2_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L2_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(config, addr, num_scheds << 2, CMD_BURST_BYTES, sync, sig);
}

__intrinsic void
tm_config_l2_sched_read(__xread void *config, unsigned int nbi,
                        unsigned int sched_num, unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l2_sched_read(config, nbi, sched_num, num_scheds, ctx_swap,
                              &sig);
}

__intrinsic void
__tm_config_l2_sched_write(__xwrite void *config, unsigned int nbi,
                           unsigned int sched_num, unsigned int num_scheds,
                           sync_t sync, SIGNAL *sig)
{
    uint32_t addr = TM_SCHED_CONFIG_ADDR(nbi, sched_num);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L2_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L2_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(config, addr, num_scheds << 2, CMD_BURST_BYTES, sync, sig);
}

__intrinsic void
tm_config_l2_sched_write(__xwrite void *config, unsigned int nbi,
                         unsigned int sched_num, unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l2_sched_write(config, nbi, sched_num, num_scheds, ctx_swap,
                               &sig);
}

__intrinsic void
__tm_config_l2_shaper_read(__xread void *rate, __xread void *threshold,
                           __xread void *overshoot, __xread void *adjust,
                           unsigned int nbi, unsigned int sched_num,
                           unsigned int num_scheds, sync_t sync,
                           SIGNAL *sig_rate, SIGNAL *sig_thresh,
                           SIGNAL *sig_over, SIGNAL *sig_adj)
{
    uint32_t addr_adj    = TM_SHAPER_RATE_ADJUST_ADDR(nbi, sched_num);
    uint32_t addr_over   = TM_SHAPER_MAX_OVERSHOOT_ADDR(nbi, sched_num);
    uint32_t addr_rate   = TM_SHAPER_RATE_ADDR(nbi, sched_num);
    uint32_t addr_thresh = TM_SHAPER_THRESHOLD_ADDR(nbi, sched_num);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L2_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L2_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_read(adjust, addr_adj, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_adj);
    __xpb_read(overshoot, addr_over, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_over);
    __xpb_read(rate, addr_rate, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_rate);
    __xpb_read(threshold, addr_thresh, num_scheds << 2, CMD_BURST_BYTES, sync,
               sig_thresh);
}

__intrinsic void
tm_config_l2_shaper_read(__xread void *rate, __xread void *threshold,
                         __xread void *overshoot, __xread void *adjust,
                         unsigned int nbi, unsigned int sched_num,
                         unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l2_shaper_read(rate, threshold, overshoot, adjust, nbi,
                               sched_num, num_scheds, ctx_swap, &sig, &sig,
                               &sig, &sig);
}

__intrinsic void
__tm_config_l2_shaper_write(__xwrite void *rate, __xwrite void *threshold,
                            __xwrite void *overshoot, __xwrite void *adjust,
                            unsigned int nbi, unsigned int sched_num,
                            unsigned int num_scheds, sync_t sync,
                            SIGNAL *sig_rate, SIGNAL *sig_thresh,
                            SIGNAL *sig_over, SIGNAL *sig_adj)
{
    uint32_t addr_adj    = TM_SHAPER_RATE_ADJUST_ADDR(nbi, sched_num);
    uint32_t addr_over   = TM_SHAPER_MAX_OVERSHOOT_ADDR(nbi, sched_num);
    uint32_t addr_rate   = TM_SHAPER_RATE_ADDR(nbi, sched_num);
    uint32_t addr_thresh = TM_SHAPER_THRESHOLD_ADDR(nbi, sched_num);

    try_ctassert(nbi < MAX_NBI_NUMBER);
    try_ctassert(sched_num < NUM_L2_SCHEDS);
    try_ctassert((num_scheds >= 1) && (num_scheds <= CMD_BURST_SZ));
    try_ctassert((sched_num + num_scheds) <= NUM_L2_SCHEDS);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    __xpb_write(adjust, addr_adj, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_adj);
    __xpb_write(overshoot, addr_over, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_over);
    __xpb_write(rate, addr_rate, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_rate);
    __xpb_write(threshold, addr_thresh, num_scheds << 2, CMD_BURST_BYTES, sync,
                sig_thresh);
}

__intrinsic void
tm_config_l2_shaper_write(__xwrite void *rate, __xwrite void *threshold,
                          __xwrite void *overshoot, __xwrite void *adjust,
                          unsigned int nbi, unsigned int sched_num,
                          unsigned int num_scheds)
{
    SIGNAL sig;

    __tm_config_l2_shaper_write(rate, threshold, overshoot, adjust, nbi,
                                sched_num, num_scheds, ctx_swap, &sig, &sig,
                                &sig, &sig);
}

static __inline int
nbi_tm_q_config__(uint32_t nbi, uint32_t qnum,
__xread struct nfp_nbi_tm_queue_config queue_config, uint32_t head_tail_sram)
{

    __gpr uint32_t addr_queue_config;
    int ret = 0;
    SIGNAL sig_queue_config;
    __xwrite uint32_t head_tail_sram_xfer[2];
    __gpr uint32_t ht_sram_hi;
    __gpr uint32_t ht_sram_lo;
    SIGNAL sig_head_tail_sram;
    __xwrite uint32_t queue_config_xw;

    if (qnum > MAX_TM_QUEUE_NUM) {
        ret = -1;
        goto out;
    }
    addr_queue_config = TM_QUEUE_CONFIG_ADDR(nbi, qnum);
    queue_config_xw = queue_config.__raw;

    head_tail_sram_xfer[0] = (head_tail_sram << 14 | head_tail_sram);
    head_tail_sram_xfer[1] = 0;
    ht_sram_hi = (NBI_TM_HEAD_TAIL_SRAM(nbi) >> 8);
    ht_sram_lo = qnum * sizeof(head_tail_sram_xfer);

    __asm {
        ct[xpb_write, *queue_config_xw, addr_queue_config, 0, 1], \
        sig_done[sig_queue_config]
        nbi[write, head_tail_sram_xfer[0], ht_sram_hi, <<8, ht_sram_lo, 1], \
        sig_done[sig_head_tail_sram]
        ctx_arb[sig_queue_config, sig_head_tail_sram]
    }

out:
    return ret;
}

static __inline int
nbi_tm_l0_scheduler_config__(uint32_t nbi,
                             __mem40 struct nfp_nbi_tm_l0_scheduler *l0_sched)
{
    __xread struct nfp_nbi_tm_scheduler_config config_xr;
    __xread struct nfp_nbi_tm_scheduler_weight weight_xr[CMD_BURST_SZ];
    __xwrite struct nfp_nbi_tm_scheduler_config config_xw;
    __xwrite struct nfp_nbi_tm_scheduler_deficit deficit_xw[CMD_BURST_SZ];
    __xwrite struct nfp_nbi_tm_scheduler_weight weight_xw[CMD_BURST_SZ];
    int i;
    int j;
    SIGNAL deficit_sig, weight_rd_sig, weight_wr_sig;
    int ret = 0;

    /* Check if configuring DWRR. */
    mem_read32(&config_xr, &(l0_sched->scheduler_config), sizeof(config_xr));

    if (config_xr.dwrrenable) {
        reg_zero(deficit_xw, sizeof(deficit_xw));

        /* Read in the L0 DWRR weights. */
        mem_read32(weight_xr, &(l0_sched->weight[0]), sizeof(weight_xr));

        for (i = 0; i < L0_SCHEDULER_INPUTS; i += CMD_BURST_SZ) {
            /* Configure the L0 DWRR weights and deficits. */
            for (j = 0; j < CMD_BURST_SZ; ++j) {
                weight_xw[j].__raw = weight_xr[j].__raw;
            }

            __tm_config_l0_dwrr_weight_write(weight_xw, deficit_xw, nbi, i,
                                             CMD_BURST_SZ, sig_done,
                                             &weight_wr_sig, &deficit_sig);

            /* Check if there are more L0 DWRR weights to configure. */
            if ((i + CMD_BURST_SZ) < L0_SCHEDULER_INPUTS) {
                __mem_read32(weight_xr, &(l0_sched->weight[i + CMD_BURST_SZ]),
                             sizeof(weight_xr), sizeof(weight_xr), sig_done,
                             &weight_rd_sig);

                __asm ctx_arb[deficit_sig, weight_rd_sig, weight_wr_sig];
            } else {
                __asm ctx_arb[deficit_sig, weight_wr_sig];
            }
        }
    }

    /* Configure the L0 scheduler. */
    config_xw.__raw = config_xr.__raw;
    tm_config_l0_sched_write(&config_xw, nbi);

out:
    return ret;
}

static __inline int
nbi_tm_l1l2_scheduler_config__(uint32_t nbi, uint32_t sched,
    __xread struct nfp_nbi_tm_scheduler_config scheduler_config,
    __xread struct nfp_nbi_tm_scheduler_weight weight[L1L2_SCHEDULER_INPUTS])
{
    int ret = 0;
    __xwrite uint32_t sched_config_xw;
    __xwrite uint32_t sched_weight_xw[L1L2_SCHEDULER_INPUTS];

    SIGNAL sched_sig, weight_sig, deficit_sig;

    if (sched > MAX_TM_L1_L2_SCHED_NUM) {
        ret = -1;
        goto out;
    }

    if (scheduler_config.dwrrenable) {
         __xwrite uint32_t sched_deficit_xw[L1L2_SCHEDULER_INPUTS] = \
                                            {0, 0, 0, 0, 0, 0, 0, 0};

        sched_config_xw = scheduler_config.__raw;
        sched_weight_xw[0] = weight[0].__raw;
        sched_weight_xw[1] = weight[1].__raw;
        sched_weight_xw[2] = weight[2].__raw;
        sched_weight_xw[3] = weight[3].__raw;
        sched_weight_xw[4] = weight[4].__raw;
        sched_weight_xw[5] = weight[5].__raw;
        sched_weight_xw[6] = weight[6].__raw;
        sched_weight_xw[7] = weight[7].__raw;

        if (sched < NUM_L2_SCHEDS) {
            __tm_config_l2_sched_write(&sched_config_xw, nbi, sched, 1,
                                       sig_done, &sched_sig);
            __tm_config_l2_dwrr_weight_write(sched_weight_xw, sched_deficit_xw,
                                             nbi, sched, 0,
                                             L1L2_SCHEDULER_INPUTS, sig_done,
                                             &weight_sig, &deficit_sig);
        } else {
            __tm_config_l1_sched_write(&sched_config_xw, nbi,
                                       sched - NUM_L2_SCHEDS, 1, sig_done,
                                       &sched_sig);
            __tm_config_l1_dwrr_weight_write(sched_weight_xw, sched_deficit_xw,
                                             nbi, sched - NUM_L2_SCHEDS, 0,
                                             L1L2_SCHEDULER_INPUTS, sig_done,
                                             &weight_sig, &deficit_sig);
        }

        __asm ctx_arb[deficit_sig, sched_sig, weight_sig];
    } else {
        if (sched < 128) {
            tm_config_l2_sched_write(&sched_config_xw, nbi, sched, 1);
        } else {
            tm_config_l1_sched_write(&sched_config_xw, nbi,
                                     sched - NUM_L2_SCHEDS, 1);
        }
    }

out:
    return ret;
}

static __inline int
nbi_tm_shaper_config__(uint32_t nbi, uint32_t shaper,
                    __xread struct nfp_nbi_tm_shaper * tm_shaper)
{
    int ret = 0;

    __xwrite uint32_t shaper_rate_xw;
    __xwrite uint32_t shaper_threshold_xw;
    __xwrite uint32_t max_overshoot_xw;
    __xwrite uint32_t rate_adjust_xw;
    SIGNAL sig_shaper_rate;
    SIGNAL sig_shaper_threshold;
    SIGNAL sig_max_overshoot;
    SIGNAL sig_rate_adjust;

    if (shaper > MAX_TM_SCHED_NUM) {
        ret = -1;
        goto out;
    }

    shaper_rate_xw = tm_shaper->shaper_rate.__raw;
    shaper_threshold_xw = tm_shaper->shaper_threshold.__raw;
    max_overshoot_xw = tm_shaper->max_overshoot.__raw;
    rate_adjust_xw = tm_shaper->rate_adjust.__raw;

    if (shaper < NUM_L2_SCHEDS) {
        __tm_config_l2_shaper_write(&shaper_rate_xw, &shaper_threshold_xw,
                                    &max_overshoot_xw, &rate_adjust_xw, nbi,
                                    shaper, 1, sig_done, &sig_shaper_rate,
                                    &sig_shaper_threshold, &sig_max_overshoot,
                                    &sig_rate_adjust);
    } else if (shaper < MAX_TM_L1_L2_SCHED_NUM) {
        __tm_config_l1_shaper_write(&shaper_rate_xw, &shaper_threshold_xw,
                                    &max_overshoot_xw, &rate_adjust_xw, nbi,
                                    shaper - NUM_L2_SCHEDS, 1, sig_done,
                                    &sig_shaper_rate, &sig_shaper_threshold,
                                    &sig_max_overshoot, &sig_rate_adjust);
    } else {
        __tm_config_l0_shaper_write(&shaper_rate_xw, &shaper_threshold_xw,
                                    &max_overshoot_xw, &rate_adjust_xw, nbi,
                                    sig_done, &sig_shaper_rate,
                                    &sig_shaper_threshold, &sig_max_overshoot,
                                    &sig_rate_adjust);
    }

    __asm ctx_arb[sig_shaper_rate, sig_shaper_threshold, sig_max_overshoot, \
                  sig_rate_adjust];

 out:
    return ret;
}

int
nfp_nbi_tm_config_schedulers(uint32_t nbi,
__mem40 struct nfp_nbi_tm_l1l2_scheduler_cluster \
tm_scheduler_cluster[TM_INIT_MAX_SCHEDULER_CLUSTERS])
{

    __gpr uint32_t i;
    __gpr uint32_t sched;
    __xread struct nfp_nbi_tm_l1l2_scheduler_cluster scheduler_cluster;
    int ret = 0;

    if (nbi > MAX_NBI_NUMBER) {
        ret = -1;
        goto out;
    }

    for (i = 0; i < TM_INIT_MAX_SCHEDULER_CLUSTERS; i++) {

        mem_read32(&scheduler_cluster, &tm_scheduler_cluster[i],
                    sizeof(scheduler_cluster));

        if (scheduler_cluster.in_use) {
            for (sched = scheduler_cluster.start;
                sched <= scheduler_cluster.end; sched++) {

                ret = nbi_tm_l1l2_scheduler_config__(nbi, sched,
                    scheduler_cluster.scheduler_config,
                    &scheduler_cluster.weight[0]);

                if(ret)
                    goto out;
            }
        }

        if (scheduler_cluster.last)
            break;
    }

out:
    return ret;
}

int
nfp_nbi_tm_config_l0_scheduler(unsigned int nbi,
                               __mem40 struct nfp_nbi_tm_l0_scheduler \
                               *tm_l0_scheduler)
{
    __xread int in_use_xr;
    int ret = 0;

    if (nbi > MAX_NBI_NUMBER) {
        ret = -1;
        goto out;
    }

    /* Check if skipping the L0 scheduler configuration. */
    mem_read32(&in_use_xr, &(tm_l0_scheduler->in_use), sizeof(in_use_xr));

    if (!in_use_xr)
        goto out;

    ret = nbi_tm_l0_scheduler_config__(nbi, tm_l0_scheduler);

out:
    return ret;
}

int nbi_tm_config_shapers(uint32_t nbi,
__mem40 struct nfp_nbi_tm_shaper_cluster \
tm_shaper_cluster[TM_INIT_MAX_SCHEDULER_CLUSTERS])
{

    __gpr uint32_t i;
    __gpr uint32_t shaper;
    __xread struct nfp_nbi_tm_shaper_cluster shaper_cluster;
    int ret = 0;

    if (nbi > MAX_NBI_NUMBER) {
        ret = -1;
        goto out;
    }

    for (i = 0; i < TM_INIT_MAX_SCHEDULER_CLUSTERS; i++) {

        mem_read32(&shaper_cluster, &tm_shaper_cluster[i],
                    sizeof(shaper_cluster));

        if (shaper_cluster.in_use) {
            for (shaper = shaper_cluster.start;
                shaper <= shaper_cluster.end; shaper++) {

                ret = nbi_tm_shaper_config__(nbi, shaper, &shaper_cluster.shaper);

                if(ret)
                    goto out;

            }
        }
        if (shaper_cluster.last)
            break;
    }

out:
    return ret;
}


int
nfp_nbi_tm_config_queues(uint32_t nbi,
        __mem40 struct nfp_nbi_tm_queue_cluster \
        tm_queue_cluster[TM_INIT_MAX_Q_CLUSTERS])
{

    __gpr uint32_t i;
    int ret = 0;
    __gpr uint32_t                              qnum;
    __xread struct nfp_nbi_tm_queue_cluster     queue_cluster;
    __gpr uint32_t                              head_tail_sram = 0;
    __gpr uint32_t                              queue_size;
    __gpr uint32_t                              shf_val;

    if (nbi > MAX_NBI_NUMBER) {
        ret = -1;
        goto out;
    }

    for (i = 0; i < TM_INIT_MAX_Q_CLUSTERS; i++) {

        mem_read32(&queue_cluster, &tm_queue_cluster[i],
                   sizeof(queue_cluster));

        if (queue_cluster.in_use) {
            queue_size = queue_cluster.queue_config.queuesize;

            /*There is no need to check the MAX and the MAX is 16 and queuesize
            is a 4 bit field*/
            if(queue_size < QUEUE_SIZE_MIN) {
                ret = -1;
                goto out;
            }
            for (qnum = queue_cluster.start;
                qnum <= queue_cluster.end; qnum++) {

                /* To prevent HEAD TAIL SRAM form overwriting the adjacent
                queue's HTSRAM, the initial values are set as a multiple of
                the queue size*/
                shf_val = head_tail_sram >> queue_size;
                if (head_tail_sram & ((1 << queue_size) - 1)) {
                    shf_val = shf_val + 1;
                }
                head_tail_sram = shf_val << queue_size;

                /* To prevent HEAD TAIL SRAM wrapping, a queue and its
                corresponding HT SRAM is only configured if the intended HT
                SRAM value + 2^queue_size will not exceed the MAX value.
                If this happens, the queue config will abort and the function
                will return an error*/
                if((head_tail_sram + (1 << queue_size)) <= HEAD_TAIL_SRAM_MAX) {
                    ret = nbi_tm_q_config__(nbi, qnum,
                        queue_cluster.queue_config,
                        head_tail_sram);
                    if(ret)
                        goto out;

                } else {
                    ret = -1;
                    goto out;
                }

                /* The next head tail SRAM entry has to start 2^queue_size after
                the previous entry*/
                head_tail_sram = head_tail_sram + (1 << queue_size);
            }
        }

        if (queue_cluster.last)
            break;
    }

out:
    return ret;
}

__intrinsic int
nfp_nbi_tm_config(uint32_t nbi,
            struct nfp_nbi_tm_traffic_manager_config traffic_manager_config)
{
    __gpr uint32_t addr;
    int ret = 0;

    if (nbi > MAX_NBI_NUMBER) {
        ret = -1;
        goto out;
    }

    addr = TM_CONFIG_REG_ADDR(nbi);

    xpb_write(addr, traffic_manager_config.__raw);
out:
    return ret;
}
