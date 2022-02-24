#include <nfp6000/nfp_ctm.h>
#include <nfp6000/nfp_me.h>

#include <assert.h>
#include <nfp.h>
#include <types.h>

#include <nfp/ctm.h>

__intrinsic void
ctm_ring_setup(unsigned int rnum, __ctm40 void *base, size_t size)
{
  unsigned int entries = size / 4;
  __gpr struct nfp_ctm_ring_base ctm_ring_base_tmp;
  __xwrite struct nfp_ctm_ring_base ctm_ring_base;
  __xwrite unsigned int ctm_ring_ptrs = 0;
  __xread unsigned int check;
  unsigned int xpb_addr;
  SIGNAL s1, s2, s3, s4;

  try_ctassert(rnum < 16);

  /* Note, size is in bytes and an entry is 32bits */
  ctassert(__is_ct_const(size));
  ctassert(__is_log2(size));
  ctassert((entries >= 128) && (entries <= SZ_16K));

  /* Configure ring details */
  ctm_ring_base_tmp.__raw = 0;
  ctm_ring_base_tmp.size = (__log2(entries) - 7);
  ctm_ring_base_tmp.status_type = NFP_CTM_RING_BASE_STATUS_TYPE_Empty;
  ctm_ring_base_tmp.base = (((unsigned int) base & ~(size - 1)) >>
                            NFP_CTM_RING_BASE_BASE_shf);
  ctm_ring_base = ctm_ring_base_tmp;

  xpb_addr = NFP_CTM_MISC_XPB_OFF + NFP_CTM_RING_BASE(rnum);
  __asm ct[xpb_write, ctm_ring_base, xpb_addr, 0, 1], sig_done[s1];

  /* Set pointers to zero */
  xpb_addr = NFP_CTM_MISC_XPB_OFF + NFP_CTM_RING_HEAD(rnum);
  __asm ct[xpb_write, ctm_ring_ptrs, xpb_addr, 0, 1], sig_done[s2];

  xpb_addr = NFP_CTM_MISC_XPB_OFF + NFP_CTM_RING_TAIL(rnum);
  __asm ct[xpb_write, ctm_ring_ptrs, xpb_addr, 0, 1], sig_done[s3];

  /* Perform an xpb_read to confirm the configuration completed */
  __asm ct[xpb_read, check, xpb_addr, 0, 1], sig_done[s4];

  wait_for_all(&s1, &s2, &s3, &s4);
  __implicit_read(&check);
}

__intrinsic void ctm_ring_get(unsigned int isl, unsigned int rnum,
                            __xread void* data, size_t size,
                            sync_t sync, SIGNAL* sig)
{
  unsigned int ind;
  unsigned int count = size / 4;
  unsigned int ring_addr = (isl << 24) | (rnum << 2);

  ctassert(__is_read_reg(data));
  ctassert(rnum < 16);
  ctassert(__is_ct_const(size));
  ctassert(__is_aligned(size, 4));
  ctassert(size <= (16*4));

  if (size <= (8*4)) {
    __asm { ct[ring_get, *data, ring_addr, 0, __ct_const_val(count)], \
                sig_done[*sig] }
  } else {
    ind = NFP_MECSR_PREV_ALU_LENGTH(count - 1) | NFP_MECSR_PREV_ALU_OV_LEN;
    __asm {
        alu[--, --, b, ind]
        ct[ring_get, *data, ring_addr, 0, __ct_const_val(count)], \
            indirect_ref, sig_done[*sig]
    }
  }

  if (sync == ctx_swap) {
    __wait_for_all(sig);
  }
}

__intrinsic void ctm_ring_put(unsigned int isl, unsigned int rnum,
                            __xwrite void* data, size_t size,
                            sync_t sync, SIGNAL* sig)
{
  unsigned int ind;
  unsigned int count = size / 4;
  unsigned int ring_addr = (isl << 24) | (rnum << 2);

  ctassert(__is_write_reg(data));
  ctassert(rnum < 16);
  ctassert(__is_ct_const(size));
  ctassert(__is_aligned(size, 4));
  ctassert(size <= (16*4));

  if (size <= (8*4)) {
    __asm { ct[ring_put, *data, ring_addr, 0, __ct_const_val(count)], \
                sig_done[*sig] }
  } else {
    ind = NFP_MECSR_PREV_ALU_LENGTH(count - 1) | NFP_MECSR_PREV_ALU_OV_LEN;
    __asm {
        alu[--, --, b, ind]
        ct[ring_put, *data, ring_addr, 0, __ct_const_val(count)], \
            indirect_ref, sig_done[*sig]
    }
  }

  if (sync == ctx_swap) {
    __wait_for_all(sig);
  }
}
