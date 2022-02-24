/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>

#include "connect.h"
#include "tas_ll.h"
#include "sp_app_if.h"
#include "internal.h"

#define NIC_RXQ_LEN (32 * 32 * 1024)
#define NIC_TXQ_LEN (32 * 16 * 1024)

static int ksock_fd = -1;
static int sp_evfd = 0;

void flextcp_sp_kick(void)
{
  /* TODO: Add poll cycle configuration */
  assert(sp_evfd != 0);
  uint64_t val = 1;
  int r = write(sp_evfd, &val, sizeof(uint64_t));
  assert(r == sizeof(uint64_t));
}

int flextcp_sp_connect(void)
{
  int fd, *pfd;
  uint32_t num_fds;
  ssize_t r;
  struct sockaddr_un saun;
  struct cmsghdr *cmsg;

  /* prepare socket address */
  memset(&saun, 0, sizeof(saun));
  saun.sun_family = AF_UNIX;
  memcpy(saun.sun_path, FLEXNIC_SOCKET_PATH, sizeof(FLEXNIC_SOCKET_PATH));

  if ((fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
    perror("flextcp_sp_connect: socket failed");
    return -1;
  }

  if (connect(fd, (struct sockaddr *) &saun, sizeof(saun)) != 0) {
    perror("flextcp_sp_connect: connect failed");
    return -1;
  }

  struct iovec iov = {
    .iov_base = &num_fds,
    .iov_len = sizeof(uint32_t),
  };
  union {
    char buf[CMSG_SPACE(sizeof(int) * 4)];
    struct cmsghdr align;
  } u;
  struct msghdr msg = {
    .msg_name = NULL,
    .msg_namelen = 0,
    .msg_iov = &iov,
    .msg_iovlen = 1,
    .msg_control = u.buf,
    .msg_controllen = sizeof(u.buf),
    .msg_flags = 0,
  };

  /* receive welcome message:
   *   contains the fd for the sp, and the count of flexnic fds */
  if ((r = recvmsg(fd, &msg, 0)) != sizeof(uint32_t)) {
    fprintf(stderr, "flextcp_sp_connect: recvmsg failed (%zd)\n", r);
    abort();
  }

  /* get sp fd from welcome message */
  cmsg = CMSG_FIRSTHDR(&msg);
  pfd = (int *) CMSG_DATA(cmsg);
  if (msg.msg_controllen <= 0 || cmsg->cmsg_len != CMSG_LEN(sizeof(int))) {
    fprintf(stderr, "flextcp_sp_connect: accessing ancillary data "
        "failed\n");
    abort();
  }

  sp_evfd = *pfd;
  ksock_fd = fd;

  return 0;
}

int flextcp_sp_newctx(struct flextcp_context *ctx)
{
  ssize_t sz, off, total_sz;
  struct sp_uxsock_response *resp;
  uint8_t resp_buf[sizeof(*resp)];
  struct sp_uxsock_request req = {
      .rxq_len = NIC_RXQ_LEN,
      .txq_len = NIC_TXQ_LEN,
    };

  /* send request on sp socket */
  struct iovec iov = {
    .iov_base = &req,
    .iov_len = sizeof(req),
  };
  union {
    char buf[CMSG_SPACE(sizeof(int))];
    struct cmsghdr align;
  } u;
  struct msghdr msg = {
    .msg_name = NULL,
    .msg_namelen = 0,
    .msg_iov = &iov,
    .msg_iovlen = 1,
    .msg_control = u.buf,
    .msg_controllen = sizeof(u.buf),
    .msg_flags = 0,
  };
  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof(int));
  int *myfd = (int *)CMSG_DATA(cmsg);
  *myfd = ctx->evfd;
  sz = sendmsg(ksock_fd, &msg, 0);
  assert(sz == sizeof(req));

  /* receive response on sp socket */
  resp = (struct sp_uxsock_response *) resp_buf;
  off = 0;
  while (off < sizeof(*resp)) {
    sz = read(ksock_fd, (uint8_t *) resp + off, sizeof(*resp) - off);
    if (sz < 0) {
      perror("flextcp_sp_newctx: read failed");
      return -1;
    }
    off += sz;
  }

  /* receive queues in response */
  total_sz = sizeof(*resp);
  while (off < total_sz) {
    sz = read(ksock_fd, (uint8_t *) resp + off, total_sz - off);
    if (sz < 0) {
      perror("flextcp_sp_newctx: read failed");
      return -1;
    }
    off += sz;
  }

  if (resp->status != 0) {
    fprintf(stderr, "flextcp_sp_newctx: request failed\n");
    return -1;
  }

  /* fill in ctx struct */
  ctx->spin_base = (uint8_t *) flexnic_mem + resp->app_out_off;
  ctx->spin_len = resp->app_out_len / sizeof(struct sp_appout);
  ctx->spin_head = 0;

  ctx->spout_base = (uint8_t *) flexnic_mem + resp->app_in_off;
  ctx->spout_len = resp->app_in_len /  sizeof(struct sp_appin);
  ctx->spout_head = 0;

  ctx->db_id = resp->flexnic_db_id;

  ctx->rxq_len = NIC_RXQ_LEN/sizeof(struct flextcp_pl_arx_t);
  ctx->txq_len = NIC_TXQ_LEN/sizeof(struct flextcp_pl_atx_t);
  ctx->rxq_base = (uint8_t *) flexnic_mem + resp->rxq_off;
  ctx->txq_base = (uint8_t *) flexnic_mem + resp->txq_off;
  ctx->rxq_head = 0;
  ctx->txq_tail = 0;
  ctx->txq_avail = ctx->txq_len - 1;
  ctx->last_ts = 0;

  return 0;
}
