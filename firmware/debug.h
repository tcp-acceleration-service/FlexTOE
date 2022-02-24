/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef ME_DEBUG_H
#define ME_DEBUG_H

#include <nfp.h>
#include <nfp/mem_atomic.h>
#include <nfp/mem_bulk.h>

#include "fp_debug.h"

extern __export __shared __emem struct flextcp_pl_debug fp_debug;

#if FP_JRNL_ENABLE
  extern __export __shared __emem unsigned int fp_dbg_journal[JOURNAL_SIZE/4];
#endif

__intrinsic static void log(uint32_t _a, uint32_t _b, uint32_t _c, uint32_t _d)
{
  __xrw uint32_t _idx_val = 4;
  __xrw uint32_t _dvals[4];
  __mem40 uint8_t* addr;

  _dvals[0] = _a;
  _dvals[1] = _b;
  _dvals[2] = _c;
  _dvals[3] = _d;

  mem_test_add(&_idx_val, (__mem40 void*) &fp_debug.log_tail_idx, sizeof(_idx_val));

  addr = ((__mem40 uint8_t*) fp_debug.log_buffer) + ((_idx_val % LOGBUF_SIZE) * sizeof(uint32_t));

  mem_write_atomic(_dvals, addr, sizeof(_dvals));
}

__intrinsic static void stats_add(uint32_t _idx, uint32_t _val)
{
  __ct_assert(_val < SZ_64K);

  mem_add64_imm((_val & 0xFFFF), (__mem40 void*) &fp_debug.stats_buffer[_idx]);
}

__intrinsic static void stats_inc(uint32_t _idx)
{
  mem_incr64((__mem40 void*) &fp_debug.stats_buffer[_idx]);
}

__intrinsic static void stats_clr(uint32_t _idx)
{
  mem_write64_atomic_imm(0, (__mem40 void*) &fp_debug.stats_buffer[_idx]);
}

#if FP_PROF_ENABLE
__gpr uint32_t __pc_counter;

__intrinsic static void prof_start()
{
  __pc_counter = (uint32_t) me_pc_read();
}

/* Assume that pc_read doesn't wrap around more than once!*/
/* Use for small sections only */
__intrinsic static void prof_end(uint32_t _idx)
{
  uint32_t _val;
  uint32_t __end = (uint32_t) me_pc_read();
  if (__end > __pc_counter) {
    _val = (__end - __pc_counter);
  } else {
    _val = (0xFFFF - __pc_counter) + __end;
  }
  mem_add64_imm((_val & 0xFFFF), (__mem40 void*) &fp_debug.prof_cycles[_idx]);
  mem_incr64((__mem40 void*) &fp_debug.prof_count[_idx]);
}

__intrinsic static void prof_clr(uint32_t _idx)
{
  mem_write64_atomic_imm(0, (__mem40 void*) &fp_debug.prof_cycles[_idx]);
  mem_write64_atomic_imm(0, (__mem40 void*) &fp_debug.prof_count[_idx]);
}
#endif

#if FP_LOG_ENABLE
  #define DEBUG(_mod, _b, _c, _d) log((__LINE__ | (__MEID << 24) | (ctx() << 20) | (FP_MOD_##_mod << 16)), _b, _c, _d)
#else
  #define DEBUG(_mod, _b, _c, _d) do {} while(0)
#endif

#if FP_STAT_ENABLE
  #define STATS_ADD(_stat, _val)  stats_add(FP_STAT_##_stat, _val)
  #define STATS_INC(_stat)        stats_inc(FP_STAT_##_stat)
  #define STATS_CLR(_stat)        stats_clr(FP_STAT_##_stat)
  #define __STATS_CLR(_idx)       stats_clr(_idx)
#else
  #define STATS_ADD(_stat, _val)  do {} while(0)
  #define STATS_INC(_stat)        do {} while(0)
  #define STATS_CLR(_stat)        do {} while(0)
  #define __STATS_CLR(_idx)       do {} while(0)
#endif

#if FP_PROF_ENABLE
  #define PROF_START(_sec)        prof_start()
  #define PROF_END(_sec)          prof_end(FP_PROF_##_sec)
  #define PROF_CLR(_sec)          prof_clr(FP__PROF__##_sec)
  #define __PROF_CLR(_idx)        prof_clr(_idx)
#else
  #define PROF_START(_sec)        do {} while(0)
  #define PROF_END(_sec)          do {} while(0)
  #define PROF_CLR(_sec)          do {} while(0)
  #define __PROF_CLR(_idx)        do {} while(0)
#endif

#if FP_JRNL_ENABLE
  #define JOURNAL(_val)           mem_ring_journal_fast(MEM_RING_GET_NUM(fp_dbg_journal), MEM_RING_GET_MEMADDR(fp_dbg_journal), (_val))
#else
  #define JOURNAL(_val)           do {} while(0)
#endif

#endif /* ME_DEBUG_H */
