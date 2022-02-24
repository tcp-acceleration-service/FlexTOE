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
 * @file          lib/std/_c/event.c
 * @brief         Event filters, autopush, and event managers
 */

#include <assert.h>
#include <nfp.h>
#include <types.h>

#include <nfp/cls.h>
#include <nfp/me.h>

#include <nfp6000/nfp_cls.h>
#include <nfp6000/nfp_event.h>

#include <std/event.h>

struct event_cls_filter;

__intrinsic __cls struct event_cls_filter *
event_cls_filter_handle(int fnum)
{
    return (__cls void *) (NFP_CLS_EM +
                           NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER(fnum));
}

__intrinsic void
event_cls_filter_setup(__cls struct event_cls_filter *filter,
                       int type, unsigned int match, unsigned int mask,
                       struct nfp_em_filter_status status)
{
    __xwrite unsigned int evmask, evmatch, evstatus;
    __xread unsigned int evack;
    __cls char *faddr = (__cls char *) filter;
    SIGNAL s1, s2, s3, s4;

    evmatch = NFP_EM_FILTER_MATCH_EVENT(match);
    evmask = (NFP_EM_FILTER_MASK_TYPE(type) |
              NFP_EM_FILTER_MASK_EVENT(mask));
    evstatus = status.__raw;

    __cls_write(&evmatch, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_MATCH,
                sizeof(evmatch), sizeof(evmatch), sig_done, &s1);
    __cls_write(&evmask, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_MASK,
                sizeof(evmask), sizeof(evmask), sig_done, &s2);
    __cls_write(&evstatus,
                faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_STATUS,
                sizeof(evstatus), sizeof(evstatus), sig_done, &s4);
    __cls_read(&evack, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_ACK,
               sizeof(evack), sizeof(evack), sig_done, &s3);

    wait_for_all(&s1, &s2, &s3, &s4);

    __implicit_read(&evack);
}

__intrinsic void
event_cls_filter_disable(__cls struct event_cls_filter *filter)
{
    __xwrite unsigned int zero = 0;
    __cls char *faddr = (__cls char *) filter;
    SIGNAL s1, s2, s3;

    __cls_write(&zero, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_MATCH,
                sizeof(zero), sizeof(zero), sig_done, &s1);
    __cls_write(&zero, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_MASK,
                sizeof(zero), sizeof(zero), sig_done, &s2);
    __cls_write(&zero, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_STATUS,
                sizeof(zero), sizeof(zero), sig_done, &s3);
    wait_for_all(&s1, &s2, &s3);
}

__intrinsic unsigned int
event_cls_filter_ack(__cls struct event_cls_filter *filter)
{
    __xread unsigned int evack;
    __cls char *faddr = (__cls char *) filter;

    cls_read(&evack, faddr + NFP_CLS_EM_CSR_SPACING * NFP_EM_FILTER_ACK,
             sizeof(evack));
    __implicit_read(&evack);

    return evack;
}

__intrinsic void
event_cls_user_event(unsigned int evdata)
{
    __xwrite unsigned int out = evdata;

    cls_write(&out, (__cls void *)NFP_CLS_AUTOPUSH_USER_EVENT, sizeof(out));
}

__intrinsic void
event_cls_autopush_signal_setup(unsigned int apnum, unsigned int master,
                                unsigned int thd, unsigned int signum,
                                unsigned int xfernum)
{
    __cls void *apaddr = (__cls void *)NFP_CLS_AUTOPUSH_SIGNAL(apnum);
    __xwrite unsigned int apcfg;

    apcfg = (NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_LO(master>>4) |
             NFP_CLS_AUTOPUSH_SIGNAL_MASTER(master) |
             NFP_CLS_AUTOPUSH_SIGNAL_SIGNAL_REF(
                 ((thd & 0x7) << 4) | (signum & 0xf)) |
             NFP_CLS_AUTOPUSH_SIGNAL_MASTER_ISL_HI(master>>8) |
             NFP_CLS_AUTOPUSH_SIGNAL_DATA_REF(
                 ((thd & 0x7) << 7) | ((xfernum & 0x3f) << 2)));

    cls_write(&apcfg, apaddr, sizeof(apcfg));
}

__intrinsic void
__event_cls_autopush_filter_reset(unsigned int fnum, unsigned int type,
                                  unsigned int autopush,
                                  __xwrite unsigned int *apfwr, sync_t sync,
                                  SIGNAL *sig)
{
    __cls void *apfaddr = (__cls void *)NFP_CLS_AUTOPUSH_STATUS(fnum);

    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    *apfwr = (NFP_CLS_AUTOPUSH_STATUS_MONITOR(type) |
              NFP_CLS_AUTOPUSH_STATUS_AUTOPUSH(autopush));

    __cls_write(apfwr, apfaddr, sizeof(unsigned int), sizeof(unsigned int),
                sync, sig);
}

__intrinsic void
event_cls_autopush_filter_reset(unsigned int fnum, unsigned int type,
                                unsigned int autopush)
{
    SIGNAL sig;
    __xwrite unsigned int apfwr;

    __event_cls_autopush_filter_reset(fnum, type, autopush, &apfwr, ctx_swap,
                                      &sig);
}

__intrinsic void
event_cls_autopush_filter_disable(unsigned int fnum)
{
    __cls void *apfaddr = (__cls void *)NFP_CLS_AUTOPUSH_STATUS(fnum);
    __xwrite unsigned int zero = 0;

    cls_write(&zero, apfaddr, sizeof(zero));
}
