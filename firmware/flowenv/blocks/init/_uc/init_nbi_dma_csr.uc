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
 * @file:   init_nbi_dma_csr.uc
 * @brief:  Contains macros to initialise the NBI DMA
 */

#ifndef _INIT_NBI_DMA_CSR_UC_
#define _INIT_NBI_DMA_CSR_UC_

/** NbiDmaXpb_NbiDmaCsr_NbiDmaCfg
 *
 * Sets the NBI DMA Configuration Register
 *
 * @param NBI_ID                The NBI number, can be 0 or 1
 * @param CTM_POLL_SEARCH       When set the CTM polling logic skips to the next active CTM every poll interval. When clear the CTM polling logic increments the CTM counter by one every poll interval
 * @param RATE_LIMIT_ENABLE     Selects whether to limit the bandwidth on the four Push Buses
 * @param CTM_POLL_INTVL        Configures the amount of time between packet allocation poll commands to a CTM.
 * @param CTM_POLL_ENA          Configures whether the polling function is enabled or disabled
 *
 */
#macro NbiDmaXpb_NbiDmaCsr_NbiDmaCfg(NBI_ID,CTM_POLL_SEARCH_ENA,RATE_LIMIT_ENABLE,CTM_POLL_INTVL,CTM_POLL_ENA)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCfg.CtmPollSearchEna               CTM_POLL_SEARCH_ENA const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCfg.RateLimitEnable                RATE_LIMIT_ENABLE   const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCfg.CtmPollIntvl                   CTM_POLL_INTVL      const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCfg.CtmPollEna                     CTM_POLL_ENA        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCfg.NbiNum                         (NBI_ID+1)          const
#endm


/** NbiDmaXpb_NbiDmaCsr_NbiDmaRate
 *
 * Configures the NBI DMA Rate Register
 *
 * @param NBI_ID            The NBI number, can be 0 or 1
 * @param CREDIT_RATE0      Sets the credit rate for push bus 0. Values are in 4 cycle increments.
 * @param CREDIT_RATE1      Sets the credit rate for push bus 1. Values are in 4 cycle increments.
 * @param CREDIT_RATE2      Sets the credit rate for push bus 2. Values are in 4 cycle increments.
 * @param CREDIT_RATE3      Sets the credit rate for push bus 3. Values are in 4 cycle increments.
 *
 */
#macro NbiDmaXpb_NbiDmaCsr_NbiDmaRate(NBI_ID,CREDIT_RATE0,CREDIT_RATE1,CREDIT_RATE2,CREDIT_RATE3)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaRate.CreditRate0   CREDIT_RATE0    const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaRate.CreditRate1   CREDIT_RATE1    const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaRate.CreditRate2   CREDIT_RATE2    const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaRate.CreditRate3   CREDIT_RATE3    const
#endm


/** NbiDmaXpb_NbiDmaCsr_NbiDmaCredit
 *
 * Configures the NBI DMA Credit Register
 *
 * @param NBI_ID            The NBI number, can be 0 or 1
 * @param CREDIT0       Sets the number of credits to subtract from the leaky bucket for push bus 3. A credit equates to 8-bytes or 1 cycle on the bus. A value of zero equates to 1 credits. A value of 3 equates to 4 credits.
 * @param CREDIT1       Sets the number of credits to subtract from the leaky bucket for push bus 3. A credit equates to 8-bytes or 1 cycle on the bus. A value of zero equates to 1 credits. A value of 3 equates to 4 credits.
 * @param CREDIT2       Sets the number of credits to subtract from the leaky bucket for push bus 3. A credit equates to 8-bytes or 1 cycle on the bus. A value of zero equates to 1 credits. A value of 3 equates to 4 credits.
 * @param CREDIT3       Sets the number of credits to subtract from the leaky bucket for push bus 3. A credit equates to 8-bytes or 1 cycle on the bus. A value of zero equates to 1 credits. A value of 3 equates to 4 credits.
 * @param THRESHOLD0    Sets the threshold that the leaky bucket is closed. Values are in increments of 16 Credits. A value of 0 equates to 16 credits or 128 bytes. A value of 3 equates to 64 credits or 512 bytes.
 * @param THRESHOLD1    Sets the threshold that the leaky bucket is closed. Values are in increments of 16 Credits. A value of 0 equates to 16 credits or 128 bytes. A value of 3 equates to 64 credits or 512 bytes.
 * @param THRESHOLD2    Sets the threshold that the leaky bucket is closed. Values are in increments of 16 Credits. A value of 0 equates to 16 credits or 128 bytes. A value of 3 equates to 64 credits or 512 bytes.
 * @param THRESHOLD3    Sets the threshold that the leaky bucket is closed. Values are in increments of 16 Credits. A value of 0 equates to 16 credits or 128 bytes. A value of 3 equates to 64 credits or 512 bytes.
 *
 */
#macro NbiDmaXpb_NbiDmaCsr_NbiDmaCredit(NBI_ID,CREDIT0, CREDIT1, CREDIT2, CREDIT3, THRESHOLD0,THRESHOLD1,THRESHOLD2,THRESHOLD3)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Credit0     CREDIT0     const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Credit1     CREDIT1     const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Credit2     CREDIT2     const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Credit3     CREDIT3     const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Threshold0  THRESHOLD0  const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Threshold1  THRESHOLD1  const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Threshold2  THRESHOLD2  const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaCredit.Threshold3  THRESHOLD3  const
#endm


/** NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg
 *
 * Configures NBI DMA Buffer Pool Config Registers
 *
 * @param NBI_ID            The NBI number, can be 0 or 1
 * @param BPE_HEAD      Pointer to the first Buffer Pool Entry register in the list associated with this buffer pool.
 * @param SPLIT_LENGTH  Encoded packet size used when deciding to split packets between CTM and internal/external MUs. The encoding is in powers of 2 starting at 256 bytes.
 * @param SEC_BLIST     Secondary Buffer list number associated. There are 4 buffer lists available, which contain buffer descriptors that are used to store packet data in internal/external MUs.
 * @param PRI_BLIST     Primary Buffer list number associated. There are 4 buffer lists available, which contain buffer descriptors that are used to store packet data in internal/external MUs.
 * @param CTM_OFFSET    Defines the offset to add to the data reference when starting to write packet data to the CTM. The space in front of the packet is for the rewrite script used by the packet modifier. 0: 32B offset, 1: 64B offset
 * @param DROP_EN       Enables a packet to be dropped when buffer or packet credits run out or there are no MU buffers left on the selected buffer list
 *
 */
#macro NbiDmaXpb_NbiDmaCsr_NbiDmaBPCfg(NBI_ID,NBI_DMA_BP,BPE_HEAD,SPLIT_LENGTH,SEC_BLIST,PRI_BLIST,CTM_OFFSET,DROP_EN)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBP/**/NBI_DMA_BP/**/Cfg.BpeHead        BPE_HEAD        const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBP/**/NBI_DMA_BP/**/Cfg.SplitLength    SPLIT_LENGTH    const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBP/**/NBI_DMA_BP/**/Cfg.SecBList       SEC_BLIST       const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBP/**/NBI_DMA_BP/**/Cfg.PriBList       PRI_BLIST       const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBP/**/NBI_DMA_BP/**/Cfg.CtmOffset      CTM_OFFSET      const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBP/**/NBI_DMA_BP/**/Cfg.DropEn         DROP_EN         const
#endm


/** NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg
 *
 * Configures the NBI Buffer Pool Entry Configuration Register
 *
 * @param NBI_ID            The NBI number, can be 0 or 1
 * @param BUF_CREDIT      Configures the number of buffer credits in 2K byte increments associated with the CTM. The msbit is the sign bit and must be cleared during configuration.
 * @param PKT_CREDIT      Configures the number of packets allocated to the CTM
 * @param CTM             SConfigures the number of packets allocated to the CTM
 * @param BPE_NUM         Buffer Pool Entry Number. When the register is written this value is the latched version of the buffer pool register number 0-31. Used for debug purposes..
 *
 */
#macro NbiDmaXpb_NbiDmaCsr_NbiDmaBpeCfg(NBI_ID,BPE_NUM,CTM,BUF_CREDIT,PKT_CREDIT)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBpe/**/BPE_NUM/**/Cfg.BufCredit   BUF_CREDIT  const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBpe/**/BPE_NUM/**/Cfg.PktCredit   PKT_CREDIT  const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBpe/**/BPE_NUM/**/Cfg.Ctm         CTM         const
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBpe/**/BPE_NUM/**/Cfg.BpeNum      BPE_NUM     const
#endm


/** NbiDmaXpb_NbiDmaCsr_NbiDmaBpeChainEnd
 *
 * Configures the NBI DMA Buffer Pool Entry Chain End Register
 *
 * @param NBI_ID            The NBI number, can be 0 or 1
 * @param BPECHAINEND      Configures BPE module 'n' as the end of the buffer pool entry chain, where 'n' = 0 to 31.
 *
 */
#macro NbiDmaXpb_NbiDmaCsr_NbiDmaBpeChainEnd(NBI_ID,BPECHAINEND)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.NbiDmaXpb.NbiDmaCsr.NbiDmaBpeChainEnd.BpeChainEnd BPECHAINEND const
#endm

#endif /* _INIT_NBI_DMA_CSR_UC_ */
