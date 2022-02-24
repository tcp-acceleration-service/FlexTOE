#ifndef _STD__REORDER_H_
#define _STD__REORDER_H_

#include <nfp.h>

__intrinsic void reorder_start(unsigned int start_ctx, SIGNAL* sig);

__intrinsic void reorder_signal(unsigned int next_ctx, SIGNAL* sig);

__intrinsic void reorder_signal_next(SIGNAL* sig);

__intrinsic void reorder_self(SIGNAL* sig);

#endif /* _STD__REORDER_H_ */
