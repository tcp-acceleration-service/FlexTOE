#
# Copyright (C) 2015-2018,  Netronome Systems, Inc.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @file         apps/lab_template/Makefile
# @brief        Makefile for a template lab
#

# Define src_dir FIRST - it is the directory that this makefile resides in
# MUST OCCUR BEFORE ANY include's (which will change MAKEFILE_LIST)
app_src_dir     := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
ROOT_SRC_DIR  := $(realpath $(app_src_dir)/..)
flowenv_dir        := flowenv

microc_blocks_dir   := $(flowenv_dir)/blocks
microc_libs_dir     := $(flowenv_dir)/lib
microc_inc_dir      := $(flowenv_dir)/include

include Makefile.nfp.config
include Makefile.templates

PKT_NBI_OFFSET ?= 64

# BLM
$(eval $(call microcode.assemble,blm_obj,$(microc_blocks_dir)/blm,blm_main.uc))
$(eval $(call microcode.add_flags,blm_obj,-DBLM_CUSTOM_CONFIG -DSINGLE_NBI -DPKT_NBI_OFFSET=$(PKT_NBI_OFFSET)))
$(eval $(call microcode.add_flags,blm_obj,-DBLM_BLQ_EMEM_TYPE=emem -DNBII=8 -DBLM_INSTANCE_ID=0))
$(eval $(call microcode.add_flags,blm_obj,-DBLM_INIT_EMU_RINGS))
$(eval $(call microcode.add_flags,blm_obj,-DNFP_LIB_ANY_NFAS_VERSION))
$(eval $(call microcode.add_flags,blm_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call microcode.add_flags,blm_obj,-I$(microc_blocks_dir)/blm/_h))
$(eval $(call microcode.add_flags,blm_obj,-I$(microc_blocks_dir)/blm/_uc))


#
# SP Application definition
#
$(eval $(call micro_c.compile_with_rtl,sp_obj,slowpath_if.c))
$(eval $(call micro_c.add_src_lib,sp_obj,flow_lookup))
$(eval $(call micro_c.add_fw_libs,sp_obj,nfp pkt lu std))
$(eval $(call micro_c.add_flags,sp_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call micro_c.add_src_lib,sp_obj,libblm,$(microc_blocks_dir)/blm))

#
# ATX Application definition
#
$(eval $(call micro_c.compile_with_rtl,atx_obj,atx.c))
$(eval $(call micro_c.add_std_libs,atx_obj,intrinsic))
$(eval $(call micro_c.add_fw_libs,atx_obj,nfp std))

#
# ARX Application definition
#
$(eval $(call micro_c.compile_with_rtl,arx_obj,arx.c))
$(eval $(call micro_c.add_fw_libs,arx_obj,nfp std))

# GRO Block
$(eval $(call microcode.assemble,gro_obj,$(microc_blocks_dir)/gro/_uc,gro_out.uc))
$(eval $(call microcode.add_flags,gro_obj,-I$(microc_blocks_dir)/gro))
$(eval $(call microcode.add_flags,gro_obj,-I$(microc_blocks_dir)/gro/_uc))
$(eval $(call microcode.add_flags,gro_obj,-DGRO_BLOCK_NUM=0))
$(eval $(call microcode.add_flags,gro_obj,-DNFD_VERSION=0x03010000))
$(eval $(call microcode.add_flags,gro_obj,-keep_unreachable_code))

#
# Initialization
# PLATFORM = NS_PLATFORM_BERYLLIUM
$(eval $(call microcode.assemble_init,fw_init_obj,$(flowenv_dir)/blocks/init,init_main.uc))

#
# Preprocess Application definition
#
$(eval $(call micro_c.compile_with_rtl,preproc_obj,preprocess.c))
$(eval $(call micro_c.add_std_libs,preproc_obj,libc intrinsic))
$(eval $(call micro_c.add_src_lib,preproc_obj,flow_lookup))
$(eval $(call micro_c.add_fw_libs,preproc_obj,nfp std pkt lu gro))
$(eval $(call micro_c.add_flags,preproc_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call micro_c.add_flags,preproc_obj,-I$(microc_blocks_dir)/gro))
$(eval $(call micro_c.add_flags,preproc_obj,-I$(microc_blocks_dir)/gro/_uc))
$(eval $(call micro_c.add_src_lib,preproc_obj,libblm,$(microc_blocks_dir)/blm))

#
# Postprocess Application definition
#
$(eval $(call micro_c.compile_with_rtl,postproc_obj,postprocess.c))
$(eval $(call micro_c.add_std_libs,postproc_obj,libc intrinsic))
$(eval $(call micro_c.add_fw_libs,postproc_obj,nfp std pkt))
$(eval $(call micro_c.add_flags,postproc_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call micro_c.add_src_lib,postproc_obj,libblm,$(microc_blocks_dir)/blm))

#
# Process Application definition
#
$(eval $(call micro_c.compile_with_rtl,proc_obj,process.c))
$(eval $(call micro_c.add_src_lib,proc_obj,flows))
$(eval $(call micro_c.add_std_lib,proc_obj,intrinsic))
$(eval $(call micro_c.add_fw_libs,proc_obj,nfp std pkt))
$(eval $(call micro_c.add_flags,proc_obj,-Qlmpt_reserve))

#
# DMA-ATX engine definition
#
$(eval $(call micro_c.compile_with_rtl,dma_atx_obj,dma_atx.c))
$(eval $(call micro_c.add_fw_libs,dma_atx_obj,nfp std pkt))

#
# DMA-ARX engine definition
#
$(eval $(call micro_c.compile_with_rtl,dma_arx_obj,dma_arx.c))
$(eval $(call micro_c.add_fw_libs,dma_arx_obj,nfp std pkt))

#
# DMA-TX engine definition
#
$(eval $(call micro_c.compile_with_rtl,dma_tx_obj,dma_tx.c))
$(eval $(call micro_c.add_fw_libs,dma_tx_obj,nfp std pkt))
$(eval $(call micro_c.add_flags,dma_tx_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call micro_c.add_src_lib,dma_tx_obj,libblm,$(microc_blocks_dir)/blm))

#
# DMA-RX engine definition
#
$(eval $(call micro_c.compile_with_rtl,dma_rx_obj,dma_rx.c))
$(eval $(call micro_c.add_fw_libs,dma_rx_obj,nfp std pkt))
$(eval $(call micro_c.add_flags,dma_rx_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call micro_c.add_src_lib,dma_rx_obj,libblm,$(microc_blocks_dir)/blm))

#
# DMA-RXTX engine definition
#
$(eval $(call micro_c.compile_with_rtl,dma_rxtx_obj,dma_rxtx.c))
$(eval $(call micro_c.add_src_lib,dma_rxtx_obj,msix))
$(eval $(call micro_c.add_fw_libs,dma_rxtx_obj,nfp std pkt))
$(eval $(call micro_c.add_flags,dma_rxtx_obj,-I$(microc_blocks_dir)/blm))
$(eval $(call micro_c.add_src_lib,dma_rxtx_obj,libblm,$(microc_blocks_dir)/blm))

#
# QM Application definition
#
$(eval $(call micro_c.compile_with_rtl,qm_obj,qman.c))
$(eval $(call micro_c.add_fw_libs,qm_obj,nfp std))

#
# Fastpath firmware definition
#
$(eval $(call fw.add_obj,fastpath,blm_obj,i48.me0))
$(eval $(call fw.add_obj,fastpath,gro_obj,i48.me1))
$(eval $(call fw.add_obj,fastpath,preproc_obj,i32.me0 i32.me1 i32.me2 i32.me3 i33.me0 i33.me1 i33.me2 i33.me3 i34.me0 i34.me1 i34.me2 i34.me3 i35.me0 i35.me1 i35.me2 i35.me3))
$(eval $(call fw.add_obj,fastpath,proc_obj,i32.me11 i33.me11 i34.me11 i35.me11))
$(eval $(call fw.add_obj,fastpath,postproc_obj,i32.me8 i32.me7 i32.me6 i32.me5 i33.me8 i33.me7 i33.me6 i33.me5 i34.me8 i34.me7 i34.me6 i34.me5 i35.me8 i35.me7 i35.me6 i35.me5))
$(eval $(call fw.add_obj,fastpath,qm_obj,i36.me7))
$(eval $(call fw.add_obj,fastpath,atx_obj,i36.me9))
$(eval $(call fw.add_obj,fastpath,arx_obj,i36.me10))
$(eval $(call fw.add_obj,fastpath,dma_atx_obj,i36.me0))
$(eval $(call fw.add_obj,fastpath,dma_arx_obj,i36.me1 i36.me2))
$(eval $(call fw.add_obj,fastpath,dma_tx_obj,i36.me3 i36.me4))
$(eval $(call fw.add_obj,fastpath,dma_rx_obj,i36.me5 i36.me6))
$(eval $(call fw.add_obj,fastpath,dma_rxtx_obj,i36.me8))
$(eval $(call fw.add_obj,fastpath,sp_obj,i36.me11))
$(eval $(call fw.add_init_obj,fastpath,fw_init_obj))
$(eval $(call fw.link_with_rtsyms,fastpath))
$(eval $(call fw.add_ppc,fastpath,i8,$(PICO_CODE)))

#
# Debug
#
-include Makefile.debug

.NOTPARALLEL:
