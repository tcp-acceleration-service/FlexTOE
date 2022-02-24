/*
 * Copyright (C) 2014-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          lib/nfp/me.h
 * @brief         Interface for common ME related functions and types
 */
#ifndef _NFP__ME_H_
#define _NFP__ME_H_

#include <nfp.h>

#if defined(__NFP_LANG_MICROC)

/**
 * Get the current microengine number.
 */
__intrinsic unsigned int __ME(void);

/**
 * Wait for signal.
 * @param sig           signal
 *
 * Swap out the current context and wait for the specified signal
 * (bpt, voluntary, kill).  Please use the __wait_for_all(),
 * __wait_for_any(), or signal_test() functions to wait for user
 * signal variables.
 */
__intrinsic void ctx_wait(signal_t sig);

/**
 * Yield execution to another thread.
 */
#define ctx_swap() ctx_wait(voluntary)

/**
 * Wait for signals indicated in a signal mask.
 * @param sigmask       Signal mask
 *
 * Swap out the current context and wait for the signals indicated in the mask.
 */
__intrinsic void wait_sig_mask(SIGNAL_MASK sigmask);

/**
 * Signal a context within the same ME.
 * @param ctx            Context to signal
 * @param sig_no         Signal number to use
 */
__intrinsic void signal_ctx(unsigned int ctx, unsigned int sig_no);

/**
 * Signal the next context within the same ME.
 * @param sig_no         Signal number to use
 */
__intrinsic void signal_next_ctx(unsigned int sig_no);

/**
 * Signal a context in the next (numerically) ME in the cluster.
 * @param ctx            Context to signal
 * @param sig_no         Signal number to use
 */
__intrinsic void signal_next_me(unsigned int ctx, unsigned int sig_no);

/**
 * Signal a context in the previous (numerically) ME in the cluster.
 * @param ctx            Context to signal
 * @param sig_no         Signal number to use
 */
__intrinsic void signal_prev_me(unsigned int ctx, unsigned int sig_no);

/**
 * Set an alarm and use @sig when it goes off.
 * @param sig           Signal to assert after @cycles cycles
 * @param cycles        Approx number of cycles for the alarm
 *
 * This function asserts the signal after @cycles cycles.  A context
 * can only have one active alarm at a time.
 *
 * Note: @cycles must be less than 0x00100000 (1 << 20).
 */
__intrinsic void set_alarm(unsigned int cycles, SIGNAL *sig);

/**
 * Clear a previously set alarm.
 */
__intrinsic void clear_alarm(void);

/**
 * Sleep for a number of cycles.
 * @param cycles        Approx number of cycles to sleep
 *
 * Note: @cycles must be less than 0x00100000 (1 << 20).
 */
__intrinsic void sleep(unsigned int cycles);

/**
 * Halt all contexts on the microengine.
 */
__intrinsic void halt(void);

/**
 * Test the bit_pos in data word and return a 1 if set or 0 if clear.
 * @param data      Data word to test
 * @param bit_pos   Bit position in data to test
 *
 * This method explicitly uses the br_bset instruction.  The compiler will
 * try to use br_bset and br_bclr for regular C tests where possible.
 */
__intrinsic int bit_test(unsigned int data, unsigned int bit_pos);

/**
 * Find the first (least significant) bit set in 32bit @data.
 * @param data          Data to examine
 *
 * This function finds the first (least significant) bit set in data
 * and returns its bit position.  If there are no bits set (i.e., the
 * data argument is 0) then the return value is undefined.  Otherwise,
 * the return value is in the range 0 through 31.
 */
__intrinsic unsigned int ffs(unsigned int data);

/**
 * Find the first (least significant) bit set in 64bit @data.
 * @param data          Data to examine
 *
 * This function finds the first (least significant) bit set in 64bit
 * @data and returns its bit position.  If there are no bits set
 * (i.e., the data argument is 0) then the return value is -1,
 * otherwise, the return value is in the range 0 through 63.
 */
__intrinsic int ffs64(unsigned long long int data);


/**
 * Rotate a 32-bit value right
 *
 * @param val           The value to be rotated
 * @param amt           The number of bits to rotate
 *
 * The rotational amount is taken to be amt % 32.
 */
__intrinsic unsigned int rotr32(unsigned int val, unsigned int amt);


/**
 * Rotate a 32-bit value left
 *
 * @param val           The value to be rotated
 * @param amt           The the number of bits to rotate
 *
 * The rotational amount is taken to be amt % 32.
 */
__intrinsic unsigned int rotl32(unsigned int val, unsigned int amt);


/**
 * CRC bytes specifier
 *
 * The bytes_specifier_t enumeration is used as an argument to the CRC
 * functions and specifies one or more contiguous bytes within a
 * longword of big-endian or little-endian data. For example, the
 * bytes_0_3 item in this enumeration refers to bytes 0 through
 * 3. When using the big-endian CRC functions, byte 0 refers to the
 * most significant byte and byte 3 refers to the least significant
 * byte. When using the little-endian CRC functions, byte 0 refers to
 * the least significant byte and byte 3 refers to the most
 * significant byte. This enumeration type specifies the bytes to be
 * used with CRC operations.
 */
typedef enum {
    crc_bytes_0_3,                  /**< BE: 0, 1, 2, 3      LE: 3, 2, 1, 0. */
    crc_bytes_0_2,                  /**< BE: 0, 1, 2         LE:    2, 1, 0. */
    crc_bytes_0_1,                  /**< BE: 0, 1            LE:       1, 0. */
    crc_byte_0,                     /**< BE: 0               LE:          0. */
    crc_bytes_1_3,                  /**< BE:    1, 2, 3      LE: 3, 2, 1   . */
    crc_bytes_2_3,                  /**< BE:       2, 3      LE: 3, 2      . */
    crc_byte_3                      /**< BE:          3      LE: 3         . */
} crc_bytes_t;

/**
 * Read CRC remainder accumulated so far.
 */
__intrinsic unsigned int crc_read(void);

/**
 * Write the CRC remainder.
 * @param residue   Value to initialize the CRC remainder
 */
__intrinsic void crc_write(unsigned int residue);

/**
 * 32-bit CRC-32 computation in Big-endian format.
 * @param data      Data to perform the CRC computation on
 * @param bspec     Bytes on which to perform the computation
 *
 * Perform a CRC-32 computation on specified bytes in the data
 * argument that is assumed to be in Big-endian format and return the
 * unmodified value of data.
 *
 * crc_write() can be used to initialize the CRC value and the result
 * can be obtained from crc_read().  @see crc_read() and crc_write().
 */
__intrinsic unsigned int crc_32_be(unsigned int data, crc_bytes_t bspec);

/**
 * 32-bit iSCSI CRC computation in Big-endian format.
 * @param data      Data to perform the iSCSI CRC computation on
 * @param bspec     Bytes on which to perform the computation
 *
 * Perform a 32-bit iSCSI CRC computation on specified bytes in the
 * data argument that is assumed to be in Big-endian format and return
 * the unmodified value of data.
 *
 * crc_write() can be used to initialize the CRC value and the result
 * can be obtained from crc_read().
 */
__intrinsic unsigned int crc_iscsi_be(unsigned int data, crc_bytes_t bspec);

/*
 * Input state names.
 */
enum inp_state_e {
    inp_state_nn_empty          = 0x0,
    inp_state_nn_full           = 0x1,
    inp_state_scr_ring0_status  = 0x2,
    inp_state_scr_ring1_status  = 0x3,
    inp_state_scr_ring2_status  = 0x4,
    inp_state_scr_ring3_status  = 0x5,
    inp_state_scr_ring4_status  = 0x6,
    inp_state_scr_ring5_status  = 0x7,
    inp_state_scr_ring6_status  = 0x8,
    inp_state_scr_ring7_status  = 0x9,
    inp_state_scr_ring8_status  = 0xA,
    inp_state_scr_ring9_status  = 0xB,
    inp_state_scr_ring10_status = 0xC,
    inp_state_scr_ring11_status = 0xD,
    inp_state_scr_ring0_full    = inp_state_scr_ring0_status,
    inp_state_scr_ring1_full    = inp_state_scr_ring1_status,
    inp_state_scr_ring2_full    = inp_state_scr_ring2_status,
    inp_state_scr_ring3_full    = inp_state_scr_ring3_status,
    inp_state_scr_ring4_full    = inp_state_scr_ring4_status,
    inp_state_scr_ring5_full    = inp_state_scr_ring5_status,
    inp_state_scr_ring6_full    = inp_state_scr_ring6_status,
    inp_state_scr_ring7_full    = inp_state_scr_ring7_status,
    inp_state_scr_ring8_full    = inp_state_scr_ring8_status,
    inp_state_scr_ring9_full    = inp_state_scr_ring9_status,
    inp_state_scr_ring10_full   = inp_state_scr_ring10_status,
    inp_state_scr_ring11_full   = inp_state_scr_ring11_status,
    inp_state_fci_not_empty     = 0xE,
    inp_state_fci_full          = 0xF,
    inp_state_fcififo_empty     = 0xE,
    inp_state_fcififo_full      = 0xF
};

/**
 * Tests the value of the specified input state name.
 * @param statename    State to test
 *
 * Test the value of the specified state name and return 1 if the
 * state is set or 0 if clear.  The argument state must be a constant
 * literal as required by the microcode assembler.
 */
__intrinsic int inp_state_test(int statename);

/**
 * Return the current 64bit ME timestamp counter value.
 *
 * The timestamp counter is maintained in two adjacent CSRs. This function
 * reads these CSRs in a safe fashion and combines the values. The timestamp
 * counter increments every 16 ME clock cycles.
 *
 **/
__intrinsic unsigned long long int me_tsc_read(void);

/**
 * Return the 16bit profile count value.
 *
 * The profile count is a 16bit value which increments every clock-cycle. It
 * can be used to profile small sections of code.
 */
__intrinsic unsigned short int me_pc_read(void);

#endif /* __NFP_LANG_MICROC */

#endif /* !_NFP__NFP_ME_H_ */
