/*
 * Copyright 2012-2015 Netronome, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          lib/std/synch.h
 * @brief         Synchronization primitives
 */

#ifndef _STD__SYNCH_H_
#define _STD__SYNCH_H_

#include <nfp.h>
#include <stdint.h>

struct synch_cnt {
    uint32_t value;
};

/**
 * Semaphore object required for each call
 */
struct sem {
    int32_t next_credit;    /* Number of credits claimed */
    int32_t last_complete;  /* Number of returned credits*/
};

#define SYNCH_SEM_NAME(_name) _name##_sem
#define SYNCH_CRED_NAME(_name) _name##_max_credits

/**
 * Number of cycles to wait if no credits are available
 */
#define SYNCH_SEM_DEFAULT_POLL  1000

/**
 * Declare and initialize a CLS semaphore.
 * @param _name     Name of the semaphore
 * @param _cnt      Max number of credits
 *
 * @note CLS based semaphore should only be used for synchronization within
 *       an island.
 */
#define SEM_CLS_DECLARE(_name, _cnt)   \
    __export __shared __cls struct sem SYNCH_SEM_NAME(_name);   \
    static const uint32_t SYNCH_CRED_NAME(_name) = _cnt;

/**
 * Wrapper to take a semaphore.
 * @param _name             Name of the semaphore
 * @param _poll_interval    Cycles to wait if no credits are available
 *
 * @note poll_interval must be less than 0x00100000 (1<<20), see sleep().
 *       Start with SYNCH_SEM_DEFAULT_POLL.  Increase if there is high
 *       contention for credits.
 */
#define SEM_WAIT(_name, _poll_interval)                             \
    sem_cls_wait(&SYNCH_SEM_NAME(_name), SYNCH_CRED_NAME(_name),    \
                 _poll_interval);

/**
 * Wrapper to give a semaphore.
 * @param _name     Name of the semaphore
 */
#define SEM_POST(_name)     \
    sem_cls_post(&SYNCH_SEM_NAME(_name));

/**
 * Reset DRAM synch counter.
 * @param s     Synch counter
 * @param cnt   Reset value
 */
__intrinsic void synch_cnt_dram_reset(__dram struct synch_cnt *s, uint32_t cnt);

/**
 * Ack DRAM synch counter.
 * @param s     Synch counter
 */
__intrinsic void synch_cnt_dram_ack(__dram struct synch_cnt *s);

/**
 * Poll DRAM synch counter.
 * @param s     Synch counter
 *
 * Poll synch counter and return non-nil if not zero.
 */
__intrinsic int synch_cnt_dram_poll(__dram struct synch_cnt *s);

/**
 * Wait for DRAM synch counter to reach zero.
 * @param s     Synch counter
 */
__intrinsic void synch_cnt_dram_wait(__dram struct synch_cnt *s);

/**
 * Take a semaphore.  Users should use the SEM_WAIT() macro.
 * @param sem               Semaphore handle
 * @param max_credits       Max number of credits available
 * @param poll_interval     Cycles to wait if no credits are available
 *
 * @note poll_interval must be less than 0x00100000 (1<<20), see sleep().
 *       Start with SYNCH_SEM_DEFAULT_POLL.  Increase if there is high
 *       contention for credits.
 * @note This function will handle credit claims in order.
 */
__intrinsic void sem_cls_wait(__cls struct sem *sem, const uint32_t max_credits,
                              uint32_t poll_interval);

/**
 * Give a semaphore.  Users should use the SEM_POST() macro.
 * @param sem   Semaphore handle
 *
 * @note Users must call sem_cls_wait() before sem_cls_post().
 */
__intrinsic void sem_cls_post(__cls struct sem *sem);

#endif /* !_STD__SYNCH_H_ */
