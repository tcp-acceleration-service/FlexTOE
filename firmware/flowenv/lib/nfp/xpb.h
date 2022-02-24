/*
 * Copyright (C) 2014-2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/xpb.h
 * @brief         Interface for common XPB related functions and types
 */
#ifndef _NFP__XPB_H_
#define _NFP__XPB_H_

#include <nfp.h>

#if defined(__NFP_LANG_MICROC)

/**
 * Read the value from one or a series of XPB addresses.
 * @param data      Pointer to local read transfer register
 * @param addr      XPB address
 * @param size      Size of the read, must be a multiple of 4
 * @param max_size  Used to determine largest read, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 */
__intrinsic void __xpb_read(__xread void *data, unsigned int addr, size_t size,
                            const size_t max_size, sync_t sync, SIGNAL *sig);

/**
 * Read the value from an XPB address.
 * @param addr   XPB address
 * @return the value read
 */
__intrinsic unsigned int xpb_read(unsigned int addr);

/**
 * Write a value to one or a series of XPB addresses.
 * @param data      Pointer to local write transfer register
 * @param addr      XPB address
 * @param size      Size of the write, must be a multiple of 4
 * @param max_size  Used to determine largest write, if size is not a constant
 * @param sync      Type of synchronization (sig_done or ctx_swap)
 * @param sig       Signal to use
 */
__intrinsic void __xpb_write(__xwrite void *data, unsigned int addr,
                             size_t size, const size_t max_size, sync_t sync,
                             SIGNAL *sig);

/**
 * Write a value to an XPB address.
 * @param addr    XPB address
 * @param val     Value to write
 */
__intrinsic void xpb_write(unsigned int addr, unsigned int val);

#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP__NFP_XPB_H_ */
