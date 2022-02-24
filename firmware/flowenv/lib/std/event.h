/*
 * Copyright (C) 2012-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/std/event.h
 * @brief         Interface for event bus and event managers
 */
#ifndef _STD__EVENT_H_
#define _STD__EVENT_H_

#include <nfp.h>

#include <nfp6000/nfp_event.h>

struct event_cls_filter;

/**
 * Get handler for CLS event filter.
 * @param fnum          Event filter index
 * @return Handle to use for other event_cls_filter functions
 */
__intrinsic __cls struct event_cls_filter *event_cls_filter_handle(int fnum);

/**
 * Setup CLS event filter.
 * @param filter        Event filter handle
 * @param type          Type of event filter
 * @param match         Match value for filter
 * @param mask          Mask value for filter
 * @param status        Configuration for the event filter status field
 */
__intrinsic void event_cls_filter_setup(__cls struct event_cls_filter *filter,
                                        int type,
                                        unsigned int match, unsigned int mask,
                                        struct nfp_em_filter_status status);

/**
 * Disable CLS event filter.
 * @param filter        Event filter handle
 */
__intrinsic void event_cls_filter_disable(
    __cls struct event_cls_filter *filter);

/**
 * Acknowledge CLS event filter.
 * @param filter        Event filter handle
 */
__intrinsic unsigned int event_cls_filter_ack(
    __cls struct event_cls_filter *filter);


/**
 * Trigger event on event bus.
 * @param evdata        Event data
 *
 * Trigger an event on the event bus.  The upper four bits of the
 * event data (i.e., the event provider bits in [19:16]) are hardcoded
 * to the current cluster number.
 */
__intrinsic void event_cls_user_event(unsigned int evdata);

/**
 * Setup an event autopush signal.
 * @param apnum         Autopush signal index number
 * @param master        Master to push to
 * @param thd           Thread number
 * @param signum        Signal number
 * @param xfernum       xfer register number
 *
 * Set up an autopush entry to signal the indicated ME thread and push
 * a value to the indicated xfer register.  The @master parameter is
 * only four bits wide, indicating the ME master within the current
 * cluster. The xfernum is 6 bits wide and thd is 3 bits wide but they
 * combine into signal data reference as 8 bits.  When in 8 CTX mode
 * the CTX (thd) uses the MSB of the transfer register (CCCT TTTT). When in
 * 4 CTX mode the xfernum uses the LSB of the CTX (thd) (CCTT TTTT). They do
 * not interfere with each other because in 4 CTX mode CTX (thd) can
 * only be (0, 2, 4, 6) and in 8 CTX mode xfernum can only be (0-31).
 */
__intrinsic void event_cls_autopush_signal_setup(unsigned int apnum,
                                                 unsigned int master,
                                                 unsigned int thd,
                                                 unsigned int signum,
                                                 unsigned int xfernum);

/**
 * Reset an event autopush filter
 * @param fnum          Autopush filter number
 * @param type          Autopush filter type
 * @param autopush      Autopush signal to invoke
 * @param apfwr         Write transfer register for the reset
 * @param sync          Type of synchronization (sig_done or ctx_swap)
 * @param sig           Signal to use
 */
__intrinsic void __event_cls_autopush_filter_reset(unsigned int fnum,
                                                   unsigned int type,
                                                   unsigned int autopush,
                                                   __xwrite unsigned int *apfwr,
                                                   sync_t sync,
                                                   SIGNAL *sig);

__intrinsic void event_cls_autopush_filter_reset(unsigned int fnum,
                                                 unsigned int type,
                                                 unsigned int autopush);

/**
 * Disable an event autopush filter.
 * @param fnum          Autopush filter number
 */
__intrinsic void event_cls_autopush_filter_disable(unsigned int fnum);

#endif /* !_STD__EVENT_H_ */
