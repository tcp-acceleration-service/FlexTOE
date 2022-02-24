/*
 * Copyright (C) 2012-2017,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/libnfp.c
 * @brief         Standard library for NFP
 */


#include <assert.h>
#include <nfp.h>

#include <nfp6000/nfp_me.h>

/*
 * The following files implement all the functionality in <nfp/*.h>.
 * They may only have dependencies on <nfp.h> and the other include
 * files in <nfp/*.h>.  They are not protected by header guards and
 * should not be included elsewhere.
 */
#include "_c/cls.c"
#include "_c/macstats.c"
#include "_c/me.c"
#include "_c/mem_atomic.c"
#include "_c/mem_bulk.c"
#include "_c/mem_cam.c"
#include "_c/mem_lkup.c"
#include "_c/mem_pe.c"
#include "_c/mem_ring.c"
#include "_c/pcie.c"
#include "_c/remote_me.c"
#include "_c/tmq.c"
#include "_c/xpb.c"
#include "_c/tm_config.c"
#include "_c/mac_csr_synch.c"
#include "_c/ctm.c"

/*
 * The following code implements all the functionality in <nfp.h>.  It
 * should not have any external dependencies (including dependencies
 * on <nfp/*.h> include files).
 */
__intrinsic void
___rt_assert(void *addr)
{
    local_csr_write(local_csr_mailbox_1, (unsigned int) addr);
    local_csr_write(local_csr_mailbox_0, 0);
    __asm ctx_arb[bpt];
}
