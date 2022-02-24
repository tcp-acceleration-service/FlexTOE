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
 * @file          lib/nfp/tm_config.h
 * @brief         NBI Traffic Manager Configuration library
 */

/*
 * NOTE: This does not configure the entire TM. Complete TM initialisation must
 * still be done with libraries provided in me/blocks/init/_uc.
 * The purpose of this library is to enable partial reconfigure during run-time
 * like in a QoS type use case.
 *
 * Simplified usage guidelines:
 *
 * 1. At any point during run time, stop transmission to the TM. (the specific
 * mechanism depends on the app). Recommended by not necessary.
 * 2. Disable TM queues that will reconfigured.
 * 3. Wait for TM queues to drain (can wait or poll CSR).
 * 4. Use API's provided, e.g:
 *  nbi_tm_config_shapers(0, &tm_init_spec_ptr->tm_shapers_cluster[0]);
 *  nfp_nbi_tm_config_schedulers(0, &tm_init_spec_ptr->tm_scheduler_cluster[0]);
 *  nfp_nbi_tm_config(0, tm_init_spec_ptr->tm_config);
 *  nfp_nbi_tm_config_queues(0, &tm_init_spec_ptr->tm_queue_cluster[0]);
 * 5. Queues are configured last since this will enable the queues again.
 * 6. Re-enable traffic to TM (if stopped).
 */

#ifndef TM_CONFIG_H
#define TM_CONFIG_H

#include <nfp.h>
#include <nfp6000/nfp_nbi_tm.h>

#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_LEVELCHECKENABLE_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_DESCQUEUESENABLE_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SEQUENCER0ENABLE_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_ERR_POISON_ENA_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SCHEDULERENABLE_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_SHAPERENABLE_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_MINIPACKETFCENABLE_Enabled (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_CHANNELLEVELSELECT_Level_1_sel (1)
#define     NFP_NBI_TM_TRAFFIC_MANAGER_CONFIG_L1INPUTSELECT_L1_Enabled (1)

#define     NFP_NBI_TM_QUEUE_CONFIG_DROPENABLE_Enabled       (1)
#define     NFP_NBI_TM_QUEUE_CONFIG_QUEUEENABLE_Enabled      (1)

#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP1ENABLE_Enabled    (1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_SP0ENABLE_Enabled    (1)
#define     NFP_NBI_TM_SCHEDULER_CONFIG_DWRRENABLE_Enabled   (1)

#ifndef TM_INIT_MAX_Q_CLUSTERS
#define TM_INIT_MAX_Q_CLUSTERS            24
#endif
#ifndef TM_INIT_MAX_SHAPER_CLUSTERS
#define TM_INIT_MAX_SHAPER_CLUSTERS       24
#endif
#ifndef TM_INIT_MAX_SCHEDULER_CLUSTERS
#define TM_INIT_MAX_SCHEDULER_CLUSTERS       24
#endif

#define L0_SCHEDULER_INPUTS                 128
#define L1L2_SCHEDULER_INPUTS               8

/**
 * NBI TM Queue Configuration
 *
 * This structure contains a range of TM Queue to initialise from and
 * including queue number "start" up to and including queue number "end"
 */
struct nfp_nbi_tm_queue_cluster {
    unsigned int                            in_use; /** < If 0, this range will be ignored*/
    unsigned int                            start; /** < First queue */
    unsigned int                            end; /** < Last queue */
    unsigned int                            last; /** < Last cluster in array */
    struct nfp_nbi_tm_queue_config          queue_config; /** < TrafficManager.TMQueueReg.QueueConfigX */
}__declspec(packed);

/**
 * NBI TM Shaper registers
 *
 * This structure contains all the registers pertaining to a specific shapers.
 */
struct nfp_nbi_tm_shaper {
    struct nfp_nbi_tm_shaper_rate           shaper_rate; /** < TrafficManager.TMShaperReg.ShaperRateX */
    struct nfp_nbi_tm_shaper_threshold      shaper_threshold; /** < TrafficManager.TMShaperReg.ShaperThresholdX */
    struct nfp_nbi_tm_shaper_max_overshoot  max_overshoot; /** < TrafficManager.TMShaperReg.ShaperMaxOvershootX */
    struct nfp_nbi_tm_shaper_rate_adjust    rate_adjust; /** < TrafficManager.TMShaperReg.ShaperRateAdjustX*/
}__declspec(packed);

/**
 * NBI TM Shaper Configuration
 *
 * This structure contains a range of TM shapers to initialise from and
 * including shaper number "start" up to and including shaper number "end"
 */
struct nfp_nbi_tm_shaper_cluster {
    unsigned int                            in_use; /** < If 0, this range will be ignored*/
    unsigned int                            start; /** < First shaper */
    unsigned int                            end; /** < Last shaper */
    unsigned int                            last; /** < Last cluster in array */
    struct nfp_nbi_tm_shaper                shaper; /** < See struct nfp_nbi_tm_shaper*/
}__declspec(packed);

/**
 * NBI TM L0 Scheduler Configuration
 */
struct nfp_nbi_tm_l0_scheduler {
    /** If 0, this config will be ignored. */
    unsigned int                        in_use;
    /** L0 scheduler config register settings. */
    struct nfp_nbi_tm_scheduler_config  scheduler_config;
    /** DWRR weights; will be ignored if scheduler is not in DWRR mode. */
    struct nfp_nbi_tm_scheduler_weight  weight[L0_SCHEDULER_INPUTS];
}__declspec(packed);

/**
 * NBI TM Scheduler Configuration
 *
 * This structure contains a range of TM schedulers to initialise from and
 * including schedulers number "start" up to and including schedulers number "end"
 */
struct nfp_nbi_tm_l1l2_scheduler_cluster {
    unsigned int                            in_use; /** < If 0, this range will be ignored*/
    unsigned int                            start; /** < First scheduler */
    unsigned int                            end; /** < Last scheduler */
    unsigned int                            last; /** < Last cluster in array */
    struct nfp_nbi_tm_scheduler_config      scheduler_config; /** < Will be written to TrafficManagerReg.TrafficManager.TMSchedulerReg.SchedulerConfigX */
    struct nfp_nbi_tm_scheduler_weight      weight[L1L2_SCHEDULER_INPUTS]; /** < DWRR weights. Will be ignored if scheduler is not in DWRR mode*/
}__declspec(packed);

/**
 * NBI TM Configuration specification
 *
 * This structure contains all the information needed to re-initialise the NBI
 * Traffic Manager. It has info for the TM config register, the queues  the
 * shaper and the schedulers.
 */
struct nfp_nbi_tm_config_spec {
    struct nfp_nbi_tm_traffic_manager_config \
        tm_config; /* Will be written to TrafficManagerReg.TrafficManagerConfig */
    struct nfp_nbi_tm_queue_cluster \
        tm_queue_cluster[TM_INIT_MAX_Q_CLUSTERS]; /* An array of queue ranges. All the queues in all the ranges will be initialised according to struct nfp_nbi_tm_queue_cluster */
    struct nfp_nbi_tm_shaper_cluster \
        tm_shapers_cluster[TM_INIT_MAX_SHAPER_CLUSTERS]; /* An array of shaper ranges. All the shapers in all the ranges will be initialised according to struct nfp_nbi_tm_shaper_cluster */
    struct nfp_nbi_tm_l1l2_scheduler_cluster \
        tm_scheduler_cluster[TM_INIT_MAX_SCHEDULER_CLUSTERS]; /* An array of scheduler ranges. All the schedulers in all the ranges will be initialised according to struct nfp_nbi_tm_l1l2_scheduler_cluster */
    /** NBI TM L0 scheduler configuration. */
    struct nfp_nbi_tm_l0_scheduler tm_l0_scheduler;
}__declspec(packed);


/**
 * Reads the DWRR weights for one or a series of TM level 0 scheduler ports.
 * @param weight         Pointer to read transfer register to store weight
 * @param nbi            NBI to read from (0-1)
 * @param sched_port     Scheduler port number (0-127)
 * @param num_sch_ports  Number of consecutive scheduler ports to read (1-16)
 * @param sync           Type of synchronization (sig_done or ctx_swap)
 * @param sig            Signal to use
 */
__intrinsic void __tm_config_l0_dwrr_weight_read(__xread void *weight,
                                                 unsigned int nbi,
                                                 unsigned int sched_port,
                                                 unsigned int num_sch_ports,
                                                 sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l0_dwrr_weight_read(__xread void *weight,
                                               unsigned int nbi,
                                               unsigned int sched_port,
                                               unsigned int num_sch_ports);

/**
 * Writes the DWRR weights for one or a series of TM level 0 scheduler ports.
 * @param weight         Pointer to write transfer register to load weight
 * @param deficit        Pointer to write transfer register to load deficit
 * @param nbi            NBI to write to (0-1)
 * @param sched_port     Scheduler port number (0-127)
 * @param num_sch_ports  Number of scheduler ports to write to (1-16)
 * @param sync           Type of synchronization (sig_done or ctx_swap)
 * @param sig_weight     Signal to use for weight write transfer register(s)
 * @param sig_deficit    Signal to use for deficit write transfer register(s)
 *
 * @note If deficit is not specified, deficit values are set to zero by default.
 */
__intrinsic void __tm_config_l0_dwrr_weight_write(__xwrite void *weight,
                                                  __xwrite void *deficit,
                                                  unsigned int nbi,
                                                  unsigned int sched_port,
                                                  unsigned int num_sch_ports,
                                                  sync_t sync,
                                                  SIGNAL *sig_weight,
                                                  SIGNAL *sig_deficit);

__intrinsic void tm_config_l0_dwrr_weight_write(__xwrite void *weight,
                                                unsigned int nbi,
                                                unsigned int sched_port,
                                                unsigned int num_sch_ports);

/**
 * Reads the configuration of the TM L0 scheduler.
 * @param config  Pointer to read transfer register to store config
 * @param nbi     NBI to read from (0-1)
 * @param sync    Type of synchronization (sig_done or ctx_swap)
 * @param sig     Signal to use
 */
__intrinsic void __tm_config_l0_sched_read(__xread void *config,
                                           unsigned int nbi, sync_t sync,
                                           SIGNAL *sig);

__intrinsic void tm_config_l0_sched_read(__xread void *config,
                                         unsigned int nbi);

/**
 * Writes the configuration of the TM L0 scheduler.
 * @param config  Pointer to write transfer register to load config
 * @param nbi     NBI to write to (0-1)
 * @param sync    Type of synchronization (sig_done or ctx_swap)
 * @param sig     Signal to use
 */
__intrinsic void __tm_config_l0_sched_write(__xwrite void *config,
                                            unsigned int nbi, sync_t sync,
                                            SIGNAL *sig);

__intrinsic void tm_config_l0_sched_write(__xwrite void *config,
                                          unsigned int nbi);

/**
 * Reads the configuration of the TM L0 shaper.
 * @param rate        Pointer to read transfer register to store rate
 * @param threshold   Pointer to read transfer register to store threshold
 * @param overshoot   Pointer to read transfer register to store max overshoot
 * @param adjust      Pointer to read transfer register to store rate adjust
 * @param nbi         NBI to read from (0-1)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig_rate    Signal to use for shaper rate
 * @param sig_thresh  Signal to use for shaper threshold
 * @param sig_over    Signal to use for shaper overshoot
 * @param sig_adj     Signal to use for shaper rate adjust
 */
__intrinsic void __tm_config_l0_shaper_read(__xread void *rate,
                                            __xread void *threshold,
                                            __xread void *overshoot,
                                            __xread void *adjust,
                                            unsigned int nbi, sync_t sync,
                                            SIGNAL *sig_rate,
                                            SIGNAL *sig_thresh,
                                            SIGNAL *sig_over, SIGNAL *sig_adj);

__intrinsic void tm_config_l0_shaper_read(__xread void *rate,
                                          __xread void *threshold,
                                          __xread void *overshoot,
                                          __xread void *adjust,
                                          unsigned int nbi);

/**
 * Writes the configuration of the TM L0 shaper.
 * @param rate        Pointer to write transfer register to load rate
 * @param threshold   Pointer to write transfer register to load threshold
 * @param overshoot   Pointer to write transfer register to load max overshoot
 * @param adjust      Pointer to write transfer register to load rate adjust
 * @param nbi         NBI to write to (0-1)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig_rate    Signal to use for shaper rate
 * @param sig_thresh  Signal to use for shaper threshold
 * @param sig_over    Signal to use for shaper overshoot
 * @param sig_adj     Signal to use for shaper rate adjust
 */
__intrinsic void __tm_config_l0_shaper_write(__xwrite void *rate,
                                             __xwrite void *threshold,
                                             __xwrite void *overshoot,
                                             __xwrite void *adjust,
                                             unsigned int nbi, sync_t sync,
                                             SIGNAL *sig_rate,
                                             SIGNAL *sig_thresh,
                                             SIGNAL *sig_over,
                                             SIGNAL *sig_adj);

__intrinsic void tm_config_l0_shaper_write(__xwrite void *rate,
                                           __xwrite void *threshold,
                                           __xwrite void *overshoot,
                                           __xwrite void *adjust,
                                           unsigned int nbi);

/**
 * Reads the DWRR weights for one or a series of TM level 1 scheduler ports.
 * @param weight         Pointer to read transfer register to store weight
 * @param nbi            NBI to read from (0-1)
 * @param sched          Scheduler number (0-15)
 * @param sched_port     Scheduler port number (0-7)
 * @param num_sch_ports  Number of consecutive scheduler ports to read (1-8)
 * @param sync           Type of synchronization (sig_done or ctx_swap)
 * @param sig            Signal to use
 */
__intrinsic void __tm_config_l1_dwrr_weight_read(__xread void *weight,
                                                 unsigned int nbi,
                                                 unsigned int sched,
                                                 unsigned int sched_port,
                                                 unsigned int num_sch_ports,
                                                 sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l1_dwrr_weight_read(__xread void *weight,
                                               unsigned int nbi,
                                               unsigned int sched,
                                               unsigned int sched_port,
                                               unsigned int num_sch_ports);

/**
 * Writes the DWRR weights for one or a series of TM level 1 scheduler ports.
 * @param weight         Pointer to write transfer register to load weight
 * @param deficit        Pointer to write transfer register to load deficit
 * @param nbi            NBI to write to (0-1)
 * @param sched          Scheduler number (0-127)
 * @param sched_port     Scheduler port number (0-7)
 * @param num_sch_ports  Number of scheduler ports to write to (1-8)
 * @param sync           Type of synchronization (sig_done or ctx_swap)
 * @param sig_weight     Signal to use for weight write transfer register(s)
 * @param sig_deficit    Signal to use for deficit write transfer register(s)
 *
 * @note If deficit is not specified, deficit values are set to zero by default.
 */
__intrinsic void __tm_config_l1_dwrr_weight_write(__xwrite void *weight,
                                                  __xwrite void *deficit,
                                                  unsigned int nbi,
                                                  unsigned int sched,
                                                  unsigned int sched_port,
                                                  unsigned int num_sch_ports,
                                                  sync_t sync,
                                                  SIGNAL *sig_weight,
                                                  SIGNAL *sig_deficit);

__intrinsic void tm_config_l1_dwrr_weight_write(__xwrite void *weight,
                                                unsigned int nbi,
                                                unsigned int sched,
                                                unsigned int sched_port,
                                                unsigned int num_sch_ports);

/**
 * Reads the configuration of one or a series of TM L1 schedulers.
 * @param config      Pointer to read transfer register to store config
 * @param nbi         NBI to read from (0-1)
 * @param sched_num   Scheduler number (0-15)
 * @param num_scheds  Number of consecutive schedulers to read (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 */
__intrinsic void __tm_config_l1_sched_read(__xread void *config,
                                           unsigned int nbi,
                                           unsigned int sched_num,
                                           unsigned int num_scheds,
                                           sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l1_sched_read(__xread void *config,
                                         unsigned int nbi,
                                         unsigned int sched_num,
                                         unsigned int num_scheds);

/**
 * Writes the configuration of one or a series of TM L1 schedulers.
 * @param config      Pointer to write transfer register to load config
 * @param nbi         NBI to write to (0-1)
 * @param sched_num   Scheduler number (0-15)
 * @param num_scheds  Number of consecutive schedulers to write (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 */
__intrinsic void __tm_config_l1_sched_write(__xwrite void *config,
                                            unsigned int nbi,
                                            unsigned int sched_num,
                                            unsigned int num_scheds,
                                            sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l1_sched_write(__xwrite void *config,
                                          unsigned int nbi,
                                          unsigned int sched_num,
                                          unsigned int num_scheds);

/**
 * Reads the configuration of one or a series of TM L1 shapers.
 * @param rate        Pointer to read transfer registers to store rate
 * @param threshold   Pointer to read transfer registers to store threshold
 * @param overshoot   Pointer to read transfer registers to store max overshoot
 * @param adjust      Pointer to read transfer registers to store rate adjust
 * @param nbi         NBI to read from (0-1)
 * @param sched_num   Scheduler number (0-15)
 * @param num_scheds  Number of consecutive shapers to read (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig_rate    Signal to use for shaper rate
 * @param sig_thresh  Signal to use for shaper threshold
 * @param sig_over    Signal to use for shaper overshoot
 * @param sig_adj     Signal to use for shaper rate adjust
 */
__intrinsic void __tm_config_l1_shaper_read(__xread void *rate,
                                            __xread void *threshold,
                                            __xread void *overshoot,
                                            __xread void *adjust,
                                            unsigned int nbi,
                                            unsigned int sched_num,
                                            unsigned int num_scheds,
                                            sync_t sync, SIGNAL *sig_rate,
                                            SIGNAL *sig_thresh,
                                            SIGNAL *sig_over, SIGNAL *sig_adj);

__intrinsic void tm_config_l1_shaper_read(__xread void *rate,
                                          __xread void *threshold,
                                          __xread void *overshoot,
                                          __xread void *adjust,
                                          unsigned int nbi,
                                          unsigned int sched_num,
                                          unsigned int num_scheds);

/**
 * Writes the configuration of one or a series of TM L1 shapers.
 * @param rate        Pointer to write transfer registers to load rate
 * @param threshold   Pointer to write transfer registers to load threshold
 * @param overshoot   Pointer to write transfer registers to load max overshoot
 * @param adjust      Pointer to write transfer registers to load rate adjust
 * @param nbi         NBI to write to (0-1)
 * @param sched_num   Scheduler number (0-15)
 * @param num_scheds  Number of consecutive shapers to write to (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig_rate    Signal to use for shaper rate
 * @param sig_thresh  Signal to use for shaper threshold
 * @param sig_over    Signal to use for shaper overshoot
 * @param sig_adj     Signal to use for shaper rate adjust
 */
__intrinsic void __tm_config_l1_shaper_write(__xwrite void *rate,
                                             __xwrite void *threshold,
                                             __xwrite void *overshoot,
                                             __xwrite void *adjust,
                                             unsigned int nbi,
                                             unsigned int sched_num,
                                             unsigned int num_scheds,
                                             sync_t sync, SIGNAL *sig_rate,
                                             SIGNAL *sig_thresh,
                                             SIGNAL *sig_over,
                                             SIGNAL *sig_adj);

__intrinsic void tm_config_l1_shaper_write(__xwrite void *rate,
                                           __xwrite void *threshold,
                                           __xwrite void *overshoot,
                                           __xwrite void *adjust,
                                           unsigned int nbi,
                                           unsigned int sched_num,
                                           unsigned int num_scheds);

/**
 * Reads the DWRR weights for one or a series of TM level 2 scheduler ports.
 * @param weight         Pointer to read transfer register to store weight
 * @param nbi            NBI to read from (0-1)
 * @param sched          Scheduler number (0-127)
 * @param sched_port     Scheduler port number (0-7)
 * @param num_sch_ports  Number of consecutive scheduler ports to read (1-8)
 * @param sync           Type of synchronization (sig_done or ctx_swap)
 * @param sig            Signal to use
 */
__intrinsic void __tm_config_l2_dwrr_weight_read(__xread void *weight,
                                                 unsigned int nbi,
                                                 unsigned int sched,
                                                 unsigned int sched_port,
                                                 unsigned int num_sch_ports,
                                                 sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l2_dwrr_weight_read(__xread void *weight,
                                               unsigned int nbi,
                                               unsigned int sched,
                                               unsigned int sched_port,
                                               unsigned int num_sch_ports);

/**
 * Writes the DWRR weights for one or a series of TM level 2 scheduler ports.
 * @param weight         Pointer to write transfer register to load weight
 * @param deficit        Pointer to write transfer register to load deficit
 * @param nbi            NBI to write to (0-1)
 * @param sched          Scheduler number (0-15)
 * @param sched_port     Scheduler port number (0-7)
 * @param num_sch_ports  Number of scheduler ports to write to (1-8)
 * @param sync           Type of synchronization (sig_done or ctx_swap)
 * @param sig_weight     Signal to use for weight write transfer register(s)
 * @param sig_deficit    Signal to use for deficit write transfer register(s)
 *
 * @note If deficit is not specified, deficit values are set to zero by default.
 */
__intrinsic void __tm_config_l2_dwrr_weight_write(__xwrite void *weight,
                                                  __xwrite void *deficit,
                                                  unsigned int nbi,
                                                  unsigned int sched,
                                                  unsigned int sched_port,
                                                  unsigned int num_sch_ports,
                                                  sync_t sync,
                                                  SIGNAL *sig_weight,
                                                  SIGNAL *sig_deficit);

__intrinsic void tm_config_l2_dwrr_weight_write(__xwrite void *weight,
                                                unsigned int nbi,
                                                unsigned int sched,
                                                unsigned int sched_port,
                                                unsigned int num_sch_ports);

/**
 * Reads the configuration of one or a series of TM L2 schedulers.
 * @param config      Pointer to read transfer register to store config
 * @param nbi         NBI to read from (0-1)
 * @param sched_num   Scheduler number (0-127)
 * @param num_scheds  Number of consecutive schedulers to read (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 */
__intrinsic void __tm_config_l2_sched_read(__xread void *config,
                                           unsigned int nbi,
                                           unsigned int sched_num,
                                           unsigned int num_scheds,
                                           sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l2_sched_read(__xread void *config,
                                         unsigned int nbi,
                                         unsigned int sched_num,
                                         unsigned int num_scheds);

/**
 * Writes the configuration of one or a series of TM L2 schedulers.
 * @param config      Pointer to write transfer register to load config
 * @param nbi         NBI to write to (0-1)
 * @param sched_num   Scheduler number (0-127)
 * @param num_scheds  Number of consecutive schedulers to write (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 */
__intrinsic void __tm_config_l2_sched_write(__xwrite void *config,
                                            unsigned int nbi,
                                            unsigned int sched_num,
                                            unsigned int num_scheds,
                                            sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l2_sched_write(__xwrite void *config,
                                          unsigned int nbi,
                                          unsigned int sched_num,
                                          unsigned int num_scheds);

/**
 * Writes the configuration of one or a series of TM L1 schedulers.
 * @param config      Pointer to write transfer register to load config
 * @param nbi         NBI to write to (0-1)
 * @param sched_num   Scheduler number (0-127)
 * @param num_scheds  Number of consecutive schedulers to write (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 */
__intrinsic void __tm_config_l1_sched_write(__xwrite void *config,
                                            unsigned int nbi,
                                            unsigned int sched_num,
                                            unsigned int num_scheds,
                                            sync_t sync, SIGNAL *sig);

__intrinsic void tm_config_l1_sched_write(__xwrite void *config,
                                          unsigned int nbi,
                                          unsigned int sched_num,
                                          unsigned int num_scheds);

/**
 * Reads the configuration of one or a series of TM L2 shapers.
 * @param rate        Pointer to read transfer registers to store rate
 * @param threshold   Pointer to read transfer registers to store threshold
 * @param overshoot   Pointer to read transfer registers to store max overshoot
 * @param adjust      Pointer to read transfer registers to store rate adjust
 * @param nbi         NBI to read from (0-1)
 * @param sched_num   Scheduler number (0-127)
 * @param num_scheds  Number of consecutive shapers to read (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig_rate    Signal to use for shaper rate
 * @param sig_thresh  Signal to use for shaper threshold
 * @param sig_over    Signal to use for shaper overshoot
 * @param sig_adj     Signal to use for shaper rate adjust
 */
__intrinsic void __tm_config_l2_shaper_read(__xread void *rate,
                                            __xread void *threshold,
                                            __xread void *overshoot,
                                            __xread void *adjust,
                                            unsigned int nbi,
                                            unsigned int sched_num,
                                            unsigned int num_scheds,
                                            sync_t sync, SIGNAL *sig_rate,
                                            SIGNAL *sig_thresh,
                                            SIGNAL *sig_over, SIGNAL *sig_adj);

__intrinsic void tm_config_l2_shaper_read(__xread void *rate,
                                          __xread void *threshold,
                                          __xread void *overshoot,
                                          __xread void *adjust,
                                          unsigned int nbi,
                                          unsigned int sched_num,
                                          unsigned int num_scheds);

/**
 * Writes the configuration of one or a series of TM L2 shapers.
 * @param rate        Pointer to write transfer registers to load rate
 * @param threshold   Pointer to write transfer registers to load threshold
 * @param overshoot   Pointer to write transfer registers to load max overshoot
 * @param adjust      Pointer to write transfer registers to load rate adjust
 * @param nbi         NBI to write to (0-1)
 * @param sched_num   Scheduler number (0-15)
 * @param num_scheds  Number of consecutive shapers to write to (1-16)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig_rate    Signal to use for shaper rate
 * @param sig_thresh  Signal to use for shaper threshold
 * @param sig_over    Signal to use for shaper overshoot
 * @param sig_adj     Signal to use for shaper rate adjust
 */
__intrinsic void __tm_config_l2_shaper_write(__xwrite void *rate,
                                             __xwrite void *threshold,
                                             __xwrite void *overshoot,
                                             __xwrite void *adjust,
                                             unsigned int nbi,
                                             unsigned int sched_num,
                                             unsigned int num_scheds,
                                             sync_t sync, SIGNAL *sig_rate,
                                             SIGNAL *sig_thresh,
                                             SIGNAL *sig_over,
                                             SIGNAL *sig_adj);

__intrinsic void tm_config_l2_shaper_write(__xwrite void *rate,
                                           __xwrite void *threshold,
                                           __xwrite void *overshoot,
                                           __xwrite void *adjust,
                                           unsigned int nbi,
                                           unsigned int sched_num,
                                           unsigned int num_scheds);

/**
 * Configure the NBI Traffic Manager shapers
 * @param nbi               NBI number
 * @param tm_shaper_cluster[] Pointer to an array of struct nfp_nbi_tm_shaper_cluster
 * @return 0 if successful, -1 if the NBI number or a shaper number is out of
 * range
 *
 * This function takes a pointer to an array of struct nfp_nbi_tm_shaper_cluster
 * and configures all the shapers in the ranges found to be in use.
 */
int nbi_tm_config_shapers(unsigned int nbi,
                        __mem40 struct nfp_nbi_tm_shaper_cluster \
                        tm_shaper_cluster[TM_INIT_MAX_SHAPER_CLUSTERS]);

/**
 * Configure the NBI Traffic Manager schedulers
 * @param nbi                   NBI number
 * @param tm_scheduler_cluster[]  Pointer to an array of struct nfp_nbi_tm_l1l2_scheduler_cluster
 * @return 0 if successful, -1 if the NBI number or a schedulers number is out of
 * range
 *
 * This function takes a pointer to an array of struct nfp_nbi_tm_l1l2_scheduler_cluster
 * and configures all the schedulers in the ranges found to be in use.
 */
int nfp_nbi_tm_config_schedulers(unsigned int nbi,
                        __mem40 struct nfp_nbi_tm_l1l2_scheduler_cluster \
                        tm_scheduler_cluster[TM_INIT_MAX_SCHEDULER_CLUSTERS]);

/**
 * Configure the NBI Traffic Manager L0 scheduler.
 *
 * @param nbi              NBI number (0 or 1)
 * @param tm_l0_scheduler  Pointer to NBI TM L0 scheduler config info.
 * @return 0 if successful, or -1 if the NBI number is out-of-range
 */
int nfp_nbi_tm_config_l0_scheduler(unsigned int nbi,
                                   __mem40 struct nfp_nbi_tm_l0_scheduler \
                                   *tm_l0_scheduler);

/**
 * Configures the NBI Traffic Manager queues
 * @param nbi                   NBI number
 * @param tm_queue_cluster[]  Pointer to an array of struct nfp_nbi_tm_queue_cluster
 * @return 0 if successful, -1 if the NBI number or a schedulers number is out of
 * range. Or if the HeadTail SRAM went out of range.
 *
 * This function takes a pointer to an array of struct nfp_nbi_tm_queue_cluster
 * and configures all the queues in the ranges found to be in use.
 * Additionally the HTSRAM is initialised according to the databook. If the
 * queue sizes provided in the struct are too large, and the next HTSRAM value
 * will point outside of the 16K TM descriptor RAM, the function terminate early
 * and return -1
 */
int nfp_nbi_tm_config_queues(unsigned int nbi,
    __mem40 struct nfp_nbi_tm_queue_cluster \
    tm_queue_cluster[TM_INIT_MAX_Q_CLUSTERS]);

/**
 * Configures the NBI Traffic Manager Config Register
 * @param nbi                       NBI number
 * @param traffic_manager_config    Pointer to struct nfp_nbi_tm_traffic_manager_config
 * @return 0 if successful, -1 if the NBI number is out of range
 *
 * This function takes a pointer to an array of struct nfp_nbi_tm_traffic_manager_config
 * and configures the TM Config CSR.
 */
int nfp_nbi_tm_config(unsigned int nbi,
    struct nfp_nbi_tm_traffic_manager_config traffic_manager_config);

#endif
