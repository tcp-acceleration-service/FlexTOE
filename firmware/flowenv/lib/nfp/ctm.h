#ifndef _NFP__CTM_H_
#define _NFP__CTM_H_

#include <nfp.h>
#include <types.h>

__intrinsic void ctm_ring_setup(unsigned int rnum, __ctm40 void* base, size_t size);
__intrinsic void ctm_ring_get(unsigned int isl, unsigned int rnum,
                            __xread void* data, size_t size,
                            sync_t sync, SIGNAL* sig);
__intrinsic void ctm_ring_put(unsigned int isl, unsigned int rnum,
                            __xwrite void* data, size_t size,
                            sync_t sync, SIGNAL* sig);

#endif /* _NFP__CTM_H_ */
