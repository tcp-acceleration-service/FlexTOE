/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>

#include <rte/io.h>
#include <rte/byteorder.h>
#include <rte/hash_crc.h>

#include "util/common.h"
#include "util/timeout.h"
#include "util/log.h"
#include "util/shm.h"

#include "flextoe.h"
#include "fp_mem.h"
#include "internal.h"
#include "packet_defs.h"

struct nic_buffer {
  uint64_t addr;
  void *buf;
};

struct flow_id_item {
  uint32_t flow_id;
  struct flow_id_item *next;
};

/* NUM_FLOW_GROUPS = 4 */
/* NUM_CLS_CACHE_SLOTS = 512 */
static int flow_slot_status[4][512];

static int adminq_init(void);
static inline int rxq_poll(void);
static inline void process_packet(const void *buf, uint16_t len, uint16_t flow_group);
static inline volatile struct flextcp_pl_sptx_t *sptx_try_alloc(
    struct nic_buffer **buf, uint32_t *new_tail);
static inline uint32_t flow_hash(ip_addr_t lip, beui16_t lp,
    ip_addr_t rip, beui16_t rp);
static inline int flow_slot_alloc(uint32_t lip, uint32_t rip,
    uint16_t lp, uint16_t rp, uint32_t f_id);
static inline int flow_slot_clear(uint32_t f_id, uint32_t lip, uint16_t lp,
    uint32_t rip, uint16_t rp);
static void flow_id_alloc_init(void);
static int flow_id_alloc(uint32_t *fid, uint32_t fgrp);
static void flow_id_free(uint32_t flow_id, uint32_t fgrp);

struct flow_id_item flow_id_items[FLEXNIC_PL_FLOWST_NUM];
struct flow_id_item *flow_id_freelist;

static struct nic_buffer *rxq_bufs;
static struct flextcp_pl_sprx_t *rxq_base;
static uint32_t rxq_head;
static uint32_t rxq_len;
static uint32_t rxq_next;

static struct nic_buffer *txq_bufs;
static struct flextcp_pl_sptx_t *txq_base;
static uint32_t txq_tail;
static uint32_t txq_len;

int nicif_init(void)
{
  /* prepare packet memory manager */
  if (packetmem_init()) {
    fprintf(stderr, "nicif_init: pkt_mem_init failed\n");
    return -1;
  }

  /* prepare flow_id allocator */
  flow_id_alloc_init();

  if (adminq_init()) {
    fprintf(stderr, "nicif_init: initializing admin queue failed\n");
    return -1;
  }

  return 0;
}

unsigned nicif_poll(void)
{
  unsigned i, ret = 0;
  int x;

  for (i = 0; i < 512; i++) {
    x = rxq_poll();

    ret += (x == -1 ? 0 : 1);
  }

  return ret;
}

static int nicif_appctx_doorbell_write(uint32_t db, int evfd)
{
  char irq_config_path[PATH_MAX];
  char irq_fd_config[PAGE_SIZE];
  int fd;
  ssize_t ret, count;

  /* Build irq_config_path */
  snprintf(irq_config_path, PATH_MAX,
            "%s/" PCI_PRI_FMT "/irq_fds",
            "/sys/bus/pci/devices",
            nic_handle.dev->addr.domain,
            nic_handle.dev->addr.bus,
            nic_handle.dev->addr.devid,
            nic_handle.dev->addr.function);

  if (db >= FLEXNIC_PL_APPCTX_NUM) {
    fprintf(stderr, "%s: appctx id too high (%u, max=%u)\n",
            __func__, db, FLEXNIC_PL_APPCTX_NUM);
    return -1;
  }

  count = snprintf(irq_fd_config, PAGE_SIZE,
                  "%u %d\n", db, evfd);

  fd = open(irq_config_path, O_WRONLY);
  if (fd < 0) {
    fprintf(stderr, "%s: failed to configure IRQ with path=%s error=%s\n",
          __func__, irq_config_path, strerror(errno));
    return -1;
  }

  /* TODO: Add error handling */
  ret = write(fd, irq_fd_config, count);
  if (ret < count) {
    fprintf(stderr, "%s: failed to configure IRQ\n", __func__);
    close(fd);
    return -1;
  }

  return 0;
}

static int nicif_appctx_doorbell_register(uint32_t db, int evfd)
{
  return nicif_appctx_doorbell_write(db, evfd);
}

static int nicif_appctx_doorbell_clear(uint32_t db)
{
  return nicif_appctx_doorbell_write(db, -1);
}

/** Register application context */
int nicif_appctx_add(uint16_t appid, uint32_t db, uint64_t rxq_base,
    uint32_t rxq_len, uint64_t txq_base, uint32_t txq_len, int evfd)
{
  struct flextcp_pl_appctx_t *actx;
  struct flextcp_pl_appst_t *ast = &fp_state->appst[appid];
  uint16_t i;

  if (appid >= FLEXNIC_PL_APPST_NUM) {
    fprintf(stderr, "nicif_appctx_add: app id too high (%u, max=%u)\n", appid,
        FLEXNIC_PL_APPST_NUM);
    return -1;
  }

  if (nn_readw(&ast->ctx_num) >= FLEXNIC_PL_APPCTX_NUM) {
    fprintf(stderr, "nicif_appctx_add: too many contexts in app\n");
    return -1;
  }

  actx = &fp_state->appctx[db];
  nn_writel(appid, &actx->appst_id);
  nn_writel((uint32_t) (util_virt2phy((void*) rxq_base) - util_virt2phy(flextoe_dma_mem)), &actx->rx.base_lo);
  nn_writel((uint32_t) (util_virt2phy((void*) txq_base) - util_virt2phy(flextoe_dma_mem)), &actx->tx.base_lo);

  MEM_BARRIER();

  nn_writel(txq_len/sizeof(struct flextcp_pl_atx_t), &actx->tx.len);
  nn_writel(0, &actx->tx.p_idx);
  nn_writel(0, &actx->tx.c_idx);
  nn_writel(rxq_len/sizeof(struct flextcp_pl_arx_t), &actx->rx.len);
  nn_writel(0, &actx->rx.c_idx);
  nn_writel(0, &actx->rx.p_idx);

  MEM_BARRIER();
  nn_writew(db, &ast->ctx_ids[ast->ctx_num]);
  MEM_BARRIER();
  i = nn_readw(&ast->ctx_num);
  nn_writew(i + 1, &ast->ctx_num);

  if (nicif_appctx_doorbell_register(db, evfd) < 0) {
    return -1;
  }

  return 0;
}

/** Close application */
int nicif_appctx_clear(uint16_t appid, uint32_t db)
{
  // TODO: Not yet implemented
  fprintf(stderr, "%s: Not yet implemented!\n", __func__);

  if (nicif_appctx_doorbell_clear(db) < 0) {
    return -1;
  }

  return 0;
}

/** Register flow */
int nicif_connection_add(uint32_t db, uint64_t mac_remote, uint32_t ip_local,
    uint16_t port_local, uint32_t ip_remote, uint16_t port_remote,
    uint64_t rx_base, uint32_t rx_len, uint64_t tx_base, uint32_t tx_len,
    uint32_t remote_seq, uint32_t local_seq, uint64_t app_opaque,
    uint32_t flags, uint32_t rate, uint16_t flow_group,
    uint32_t *pf_id)
{
  struct flowst_tcp_t* fs_tcp;
  struct flowst_conn_t* fs_conn;
  struct flowst_mem_t* fs_mem;
  struct flowst_cc_t* fs_cc;
  uint32_t f_id;
  uint16_t tx_flags = 0;

  /* allocate flow id */
  if (flow_id_alloc(&f_id, flow_group) != 0) {
    fprintf(stderr, "nicif_connection_add: allocating flow state\n");
    return -1;
  }

  rx_base = util_virt2phy((void*) rx_base);
  tx_base = util_virt2phy((void*) tx_base);

  if ((flags & NICIF_CONN_ECN) == NICIF_CONN_ECN) {
    tx_flags |= FLEXNIC_PL_FLOWST_ECN;
  }

  fs_tcp = &fp_state->flows_tcp_state[f_id];
  fs_conn = &fp_state->flows_conn_info[f_id];
  fs_mem = &fp_state->flows_mem_info[f_id];
  fs_cc = &fp_state->flows_cc_info[f_id];

  nn_writel(tx_len, &fs_tcp->tx_len);
  nn_writel(0, &fs_tcp->tx_avail);
  nn_writel(rx_len, &fs_tcp->tx_remote_avail);
  nn_writel(0, &fs_tcp->tx_sent);
  nn_writel(local_seq, &fs_tcp->tx_next_seq);
  nn_writel(0, &fs_tcp->tx_next_pos);
  nn_writew(0, &fs_tcp->dupack_cnt);
  nn_writew(flags, &fs_tcp->flags);
  nn_writel(rx_len, &fs_tcp->rx_len);
  nn_writel(rx_len, &fs_tcp->rx_avail);
  nn_writel(remote_seq, &fs_tcp->rx_next_seq);
  nn_writel(0, &fs_tcp->rx_next_pos);
  nn_writel(0, &fs_tcp->rx_ooo_len);
  nn_writel(0, &fs_tcp->rx_ooo_start);

  nn_writel(flow_group, &fs_conn->flow_grp);
  nn_writeq(htobe64(mac_remote), &fs_conn->remote_mac_1);
  nn_writew(flags, &fs_conn->flags);
  nn_writel(ip_local, &fs_conn->local_ip);
  nn_writel(ip_remote, &fs_conn->remote_ip);
  nn_writew(port_local, &fs_conn->local_port);
  nn_writew(port_remote, &fs_conn->remote_port);

  nn_writew((uint16_t) ((app_opaque >> 32) & 0xFFFF), &fs_mem->opaque_hi);
  nn_writew(db, &fs_mem->db_id);
  nn_writel((uint32_t) (app_opaque & 0xFFFFFFFF), &fs_mem->opaque_lo);
  nn_writeq(rx_base, &fs_mem->rx_base);
  nn_writeq(tx_base, &fs_mem->tx_base);
  nn_writel(rx_len, &fs_mem->rx_len);
  nn_writel(tx_len, &fs_mem->tx_len);

  nn_writel(0, &fs_cc->tx_avail);
  nn_writel(0, &fs_cc->tx_rate);
  nn_writel(config.tcp_rtt_init, &fs_cc->rtt_est);
  nn_writel(0, &fs_cc->txp);
  nn_writel(0, &fs_cc->cnt_tx_drops);
  nn_writel(0, &fs_cc->cnt_rx_acks);
  nn_writel(0, &fs_cc->cnt_rx_ack_bytes);
  nn_writel(0, &fs_cc->cnt_rx_ecn_bytes);

  /* write to empty entry first */
  MEM_BARRIER();
  /* calculate hash and find empty slot */
  if (flow_slot_alloc(ip_local, ip_remote, port_local, port_remote, f_id) != 0) {
    flow_id_free(f_id, flow_group);
    fprintf(stderr, "nicif_connection_add: allocating slot failed\n");
    return -1;
  }

  *pf_id = f_id;
  return 0;
}

int nicif_connection_disable(uint32_t f_id, uint32_t *tx_seq, uint32_t *rx_seq,
    int *tx_closed, int *rx_closed)
{
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;
  uint32_t tail;

  if ((sptx = sptx_try_alloc(&buf, &tail)) == NULL) {
    return -1;
  }
  txq_tail = tail;

  sptx->msg.connclose.flow_id = htobe32(f_id);

  sptx->type = htobe32(FLEXTCP_PL_SPTX_CONN_CLOSE);

  rte_wmb();

  /* Doorbell to consumer */
  nn_writel(tail, &fp_state->spctx.tx_tail);

  *tx_closed = 1;
  *rx_closed = 1;

  return 0;
}

void nicif_connection_free(uint32_t f_id, uint32_t flow_grp)
{
  flow_id_free(f_id, flow_grp);
}

/** Move flow to new db */
int nicif_connection_move(uint32_t dst_db, uint32_t f_id)
{
  struct flowst_mem_t *fs;

  if (f_id >= FLEXNIC_PL_FLOWST_NUM) {
    fprintf(stderr, "%s: bad flow id\n", __func__);
    return -1;
  }

  fs = &fp_state->flows_mem_info[f_id];
  nn_writew(dst_db, &fs->db_id);
  return 0;
}

/** Read connection stats from NIC. */
int nicif_connection_stats(uint32_t f_id,
    struct nicif_connection_stats *p_stats)
{
  struct flowst_cc_t *fs;

  if (f_id >= FLEXNIC_PL_FLOWST_NUM) {
    fprintf(stderr, "%s: bad flow id\n", __func__);
    return -1;
  }

  fs = &fp_state->flows_cc_info[f_id];
  p_stats->rtt = nn_readl(&fs->rtt_est);
  p_stats->txp = nn_readl(&fs->txp);
  p_stats->c_drops = (uint16_t) nn_readl(&fs->cnt_tx_drops);
  p_stats->c_acks  = (uint16_t) nn_readl(&fs->cnt_rx_acks);
  p_stats->c_ackb  = nn_readl(&fs->cnt_rx_ack_bytes);
  p_stats->c_ecnb  = nn_readl(&fs->cnt_rx_ecn_bytes);

  return 0;
}

/**
 * Set rate for flow.
 *
 * @param f_id  ID of flow
 * @param rate  Rate to set [Kbps]
 *
 * @return 0 on success, <0 else
 */
int nicif_connection_setrate(uint32_t f_id, uint32_t rate)
{
  uint64_t cyc;

  if (f_id >= FLEXNIC_PL_FLOWST_NUM) {
    fprintf(stderr, "nicif_connection_stats: bad flow id\n");
    return -1;
  }

  /* Calculate ME Clk Cnts to achieve rate */
  if (rate == 0) {
    cyc = 0;
  }
  else {
    /* Measure cyc in 1024 subcycles */
    // TODO: Explain this formula. Define a standard macro
    cyc = (8 * 8 * 1000000ull * 1024)/(rate * 10ull);
    cyc = MIN(cyc, 512 * 512 * 1024);
  }

  /* Write rate using MMIO */
  nn_writel((uint32_t) cyc, &fp_state->flows_cc_info[f_id].tx_rate);

  return 0;
}

/** Mark flow for retransmit after timeout. */
int nicif_connection_retransmit(uint32_t f_id, uint16_t flow_group)
{
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;
  uint32_t tail;

  if ((sptx = sptx_try_alloc(&buf, &tail)) == NULL) {
    return -1;
  }
  txq_tail = tail;

  sptx->msg.connretran.flow_id = htobe32(f_id);
  sptx->msg.connretran.flow_grp = htobe32(flow_group);
  sptx->type = htobe32(FLEXTCP_PL_SPTX_CONN_RETX);

  rte_wmb();

  /* Doorbell to consumer */
  nn_writel(tail, &fp_state->spctx.tx_tail);

  return 0;
}

/** Debug reset */
int nicif_debug_reset()
{
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;
  uint32_t tail;

  if ((sptx = sptx_try_alloc(&buf, &tail)) == NULL) {
    return -1;
  }
  txq_tail = tail;
  sptx->type = htobe32(FLEXTCP_PL_SPTX_DEBUG_RESET);

  rte_wmb();

  /* Doorbell to consumer */
  nn_writel(tail, &fp_state->spctx.tx_tail);

  return 0;
}

/** Allocate transmit buffer */
int nicif_tx_alloc(uint16_t len, void **pbuf, uint32_t *opaque)
{
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;

  if ((sptx = sptx_try_alloc(&buf, opaque)) == NULL) {
    return -1;
  }

  sptx->msg.packet.len = htobe32(len);
  *pbuf = buf->buf;
  return 0;
}

/** Actually send out transmit buffer (lens need to match) */
void nicif_tx_send(uint32_t opaque, int no_ts, uint32_t ts_offset)
{
  uint32_t tail = (opaque == 0 ? txq_len - 1 : opaque - 1);
  volatile struct flextcp_pl_sptx_t *sptx = &txq_base[tail];

  sptx->msg.packet.ts_offset = htobe32(ts_offset);
  sptx->type = htobe32((!no_ts ? FLEXTCP_PL_SPTX_PACKET : FLEXTCP_PL_SPTX_PACKET_NOTS));

  rte_wmb();

  /* Doorbell to consumer */
  nn_writel(opaque, &fp_state->spctx.tx_tail);
}

static int adminq_init(void)
{
  struct packetmem_handle *pm_bufs, *pm_desc;
  uintptr_t off_bufs, off_rxbufs, off_txbufs, off_desc;
  size_t i, sz_bufs, sz_rx, sz_tx;

  rxq_len = config.nic_rx_len;
  txq_len = config.nic_tx_len;

  rxq_next = 0;

  if ((rxq_bufs = calloc(config.nic_rx_len, sizeof(*rxq_bufs)))
      == NULL)
  {
    fprintf(stderr, "adminq_init: calloc rx bufs failed\n");
    return -1;
  }
  if ((txq_bufs = calloc(config.nic_tx_len, sizeof(*txq_bufs)))
      == NULL)
  {
    fprintf(stderr, "adminq_init: calloc tx bufs failed\n");
    goto free_rxqbufs;
  }

  sz_bufs = ((config.nic_rx_len + config.nic_tx_len) * PKTBUF_SIZE) & ~0xfffULL;
  if (packetmem_alloc(sz_bufs, &off_bufs, &pm_bufs) != 0) {
    fprintf(stderr, "adminq_init: packetmem_alloc bufs failed\n");
    goto free_txqbufs;
  }

  sz_rx = config.nic_rx_len * sizeof(struct flextcp_pl_sprx_t);
  sz_tx = config.nic_tx_len * sizeof(struct flextcp_pl_sptx_t);

  if (packetmem_alloc(sz_rx + sz_tx, &off_desc, &pm_desc) != 0) {
    fprintf(stderr, "adminq_init: packetmem_alloc tx failed\n");
    goto free_pmbufs;
  }

  rxq_base = (struct flextcp_pl_sprx_t*) ((uint8_t*) flextoe_dma_mem + off_desc);
  rxq_head = 0;
  txq_base = (struct flextcp_pl_sptx_t*) ((uint8_t*) flextoe_dma_mem + off_desc + sz_rx);
  txq_tail = 0;

  memset((void *) rxq_base, 0, sz_rx);
  memset((void *) txq_base, 0, sz_tx);

  off_rxbufs = off_bufs;
  for (i = 0; i < config.nic_rx_len; i++) {
    rxq_bufs[i].addr = off_bufs;
    rxq_bufs[i].buf = (uint8_t *) flextoe_dma_mem + off_bufs;
    off_bufs += PKTBUF_SIZE;
  }

  off_txbufs = off_bufs;
  for (i = 0; i < config.nic_tx_len; i++) {
    txq_bufs[i].addr = off_bufs;
    txq_bufs[i].buf = (uint8_t *) flextoe_dma_mem + off_bufs;
    off_bufs += PKTBUF_SIZE;
  }

  nn_writeq(util_virt2phy((uint8_t*) flextoe_dma_mem + off_rxbufs), &fp_state->spctx.rx_base);
  nn_writeq(util_virt2phy(rxq_base), &fp_state->spctx.rx_desc_base);
  nn_writel(config.nic_rx_len, &fp_state->spctx.rx_len);
  nn_writeq(util_virt2phy((uint8_t*) flextoe_dma_mem + off_txbufs), &fp_state->spctx.tx_base);
  nn_writeq(util_virt2phy(txq_base), &fp_state->spctx.tx_desc_base);
  nn_writel(config.nic_tx_len, &fp_state->spctx.tx_len);
  nn_writel(0, &fp_state->spctx.rx_head);
  nn_writel(0, &fp_state->spctx.rx_tail);
  nn_writel(0, &fp_state->spctx.tx_head);
  nn_writel(0, &fp_state->spctx.tx_tail);

  rte_io_mb();

  return 0;

free_pmbufs:
  packetmem_free(pm_bufs);
free_txqbufs:
  free(txq_bufs);
free_rxqbufs:
  free(rxq_bufs);
  return -1;
}

static inline int rxq_poll(void)
{
  uint32_t head, updated_head;
  volatile struct flextcp_pl_sprx_t *sprx;
  struct nic_buffer *buf;
  int ret = 0;
  uint16_t type;

  head = rxq_head;
  sprx = &rxq_base[head];
  buf = &rxq_bufs[head];

  /* handle based on queue entry type */
  type = be32toh(sprx->type);
  if (type == FLEXTCP_PL_SPRX_INVALID)
    return -1;

  /* prevent reordering */
  rte_rmb();

  switch (type) {
    case FLEXTCP_PL_SPRX_PACKET:
      process_packet(buf->buf,
          (uint16_t) be32toh(sprx->msg.packet.len),
          (uint16_t) be32toh(sprx->msg.packet.flow_group));
      break;

    default:
      fprintf(stderr, "rxq_poll: unknown rx type 0x%x head %x\n",
          type, head);
      exit(0);
  }

  /* invalidate the descriptor */
  sprx->type = htobe32(FLEXTCP_PL_SPRX_INVALID);

  /* update head */
  updated_head = head + 1;
  if (updated_head == rxq_len) {
    updated_head -= rxq_len;
  }
  rxq_head = updated_head;

  /* prevent reordering */
  rte_wmb();

  /* write head to NIC memory */
  nn_writel(updated_head, &fp_state->spctx.rx_head);

  return ret;
}

static inline void process_packet(const void *buf, uint16_t len, uint16_t flow_group)
{
  const struct eth_hdr *eth = buf;
  const struct ip_hdr *ip = (struct ip_hdr *) (eth + 1);
  const struct tcp_hdr *tcp = (struct tcp_hdr *) (ip + 1);
  int ret = 1;

  if (f_beui16(eth->type) == ETH_TYPE_ARP) {
    if (len < sizeof(struct pkt_arp)) {
      fprintf(stderr, "process_packet: short arp packet\n");
      return;
    }

    arp_packet(buf, len);
    ret = 0;
  } else if (f_beui16(eth->type) == ETH_TYPE_IP) {
    if (len < sizeof(*eth) + sizeof(*ip)) {
      fprintf(stderr, "process_packet: short ip packet\n");
      return;
    }

    if (ip->proto == IP_PROTO_TCP) {
      if (len < sizeof(*eth) + sizeof(*ip) + sizeof(*tcp)) {
        fprintf(stderr, "process_packet: short tcp packet\n");
        return;
      }

      ret = !!tcp_packet(buf, len, flow_group);
    }
  }

  if (ret && !config.quiet) {
    fprintf(stderr, "fail to process nicif packet\n");
  }
}

static inline volatile struct flextcp_pl_sptx_t *sptx_try_alloc(
    struct nic_buffer **pbuf, uint32_t *new_tail)
{
  uint32_t tail, updated_tail;
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;

  tail = txq_tail;

  sptx = &txq_base[tail];
  buf = &txq_bufs[tail];

  /* queue is full */
  if (sptx->type != FLEXTCP_PL_SPTX_INVALID)
    return NULL;

  rte_rmb();

  updated_tail = tail + 1;
  if (updated_tail >= txq_len)
    updated_tail = 0;

  *pbuf = buf;
  *new_tail = updated_tail;

  txq_tail = updated_tail;

  return sptx;
}

static const uint8_t bit_reflect_table_256[] =
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

static inline uint32_t flow_hash(beui32_t lip, beui16_t lp,
    beui32_t rip, beui16_t rp)
{
  uint8_t hkey[12];

  hkey[0] = bit_reflect_table_256[((lip.x) & 0xFF)];
  hkey[1] = bit_reflect_table_256[((lip.x >> 8) & 0xFF)];
  hkey[2] = bit_reflect_table_256[((lip.x >> 16) & 0xFF)];
  hkey[3] = bit_reflect_table_256[((lip.x >> 24) & 0xFF)];
  hkey[4] = bit_reflect_table_256[((rip.x) & 0xFF)];
  hkey[5] = bit_reflect_table_256[((rip.x >> 8) & 0xFF)];
  hkey[6] = bit_reflect_table_256[((rip.x >> 16) & 0xFF)];
  hkey[7] = bit_reflect_table_256[((rip.x >> 24) & 0xFF)];
  hkey[8] = bit_reflect_table_256[((lp.x) & 0xFF)];
  hkey[9] = bit_reflect_table_256[((lp.x >> 8) & 0xFF)];
  hkey[10] = bit_reflect_table_256[((rp.x) & 0xFF)];
  hkey[11] = bit_reflect_table_256[((rp.x >> 8) & 0xFF)];

  MEM_BARRIER();

  uint32_t hash = rte_hash_crc(hkey, sizeof(hkey), 0);
  uint32_t hash_reflect = ((((uint32_t) bit_reflect_table_256[(hash & 0xFF)]) << 24) |
                            (((uint32_t) bit_reflect_table_256[((hash >> 8) & 0xFF)]) << 16) |
                            (((uint32_t) bit_reflect_table_256[((hash >> 16) & 0xFF)]) << 8) |
                            (((uint32_t) bit_reflect_table_256[((hash >> 24) & 0xFF)])));

  return hash_reflect;
}

static inline int flow_slot_alloc(uint32_t lip, uint32_t rip,
    uint16_t lp, uint16_t rp, uint32_t f_id)
{
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;
  uint32_t tail;

  if (f_id >= FLEXNIC_PL_FLOWST_NUM) {
    fprintf(stderr, "flow_slot_alloc: bad flow id\n");
    return -1;
  }

  if ((sptx = sptx_try_alloc(&buf, &tail)) == NULL) {
    return -1;
  }
  txq_tail = tail;

  sptx->msg.flowht.flow_id = htobe32(f_id);
  sptx->msg.flowht.local_ip = htobe32(lip);
  sptx->msg.flowht.remote_ip = htobe32(rip);
  sptx->msg.flowht.local_port = htobe16(lp);
  sptx->msg.flowht.remote_port = htobe16(rp);

  sptx->type = htobe32(FLEXTCP_PL_SPTX_FLOWHT_ADD);

  rte_wmb();

  /* Doorbell to consumer */
  nn_writel(tail, &fp_state->spctx.tx_tail);

  return 0;
}

static inline int flow_slot_clear(uint32_t f_id, uint32_t lip, uint16_t lp,
    uint32_t rip, uint16_t rp)
{
  volatile struct flextcp_pl_sptx_t *sptx;
  struct nic_buffer *buf;
  uint32_t tail;

  if (f_id >= FLEXNIC_PL_FLOWST_NUM) {
    fprintf(stderr, "flow_slot_clear: bad flow id\n");
    return -1;
  }

  if ((sptx = sptx_try_alloc(&buf, &tail)) == NULL) {
    return -1;
  }
  txq_tail = tail;

  sptx->msg.flowht.flow_id = htobe32(f_id);
  sptx->msg.flowht.local_ip = htobe32(lip);
  sptx->msg.flowht.remote_ip = htobe32(rip);
  sptx->msg.flowht.local_port = htobe16(lp);
  sptx->msg.flowht.remote_port = htobe16(rp);

  sptx->type = htobe32(FLEXTCP_PL_SPTX_FLOWHT_DEL);

  rte_wmb();

  /* Doorbell to consumer */
  nn_writel(tail, &fp_state->spctx.tx_tail);

  return 0;
}

static void flow_id_alloc_init(void)
{
  size_t i;
  struct flow_id_item *it, *prev = NULL;
  for (i = 1; i < FLEXNIC_PL_FLOWST_NUM; i++) {
    it = &flow_id_items[i];
    it->flow_id = i;
    it->next = NULL;

    if (prev == NULL) {
      flow_id_freelist = it;
    } else {
      prev->next = it;
    }
    prev = it;
  }
}

static int flow_id_alloc(uint32_t *fid, uint32_t fgrp)
{
  int i;
  struct flow_id_item *it = flow_id_freelist;
  struct flow_id_item *iprev;

  if (it == NULL)
    return -1;

  for (i = 0; i < 512; i++) {
    if (flow_slot_status[fgrp][i] == 0)
      break;
  }

  if (i == 512) {
    flow_id_freelist = it->next;
    *fid = it->flow_id;
    return 0;
  }

  if (it->flow_id % 512 == i) {
    flow_slot_status[fgrp][i]++;
    *fid = it->flow_id;
    flow_id_freelist = it->next;
    return 0;
  }

  iprev = it;
  it = it->next;

  /* Find right slot */
  while (it != NULL) {
    if (it->flow_id % 512 == i) {
      flow_slot_status[fgrp][i]++;
      *fid = it->flow_id;
      iprev->next = it->next;

      return 0;
    }

    iprev = it;
    it = it->next;
  }

  flow_id_freelist = it->next;
  *fid = it->flow_id;
  flow_slot_status[fgrp][it->flow_id % 512]++;
  return 0;
}

static void flow_id_free(uint32_t flow_id, uint32_t fgrp)
{
  struct flow_id_item *it = &flow_id_items[flow_id];
  it->next = flow_id_freelist;
  flow_id_freelist = it;
  flow_slot_status[fgrp][flow_id % 512]--;
}
