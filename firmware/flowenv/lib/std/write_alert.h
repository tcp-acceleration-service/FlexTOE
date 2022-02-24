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
 * @file          lib/std/write_alert.h
 * @brief         Interface for CLS write alert
 */
#ifndef _STD__WRITE_ALERT_H_
#define _STD__WRITE_ALERT_H_

#include <nfp.h>

#include <nfp6000/nfp_cls.h>

/**
 * This module provides an API to use the CLS write alert feature. A write
 * alert provides the ability to have a notification when CLS is written too.
 * Write alert has options to configure how the write alert is triggered,
 * the method of notification and how the CLS memory areas sizes are
 * configured.
 *
 * Write alert is triggered as a write occurs to a CLS address with bit 20 of
 * the 16 bit CLS address set. If bit 20 is not set then the CLS write does
 * not cause a write alert even when write alert is configured. This allows
 * the user the ability to choose when to cause a write alert or not depending
 * on how the CLS write address is configured regardless of write alert
 * configuration.
 *
 * Write alert notification can be accomplished by just reading the 64 bit
 * write alert pending register. This register indicates by bits set where a
 * write in CLS a write alert has been triggered. This register clears when it
 * is read.
 *
 * Write alert notifications can also be used to generate an event as well as
 * setting the write alert pending bits. The event returns the bit indicating
 * the region of CLS that was written to. The write alert events are normal
 * CLS events; however a separate write alert event  API is provided to
 * configure them for write alerting. To enable events the "events" bit must
 * be set in the write alert configuration register. Write alert areas also
 * can be configured to cover different address ranges. Each of the 64 bits of
 * the write alert pending register represents a configurable number of bytes
 * from 8B to 1024B per bit called a region. The region is set using the
 * region bits in the write alert configuration register. The write alert
 * region control also includes a mask/match component which provides a way to
 * select the region of interest or split any or those regions in half. The
 * mask is 8 bits which is applied on the most significant bits of the CLS
 * write address giving the ability to move the write alert window to
 * different regions.
 */

/**
 * Usage example:
 * A write alert CLS address that does not cause a write alert: 0x000100
 * A write alert CLS address that does cause a write alert:     0x100100
 *
 * Configures the first 512 bytes of CLS for write alerting with events.
 * cls_write_alert_setup(0xFE, 0x0, 0, 1, 0);
 *
 * Configures a write alert event filter on filter 2 autopush 11.
 * wa_filter = cls_write_alert_event_setup(2, 11, meid, ctx(),
 *                                         __signal_number(&wa_evt_sig),
 *                                         __xfer_reg_number(&wa_evt_xfer));
 *
 * Reads the write alert pending bits.
 * __cls_read_write_alert_pending((void *)&wa_pending, sizeof(wa_pending),
 *                                sig_done, &wa_pending_sig);
 *
 * Re-arm the event filter.
 * cls_write_alert_event_reset(2, 11);
 */

/**
 * Setup a CLS write alert config.
 * @param addr_mask   Write alert address mask (8 bits)
 * @param addr_match  Write_alert address match (8 bits)
 * @param region      Region (0-7) Sets number of bytes per bit set
 * @param events      Enable write_alert event generation
 * @param data63      Enable write alert on 63rd bit set in data
 */
__intrinsic void cls_write_alert_setup(unsigned int addr_mask,
                                       unsigned int addr_match,
                                       unsigned int region,
                                       unsigned int events,
                                       unsigned int data63);

/**
 * Disables a CLS write alert config
 */
__intrinsic void cls_write_alert_disable();

/**
 * Read CLS write alert pending value.
 * @param wa_pending  Each bit indicates the region a write alert occurred in
 * @param size        Size of bytes to read (must be 8)
 * @param sync        Type of synchronization (sig_done or ctx_swap)
 * @param sig         Signal to use
 *
 * Reads the write alert pending and returns it in wa_pending.
 * Write alert pending is a 64 bit value.
 * The process of reading write alert pending also auto clears the bits on
 * read.
 */
__intrinsic void __cls_read_write_alert_pending(__xread void *wa_pending,
                                                size_t size, sync_t sync,
                                                SIGNAL *sig);

__intrinsic void cls_read_write_alert_pending(__xread void *wa_pending,
                                              size_t size);

/**
 * Configures CLS write alert events.
 * @param fnum      Event filter number
 * @param apnum     Autopush signal number
 * @parpm emeid     Event micro engine id
 * @param ectx      Event context
 * @param esig      Event signal
 * @param exfer     Event read register
 * @return Filter handle to be used if a write alert is being disabled later.
 *
 * The write alert feature only sends a NFP_EVENT_TYPE_VALUE_UPDATED event
 * type when write alerting has "events" enabled. This function does not need
 * called if events are not enabled. The match and mask are fixed to only see
 * this event type. This setup routine also resets a CLS write alert autopush
 * event so it is ready to receive events once the configuration is setup.
 * This function creates an event handle, configures the event filter and
 * autopush, and arms the event.
 * NOTE: after an event is received cls_write_alert_event_reset() must be
 * called to re-arm the the event.
 */
__intrinsic __cls struct event_cls_filter *cls_write_alert_event_setup(
    unsigned int fnum, unsigned int apnum,
    unsigned int emeid, unsigned int ectx,
    unsigned int esig, unsigned int exfer);

/**
 * Disable CLS write alert events.
 * @param filter  Event filter handle
 * @param fnum    Event filter number
 *
 * This function tears down both the event filter and autopush using the filter
 * event handle obtained when the write alert event was setup.
 * NOTE: there is almost never a time where we would disable an already
 * configured write alert event.
 */
__intrinsic void cls_write_alert_event_disable(__cls struct event_cls_filter
                                               *filter, unsigned int fnum);

/**
 * Resets (re-arms) a CLS write alert event.
 * @param fnum      Event filter number
 * @param apnum     Autopush signal number
 *
 * Resets a CLS write alert autopush event. The write alert always uses a
 * NFP_CLS_AUTOPUSH_STATUS_MONITOR_ONE_SHOT_ACK monitor type.
 */
__intrinsic void cls_write_alert_event_reset(unsigned int fnum,
                                             unsigned int apnum);

#endif /* !_STD__WRITE_ALERT_H_ */
