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
 * @file          lib/std/_c/write_alert.c
 * @brief         CLS alert functions
 */

#include <assert.h>
#include <nfp.h>

#include <nfp/cls.h>
#include <nfp/me.h>

#include <nfp6000/nfp_cls.h>

#include <std/event.h>
#include <std/write_alert.h>

__intrinsic void
cls_write_alert_setup(unsigned int addr_mask, unsigned int addr_match,
                      unsigned int region, unsigned int events,
                      unsigned int data63)
{
    __cls void *waaddr = (__cls void *)NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG;
    __gpr unsigned int wacfg_temp = 0;
    __xwrite unsigned int wacfg;

    if (events)
       wacfg_temp |= NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_EVENTS;

    if (data63)
        wacfg_temp |= NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_DATA63;

    wacfg_temp |= (NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MASK(addr_mask) |
                  NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ADDR_MATCH(addr_match) |
                  NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_REGION(region) |
                  NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG_ENABLE);

    wacfg = wacfg_temp;
    cls_write(&wacfg, waaddr, sizeof(wacfg));
}

__intrinsic void
cls_write_alert_disable()
{
    __cls void *waaddr = (__cls void *)NFP_CLS_AUTOPUSH_WRITE_ALERT_CFG;
    __xwrite unsigned int wacfg = 0;

    cls_write(&wacfg, waaddr, sizeof(wacfg));
}

__intrinsic void
__cls_read_write_alert_pending(__xread void *wa_pending, size_t size,
                               sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(wa_pending));
    try_ctassert(size == 8);

    __cls_read((void *)&wa_pending,
               (__cls void *)NFP_CLS_AUTOPUSH_WRITE_ALERT_PEND,
               size, size, sync, sig);
}

__intrinsic void
cls_read_write_alert_pending(__xread void *wa_pending, size_t size)
{
    SIGNAL sig;

    __cls_read_write_alert_pending(wa_pending, size, ctx_swap, &sig);
}

__intrinsic __cls struct event_cls_filter *
cls_write_alert_event_setup(unsigned int fnum, unsigned int apnum,
                            unsigned int emeid, unsigned int ectx,
                            unsigned int esig, unsigned int exfer)
{
    struct nfp_em_filter_status status;
    __cls struct event_cls_filter *filter;
    unsigned int event_mask;
    unsigned int event_match;

    event_mask = NFP_EVENT_MATCH(0xFF, 0, NFP_EVENT_TYPE_VALUE_UPDATED);
    event_match = NFP_EVENT_MATCH((NFP_EVENT_PROVIDER_NUM(
                                       emeid >> 4,
                                       NFP_EVENT_PROVIDER_INDEX_CLS)),
                                  0, NFP_EVENT_TYPE_VALUE_UPDATED);

    status.__raw = 0;
    filter = event_cls_filter_handle(fnum);
    event_cls_filter_setup(filter, NFP_EVENT_TYPE_VALUE_UPDATED, event_match,
                           event_mask, status);
    event_cls_autopush_signal_setup(apnum, emeid, ectx, esig, exfer);
    cls_write_alert_event_reset(fnum, apnum);

    return filter;
}

__intrinsic void
cls_write_alert_event_disable(__cls struct event_cls_filter *filter,
                              unsigned int fnum)
{
    event_cls_autopush_filter_disable(fnum);
    event_cls_filter_disable(filter);
}

__intrinsic void
cls_write_alert_event_reset(unsigned int fnum, unsigned int apnum)
{
    event_cls_autopush_filter_reset(
        fnum, NFP_CLS_AUTOPUSH_STATUS_MONITOR_ONE_SHOT_ACK, apnum);
}
