# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

DIR := $(shell pwd)

CFLAGS := -I$(DIR)\
		-I$(DIR)/../include

SRCS-LIBS += ringbuffer.c \
		rng.c \
		utils.c \
		timeout.c \
		shm.c

OBJS-LIBS := $(SRCS-LIBS:.c=.o)
DEPS-LIBS := $(SRCS-LIBS:.c=.d)

CFLAGS += -g3 -O3 -Wall -MD -MP -pthread -fPIC

LIB := libutil.a

all: $(LIB)

$(LIB): $(OBJS-LIBS)
	$(AR) rcu $@ $+
	ranlib $@

clean:
	rm -vf $(OBJS-LIBS) $(DEPS-LIBS) $(LIB)

-include $(DEPS-LIBS)

.PHONY: all clean
