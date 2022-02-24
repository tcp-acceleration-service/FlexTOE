# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

DIR := $(shell pwd)

CFLAGS := -I$(DIR) -I$(DIR)/../../include

SRCS-NFPCORE += nfp_cppcore.c \
		nfp_cpp_pcie_ops.c \
		nfp_mutex.c \
		nfp_resource.c \
		nfp_crc.c \
		nfp_mip.c \
		nfp_nffw.c \
		nfp_hwinfo.c \
		nfp_nsp.c \
		nfp_nsp_cmds.c \
		nfp_nsp_eth.c \
		nfp_rtsym.c \
		pci.c \
		nfp_cpp_dev_ops.c \
		nfp_cpp_dev.c \
		driver.c

OBJS-NFPCORE := $(SRCS-NFPCORE:.c=.o)
DEPS-NFPCORE := $(SRCS-NFPCORE:.c=.d)

CFLAGS += -g3 -O3 -Wall -MD -MP -pthread

LIB := libdriver.a

all: $(LIB)

$(LIB): $(OBJS-NFPCORE)
	$(AR) rcu $@ $+
	ranlib $@

clean:
	rm -vf $(OBJS-NFPCORE) $(DEPS-NFPCORE) $(LIB)

-include $(DEPS-NFPCORE)

.PHONY: all clean
