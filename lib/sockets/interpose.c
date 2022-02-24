/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#define __USE_GNU
#include <dlfcn.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

#include "util/common.h"
#include "tas_sockets.h"

// #define STRACE_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#define STRACE_DEBUG(...) do {} while(0)

static inline void ensure_init(void);

/* Function pointers to the libc functions */
static int (*libc_socket)(int domain, int type, int protocol) = NULL;
static int (*libc_close)(int sockfd) = NULL;
static int (*libc_shutdown)(int sockfd, int how) = NULL;
static int (*libc_bind)(int sockfd, const struct sockaddr *addr,
    socklen_t addrlen) = NULL;
static int (*libc_connect)(int sockfd, const struct sockaddr *addr,
    socklen_t addrlen) = NULL;
static int (*libc_listen)(int sockfd, int backlog) = NULL;
static int (*libc_accept4)(int sockfd, struct sockaddr *addr,
    socklen_t *addrlen, int flags) = NULL;
static int (*libc_accept)(int sockfd, struct sockaddr *addr,
    socklen_t *addrlen) = NULL;
static int (*libc_fcntl)(int sockfd, int cmd, ...) = NULL;
static int (*libc_fcntl64)(int sockfd, int cmd, ...) = NULL;
static int (*libc_getsockopt)(int sockfd, int level, int optname, void *optval,
    socklen_t *optlen) = NULL;
static int (*libc_setsockopt)(int sockfd, int level, int optname,
    const void *optval, socklen_t optlen) = NULL;
static int (*libc_getsockname)(int sockfd, struct sockaddr *addr,
    socklen_t *addrlen) = NULL;
static int (*libc_getpeername)(int sockfd, struct sockaddr *addr,
    socklen_t *addrlen) = NULL;
static ssize_t (*libc_read)(int fd, void *buf, size_t count) = NULL;
static ssize_t (*libc_recv)(int sockfd, void *buf, size_t len,
    int flags) = NULL;
static ssize_t (*libc_recvfrom)(int sockfd, void *buf, size_t len, int flags,
    struct sockaddr *src_addr, socklen_t *addrlen) = NULL;
static ssize_t (*libc_recvmsg)(int sockfd, struct msghdr *msg, int flags)
    = NULL;
static ssize_t (*libc_readv)(int sockfd, const struct iovec *iov, int iovcnt)
    = NULL;
static ssize_t (*libc_pread)(int sockfd, void *buf, size_t count, off_t offset)
    = NULL;
static ssize_t (*libc_write)(int fd, const void *buf, size_t count) = NULL;
static ssize_t (*libc_send)(int sockfd, const void *buf, size_t len, int flags)
    = NULL;
static ssize_t (*libc_sendto)(int sockfd, const void *buf, size_t len,
    int flags, const struct sockaddr *dest_addr, socklen_t addrlen) = NULL;
static ssize_t (*libc_sendmsg)(int sockfd, const struct msghdr *msg, int flags)
    = NULL;
static ssize_t (*libc_writev)(int sockfd, const struct iovec *iov, int iovcnt)
    = NULL;
static ssize_t (*libc_pwrite)(int sockfd, const void *buf, size_t count,
    off_t offset) = NULL;
static ssize_t (*libc_sendfile)(int sockfd, int in_fd, off_t *offset,
    size_t len) = NULL;
static long (*libc_syscall)(long num, ...) = NULL;

int socket(int domain, int type, int protocol)
{
  int ret;
  ensure_init();

  /* if not a TCP socket, pass call to libc */
  if (domain != AF_INET ||
      (type & ~(SOCK_NONBLOCK | SOCK_CLOEXEC)) != SOCK_STREAM) {
    return libc_socket(domain, type, protocol);
  }

  ret = tas_socket(domain, type, protocol);
  STRACE_DEBUG("%s(%d, %d, %d) = %d\n", __func__, domain, type, protocol, ret);
  return ret;
}

int close(int sockfd)
{
  int ret;
  ensure_init();
  if ((ret = tas_close(sockfd)) == -1 && errno == EBADF) {
    return libc_close(sockfd);
  }
  STRACE_DEBUG("%s(%d) = %d\n", __func__, sockfd, ret);
  return ret;
}

int shutdown(int sockfd, int how)
{
  int ret;
  ensure_init();
  if ((ret = tas_shutdown(sockfd, how)) == -1 && errno == EBADF) {
    return libc_shutdown(sockfd, how);
  }
  STRACE_DEBUG("%s(%d, %d) = %d\n", __func__, sockfd, how, ret);
  return ret;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_bind(sockfd, addr, addrlen)) == -1 && errno == EBADF) {
    return libc_bind(sockfd, addr, addrlen);
  }

  STRACE_DEBUG("%s(%d, %p, %u) = %d\n", __func__, sockfd, addr, addrlen, ret);
  return ret;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_connect(sockfd, addr, addrlen)) == -1 && errno == EBADF) {
    return libc_connect(sockfd, addr, addrlen);
  }

  STRACE_DEBUG("%s(%d, %p, %u) = %d\n", __func__, sockfd, addr, addrlen, ret);
  return ret;
}

int listen(int sockfd, int backlog)
{
  int ret;
  ensure_init();
  if ((ret = tas_listen(sockfd, backlog)) == -1 && errno == EBADF) {
    return libc_listen(sockfd, backlog);
  }
  STRACE_DEBUG("%s(%d, %d) = %d\n", __func__, sockfd, backlog, ret);
  return ret;
}

int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen,
    int flags)
{
  int ret;
  ensure_init();
  if ((ret = tas_accept4(sockfd, addr, addrlen, flags)) == -1 &&
      errno == EBADF)
  {
    return libc_accept4(sockfd, addr, addrlen, flags);
  }
  STRACE_DEBUG("%s(%d, %p, %p, %d) = %d\n", __func__, sockfd, addr, addrlen, flags, ret);
  return ret;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_accept(sockfd, addr, addrlen)) == -1 && errno == EBADF) {
    return libc_accept(sockfd, addr, addrlen);
  }
  STRACE_DEBUG("%s(%d, %p, %p) = %d\n", __func__, sockfd, addr, addrlen, ret);
  return ret;
}

static int vfcntl(int sockfd, int cmd, va_list val, int is_64)
{
  int ret, arg_i;
  void *arg_p;

  /* this is pretty ugly, but unfortunately there is no other way to interpose
   * on variadic functions and pass along all arguments. */
  switch (cmd) {
    /* these take no argument */
    case F_GETFD:
    case F_GETFL:
    case F_GETOWN:
    case F_GETSIG:
    case F_GETLEASE:
    case F_GETPIPE_SZ:
#ifdef F_GET_SEALS
    case F_GET_SEALS:
#endif
      if ((ret = tas_fcntl(sockfd, cmd)) == -1 && errno == EBADF) {
        if (is_64)
          ret = libc_fcntl64(sockfd, cmd);
        else
          ret = libc_fcntl(sockfd, cmd);
      }
      break;


    /* these take int as an argument */
    case F_DUPFD:
    case F_DUPFD_CLOEXEC:
    case F_SETFD:
    case F_SETFL:
    case F_SETOWN:
    case F_SETSIG:
    case F_SETLEASE:
    case F_NOTIFY:
    case F_SETPIPE_SZ:
#ifdef F_ADD_SEALS
    case F_ADD_SEALS:
#endif
      arg_i = va_arg(val, int);
      if ((ret = tas_fcntl(sockfd, cmd, arg_i)) == -1 && errno == EBADF) {
        if (is_64)
          ret = libc_fcntl64(sockfd, cmd, arg_i);
        else
          ret = libc_fcntl(sockfd, cmd, arg_i);
      }
      break;


    /* these take a pointer as an argument */
    case F_SETLK:
    case F_SETLKW:
    case F_GETLK:
    case F_OFD_SETLK:
    case F_OFD_SETLKW:
    case F_OFD_GETLK:
    case F_GETOWN_EX:
    case F_SETOWN_EX:
#ifdef F_GET_RW_HINT
    case F_GET_RW_HINT:
    case F_SET_RW_HINT:
#endif
#ifdef F_GET_FILE_RW_HINT
    case F_GET_FILE_RW_HINT:
    case F_SET_FILE_RW_HINT:
#endif
      arg_p = va_arg(val, void *);
      if ((ret = tas_fcntl(sockfd, cmd, arg_p)) == -1 && errno == EBADF) {
        if (is_64)
          ret = libc_fcntl64(sockfd, cmd, arg_p);
        else
          ret = libc_fcntl(sockfd, cmd, arg_p);
      }
      break;

    /* unsupported */
    default:
      fprintf(stderr, "tas fcntl wrapper: unsupported cmd (%u)\n", cmd);
      errno = EINVAL;
      ret = -1;
      break;
  }

  return ret;

}

int fcntl(int sockfd, int cmd, ...)
{
  int ret;
  va_list val;
  ensure_init();

  va_start(val, cmd);
  ret = vfcntl(sockfd, cmd, val, 0);
  va_end(val);

  return ret;
}

int fcntl64(int sockfd, int cmd, ...)
{
  int ret;
  va_list val;
  ensure_init();

  va_start(val, cmd);
  ret = vfcntl(sockfd, cmd, val, 1);
  va_end(val);

  return ret;
}

int getsockopt(int sockfd, int level, int optname, void *optval,
    socklen_t *optlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_getsockopt(sockfd, level, optname, optval, optlen)) == -1 &&
      errno == EBADF)
  {
    return libc_getsockopt(sockfd, level, optname, optval, optlen);
  }
  STRACE_DEBUG("%s(%d, %d, %d, %p, %p) = %d\n", __func__, sockfd, level, optname, optval, optlen, ret);
  return ret;
}

int setsockopt(int sockfd, int level, int optname, const void *optval,
    socklen_t optlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_setsockopt(sockfd, level, optname, optval, optlen)) == -1 &&
      errno == EBADF)
  {
    return libc_setsockopt(sockfd, level, optname, optval, optlen);
  }
  STRACE_DEBUG("%s(%d, %d, %d, %p, %u) = %d\n", __func__, sockfd, level, optname, optval, optlen, ret);
  return ret;
}

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_getsockname(sockfd, addr, addrlen)) == -1 && errno == EBADF) {
    return libc_getsockname(sockfd, addr, addrlen);
  }
  return ret;
}

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  int ret;
  ensure_init();
  if ((ret = tas_getpeername(sockfd, addr, addrlen)) == -1 && errno == EBADF) {
    return libc_getpeername(sockfd, addr, addrlen);
  }
  return ret;
}

ssize_t read(int sockfd, void *buf, size_t count)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_read(sockfd, buf, count)) == -1 && errno == EBADF) {
    return libc_read(sockfd, buf, count);
  }
  STRACE_DEBUG("%s(%d, %p, %lu) = %ld\n", __func__, sockfd, buf, count, ret);
  return ret;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_recv(sockfd, buf, len, flags)) == -1 && errno == EBADF) {
    return libc_recv(sockfd, buf, len, flags);
  }
  STRACE_DEBUG("%s(%d, %p, %lu, %d) = %ld\n", __func__, sockfd, buf, len, flags, ret);
  return ret;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
    struct sockaddr *src_addr, socklen_t *addrlen)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) == -1 &&
      errno == EBADF)
  {
    return libc_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
  }
  STRACE_DEBUG("%s(%d, %p, %lu, %d, %p, %p) = %ld\n", __func__, sockfd, buf, len, flags, src_addr, addrlen, ret);
  return ret;
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_recvmsg(sockfd, msg, flags)) == -1 && errno == EBADF) {
    return libc_recvmsg(sockfd, msg, flags);
  }
  STRACE_DEBUG("%s(%d, %p, %d) = %ld\n", __func__, sockfd, msg, flags, ret);
  return ret;
}

ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_readv(sockfd, iov, iovcnt)) == -1 && errno == EBADF) {
    return libc_readv(sockfd, iov, iovcnt);
  }
  STRACE_DEBUG("%s(%d, %p, %d) = %ld\n", __func__, sockfd, iov, iovcnt, ret);
  return ret;
}

ssize_t pread(int sockfd, void *buf, size_t count, off_t offset)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_pread(sockfd, buf, count, offset)) == -1 && errno == EBADF) {
    return libc_pread(sockfd, buf, count, offset);
  }
  STRACE_DEBUG("%s(%d, %p, %lu, %ld) = %ld\n", __func__, sockfd, buf, count, offset, ret);
  return ret;
}

ssize_t write(int sockfd, const void *buf, size_t count)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_write(sockfd, buf, count)) == -1 && errno == EBADF) {
    return libc_write(sockfd, buf, count);
  }
  STRACE_DEBUG("%s(%d, %p, %lu) = %ld\n", __func__, sockfd, buf, count, ret);
  return ret;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_send(sockfd, buf, len, flags)) == -1 && errno == EBADF) {
    return libc_send(sockfd, buf, len, flags);
  }
  STRACE_DEBUG("%s(%d, %p, %lu, %d) = %ld\n", __func__, sockfd, buf, len, flags, ret);
  return ret;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_sendto(sockfd, buf, len, flags, dest_addr, addrlen)) == -1 &&
      errno == EBADF)
  {
    return libc_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
  }
  STRACE_DEBUG("%s(%d, %p, %lu, %d, %p, %u) = %ld\n", __func__, sockfd, buf, len, flags, dest_addr, addrlen, ret);
  return ret;
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_sendmsg(sockfd, msg, flags)) == -1 && errno == EBADF) {
    return libc_sendmsg(sockfd, msg, flags);
  }
  STRACE_DEBUG("%s(%d, %p, %d) = %ld\n", __func__, sockfd, msg, flags, ret);
  return ret;
}

ssize_t writev(int sockfd, const struct iovec *iov, int iovcnt)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_writev(sockfd, iov, iovcnt)) == -1 && errno == EBADF) {
    return libc_writev(sockfd, iov, iovcnt);
  }
  STRACE_DEBUG("%s(%d, %p, %d) = %ld\n", __func__, sockfd, iov, iovcnt, ret);
  return ret;
}

ssize_t pwrite(int sockfd, const void *buf, size_t count, off_t offset)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_pwrite(sockfd, buf, count, offset)) == -1 && errno == EBADF) {
    return libc_pwrite(sockfd, buf, count, offset);
  }
  STRACE_DEBUG("%s(%d, %p, %lu, %ld) = %ld\n", __func__, sockfd, buf, count, offset, ret);
  return ret;
}

ssize_t sendfile(int sockfd, int in_fd, off_t *offset, size_t len)
{
  ssize_t ret;
  ensure_init();
  if ((ret = tas_sendfile(sockfd, in_fd, offset, len)) == -1 && errno == EBADF) {
    return libc_sendfile(sockfd, in_fd, offset, len);
  }
  STRACE_DEBUG("%s(%d, %d, %p, %lu) = %ld\n", __func__, sockfd, in_fd, offset, len, ret);
  return ret;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
    struct timeval *timeout)
{
  int ret;
  ensure_init();
  ret = tas_select(nfds, readfds, writefds, exceptfds, timeout);
  STRACE_DEBUG("%s(%d, %p, %p, %p, %p) = %d\n", __func__, nfds, readfds, writefds, exceptfds, timeout, ret);
  return ret;
}

int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
    const struct timespec *timeout, const sigset_t *sigmask)
{
  int ret;
  ensure_init();
  ret = tas_pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
  STRACE_DEBUG("%s(%d, %p, %p, %p, %p, %p) = %d\n", __func__, nfds, readfds, writefds, exceptfds, timeout, sigmask, ret);
  return ret;
}

int epoll_create(int size)
{
  int ret;
  ensure_init();
  ret = tas_epoll_create(size);
  STRACE_DEBUG("%s(%d) = %d\n", __func__, size, ret);
  return ret;
}

int epoll_create1(int flags)
{
  int ret;
  ensure_init();
  ret = tas_epoll_create1(flags);
  STRACE_DEBUG("%s(%d) = %d\n", __func__, flags, ret);
  return ret;
}

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  int ret;
  ensure_init();
  ret = tas_epoll_ctl(epfd, op, fd, event);
  STRACE_DEBUG("%s(%d, %d, %d, %p) = %d\n", __func__, epfd, op, fd, event, ret);
  return ret;
}

int epoll_wait(int epfd, struct epoll_event *events, int maxevents,
    int timeout)
{
  int ret;
  ensure_init();
  ret = tas_epoll_wait(epfd, events, maxevents, timeout);
  STRACE_DEBUG("%s(%d, %p, %d, %d) = %d\n", __func__, epfd, events, maxevents, timeout, ret);
  return ret;
}

int epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
    int timeout, const sigset_t *sigmask)
{
  int ret;
  ensure_init();
  ret = tas_epoll_pwait(epfd, events, maxevents, timeout, sigmask);
  STRACE_DEBUG("%s(%d, %p, %d, %d, %p) = %d\n", __func__, epfd, events, maxevents, timeout, sigmask, ret);
  return ret;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
  int ret;
  ensure_init();
  ret = tas_poll(fds, nfds, timeout);
  STRACE_DEBUG("%s(%p, %lu, %d) = %d\n", __func__, fds, nfds, timeout, ret);
  return ret;
}

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *tmo_p,
    const sigset_t *sigmask)
{
  int ret;
  ensure_init();
  ret = tas_ppoll(fds, nfds, tmo_p, sigmask);
  STRACE_DEBUG("%s(%p, %lu, %p, %p) = %d\n", __func__, fds, nfds, tmo_p, sigmask, ret);
  return ret;
}

int dup(int oldfd)
{
  return tas_dup(oldfd);
}

int dup2(int oldfd, int newfd)
{
  return tas_dup2(oldfd, newfd);
}

int dup3(int oldfd, int newfd, int flags)
{
  return tas_dup3(oldfd, newfd, flags);
}

/* I really apologize to anyone reading this particular piece of code.
 * So apparently there is code out there that calls some socket calls directly
 * with syscall (nodejs being one example). In this case these get past our
 * interposition layer and break things.
 */
long syscall(long number, ...)
{
  long ret;
  va_list val;
  long arg1, arg2, arg3, arg4, arg5, arg6;

  ensure_init();

  /* pray to god that this is safe on X86-64... */
  va_start(val, number);
  arg1 = va_arg(val, long);
  arg2 = va_arg(val, long);
  arg3 = va_arg(val, long);
  arg4 = va_arg(val, long);
  arg5 = va_arg(val, long);
  arg6 = va_arg(val, long);
  va_end(val);

  switch (number) {
    case SYS_socket:
      return socket((int) arg1, (int) arg2, (int) arg3);
    case SYS_close:
      return close((int) arg1);
    case SYS_shutdown:
      return shutdown((int) arg1, (int) arg2);
    case SYS_bind:
      return bind((int) arg1, (const struct sockaddr *) (uintptr_t) arg2,
          (socklen_t) arg3);
    case SYS_connect:
      return connect((int) arg1, (const struct sockaddr *) (uintptr_t) arg2,
          (socklen_t) arg3);
    case SYS_listen:
      return listen((int) arg1, (int) arg2);
    case SYS_accept4:
      return accept4((int) arg1, (struct sockaddr *) (uintptr_t) arg2,
          (socklen_t *) (uintptr_t) arg3, (int) arg4);
    case SYS_accept:
      return accept((int) arg1, (struct sockaddr *) (uintptr_t) arg2,
          (socklen_t *) (uintptr_t) arg3);
    case SYS_fcntl:
      va_start(val, number);
      (void) va_arg(val, long);
      (void) va_arg(val, long);
      ret = vfcntl((int) arg1, (int) arg2, val, 0);
      va_end(val);
      return ret;
#ifdef SYS_fcntl64
    case SYS_fcntl64:
      va_start(val, number);
      (void) va_arg(val, long);
      (void) va_arg(val, long);
      ret = vfcntl((int) arg1, (int) arg2, val, 1);
      va_end(val);
      return ret;
#endif
    case SYS_getsockopt:
      return getsockopt((int) arg1, (int) arg2, (int) arg3,
          (void *) (uintptr_t) arg4, (socklen_t *) (uintptr_t) arg5);
    case SYS_setsockopt:
      return setsockopt((int) arg1, (int) arg2, (int) arg3,
          (const void *) (uintptr_t) arg4, (socklen_t) arg5);
    case SYS_getsockname:
      return getsockname((int) arg1, (struct sockaddr *) (uintptr_t) arg2,
          (socklen_t *) (uintptr_t) arg3);
    case SYS_getpeername:
      return getpeername((int) arg1, (struct sockaddr *) (uintptr_t) arg2,
          (socklen_t *) (uintptr_t) arg3);

    case SYS_read:
      return read((int) arg1, (void *) (uintptr_t) arg2, (size_t) arg3);
#ifdef SYS_recv
    case SYS_recv:
      return recv((int) arg1, (void *) (uintptr_t) arg2, (size_t) arg3,
          (int) arg4);
#endif
    case SYS_recvfrom:
      return recvfrom((int) arg1, (void *) (uintptr_t) arg2, (size_t) arg3,
          (int) arg4, (struct sockaddr *) (uintptr_t) arg5,
          (socklen_t *) (uintptr_t) arg6);
    case SYS_recvmsg:
      return recvmsg((int) arg1, (struct msghdr *) (uintptr_t) arg2,
          (int) arg3);
    case SYS_readv:
      return readv((int) arg1, (struct iovec *) (uintptr_t) arg2,
          (int) arg3);
    case SYS_pread64:
      return pread((int) arg1, (void *) (uintptr_t) arg2, (size_t) arg3,
          (off_t) arg4);

    case SYS_write:
      return write((int) arg1, (const void *) (uintptr_t) arg2, (size_t) arg3);
#ifdef SYS_send
    case SYS_send:
      return send((int) arg1, (const void *) (uintptr_t) arg2, (size_t) arg3,
          (int) arg4);
#endif
    case SYS_sendto:
      return sendto((int) arg1,(const void *) (uintptr_t) arg2, (size_t) arg3,
          (int) arg4, (const struct sockaddr *) (uintptr_t) arg5,
          (socklen_t) arg6);
    case SYS_sendmsg:
      return sendmsg((int) arg1, (const struct msghdr *) (uintptr_t) arg2,
          (int) arg3);
    case SYS_writev:
      return writev((int) arg1, (const struct iovec *) (uintptr_t) arg2,
          (int) arg3);
    case SYS_pwrite64:
      return pwrite((int) arg1, (const void *) (uintptr_t) arg2, (size_t) arg3,
          (off_t) arg4);
    case SYS_sendfile:
      return sendfile((int) arg1, (int) arg2, (off_t *) (uintptr_t) arg3,
          (size_t) arg4);

    case SYS_select:
      return select((int) arg1, (fd_set *) (uintptr_t) arg2,
          (fd_set *) (uintptr_t) arg3, (fd_set *) (uintptr_t) arg4,
          (struct timeval *) (uintptr_t) arg5);
    case SYS_pselect6:
      fprintf(stderr, "tas syscall(): warning our pselect6 does not update "
          "timeout value\n");
      return pselect((int) arg1, (fd_set *) (uintptr_t) arg2,
          (fd_set *) (uintptr_t) arg3, (fd_set *) (uintptr_t) arg4,
          (struct timespec *) (uintptr_t) arg5,
          (const sigset_t *) (uintptr_t) arg6);
    case SYS_epoll_create:
      return epoll_create((int) arg1);
    case SYS_epoll_create1:
      return epoll_create1((int) arg1);
    case SYS_epoll_ctl:
      return epoll_ctl((int) arg1, (int) arg2, (int) arg3,
          (struct epoll_event *) (uintptr_t) arg4);
    case SYS_epoll_wait:
      return epoll_wait((int) arg1, (struct epoll_event *) (uintptr_t) arg2,
          (int) arg3, (int) arg4);
    case SYS_epoll_pwait:
      return epoll_pwait((int) arg1, (struct epoll_event *) (uintptr_t) arg2,
          (int) arg3, (int) arg4, (const sigset_t *) (uintptr_t) arg5);
    case SYS_poll:
      return poll((struct pollfd *) (uintptr_t) arg1, (nfds_t) arg2,
          (int) arg3);
    case SYS_ppoll:
      return ppoll((struct pollfd *) (uintptr_t) arg1, (nfds_t) arg2,
          (const struct timespec *) (uintptr_t) arg3,
          (const sigset_t *) (uintptr_t) arg4);

    case SYS_dup:
      return dup((int) arg1);
    case SYS_dup2:
      return dup2((int) arg1, (int) arg2);
    case SYS_dup3:
      return dup3((int) arg1, (int) arg2, (int) arg3);
  }

  return libc_syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
}

/******************************************************************************/
/* Helper functions */

static void *bind_symbol(const char *sym)
{
  void *ptr;
  if ((ptr = dlsym(RTLD_NEXT, sym)) == NULL) {
    fprintf(stderr, "flextcp socket interpose: dlsym failed (%s)\n", sym);
    abort();
  }
  return ptr;
}

static void init(void)
{
  libc_socket = bind_symbol("socket");
  libc_close = bind_symbol("close");
  libc_shutdown = bind_symbol("shutdown");
  libc_bind = bind_symbol("bind");
  libc_connect = bind_symbol("connect");
  libc_listen = bind_symbol("listen");
  libc_accept4 = bind_symbol("accept4");
  libc_accept = bind_symbol("accept");
  libc_fcntl = bind_symbol("fcntl");

  libc_fcntl64 = dlsym(RTLD_NEXT, "fcntl64");
  if (libc_fcntl64 == NULL)
      libc_fcntl64 = libc_fcntl;

  libc_getsockopt = bind_symbol("getsockopt");
  libc_setsockopt = bind_symbol("setsockopt");
  libc_getsockname = bind_symbol("getsockname");
  libc_getpeername = bind_symbol("getpeername");
  libc_read = bind_symbol("read");
  libc_recv = bind_symbol("recv");
  libc_recvfrom = bind_symbol("recvfrom");
  libc_recvmsg = bind_symbol("recvmsg");
  libc_readv = bind_symbol("readv");
  libc_pread = bind_symbol("pread");
  libc_write = bind_symbol("write");
  libc_send = bind_symbol("send");
  libc_sendto = bind_symbol("sendto");
  libc_sendmsg = bind_symbol("sendmsg");
  libc_writev = bind_symbol("writev");
  libc_pwrite = bind_symbol("pwrite");
  libc_sendfile = bind_symbol("sendfile");
  libc_syscall = bind_symbol("syscall");

  if (tas_init() != 0) {
    abort();
  }
}

static inline void ensure_init(void)
{
  static volatile uint32_t init_cnt = 0;
  static volatile uint8_t init_done = 0;
  static __thread uint8_t in_init = 0;

  if (init_done == 0) {
    /* during init the socket functions will be used to connect to the sp on
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
