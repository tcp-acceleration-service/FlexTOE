/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "util/common.h"
#include "common.h"

#include "config.h"

enum cfg_params {
  CP_SHM_LEN,
  CP_NIC_RX_LEN,
  CP_NIC_TX_LEN,
  CP_APP_SPIN_LEN,
  CP_APP_SPOUT_LEN,
  CP_ARP_TO,
  CP_ARP_TO_MAX,
  CP_TCP_RTT_INIT,
  CP_TCP_LINK_BW,
  CP_TCP_RXBUF_LEN,
  CP_TCP_TXBUF_LEN,
  CP_TCP_HANDSHAKE_TO,
  CP_TCP_HANDSHAKE_RETRIES,
  CP_CC,
  CP_CC_CONTROL_GRANULARITY,
  CP_CC_CONTROL_INTERVAL,
  CP_CC_REXMIT_INTS,
  CP_CC_DCTCP_WEIGHT,
  CP_CC_DCTCP_INIT,
  CP_CC_DCTCP_STEP,
  CP_CC_DCTCP_MIMD,
  CP_CC_DCTCP_MIN,
  CP_CC_DCTCP_MINPKTS,
  CP_CC_CONST_RATE,
  CP_CC_TIMELY_TLOW,
  CP_CC_TIMELY_THIGH,
  CP_CC_TIMELY_STEP,
  CP_CC_TIMELY_INIT,
  CP_CC_TIMELY_ALPHA,
  CP_CC_TIMELY_BETA,
  CP_CC_TIMELY_MINRTT,
  CP_CC_TIMELY_MINRATE,
  CP_IP_ROUTE,
  CP_IP_ADDR,
  CP_FP_POLL_INTERVAL_APP,
  CP_QUIET,
  CP_DEBUG_CONSOLE,
};

static struct option opts[] = {
  { .name = "shm-len",
    .has_arg = required_argument,
    .val = CP_SHM_LEN },
  { .name = "nic-rx-len",
    .has_arg = required_argument,
    .val = CP_NIC_RX_LEN },
  { .name = "nic-tx-len",
    .has_arg = required_argument,
    .val = CP_NIC_TX_LEN },
  { .name = "app-spin-len",
    .has_arg = required_argument,
    .val = CP_APP_SPIN_LEN },
  { .name = "app-spout-len",
    .has_arg = required_argument,
    .val = CP_APP_SPOUT_LEN },
  { .name = "arp-timout",
    .has_arg = required_argument,
    .val = CP_ARP_TO },
  { .name = "arp-timeout-max",
    .has_arg = required_argument,
    .val = CP_ARP_TO },
  { .name = "tcp-rtt-init",
    .has_arg = required_argument,
    .val = CP_TCP_RTT_INIT },
  { .name = "tcp-link-bw",
    .has_arg = required_argument,
    .val = CP_TCP_LINK_BW },
  { .name = "tcp-rxbuf-len",
    .has_arg = required_argument,
    .val = CP_TCP_RXBUF_LEN },
  { .name = "tcp-txbuf-len",
    .has_arg = required_argument,
    .val = CP_TCP_TXBUF_LEN },
  { .name = "tcp-handshake-timeout",
    .has_arg = required_argument,
    .val = CP_TCP_HANDSHAKE_TO },
  { .name = "tcp-handshake-retries",
    .has_arg = required_argument,
    .val = CP_TCP_HANDSHAKE_RETRIES },
  { .name = "cc",
    .has_arg = required_argument,
    .val = CP_CC },
  { .name = "cc-control-granularity",
    .has_arg = required_argument,
    .val = CP_CC_CONTROL_GRANULARITY },
  { .name = "cc-control-interval",
    .has_arg = required_argument,
    .val = CP_CC_CONTROL_INTERVAL },
  { .name = "cc-rexmit-ints",
    .has_arg = required_argument,
    .val = CP_CC_REXMIT_INTS },
  { .name = "cc-dctcp-weight",
    .has_arg = required_argument,
    .val = CP_CC_DCTCP_WEIGHT },
  { .name = "cc-dctcp-init",
    .has_arg = required_argument,
    .val = CP_CC_DCTCP_INIT },
  { .name = "cc-dctcp-step",
    .has_arg = required_argument,
    .val = CP_CC_DCTCP_STEP },
  { .name = "cc-dctcp-mimd",
    .has_arg = required_argument,
    .val = CP_CC_DCTCP_MIMD },
  { .name = "cc-dctcp-min",
    .has_arg = required_argument,
    .val = CP_CC_DCTCP_MIN },
  { .name = "cc-dctcp-minpkts",
    .has_arg = required_argument,
    .val = CP_CC_DCTCP_MINPKTS },
  { .name = "cc-const-rate",
    .has_arg = required_argument,
    .val = CP_CC_CONST_RATE },
  { .name = "cc-timely-tlow",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_TLOW },
  { .name = "cc-timely-thigh",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_THIGH },
  { .name = "cc-timely-step",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_STEP },
  { .name = "cc-timely-init",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_INIT },
  { .name = "cc-timely-alpha",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_ALPHA },
  { .name = "cc-timely-beta",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_BETA },
  { .name = "cc-timely-minrtt",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_MINRTT },
  { .name = "cc-timely-minrate",
    .has_arg = required_argument,
    .val = CP_CC_TIMELY_MINRATE },
  { .name = "ip-route",
    .has_arg = required_argument,
    .val = CP_IP_ROUTE },
  { .name = "ip-addr",
    .has_arg = required_argument,
    .val = CP_IP_ADDR },
  { .name = "fp-poll-interval-app",
    .has_arg = required_argument,
    .val = CP_FP_POLL_INTERVAL_APP },
  { .name = "quiet",
    .has_arg = no_argument,
    .val = CP_QUIET },
  { .name = "debug-console",
    .has_arg = no_argument,
    .val = CP_DEBUG_CONSOLE },
  { .name = NULL },
};

static int config_defaults(struct configuration *c, char *progname);
static void print_usage(struct configuration *c, char *progname);
static inline int parse_int64(const char *s, uint64_t *pu64);
static inline int parse_int32(const char *s, uint32_t *pu32);
static inline int parse_int8(const char *s, uint8_t *pu8);
static inline int parse_double(const char *s, double *pd);
static inline int parse_cidr(char *s, uint32_t *ip, uint8_t *prefix);
static inline int parse_route(char *s, struct configuration *c);

int config_parse(struct configuration *c, int argc, char *argv[])
{
  int ret, done = 0;
  double d;

  if (config_defaults(c, argv[0]) != 0) {
    fprintf(stderr, "config_parse: config defaults failed\n");
    goto failed;
  }

  while (!done) {
    ret = getopt_long(argc, argv, "", opts, NULL);
    switch (ret) {
      case CP_SHM_LEN:
        if (parse_int64(optarg, &c->shm_len) != 0) {
          fprintf(stderr, "shm len parsing failed\n");
          goto failed;
        }
        break;
      case CP_NIC_RX_LEN:
        if (parse_int64(optarg, &c->nic_rx_len) != 0) {
          fprintf(stderr, "nic rx len parsing failed\n");
          goto failed;
        }
        break;
      case CP_NIC_TX_LEN:
        if (parse_int64(optarg, &c->nic_tx_len) != 0) {
          fprintf(stderr, "nic tx len parsing failed\n");
          goto failed;
        }
        break;
      case CP_APP_SPIN_LEN:
        if (parse_int64(optarg, &c->app_spin_len) != 0) {
          fprintf(stderr, "app spin len parsing failed\n");
          goto failed;
        }
        break;
      case CP_APP_SPOUT_LEN:
        if (parse_int64(optarg, &c->app_spout_len) != 0) {
          fprintf(stderr, "app spout len parsing failed\n");
          goto failed;
        }
        break;
      case CP_ARP_TO:
        if (parse_int32(optarg, &c->arp_to) != 0) {
          fprintf(stderr, "arp timeout parsing failed\n");
          goto failed;
        }
        break;
      case CP_ARP_TO_MAX:
        if (parse_int32(optarg, &c->arp_to_max) != 0) {
          fprintf(stderr, "arp max timeout parsing failed\n");
          goto failed;
        }
        break;
      case CP_TCP_RTT_INIT:
        if (parse_int32(optarg, &c->tcp_rtt_init) != 0) {
          fprintf(stderr, "tcp rtt init parsing failed\n");
          goto failed;
        }
        break;
      case CP_TCP_LINK_BW:
        if (parse_int32(optarg, &c->tcp_link_bw) != 0) {
          fprintf(stderr, "tcp link bw timeout parsing failed\n");
          goto failed;
        }
        break;
      case CP_TCP_RXBUF_LEN:
        if (parse_int64(optarg, &c->tcp_rxbuf_len) != 0) {
          fprintf(stderr, "tcp rxbuf len parsing failed\n");
          goto failed;
        }
        if (!rte_is_power_of_2(c->tcp_rxbuf_len)) {
          fprintf(stderr, "tcp rxbuf len is not power of 2\n");
          goto failed;
        }
        break;
      case CP_TCP_TXBUF_LEN:
        if (parse_int64(optarg, &c->tcp_txbuf_len) != 0) {
          fprintf(stderr, "tcp txbuf len parsing failed\n");
          goto failed;
        }
        if (!rte_is_power_of_2(c->tcp_txbuf_len)) {
          fprintf(stderr, "tcp txbuf len is not power of 2\n");
          goto failed;
        }
        break;
      case CP_TCP_HANDSHAKE_TO:
        if (parse_int32(optarg, &c->tcp_handshake_to) != 0) {
          fprintf(stderr, "tcp handshake timeout parsing failed\n");
          goto failed;
        }
        break;
      case CP_TCP_HANDSHAKE_RETRIES:
        if (parse_int32(optarg, &c->tcp_handshake_retries) != 0) {
          fprintf(stderr, "tcp handshake retries parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC:
        if (!strcmp(optarg, "dctcp-win")) {
          c->cc_algorithm = CONFIG_CC_DCTCP_WIN;
        } else if (!strcmp(optarg, "dctcp-rate")) {
          c->cc_algorithm = CONFIG_CC_DCTCP_RATE;
        } else if (!strcmp(optarg, "const-rate")) {
          c->cc_algorithm = CONFIG_CC_CONST_RATE;
        } else if (!strcmp(optarg, "timely")) {
          c->cc_algorithm = CONFIG_CC_TIMELY;
        } else {
          fprintf(stderr, "cc algorithm parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_CONTROL_GRANULARITY:
        if (parse_int32(optarg, &c->cc_control_granularity) != 0) {
          fprintf(stderr, "cc control granularity parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_CONTROL_INTERVAL:
        if (parse_int32(optarg, &c->cc_control_interval) != 0) {
          fprintf(stderr, "cc control granularity parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_REXMIT_INTS:
        if (parse_int32(optarg, &c->cc_rexmit_ints) != 0) {
          fprintf(stderr, "cc rexmit intervals parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_DCTCP_WEIGHT:
        if (parse_double(optarg, &d) != 0 || d < 0 || d > 1) {
          fprintf(stderr, "cc dctcp weight parsing failed\n");
          goto failed;
        }
        c->cc_dctcp_weight = UINT32_MAX * d;
        break;
      case CP_CC_DCTCP_INIT:
        if (parse_int32(optarg, &c->cc_dctcp_init) != 0) {
          fprintf(stderr, "cc dctcp init parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_DCTCP_STEP:
        if (parse_int32(optarg, &c->cc_dctcp_step) != 0) {
          fprintf(stderr, "cc dctcp step parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_DCTCP_MIMD:
        if (parse_double(optarg, &d) != 0 || d < 1 || d > 2) {
          fprintf(stderr, "cc dctcp mimd parsing failed\n");
          goto failed;
        }
        c->cc_dctcp_mimd = UINT32_MAX * (d - 1);
        break;
      case CP_CC_DCTCP_MIN:
        if (parse_int32(optarg, &c->cc_dctcp_min) != 0) {
          fprintf(stderr, "cc dctcp min parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_DCTCP_MINPKTS:
        if (parse_int32(optarg, &c->cc_dctcp_minpkts) != 0) {
          fprintf(stderr, "cc dctcp min packets parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_CONST_RATE:
        if (parse_int32(optarg, &c->cc_const_rate) != 0) {
          fprintf(stderr, "cc constant rate parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_TIMELY_TLOW:
        if (parse_int32(optarg, &c->cc_timely_tlow) != 0) {
          fprintf(stderr, "cc timely Tlow parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_TIMELY_THIGH:
        if (parse_int32(optarg, &c->cc_timely_thigh) != 0) {
          fprintf(stderr, "cc timely Thigh parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_TIMELY_STEP:
        if (parse_int32(optarg, &c->cc_timely_step) != 0) {
          fprintf(stderr, "cc timely step parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_TIMELY_INIT:
        if (parse_int32(optarg, &c->cc_timely_init) != 0) {
          fprintf(stderr, "cc timely init parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_TIMELY_ALPHA:
        if (parse_double(optarg, &d) != 0 || d < 0 || d > 1) {
          fprintf(stderr, "cc timely alpha parsing failed\n");
          goto failed;
        }
        c->cc_timely_alpha = UINT32_MAX * d;
        break;
      case CP_CC_TIMELY_BETA:
        if (parse_double(optarg, &d) != 0 || d < 0 || d > 1) {
          fprintf(stderr, "cc timely beta parsing failed\n");
          goto failed;
        }
        c->cc_timely_beta = UINT32_MAX * d;
        break;
      case CP_CC_TIMELY_MINRTT:
        if (parse_int32(optarg, &c->cc_timely_min_rtt) != 0) {
          fprintf(stderr, "cc timely min rtt parsing failed\n");
          goto failed;
        }
        break;
      case CP_CC_TIMELY_MINRATE:
        if (parse_int32(optarg, &c->cc_timely_min_rate) != 0) {
          fprintf(stderr, "cc timely min rate parsing failed\n");
          goto failed;
        }
        break;
      case CP_IP_ROUTE:
        if (parse_route(optarg, c) != 0) {
          goto failed;
        }
        break;
      case CP_IP_ADDR:
        c->ip_prefix = 0;
        if (parse_cidr(optarg, &c->ip, &c->ip_prefix) != 0) {
          fprintf(stderr, "Parsing IP failed\n");
          goto failed;
        }
        break;
      case CP_FP_POLL_INTERVAL_APP:
        if (parse_int32(optarg, &c->fp_poll_interval_app) != 0) {
          fprintf(stderr, "fp app poll interval parsing failed\n");
          goto failed;
        }
      case CP_QUIET:
	      c->quiet = 1;
        break;
      case CP_DEBUG_CONSOLE:
        c->console = 1;
        break;
      case -1:
        done = 1;
        break;
      case '?':
        goto failed;
      default:
        abort();
    }
  }

  if (optind != argc) {
    goto failed;
  }

  if(c->ip == 0) {
    fprintf(stderr, "ip-addr is a required argument!\n");
    goto failed;
  }

  return 0;

failed:
  config_defaults(c, argv[0]);
  print_usage(c, argv[0]);
  return -1;
}

static int config_defaults(struct configuration *c, char *progname)
{
  char exec_path[PATH_MAX];
  realpath("/proc/self/exe", exec_path);
  strncpy(c->firmware_path, dirname(dirname(exec_path)), PATH_MAX);
  strncat(c->firmware_path, "/firmware/fastpath.fw", PATH_MAX);

  c->ip = 0;
  c->shm_len = 1024 * 1024 * 1024;
  c->nic_rx_len = 256;
  c->nic_tx_len = 256;
  c->app_spin_len = 1024 * 1024;
  c->app_spout_len = 1024 * 1024;
  c->arp_to = 500;
  c->arp_to_max = 10000000;
  c->tcp_rtt_init = 50;
  c->tcp_link_bw = NS_PLATFORM_PORT_SPEED(PORT_IN_USE);
  c->tcp_rxbuf_len = 8 * 1024;
  c->tcp_txbuf_len = 8 * 1024;
  c->tcp_handshake_to = 10000;
  c->tcp_handshake_retries = 10;
  c->cc_algorithm = CONFIG_CC_DCTCP_RATE;
  c->cc_control_granularity = 50;
  c->cc_control_interval = 2;
  c->cc_rexmit_ints = 4;
  c->cc_dctcp_weight = UINT32_MAX / 16;
  c->cc_dctcp_init = 10000;
  c->cc_dctcp_step = 10000;
  c->cc_dctcp_mimd = 0;
  c->cc_dctcp_min = 0;
  c->cc_dctcp_minpkts = 50;
  c->cc_const_rate = 0;
  c->cc_timely_tlow = 30;
  c->cc_timely_thigh = 150;
  c->cc_timely_step = 10000;
  c->cc_timely_init = 10000;
  c->cc_timely_alpha = 0.02 * UINT32_MAX;
  c->cc_timely_beta = 0.8 * UINT32_MAX;
  c->cc_timely_min_rtt = 11;
  c->cc_timely_min_rate = 10000;
  c->fp_poll_interval_app = 10000;
  c->quiet = 0;
  c->console = 0;

  return 0;
}


static void print_usage(struct configuration *c, char *progname)
{
  fprintf(stderr, "Usage: %s [OPTION]... --ip-addr=IP[/PREFIXLEN]\n"
      "\n"
      "Memory Sizes:\n"
      "  --shm-len=LEN               Shared memory len "
          "[default: %"PRIu64"]\n"
      "  --nic-rx-len=LEN            SP rx queue len "
          "[default: %"PRIu64"]\n"
      "  --nic-tx-len=LEN            SP tx queue len "
          "[default: %"PRIu64"]\n"
      "  --app-spin-len=LEN           App->SP queue len "
          "[default: %"PRIu64"]\n"
      "  --app-spout-len=LEN          SP->App queue len "
          "[default: %"PRIu64"]\n"
      "\n"
      "TCP protocol parameters:\n"
      "  --tcp-rtt-init=RTT          Initial rtt for CC (us) "
          "[default: %"PRIu32"]\n"
      "  --tcp-link-bw=BANDWIDTH     Link bandwidth (gbps) "
          "[default: %"PRIu32"]\n"
      "  --tcp-rxbuf-len             Flow rx buffer len "
          "[default: %"PRIu64"]\n"
      "  --tcp-txbuf-len             Flow tx buffer len "
          "[default: %"PRIu64"]\n"
      "  --tcp-handshake-timeout=TIMEOUT  Handshake timeout (us) "
          "[default: %"PRIu32"]\n"
      "  --tcp-handshake-retries=RETRIES  Handshake retries "
          "[default: %"PRIu32"]\n"
      "\n"
      "Congestion control parameters:\n"
      "  --cc=ALGORITHM              Congestion-control algorithm "
          "[default: dctcp-rate]\n"
      "     Options: dctcp-win, dctcp-rate, const-rate, timely\n"
      "  --cc-control-granularity=G  Minimal control iteration "
          "[default: %"PRIu32"]\n"
      "  --cc-control-interval=INT   Control interval (multiples of RTT) "
          "[default: %"PRIu32"]\n"
      "  --cc-rexmit-ints=INTERVALS  #of RTTs without ACKs before rexmit "
          "[default: %"PRIu32"]\n"
      "  --cc-dctcp-weight=WEIGHT    DCTCP: EWMA weight for ECN rate "
          "[default: %f]\n"
      "  --cc-dctcp-mimd=INC_FACT    DCTCP: enable multiplicative inc  "
          "[default: disabled]\n"
      "  --cc-dctcp-min=RATE         DCTCP: minimum cap for flow rates  "
          "[default: %"PRIu32"]\n"
      "  --cc-const-rate=RATE        Constant rate for all flows "
          "[default: %"PRIu32"]\n"
      "  --cc-timely-tlow=TIME       Timely: low threshold (us) "
          "[default: %"PRIu32"]\n"
      "  --cc-timely-thigh=TIME      Timely: high threshold (us) "
          "[default: %"PRIu32"]\n"
      "  --cc-timely-step=STEP       Timely: additive increment step (kbps) "
          "[default: %"PRIu32"]\n"
      "  --cc-timely-init=RATE       Timely: initial flow rate (kbps) "
          "[default: %"PRIu32"]\n"
      "  --cc-timely-alpha=FRAC      Timely: EWMA weight for rtt diff "
          "[default: %f]\n"
      "  --cc-timely-beta=FRAC       Timely: mult. decr. factor "
          "[default: %f]\n"
      "  --cc-timely-minrtt=RTT      Timely: minimal rtt without queueing "
          "[default: %"PRIu32"]\n"
      "  --cc-timely-minrate=RTT     Timely: minimal rate to use "
          "[default: %"PRIu32"]\n"
      "\n"
      "IP protocol parameters:\n"
      "  --ip-route=DEST[/PREFIX],NEXTHOP  Add route\n"
      "  --ip-addr=ADDR[/PREFIXLEN]        Set local IP address\n"
      "\n"
      "ARP protocol parameters:\n"
      "  --arp-timeout=TIMEOUT       ARP request timeout (us) "
          "[default: %"PRIu32"]\n"
      "  --arp-timeout-max=TIMEOUT   ARP request max timeout (us) "
          "[default: %"PRIu32"]\n"
      "\n"
      "Miscelaneous:\n"
      "  --fp-poll-interval-app      App polling interval before blocsping "
          "in us [default: %"PRIu32"]\n"
      "  --quiet                     Disable non-essential logging "
          "[default: disabled]\n"
      "  --debug-console             Enable debug console "
          "[default: disabled]\n"
      "\n",
      progname, c->shm_len,
      c->nic_rx_len, c->nic_tx_len, c->app_spin_len, c->app_spout_len,
      c->tcp_rtt_init, c->tcp_link_bw, c->tcp_rxbuf_len, c->tcp_txbuf_len,
      c->tcp_handshake_to, c->tcp_handshake_retries,
      c->cc_control_granularity, c->cc_control_interval, c->cc_rexmit_ints,
      (double) c->cc_dctcp_weight / UINT32_MAX, c->cc_dctcp_min,
      c->cc_const_rate, c->cc_timely_tlow, c->cc_timely_thigh,
      c->cc_timely_step, c->cc_timely_init,
      (double) c->cc_timely_alpha / UINT32_MAX,
      (double) c->cc_timely_beta / UINT32_MAX, c->cc_timely_min_rtt,
      c->cc_timely_min_rate, c->arp_to, c->arp_to_max, c->fp_poll_interval_app);
}
static inline int parse_int64(const char *s, uint64_t *pi)
{
  char *end;
  *pi = strtoul(s, &end, 10);
  if (!*s || *end)
    return -1;
  return 0;
}

static inline int parse_int32(const char *s, uint32_t *pi)
{
  char *end;
  *pi = strtoul(s, &end, 10);
  if (!*s || *end)
    return -1;
  return 0;
}

static inline int parse_int8(const char *s, uint8_t *pi)
{
  char *end;
  *pi = strtoul(s, &end, 10);
  if (!*s || *end)
    return -1;
  return 0;
}

static inline int parse_double(const char *s, double *pd)
{
  char *end;
  *pd = strtod(s, &end);
  if (!*s || *end)
    return -1;
  return 0;
}

static inline int parse_cidr(char *s, uint32_t *ip, uint8_t *prefix)
{
  char *slash;

  /* parse /prefix and replace / by \0 (if applicable)*/
  if ((slash = strrchr(s, '/')) != NULL) {
    if (parse_int8(slash + 1, prefix) != 0) {
      fprintf(stderr, "parse_cidr: parsing prefix (%s) failed\n", slash);
      return -1;
    }
    *slash = 0;
  }

  if (util_parse_ipv4(s, ip) != 0) {
    fprintf(stderr, "parse_cidr: parsing IP (%s) failed\n", s);
    return -1;
  }

  return 0;
}

static inline int parse_route(char *s, struct configuration *c)
{
  struct config_route *r, *r_p;
  char *comma;

  if ((r = calloc(1, sizeof(*r))) == NULL) {
    fprintf(stderr, "parse_route: alloc failed\n");
    return -1;
  }

  /* split destination from next hop */
  if ((comma = strchr(s, ',')) == NULL) {
    fprintf(stderr, "parse_route: no comma found (%s)\n", s);
    goto failed;
  }
  *comma = 0;

  /* parse destination */
  r->ip_prefix = 32;
  if (parse_cidr(s, &r->ip, &r->ip_prefix) != 0) {
    fprintf(stderr, "parse_route: parsing destination (%s) failed\n", s);
    goto failed;
  }

  /* parse next hop */
  if (util_parse_ipv4(comma + 1, &r->next_hop_ip) != 0) {
    fprintf(stderr, "parse_route: parsing next hop (%s) failed\n", comma + 1);
    goto failed;
  }

  /* add to route list */
  r->next = NULL;
  if (c->routes == NULL) {
    c->routes = r;
  } else {
    for (r_p = c->routes; r_p->next != NULL; r_p = r_p->next);
    r_p->next = r;
  }
  return 0;

failed:
  return -1;
}
