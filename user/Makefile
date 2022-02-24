# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

DIR := $(shell pwd)
NFPCOREDIR := $(DIR)/driver
DRIVERDIR := $(DIR)/util
LIBDIR := $(DIR)/..

CFLAGS := -I$(DIR)\
			-I$(NFPCOREDIR)\
			-I$(DRIVERDIR)\
			-I$(DIR)/..\
			-I$(DIR)/../include

SRCS-MAIN := shm.c \
			config.c \
			nic.c \
			arp.c \
			cc.c \
			routing.c \
			tcp.c \
			appif_ctx.c \
			appif.c \
			packetmem.c \
			nicif.c \
			slowpath.c \
			flextoe.c

OBJS-MAIN := $(SRCS-MAIN:.c=.o)
DEPS-MAIN := $(SRCS-MAIN:.c=.d)

APP := flextoe.out

all: $(APP)

CFLAGS += -g3 -O3 -Wall -pthread -MD -MP
LDFLAGS := -L$(NFPCOREDIR) -L$(DRIVERDIR) -L$(LIBDIR)/util
LDLIBS := -lutil -ldriver -lm -pthread -lrt

LIBS_DIR := driver

$(LIBS_DIR):
	$(MAKE) -C $@

DEPS := $(DEPS-MAIN)
OBJS := $(OBJS-MAIN)

$(APP): $(LIBS_DIR) $(OBJS)
	$(CC) $(LDFLAGS) -o $(APP) $(OBJS) $(LDLIBS)

clean:
	rm -rf $(DEPS) $(OBJS) $(APP)
	for dir in $(LIBS_DIR); do \
		$(MAKE) -C $$dir clean; \
	done

-include $(DEPS-MAIN)

.PHONY: all clean $(LIBS_DIR)
