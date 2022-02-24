# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

DIR := $(shell pwd)
LIBDIR := $(DIR)/../..

CFLAGS := -I$(DIR) \
			-I$(DIR)/include \
			-I$(DIR)/../../include \
			-I$(DIR)/../..

SRCS-LIBS += connect.c \
				sp.c \
				conn.c \
				init.c

OBJS-LIBS := $(SRCS-LIBS:.c=.o)
DEPS-LIBS := $(SRCS-LIBS:.c=.d)

CFLAGS += -g3 -O3 -Wall -MD -MP -pthread -shared -fPIC
LDLIBS := -lm -pthread -lrt
STATICLIBS := $(LIBDIR)/util/libutil.a

LIB := libflextoe.so

all: $(LIB)

$(LIB): $(OBJS-LIBS)
	$(CC) -Wl,--whole-archive $(STATICLIBS) -Wl,--no-whole-archive -shared -fPIC $(LDLIBS) -o $@ $^

clean:
	rm -vf $(OBJS-LIBS) $(DEPS-LIBS) $(LIB)

-include $(DEPS-LIBS)

.PHONY: all clean

