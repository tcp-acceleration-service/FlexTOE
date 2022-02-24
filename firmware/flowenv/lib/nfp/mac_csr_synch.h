/*
 * Copyright (C) 2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file        lib/nfp/mac_csr_synch.h
 * @brief       Prototypes for MAC ethernet config arbiter command. The
 *              EthCmdConfig arbiter controls access to the EthCmdConfig MAC
 *              register for every port.
 *              During a soft reset the EthCmdConfig arbiter is disabled. The
 *              application should restart the arbiter when required.
 *              The application should take care to only write to EthCmdConfig
 *              arbiter when a value has changed as not to overflow the ring.
 */

#ifndef _NFP__MAC_CSR_SYNCH_H_
#define _NFP__MAC_CSR_SYNCH_H_


/**
 * Set the MacEthCmdCfg register in a controlled manner by use of the
 * arbiter ring.
 *
 * @param nbi   The nbi to configure (0/1)
 * @param core  The MAC core to configure (0/1)
 * @param port  The MAC port to configure (0..63)
 * @param cmd   The ARB_CODE_ETH_CMD_CFG_x command to configure
 *              (see nfp_mac_csr_synch.h)
 * @param sync  Type of synchronization (sig_done or ctx_swap)
 * @param sig   Signal to use
 *
 */
__intrinsic void __mac_eth_cmd_config(unsigned int nbi, unsigned int core,
                                    unsigned int port,
                                    unsigned int cmd,
                                    sync_t sync, SIGNAL *sig);

__intrinsic void mac_eth_cmd_config(unsigned int nbi, unsigned int core,
                                  unsigned int port,
                                  unsigned int cmd);

#endif /* _NFP__MAC_CSR_SYNCH_H_ */
