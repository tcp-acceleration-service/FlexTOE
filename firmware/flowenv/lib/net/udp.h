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
 * @file          lib/net/udp.h
 * @brief         Definitions for UDP header parsing
 *
 * Incomplete, new definitions will be added as/when needed
 */

#ifndef _NET_UDP_H_
#define _NET_UDP_H_

/**
 * Header length definitions
 * @NET_UDP_LEN            Length of UDP header
 * @NET_UDP_LEN32          Length of UDP header (32bit words)
 */
#define NET_UDP_LEN              8
#define NET_UDP_LEN32            (NET_UDP_LEN / 4)


#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>

/**
 * UDP Header structure
 */
__packed struct udp_hdr {
    uint16_t sport;                     /** Source port */
    uint16_t dport;                     /** Destination port */

    uint16_t len;                       /** Length */
    uint16_t sum;                       /** Checksum */
};
#endif /* __NFP_LANG_MICROC */

#endif /* _NET_UDP_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
