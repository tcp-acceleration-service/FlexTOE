/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef INTERNAL_H_
#define INTERNAL_H_

#include "util/common.h"

#include "connect.h"
#include "tas_ll.h"
#include "fp_mem.h"

#define OPAQUE_PTR(x) ((void *) (uintptr_t) (x))
#define OPAQUE(x) ((uintptr_t) (x))

/* ctx->flags of internal state: */
#define CTX_FLAG_POLL_CALLED  (1 << 0)  /*> Set whenever context_poll is called */
#define CTX_FLAG_POLL_EVENTS  (1 << 1)  /*> Set whenever context_poll finds events in queue */
#define CTX_FLAG_WANTWAIT     (1 << 2)  /*> Indicates that the grace period for blocking is currently running with a caller waiting for permission to block. */
#define CTX_FLAG_LASTWAIT     (1 << 3)  /*> Grace period is over, after polling once more, blocking will be allowed. */

#define CONN_FLAG_TXEOS       (1 << 0)
#define CONN_FLAG_TXEOS_ALLOC (1 << 1)
#define CONN_FLAG_TXEOS_ACK   (1 << 2)
#define CONN_FLAG_RXEOS       (1 << 3)

enum conn_state {
  CONN_CLOSED,
  CONN_OPEN_REQUESTED,
  CONN_ACCEPT_REQUESTED,
  CONN_OPEN,
  CONN_CLOSE_REQUESTED,
};

extern void *flexnic_mem;
extern struct flexnic_info_t *flexnic_info;

int flextcp_sp_connect(void);
int flextcp_sp_newctx(struct flextcp_context *ctx);
void flextcp_sp_kick(void);

int flextcp_context_tx_alloc(struct flextcp_context *ctx,
    struct flextcp_pl_atx_t **atx);
void flextcp_context_tx_done(struct flextcp_context *ctx);

uint32_t flextcp_conn_txbuf_available(struct flextcp_connection *conn);
int flextcp_conn_pushtxeos(struct flextcp_context *ctx,
        struct flextcp_connection *conn);

#endif /* INTERNAL_H_ */
