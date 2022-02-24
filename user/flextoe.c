/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "connect.h"
#include "flextoe.h"
#include "fp_debug.h"
#include "config.h"
#include "driver.h"

struct configuration config;
extern int debug_reset;

int exited = 0;

static void handle_signal(int signo)
{
  switch (signo) {
  case SIGINT:
    exited = 1;
    break;

  // TODO: Handle other signals?
  default:
    // do nothing
    break;
  }
}

#define LOG_BATCH_MAX 128

#if FP_STAT_ENABLE
#define STR(S) #S
static char* fp_stats[] = {
  STR(FP_STAT_RX_TOTAL),
  STR(FP_STAT_TX_TOTAL),
  STR(FP_STAT_AC_TOTAL),
  STR(FP_STAT_RETX_TOTAL),
  STR(FP_STAT_GRP0),
  STR(FP_STAT_GRP1),
  STR(FP_STAT_GRP2),
  STR(FP_STAT_GRP3),
  STR(FP_STAT_RX_SP),
  STR(FP_STAT_RX_PROC),
  STR(FP_STAT_TX_PROC),
  STR(FP_STAT_AC_PROC),
  STR(FP_STAT_RETX_PROC),
  STR(FP_STAT_RX_POSTPROC_IN),
  STR(FP_STAT_TX_POSTPROC_IN),
  STR(FP_STAT_AC_POSTPROC_IN),
  STR(FP_STAT_RETX_POSTPROC_IN),
  STR(FP_STAT_DMA_RX_PAYLOAD_ISSUE),
  STR(FP_STAT_DMA_RX_FWD_ACK),
  STR(FP_STAT_DMA_RX_FWD_DROP),
  STR(FP_STAT_DMA_RX_FWD_ARX),
  STR(FP_STAT_DMA_TX_PAYLOAD_ISSUE),
  STR(FP_STAT_DMA_TX_FWD_SEG),
  STR(FP_STAT_DMA_TX_FWD_DROP),
  STR(FP_STAT_DMA_ARX_DESC_ISSUE),
  STR(FP_STAT_DMA_ARX_DESC_FWD_FREE),
  STR(FP_STAT_DMA_ATX_DESC_ISSUE),
  STR(FP_STAT_DMA_ATX_DESC_FWD_AC),
  STR(FP_STAT_ARX),
  STR(FP_STAT_ATX),
  STR(FP_STAT_ATX_NODESC),
  STR(FP_STAT_PROC_CACHE_MISS),
  STR(FP_STAT_PROC_EMEM_CACHE_MISS),
  STR(FP_STAT_QM_SCHEDULE),
  STR(FP_STAT_SP_FWD),
};
#endif

#if FP_PROF_ENABLE
#undef STR
#define STR(S) #S
static char* fp_prof[] = {
  STR(FP_PROF_PROC_RX),
  STR(FP_PROF_PROC_TX),
  STR(FP_PROF_PROC_AC),
  STR(FP_PROF_PROC_RETX),
  STR(FP_PROF_PROC_ACK),
};
#endif

static inline uint64_t rot32(uint64_t x) {
  return (x << 32) | (x >> 32);
}
#if FP_LOG_ENABLE || FP_STAT_ENABLE || FP_PROF_ENABLE
static void* log_main(void* arg)
{
  (void) arg;
  uint32_t debug_idx = 0;
  uint32_t tail;
  uint32_t diff;
  uint32_t idx, batch;
  uint32_t log[LOG_BATCH_MAX][4];
  struct timespec prev, curr;

  while ((flextoe_info->flags & FLEXNIC_FLAG_READY) != FLEXNIC_FLAG_READY) {
    /* sleep for 100ms */
    usleep(100 * 1000);
  }

  clock_gettime(CLOCK_MONOTONIC, &prev);

  while (1) {
    /* Read tail */
    tail = nn_readl(&fp_debug->log_tail_idx);
    if (tail == debug_idx) {
      /* Sleep for 100 us */
      usleep(100);
    }

    /* Counter hasn't wrapped around */
    if (tail >= debug_idx) {
      diff = tail - debug_idx;
    } else {
      diff = (UINT32_MAX - debug_idx) + tail;
    }

    if (diff > UINT32_MAX/2) {
      fprintf(stderr, "Log jump! Reseting debug_idx. tail=%u debug_idx=%u\n", tail, debug_idx);
      debug_idx = tail;
      continue;
    }

    batch = 0;
    while (diff > 0 && batch < LOG_BATCH_MAX) {
      /* Read line */
      memcpy(log[batch], fp_debug->log_buffer + (debug_idx % LOGBUF_SIZE), sizeof(log[0]));

      diff -= 4;
      debug_idx += 4;
      batch++;
    }

    for (idx = 0; idx < batch; idx++) {
      fprintf(stderr, "%08x %08x %08x %08x\n",
          log[idx][0], log[idx][1], log[idx][2], log[idx][3]);
    }

    clock_gettime(CLOCK_MONOTONIC, &curr);
    /* 10 seconds has elapsed */
    if ((curr.tv_sec - prev.tv_sec) > 10) {
#if FP_STAT_ENABLE
      double curr_time_ns = curr.tv_sec * 1.0E9 + curr.tv_nsec;
      double prev_time_ns = prev.tv_sec * 1.0E9 + prev.tv_nsec;
      fprintf(stderr, "Duration: %f\n", (curr_time_ns - prev_time_ns)/1.0E9);
      for (idx = 0; idx < (sizeof(fp_stats)/sizeof(fp_stats[0])); idx++) {
        fprintf(stderr, "%s %lu\n", fp_stats[idx],
                  rot32(nn_readq(&fp_debug->stats_buffer[idx])));
      }

      fprintf(stderr, "\n");
      prev = curr;
#else
      (void) curr;
      (void) prev;
#endif

#if FP_PROF_ENABLE
      for (idx = 0; idx < (sizeof(fp_prof)/sizeof(fp_prof[0])); idx++) {
        fprintf(stderr, "%s %lu %lu\n", fp_prof[idx],
                      rot32(nn_readq(&fp_debug->prof_cycles[idx])),
                      rot32(nn_readq(&fp_debug->prof_count[idx])));
      }

      fprintf(stdout, "\n");
      prev = curr;
#else
      (void) curr;
      (void) prev;
#endif
    }

    if (exited)
      break;
  }

  return NULL;
}
#endif

static void* console_main(void* arg)
{
#define MAX_CONSOLE_INPUT  128
  char line[MAX_CONSOLE_INPUT];
  char command[MAX_CONSOLE_INPUT];
  uint32_t flow_id;

  /* Wait for SP to get things up and running! */
  while ((flextoe_info->flags & FLEXNIC_FLAG_READY) != FLEXNIC_FLAG_READY) {
    /* sleep for 100ms */
    usleep(100 * 1000);
  }

  /* Run CLI loop */
  while (1) {
    /* Prompt */
    fprintf(stdout, "$> ");

    if (fgets(line, MAX_CONSOLE_INPUT, stdin)) {
      fprintf(stdout, "\n");

      if (sscanf(line, "%s %u", command, &flow_id) != 2)
        continue;

      if (strncmp(command, "dclr", 4) == 0) {
        __sync_add_and_fetch(&debug_reset, 1);
      }
      else if (strncmp(command, "fs", 2) == 0) {
        if (flow_id >= FLEXNIC_PL_FLOWST_NUM)
          continue;

        fprintf(stdout, "-------------------------------------------------------------------\n");
        fprintf(stdout, "Flow state     flow_id %u\n",  flow_id);
        fprintf(stdout, "-------------------------------------------------------------------\n");
        fprintf(stdout, "TX    avail  %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].tx_avail));
        fprintf(stdout, "TX    ravail %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].tx_remote_avail));
        fprintf(stdout, "TX    sent   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].tx_sent));
        fprintf(stdout, "TX    nseq   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].tx_next_seq));
        fprintf(stdout, "TX    npos   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].tx_next_pos));
        fprintf(stdout, "TX    flags  %hx\n", nn_readw(&fp_state->flows_tcp_state[flow_id].flags));
        fprintf(stdout, "TX    dack   %u\n", nn_readw(&fp_state->flows_tcp_state[flow_id].dupack_cnt));
        fprintf(stdout, "TX    nts    %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].tx_next_ts));
        fprintf(stdout, "-------------------------------------------------------------------\n");
        fprintf(stdout, "CC    rtt    %u\n", nn_readl(&fp_state->flows_cc_info[flow_id].rtt_est));
        fprintf(stdout, "CC    rxack  %x\n", nn_readl(&fp_state->flows_cc_info[flow_id].cnt_rx_ack_bytes));
        fprintf(stdout, "CC    rxecn  %x\n", nn_readl(&fp_state->flows_cc_info[flow_id].cnt_rx_ecn_bytes));
        fprintf(stdout, "CC    drops  %u\n", nn_readl(&fp_state->flows_cc_info[flow_id].cnt_rx_acks));
        fprintf(stdout, "CC    acks   %u\n", nn_readl(&fp_state->flows_cc_info[flow_id].cnt_tx_drops));
        fprintf(stdout, "-------------------------------------------------------------------\n");
        fprintf(stdout, "RX    avail  %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].rx_avail));
        fprintf(stdout, "RX    nseq   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].rx_next_seq));
        fprintf(stdout, "RX    npos   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].rx_next_pos));
        fprintf(stdout, "RX    opos   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].rx_ooo_start));
        fprintf(stdout, "RX    olen   %u\n", nn_readl(&fp_state->flows_tcp_state[flow_id].rx_ooo_len));
        fprintf(stdout, "-------------------------------------------------------------------\n");
      }
      else if (strncmp(command, "jrnl", 4) == 0) {
        char tmp_path[32] = "flextoe-journal-XXXXXX";
        int tmp_fd = mkstemp(tmp_path);
        unsigned int entry;

        if (tmp_fd < 0) {
          fprintf(stderr, "failed to create a temp file\n");
          continue;
        }
        fprintf(stdout, "Journal tmpfile: %s\n", tmp_path);
        for (int i = 0; i < (JOURNAL_SIZE/4); i++) {
          entry = fp_journal[i];
          write(tmp_fd, &entry, sizeof(entry));
        }

        close(tmp_fd);
      }
      else {
        /* TODO: print usage */
        continue;
      }
    }
  }

  return NULL;
}

int main(int argc, char* argv[])
{
  pthread_t log_thread, console_thread;
  int res = EXIT_SUCCESS;

  /* parse command line options */
  if (config_parse(&config, argc, argv) != 0) {
      res = EXIT_FAILURE;
      goto error_exit;
  }

  /* allocate shared memory */
  if (shm_init() != 0) {
    res = EXIT_FAILURE;
    goto error_exit;
  }

  if (nic_init() != 0) {
    res = EXIT_FAILURE;
    goto error_shm_cleanup;
  }

  RTE_SET_USED(log_thread);
#if FP_LOG_ENABLE || FP_STAT_ENABLE || FP_DEBUG_ENABLE
  /* Start log thread */
  if (pthread_create(&log_thread, NULL, log_main, NULL) < 0) {
    fprintf(stderr, "failed to launch logging thread\n");
    goto error_thread_cleanup;
  }
#endif

  /* Start console thread */
  if (config.console) {
    if (pthread_create(&console_thread, NULL, console_main, NULL) < 0) {
      fprintf(stderr, "failed to launch console thread\n");
      goto error_thread_cleanup;
    }
  }

  /* Register signal handler */
  struct sigaction act = {
    .sa_handler = handle_signal,
  };
  sigaction(SIGINT, &act, NULL);

  slowpath_main();

  if (config.console) {
    pthread_cancel(console_thread);
  }

#if FP_LOG_ENABLE || FP_STAT_ENABLE || FP_PROF_ENABLE
  pthread_join(log_thread, NULL);
#endif

error_thread_cleanup:
  nic_cleanup();
error_shm_cleanup:
  shm_cleanup();
error_exit:
  return res;
}
