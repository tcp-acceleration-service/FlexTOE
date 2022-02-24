/*
 * Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/net/sctp.h
 * @brief         Definitions for SCTP header parsing
 *
 * Incomplete, new definitions will be added as/when needed
 */

#ifndef _NET_SCTP_H_
#define _NET_SCTP_H_

/**
 * Header length definitions
 * @NET_SCTP_LEN            Length of SCTP header
 * @NET_SCTP_LEN32          Length of SCTP header (32bit words)
 */
#define NET_SCTP_LEN              12
#define NET_SCTP_LEN32            (NET_SCTP_LEN / 4)

/**
 * SCTP type definitions
 */

#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>

/**
 * SCTP Header structure
 */
__packed struct sctp_hdr {
    uint16_t sport;                     /** Source port */
    uint16_t dport;                     /** Destination port */

    uint32_t ver;                       /** Verification tag */

    uint32_t sum;                       /** Checksum */
};

#endif /* __NFP_LANG_MICROC */

#endif /* _NET_SCTP_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
