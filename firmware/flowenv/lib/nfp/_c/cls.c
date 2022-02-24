/*
 * Copyright (C) 2012-2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/_c/cls.c
 * @brief         NFP CLS interface
 */

#include <assert.h>
#include <nfp.h>
#include <nfp/me.h>
#include <types.h>

#include <nfp6000/nfp_me.h>

#include <nfp/cls.h>

#define _CLS_CMD(cmdname, data, addr, size, max_size, sync, sig, cmin, cmax) \
do {                                                                    \
    struct nfp_mecsr_prev_alu ind;                                      \
    unsigned int count = (size >> 2);                                   \
    unsigned int max_count = (max_size >> 2);                           \
                                                                        \
    ctassert(__is_ct_const(max_size));                                  \
    ctassert(__is_ct_const(sync));                                      \
    ctassert(sync == sig_done || sync == ctx_swap);                     \
                                                                        \
    if (__is_ct_const(size)) {                                          \
        ctassert(size != 0);                                            \
        ctassert(__is_aligned(size, 4));                                \
                                                                        \
        if(size <= (cmax * 4)) {                                        \
            if (sync == sig_done) {                                     \
                __asm { cls[cmdname, *data, addr, 0, __ct_const_val(count)], \
                        sig_done[*sig] }                                \
            } else {                                                    \
                __asm { cls[cmdname, *data, addr, 0, __ct_const_val(count)], \
                        ctx_swap[*sig] }                                \
            }                                                           \
        } else {                                                        \
            ctassert(size <= 128);                                      \
                                                                        \
            /* Setup length in PrevAlu for the indirect */              \
            ind.__raw = 0;                                              \
            ind.ov_len = 1;                                             \
            ind.length = count - 1;                                     \
                                                                        \
            if (sync == sig_done) {                                     \
                __asm { alu[--, --, B, ind.__raw] }                     \
                __asm { cls[cmdname, *data, addr, 0, __ct_const_val(count)], \
                        sig_done[*sig], indirect_ref }                  \
            } else {                                                    \
                __asm { alu[--, --, B, ind.__raw] }                     \
                __asm { cls[cmdname, *data, addr, 0, __ct_const_val(count)], \
                        ctx_swap[*sig], indirect_ref }                  \
            }                                                           \
        }                                                               \
    } else {                                                            \
        /* Setup length in PrevAlu for the indirect */                  \
        ind.__raw = 0;                                                  \
        ind.ov_len = 1;                                                 \
        ind.length = count - 1;                                         \
                                                \
        if (sync == sig_done) {                                         \
            __asm { alu[--, --, B, ind.__raw] }                         \
            __asm { cls[cmdname, *data, addr, 0, __ct_const_val(max_count)], \
                    sig_done[*sig], indirect_ref }                      \
        } else {                                                        \
            __asm { alu[--, --, B, ind] }                               \
            __asm { cls[cmdname, *data, addr, 0, __ct_const_val(max_count)], \
                    ctx_swap[*sig], indirect_ref }                      \
        }                                                               \
    }                                                                   \
} while (0)

__intrinsic void
__cls_read_be(__xread void *data, __cls void *addr,
              size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));

    _CLS_CMD(read_be, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_read_be(__xread void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_read_be(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__cls_write_be(__xwrite void *data, __cls void *addr,
               size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));

    _CLS_CMD(write_be, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_write_be(__xwrite void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_write_be(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__cls_read_le(__xread void *data, __cls void *addr,
              size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_read_reg(data));

    _CLS_CMD(read_le, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_read_le(__xread void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_read_le(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__cls_write_le(__xwrite void *data, __cls void *addr,
               size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    ctassert(__is_write_reg(data));

    _CLS_CMD(write_le, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_write_le(__xwrite void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_write_le(data, addr, size, size, ctx_swap, &sig);
}


/*
 * Endian neutral version of the above functions.
 */

#if __BIGENDIAN

__intrinsic void
__cls_read(__xread void *data, __cls void *addr,
           size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    __cls_read_be(data, addr, size, max_size, sync, sig);
}

__intrinsic void
cls_read(__xread void *data, __cls void *addr, size_t size)
{
    cls_read_be(data, addr, size);
}

__intrinsic void
__cls_write(__xwrite void *data, __cls void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    __cls_write_be(data, addr, size, max_size, sync, sig);
}

__intrinsic void
cls_write(__xwrite void *data, __cls void *addr, size_t size)
{
    cls_write_be(data, addr, size);
}

#else /* __LITLEENDIAN */

__intrinsic void
__cls_read(__xread void *data, __cls void *addr,
           size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    __cls_read_le(data, addr, size, max_size, sync, sig);
}

__intrinsic void
cls_read(__xread void *data, __cls void *addr, size_t size)
{
    cls_read_le(data, addr, size, size);
}

__intrinsic void
__cls_write(__xwrite void *data, __cls void *addr,
            size_t size, const size_t max_size, sync_t sync, SIGNAL *sig)
{
    __cls_write_le(data, addr, size, max_size, sync, sig);
}

__intrinsic void
cls_write(__xwrite void *data, __cls void *addr, size_t size)
{
    cls_write_le(data, addr, size, size);
}

#endif /* __LITLEENDIAN */

__intrinsic void
__cls_hash_mask_clr(__xwrite void *key, __cls void *mask, size_t size,
                    const size_t max_size, uint32_t idx, sync_t sync,
                    SIGNAL *sig)
{
    __cls void *temp_mask;

    ctassert(__is_write_reg(key));
    ctassert(__is_aligned(size, 2));
    ctassert(__is_aligned(max_size, 2));

    /* hash_index_select = addr[18:16]. See hash_mask_clear in databook */
    temp_mask = (__cls void *)((idx << 16) | (uint32_t)mask);
    _CLS_CMD(hash_mask_clear, key, temp_mask, size, max_size, sync, sig, 2, 8);
}

__intrinsic void
cls_hash_mask_clr(__xwrite void *key, __cls void *mask, size_t size,
                  uint32_t idx)
{
    SIGNAL sig;

    __cls_hash_mask_clr(key, mask, size, size, idx, ctx_swap, &sig);
}

__intrinsic void
__cls_hash_mask(__xwrite void *key, __cls void *mask, size_t size,
                const size_t max_size, uint32_t idx, sync_t sync, SIGNAL *sig)
{
    __cls void *temp_mask;

    ctassert(__is_write_reg(key));
    ctassert(__is_aligned(size, 2));
    ctassert(__is_aligned(max_size, 2));

    /* hash_index_select = addr[18:16] See hash_mask in databook*/
    temp_mask = (__cls void *)((idx << 16) | (uint32_t)mask);
    _CLS_CMD(hash_mask, key, temp_mask, size, max_size, sync, sig, 2, 8);
}

__intrinsic void
cls_hash_mask(__xwrite void *key, __cls void *mask, size_t size, uint32_t idx)
{
    SIGNAL sig;

    __cls_hash_mask(key, mask, size, size, idx, ctx_swap, &sig);
}

/* bit atomic functions */
__intrinsic void
__cls_clr(__xwrite void *data, __cls void *addr, size_t size,
          const size_t max_size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);
    _CLS_CMD(clr, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_clr(__xwrite void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_clr(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__cls_set(__xwrite void *data, __cls void *addr, size_t size,
          const size_t max_size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);

    _CLS_CMD(set, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_set(__xwrite void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_set(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void __cls_swap(__xrw void* data, __cls void* addr,
                            size_t size, const size_t max_size,
                            sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);

    _CLS_CMD(swap, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void cls_swap(__xrw void* data, __cls void* addr,
                          size_t size)
{
    SIGNAL sig;

    __cls_swap(data, addr, size, size, ctx_swap, &sig);
}

__intrinsic void
__cls_test_clr(__xrw void *data, __cls void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);

    _CLS_CMD(test_clr, data, addr, size, max_size, sync, sig, 1, 8);
}

__intrinsic void
cls_test_clr(__xrw void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_test_clr(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__cls_test_set(__xrw void *data, __cls void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);

    _CLS_CMD(test_set, data, addr, size, size, sync, sig, 1, 8);
}

__intrinsic void
cls_test_set(__xrw void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_test_set(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__cls_test_add(__xrw void *data, __cls void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);

    _CLS_CMD(test_add, data, addr, size, size, sync, sig, 1, 8);
}

__intrinsic void
cls_test_add(__xrw void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_test_add(data, addr, size, size, ctx_swap, &sig);
}


__intrinsic void
__cls_test_sub(__xrw void *data, __cls void *addr, size_t size,
               const size_t max_size, sync_t sync, SIGNAL *sig)
{
    try_ctassert(size <= 64);

    _CLS_CMD(test_sub, data, addr, size, size, sync, sig, 1, 8);
}

__intrinsic void
cls_test_sub(__xrw void *data, __cls void *addr, size_t size)
{
    SIGNAL sig;

    __cls_test_sub(data, addr, size, size, ctx_swap, &sig);
}

/* cls arithmetic functions */
__intrinsic void
cls_incr(__cls void *addr)
{
    __asm cls[incr, --, addr, 0]
}

__intrinsic void
cls_incr64(__cls void *addr)
{
    __asm cls[incr64, --, addr, 0]
}

__intrinsic void
cls_decr(__cls void *addr)
{
    __asm cls[decr, --, addr, 0]
}

__intrinsic void
cls_decr64(__cls void *addr)
{
    __asm cls[decr64, --, addr, 0]
}

/* VF Translate functions */
__intrinsic void
cls_vf_translate_setup(unsigned int base_addr, unsigned int cls_bytes,
                       unsigned int page_size, unsigned int alert_trig,
                       unsigned int alignment, unsigned int padding)
{
    __cls void *vftaddr = (__cls void *)NFP_CLS_AUTOPUSH_VF_TRANSLATION;
    __gpr unsigned int vftcfg_temp = 0;
    __xwrite unsigned int vftcfg;

    if (padding)
       vftcfg_temp |= NFP_CLS_AUTOPUSH_VF_TRANSLATION_PROT_VFS;

    vftcfg_temp |= (NFP_CLS_AUTOPUSH_VF_TRANSLATION_BASE(base_addr) |
                    NFP_CLS_AUTOPUSH_VF_TRANSLATION_OFFSET_SIZE(cls_bytes) |
                    NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_OFS(page_size) |
                    NFP_CLS_AUTOPUSH_VF_TRANSLATION_VF_ALERT(alert_trig) |
                    NFP_CLS_AUTOPUSH_VF_TRANSLATION_ALIGN(alignment) |
                    NFP_CLS_AUTOPUSH_VF_TRANSLATION_ENABLE);

    vftcfg = vftcfg_temp;
    cls_write(&vftcfg, vftaddr, sizeof(vftcfg));
}

__intrinsic void
cls_vf_translate_disable()
{
    __cls void *vftaddr = (__cls void *)NFP_CLS_AUTOPUSH_VF_TRANSLATION;
    __xwrite unsigned int vftcfg = 0;

    cls_write(&vftcfg, vftaddr, sizeof(vftcfg));
}

__intrinsic void cls_ring_setup(unsigned int ring_num,  __cls void* base_addr, unsigned int size)
{
    struct nfp_cls_rings_ring_base ring_base;
    struct nfp_cls_rings_ring_ptrs ring_ptrs;
    __xwrite unsigned int ring_xfer;
    uint64_t addr;
    __cls void *ring_csr;

    ctassert(__is_ct_const(ring_num));
    ctassert(ring_num < 16);
    ctassert(__is_ct_const(size));
    ctassert(128 <= size && size <= 4096);
    ctassert((size & (size - 1)) == 0);

    addr = (uint64_t) base_addr;

    ring_base.__raw = 0;
    ring_base.report = 0;   /* No overflow events! */
    ring_base.size = __log2(size >> 7);
    ring_base.base = (addr >> 7);
    ring_xfer = ring_base.__raw;

    ring_csr = (__cls void*) NFP_CLS_RINGS_RING_BASE(ring_num);
    cls_write(&ring_xfer, ring_csr, sizeof(ring_xfer));

    ring_ptrs.__raw = 0;
    ring_xfer = ring_ptrs.__raw;

    ring_csr = (__cls void*) NFP_CLS_RINGS_RING_PTRS(ring_num);
    cls_write(&ring_xfer, ring_csr, sizeof(ring_xfer));
}

__intrinsic void cls_workq_setup(unsigned int ring_num,  __cls void* base_addr, unsigned int size)
{
    struct nfp_cls_rings_ring_base ring_base;
    struct nfp_cls_rings_ring_ptrs ring_ptrs;
    __xwrite unsigned int ring_xfer;
    uint64_t addr;
    __cls void *ring_csr;

    ctassert(__is_ct_const(ring_num));
    ctassert(ring_num < 16);
    ctassert(__is_ct_const(size));
    ctassert(128 <= size && size <= 4096);
    ctassert((size & (size - 1)) == 0);

    addr = (uint64_t) base_addr;

    ring_base.__raw = 0;
    ring_base.work = 1;
    ring_base.report = 0;   /* No overflow events! */
    ring_base.size = __log2(size >> 7);
    ring_base.base = (addr >> 7);
    ring_xfer = ring_base.__raw;

    ring_csr = (__cls void*) NFP_CLS_RINGS_RING_BASE(ring_num);
    cls_write(&ring_xfer, ring_csr, sizeof(ring_xfer));

    ring_ptrs.__raw = 0;
    ring_xfer = ring_ptrs.__raw;

    ring_csr = (__cls void*) NFP_CLS_RINGS_RING_PTRS(ring_num);
    cls_write(&ring_xfer, ring_csr, sizeof(ring_xfer));
}

__intrinsic void __cls_ring_put(unsigned int rnum, __xwrite void* data,
            size_t size, sync_t sync, SIGNAL* sig)
{
    unsigned int addr_reg;
    unsigned int ref_cnt;

    ctassert(__is_write_reg(data));
    ctassert(__is_ct_const(rnum));
    ctassert(__is_ct_const(size));
    ctassert(rnum <  16);
    ctassert(4 <= size && size <= 32);
    ctassert(sync == sig_done);

    addr_reg = (rnum << 2);
    ref_cnt  = (size >> 2);
    __asm {
        cls[ring_put, *data, 0, addr_reg, ref_cnt], sig_done[*sig];
    }
}

__intrinsic void cls_ring_put(unsigned int rnum, __xwrite void* data, size_t size)
{
    SIGNAL sig;

    __cls_ring_put(rnum, data, size, sig_done, &sig);

    __wait_for_all(&sig);
}

__intrinsic void __cls_ring_get(unsigned int rnum, __xread void* data,
            size_t size, sync_t sync, SIGNAL* sig)
{
    unsigned int addr_reg;
    unsigned int ref_cnt;

    ctassert(__is_read_reg(data));
    ctassert(__is_ct_const(rnum));
    ctassert(__is_ct_const(size));
    ctassert(rnum <  16);
    ctassert(4 <= size && size <= 32);
    ctassert(sync == sig_done);

    addr_reg = (rnum << 2);
    ref_cnt  = (size >> 2);
    __asm {
        cls[ring_get, *data, 0, addr_reg, ref_cnt], sig_done[*sig];
    }
}

__intrinsic void cls_ring_get(unsigned int rnum, __xread void* data, size_t size)
{
    SIGNAL sig;

    __cls_ring_get(rnum, data, size, sig_done, &sig);

    __wait_for_all(&sig);
}

__intrinsic void __cls_ring_get_freely(unsigned int rnum, __xread void* data,
            size_t size, sync_t sync, SIGNAL* sig)
{
    unsigned int addr_reg;
    unsigned int ref_cnt;

    ctassert(__is_read_reg(data));
    ctassert(__is_ct_const(rnum));
    ctassert(__is_ct_const(size));
    ctassert(rnum <  16);
    ctassert(4 <= size && size <= 32);
    ctassert(sync == sig_done);

    addr_reg = (rnum << 2);
    ref_cnt  = (size >> 2);
    __asm {
        cls[ring_get_freely, *data, 0, addr_reg, ref_cnt], sig_done[*sig];
    }
}

__intrinsic void cls_ring_get_freely(unsigned int rnum, __xread void* data, size_t size)
{
    SIGNAL sig;

    __cls_ring_get_freely(rnum, data, size, sig_done, &sig);

    __wait_for_all(&sig);
}

__intrinsic void __cls_workq_add_thread(unsigned int rnum, __xread void* data, size_t size, sync_t sync, SIGNAL* sig)
{
    unsigned int addr_reg;
    unsigned int ref_cnt;
    struct nfp_mecsr_prev_alu ind;

    ctassert(__is_read_reg(data));
    ctassert(__is_ct_const(rnum));
    ctassert(__is_ct_const(size));
    ctassert(rnum <  16);
    ctassert(4 <= size && size <= 64);
    ctassert(sync == sig_done);

    addr_reg = (rnum << 2);
    ref_cnt  = (size >> 2);

    if (ref_cnt <= 8) {
        __asm {
            cls[ring_workq_add_thread, *data, 0, addr_reg, __ct_const_val(ref_cnt)], sig_done[*sig];
        }
    }
    else {
        ind.__raw = 0;
        ind.ov_len = 1;
        ind.length = ref_cnt - 1;
        __asm {
            alu[--, --, B, ind];
            cls[ring_workq_add_thread, *data, 0, addr_reg, __ct_const_val(ref_cnt)], indirect_ref, sig_done[*sig];
        }
    }
}

__intrinsic void cls_workq_add_thread(unsigned int rnum, __xread void* data, size_t size)
{
    SIGNAL sig;

    __cls_workq_add_thread(rnum, data, size, sig_done, &sig);

    __wait_for_all(&sig);
}

__intrinsic void __cls_workq_add_work(unsigned int rnum, __xwrite void* data, size_t size, sync_t sync, SIGNAL* sig)
{
    unsigned int addr_reg;
    unsigned int ref_cnt;
    struct nfp_mecsr_prev_alu ind;

    ctassert(__is_write_reg(data));
    ctassert(__is_ct_const(rnum));
    ctassert(__is_ct_const(size));
    ctassert(rnum <  16);
    ctassert(4 <= size && size <= 64);
    ctassert(sync == sig_done);

    addr_reg = (rnum << 2);
    ref_cnt  = (size >> 2);

    if (ref_cnt <= 8) {
        __asm {
            cls[ring_workq_add_work, *data, 0, addr_reg, __ct_const_val(ref_cnt)], sig_done[*sig];
        }
    }
    else {
        ind.__raw = 0;
        ind.ov_len = 1;
        ind.length = ref_cnt - 1;

        __asm {
            alu[--, --, B, ind];
            cls[ring_workq_add_work, *data, 0, addr_reg, __ct_const_val(ref_cnt)], indirect_ref, sig_done[*sig];
        }
    }
}

__intrinsic void cls_workq_add_work(unsigned int rnum, __xwrite void* data, size_t size)
{
    SIGNAL sig;

    __cls_workq_add_work(rnum, data, size, sig_done, &sig);

    __wait_for_all(&sig);
}

__intrinsic int cls_lockq_lock(__cls void* addr)
{
    SIGNAL_PAIR sigpair;

    try_ctassert(__is_aligned(addr, 8));

    __asm {
        cls[queue_lock, --, addr, 0], sig_done[*__ct_sig_pair(&sigpair)]
    }

    __wait_for_all_single(&sigpair.even);

    if (signal_test(&sigpair.odd))
        return -1;

    return 0;
}

__intrinsic void cls_lockq_lock_retry(__cls void* addr, unsigned int retry_timeout)
{
    SIGNAL_PAIR sigpair;

    try_ctassert(__is_aligned(addr, 8));
    ctassert(__is_ct_const(retry_timeout));
    ctassert(retry_timeout < (1 << 20));

    while (1) {
        __asm {
            cls[queue_lock, --, addr, 0], sig_done[*__ct_sig_pair(&sigpair)]
        }

        __wait_for_all_single(&sigpair.even);

        if (signal_test(&sigpair.odd)) {
            sleep(retry_timeout);
            continue;
        }

        break;
    }

    return;
}

__intrinsic void cls_lockq_unlock(__cls void* addr)
{
    try_ctassert(__is_aligned(addr, 8));

    __asm {
        cls[queue_unlock, --, addr, 0]
    }
}
