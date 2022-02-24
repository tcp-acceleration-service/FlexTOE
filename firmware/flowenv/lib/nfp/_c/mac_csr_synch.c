/*
 * Copyright 2017 Netronome, Inc.
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
 * @file          lib/nfp/_c/mac_csr_synch.c
 * @brief         Sets the MAC ethernet config command using arbiter (ring).
 */

#include <nfp.h>
#include <nfp/cls.h>
#include <nfp6000/nfp_mac_csr_synch.h>
#include <nfp/mac_csr_synch.h>

/* Do not force recache */
#ifndef MAC_FORCE_RECACHE
    #define MAC_FORCE_RECACHE  0
#endif

/* an arbitrary number to insert as the arbiter source value */
#ifndef MAC_ARB_SRC_APP
    #define MAC_ARB_SRC_APP     4
#endif

__intrinsic void
__mac_eth_cmd_config(unsigned int nbi, unsigned int core, unsigned int port,
                  unsigned int cmd, sync_t sync, SIGNAL *sig)
{
    __gpr uint32_t ring_num = (ARB_CLS_RING_NUM <<2);
    __gpr uint32_t ring_isl = ARB_CLS_BASE_ADDR_Hi32;
    __xwrite uint32_t xfr_flush;

    try_ctassert(nbi < NFP_MAC_MAX_ISLANDS_PER_NFP);
    try_ctassert(core < NFP_MAX_MAC_CORES_PER_MAC_ISL);
    try_ctassert(port < NFP_MAX_ETH_PORTS_PER_MAC_CORE);
    try_ctassert(cmd <= ARB_CODE_MAX);
    ctassert(__is_ct_const(sync));
    ctassert(sync == sig_done || sync == ctx_swap);

    xfr_flush = ARB_SOURCE(MAC_ARB_SRC_APP) | ARB_RECACHE(MAC_FORCE_RECACHE) |
                ARB_NBI(nbi) | ARB_PORT(port) | ARB_CORE(core) | ARB_CMD(cmd);

    if (sync == sig_done) {
        __asm cls[ring_workq_add_work, xfr_flush, ring_isl, <<8, ring_num, 1],\
                  sig_done[*sig]
    } else {
        __asm cls[ring_workq_add_work, xfr_flush, ring_isl, <<8, ring_num, 1],\
                  ctx_swap[*sig];
    }
}


__intrinsic void
mac_eth_cmd_config(unsigned int nbi, unsigned int core, unsigned int port,
                   unsigned int cmd)
{
    SIGNAL sig;
    __mac_eth_cmd_config(nbi, core, port, cmd, ctx_swap, &sig);
}