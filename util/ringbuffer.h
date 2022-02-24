/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef USERSPACE_RINGBUFFER_H
#define USERSPACE_RINGBUFFER_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <rte/atomic.h>

struct ringbuffer_t
{
    void*       base_addr;      /*> Base address */
    uint32_t    entry_size;     /*> Size of each entry */
    uint32_t    capacity;       /*> Ring buffer capacity */
    uint32_t    head;           /*> Head pointer */
    uint32_t    tail;           /*> Tail pointer */
} __attribute__((__packed__));

static inline int ringbuffer_empty(struct ringbuffer_t* rb)
{
    return (rb->head == rb->tail);
}

static inline uint32_t ringbuffer_size(struct ringbuffer_t* rb)
{
    if (rb->head > rb->tail)
    {
        return (rb->capacity - rb->head) + rb->tail;
    }
    else
    {
        return (rb->tail - rb->head);
    }
}

static inline int ringbuffer_full(struct ringbuffer_t* rb)
{
    return (((rb->tail + rb->entry_size) % rb->capacity) == rb->head);
}

static inline void* ringbuffer_front(struct ringbuffer_t* rb)
{
    return (char*) rb->base_addr + rb->head;
}

static inline void* ringbuffer_back(struct ringbuffer_t* rb)
{
    return (char*) rb->base_addr + rb->tail;
}

extern void ringbuffer_push(struct ringbuffer_t* rb);
extern void ringbuffer_pop(struct ringbuffer_t* rb);

#endif /* USERSPACE_RINGBUFFER_H */
