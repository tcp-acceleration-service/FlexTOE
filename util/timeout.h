/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef UTILS_TIMEOUT_H_
#define UTILS_TIMEOUT_H_

#include <stdint.h>

/**
 * @addtogroup utils-timeouts
 * @brief Timeout Handling
 * @ingroup utils
 * @{ */

/** Object for an individual timeout. (opaque) */
struct timeout {
  /**
   * @brief Time and type.
   * Type is stored in the 4 most significant bits, and the time in the 28 least
   * significant bits.
   */
  uint32_t timeout_type;

  /** Next pointer for internal list */
  struct timeout *next;
  /** Previous pointer for internal list */
  struct timeout *prev;
};


/** Timeout manager state (opaque) */
struct timeout_manager {
  /** Head of linked list of all pending timeouts */
  struct timeout *timeouts_first;
  /** Tail of linked list of all pending timeouts */
  struct timeout *timeouts_last;
  /** Head of list of due pending timeouts, no longer in #timeouts */
  struct timeout *due_first;
  /** Tail of list of due pending timeouts, no longer in #timeouts */
  struct timeout *due_last;
  /** Handler for timeouts. Arguments are the timeout struct and the type of
   * timeout.*/
  void (*handler)(struct timeout *, uint8_t, void *);
  /** Opaque parameter passed through to handler */
  void *handler_opaque;
};

/**
 * Initialize timer subsystem.
 *
 * @return 0 on success, <0 else
 */
int util_timeout_init(struct timeout_manager *mgr,
    void (*handler)(struct timeout *, uint8_t, void *), void *handler_opaque);

/**
 * Timestamp in microseconds.
 */
uint32_t util_timeout_time_us(void);

/**
 * us to cycles.
 */
uint64_t util_us_to_cyc(uint64_t us);

/**
 * Process timers.
 */
void util_timeout_poll(struct timeout_manager *mgr);

/**
 * Process timers based on provided timestamp.
 */
void util_timeout_poll_ts(struct timeout_manager *mgr, uint32_t cur_ts);

uint32_t util_timeout_next(struct timeout_manager *mgr, uint32_t cur_ts);

/**
 * Arm timer for timeout.
 *
 * @param to    Timeout object to use.
 * @param us    # of microseconds to wait.
 * @param type  Type of timeout.
 */
void util_timeout_arm(struct timeout_manager *mgr, struct timeout *to,
    uint32_t us, uint8_t type);

/**
 * Arm timer for timeout based on provided timestamp.
 */
void util_timeout_arm_ts(struct timeout_manager *mgr, struct timeout *to,
    uint32_t us, uint8_t type, uint32_t cur_ts);

/**
 * Disarm timer.
 *
 * @param to  Timeout object to cancel.
 */
void util_timeout_disarm(struct timeout_manager *mgr, struct timeout *to);

/** @} */

#endif // ndef UTILS_TIMEOUT_H_
