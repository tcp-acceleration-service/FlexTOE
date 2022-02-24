/*
 * Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/net/arp.h
 * @brief         Definitions for ARP header parsing
 *
 */

#ifndef _NET_ARP_H_
#define _NET_ARP_H_

#define NET_ARP_HA_LEN             6

#if defined(__NFP_LANG_MICROC)

#include <nfp.h>
#include <stdint.h>

/**
 * ARP header structure
 */
__packed struct arp_hdr {
    unsigned int htype:16;          /* hardware type */
    unsigned int ptype:16;          /* protocol type */

    unsigned int hlen:8;            /* hardware length */
    unsigned int plen:8;            /* protocol length */
    unsigned int proto:16;          /* operation (1=request, 2=reply) */

    uint8_t sha[NET_ARP_HA_LEN];   /* sender hardware address */

    unsigned int spa;              /* sender protocol address */

    uint8_t tha[NET_ARP_HA_LEN];   /* target hardware address */

    unsigned int tpa;              /* target protocol address */
};
#endif /* __NFP_LANG_MICROC */

#endif /* _NET_ARP_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
