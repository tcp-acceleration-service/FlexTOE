/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include "ringbuffer.h"

void ringbuffer_push(struct ringbuffer_t* rb)
{
  /* Crash if buffer is full! */
  assert(!ringbuffer_full(rb));

  rte_wmb();  /* Ensure data is copied before updating pointer! */

  rb->tail = rb->tail + rb->entry_size;
  if (rb->tail >= rb->capacity)
    rb->tail = 0;
}

void ringbuffer_pop(struct ringbuffer_t* rb)
{
  /* Crash if buffer is empty! */
  assert(!ringbuffer_empty(rb));

  rte_wmb();  /* Ensure data is copied before updating pointer! */

  rb->head = rb->head + rb->entry_size;
  if (rb->head >= rb->capacity)
    rb->head = 0;
}
