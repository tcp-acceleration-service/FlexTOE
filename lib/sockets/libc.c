/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "util/common.h"

static int (*libc_epoll_create1)(int flags) = NULL;
static int (*libc_epoll_ctl)(int epfd, int op, int fd,
    struct epoll_event *event) = NULL;
static int (*libc_epoll_wait)(int epfd, struct epoll_event *events,
    int maxevents, int timeout) = NULL;
static int (*libc_poll)(struct pollfd *fds, nfds_t nfds, int timeout);
static int (*libc_close)(int fd);
static int (*libc_dup)(int oldfd);
static int (*libc_dup2)(int oldfd, int newfd);
static int (*libc_dup3)(int oldfd, int newfd, int flags);

static inline void ensure_init(void);


int tas_libc_epoll_create1(int flags)
{
  ensure_init();
  return libc_epoll_create1(flags);
}

int tas_libc_epoll_ctl(int epfd, int op, int fd,
    struct epoll_event *event)
{
  ensure_init();
  return libc_epoll_ctl(epfd, op, fd, event);
}

int tas_libc_epoll_wait(int epfd, struct epoll_event *events,
    int maxevents, int timeout)
{
  ensure_init();
  return libc_epoll_wait(epfd, events, maxevents, timeout);
}

int tas_libc_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
  ensure_init();
  return libc_poll(fds, nfds, timeout);
}

int tas_libc_close(int fd)
{
  ensure_init();
  return libc_close(fd);
}

int tas_libc_dup(int oldfd)
{
  ensure_init();
  return libc_dup(oldfd);
}

int tas_libc_dup2(int oldfd, int newfd)
{
  ensure_init();
  return libc_dup2(oldfd, newfd);
}

int tas_libc_dup3(int oldfd, int newfd, int flags)
{
  ensure_init();
  return libc_dup3(oldfd, newfd, flags);
}


/******************************************************************************/
/* Helper functions */

static void *bind_symbol(void *handle, const char *sym)
{
  void *ptr;
  if ((ptr = dlsym(handle, sym)) == NULL) {
    fprintf(stderr, "tas libc lookup: dlsym failed (%s)\n", sym);
    abort();
  }
  return ptr;
}

static void init(void)
{
  void *handle;

  if ((handle = dlopen("libc.so.6", RTLD_LAZY)) == NULL) {
    perror("tas libc lookup: dlopen on libc failed");
    abort();
  }

  libc_close = bind_symbol(handle, "close");
  libc_epoll_create1 = bind_symbol(handle, "epoll_create1");
  libc_epoll_ctl = bind_symbol(handle, "epoll_ctl");
  libc_epoll_wait = bind_symbol(handle, "epoll_wait");
  libc_poll = bind_symbol(handle, "poll");
  libc_dup = bind_symbol(handle, "dup");
  libc_dup2 = bind_symbol(handle, "dup2");
  libc_dup3 = bind_symbol(handle, "dup3");
}

static inline void ensure_init(void)
{
  static volatile uint32_t init_cnt = 0;
  static volatile uint8_t init_done = 0;
  static __thread uint8_t in_init = 0;

  if (init_done == 0) {
    /* during init the socket functions will be used to connect to the kernel on
     * a unix socket, so make sure that runs through. */
    if (in_init) {
      return;
    }

    if (__sync_fetch_and_add(&init_cnt, 1) == 0) {
      in_init = 1;
      init();
      in_init = 0;
      MEM_BARRIER();
      init_done = 1;
    } else {
      while (init_done == 0) {
        pthread_yield();
      }
      MEM_BARRIER();
    }
  }
}
