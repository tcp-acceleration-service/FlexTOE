/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef UTILS_NBQUEUE_H_
#define UTILS_NBQUEUE_H_

#include <assert.h>
#include <pthread.h>

struct nbqueue_el {
  struct nbqueue_el *next;
};

struct nbqueue {
  struct nbqueue_el *head;
  pthread_mutex_t mutex;
};

static inline void nbqueue_init(struct nbqueue *nbq)
{
  nbq->head = NULL;
  pthread_mutex_init(&nbq->mutex, NULL);
}

static inline void nbqueue_enq(struct nbqueue *nbq, struct nbqueue_el *el)
{
  pthread_mutex_lock(&nbq->mutex);
  el->next = nbq->head;
  nbq->head = el;
  pthread_mutex_unlock(&nbq->mutex);
}

static inline void *nbqueue_deq(struct nbqueue *nbq)
{
  struct nbqueue_el *el, *el_p;
  if (nbq->head == NULL) {
    return NULL;
  }

  pthread_mutex_lock(&nbq->mutex);

  for (el = nbq->head, el_p = NULL; el != NULL && el->next != NULL;
      el = el->next)
  {
    el_p = el;
  }
  assert(el->next == NULL);

  if (el != NULL) {
    if (el_p != NULL) {
      el_p->next = NULL;
    } else {
      nbq->head = NULL;
    }
  }

  pthread_mutex_unlock(&nbq->mutex);

  return el;
}

#endif /* ndef UTILS_NBQUEUE_H_ */
