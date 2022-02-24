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
 * @file          lib/nfp/remote_me.h
 * @brief         Interface for local-ME-to-remote-ME related functions
 */
#ifndef _NFP__REMOTE_ME_H_
#define _NFP__REMOTE_ME_H_

#include <nfp.h>

#if defined(__NFP_LANG_MICROC)

/**
 * Read from a remote ME register, signalling only the local ME.
 * @param data            Pointer to local read transfer register
 * @param island          Remote island number
 * @param me              Remote ME number
 * @param reg_is_csr      Indicates remote ME register is a CSR
 *                        (0 = transfer register, 1 = CSR)
 * @param reg_addr        Remote ME register address
 * @param size            Size of the read, must be a multiple of 4
 * @param sync            Type of synchronisation (sig_done or ctx_swap)
 * @param local_sig       Local signal to use
 */
__intrinsic void __remote_me_reg_read_signal_local(__xread void *data,
                                                   unsigned int island,
                                                   unsigned int me,
                                                   unsigned int reg_is_csr,
                                                   unsigned int reg_addr,
                                                   size_t size,
                                                   sync_t sync,
                                                   SIGNAL *local_sig);

__intrinsic void remote_me_reg_read_signal_local(__xread void *data,
                                                 unsigned int island,
                                                 unsigned int me,
                                                 unsigned int reg_is_csr,
                                                 unsigned int reg_addr,
                                                 size_t size);

/**
 * Read from a remote ME register, signalling neither ME.
 * @param data            Pointer to local read transfer register
 * @param island          Remote island number
 * @param me              Remote ME number
 * @param reg_is_csr      Indicates remote ME register is a CSR
 *                        (0 = transfer register, 1 = CSR)
 * @param reg_addr        Remote ME register address
 * @param size            Size of the read, must be a multiple of 4
 */
__intrinsic void remote_me_reg_read_signal_none(__xread void *data,
                                                unsigned int island,
                                                unsigned int me,
                                                unsigned int reg_is_csr,
                                                unsigned int reg_addr,
                                                size_t size);

/**
 * Write to a remote ME register, signalling only the local ME.
 * @param data            Pointer to local write transfer register
 * @param island          Remote island number
 * @param me              Remote ME number
 * @param reg_is_csr      Indicates remote ME register is a CSR
 *                        (0 = transfer register, 1 = CSR)
 * @param reg_addr        Remote ME register address
 * @param size            Size of the write, must be a multiple of 4
 * @param sync            Type of synchronisation (sig_done or ctx_swap)
 * @param local_sig       Local signal to use
 */
__intrinsic void __remote_me_reg_write_signal_local(__xwrite void *data,
                                                    unsigned int island,
                                                    unsigned int me,
                                                    unsigned int reg_is_csr,
                                                    unsigned int reg_addr,
                                                    size_t size,
                                                    sync_t sync,
                                                    SIGNAL *local_sig);

__intrinsic void remote_me_reg_write_signal_local(__xwrite void *data,
                                                  unsigned int island,
                                                  unsigned int me,
                                                  unsigned int reg_is_csr,
                                                  unsigned int reg_addr,
                                                  size_t size);

/**
 * Write to a remote ME register, signalling neither ME.
 * @param data            Pointer to local write transfer register
 * @param island          Remote island number
 * @param me              Remote ME number
 * @param reg_is_csr      Indicates remote ME register is a CSR
 *                        (0 = transfer register, 1 = CSR)
 * @param reg_addr        Remote ME register address
 * @param size            Size of the write, must be a multiple of 4
 */
__intrinsic void remote_me_reg_write_signal_none(__xwrite void *data,
                                                 unsigned int island,
                                                 unsigned int me,
                                                 unsigned int reg_is_csr,
                                                 unsigned int reg_addr,
                                                 size_t size);

/**
 * Read the value from a remote ME CSR.
 * @param island    Remote island number
 * @param me        Remote ME number
 * @param reg_addr  Remote ME CSR address
 * @return The value read
 */
__intrinsic unsigned int remote_csr_read(unsigned int island, unsigned int me,
                                         unsigned int reg_addr);

/**
 * Write a value to a remote ME CSR.
 * @param island    Remote island number
 * @param me        Remote ME number
 * @param reg_addr  Remote ME CSR address
 * @param val       Value to write
 */
__intrinsic void remote_csr_write(unsigned int island, unsigned int me,
                                  unsigned int reg_addr, unsigned int val);

#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP__NFP_REMOTE_ME_H_ */
