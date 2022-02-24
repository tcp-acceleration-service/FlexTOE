#include <nfp/me.h>
#include <std/reorder.h>

__intrinsic
void reorder_start(unsigned int start_ctx, SIGNAL* sig)
{
  ctassert(__is_ct_const(start_ctx));

  signal_ctx(start_ctx, __signal_number(sig));
  __implicit_write(sig);
}

__intrinsic
void reorder_signal(unsigned int next_ctx, SIGNAL* sig)
{
  ctassert(__is_ct_const(next_ctx));

  signal_ctx(next_ctx, __signal_number(sig));
  __implicit_write(sig);
}

__intrinsic
void reorder_signal_next(SIGNAL* sig)
{
  signal_next_ctx(__signal_number(sig));
  __implicit_write(sig);
}

__intrinsic void reorder_self(SIGNAL* sig)
{
  unsigned int ctx;

  ctx = ctx();
  signal_ctx(ctx, __signal_number(sig));
  __implicit_write(sig);
}
