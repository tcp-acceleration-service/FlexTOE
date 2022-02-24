# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

DIR := $(shell pwd)
LIBDIR := $(DIR)/../..

CFLAGS := -I$(DIR) \
			-I$(DIR)/include \
			-I$(DIR)/../../include \
			-I$(DIR)/../..\
      -I$(DIR)/../tas/include

SRCS-LIBS += control.c  \
            transfer.c  \
            context.c   \
            manage_fd.c \
            epoll.c     \
            poll.c      \
            libc.c

SRCS-TAS += ../tas/connect.c \
				../tas/sp.c \
				../tas/conn.c \
				../tas/init.c
OBJS-TAS := $(SRCS-TAS:.c=.o)
SRCS-LIBS-INTERPOSE := interpose.c

OBJS-LIBS := $(SRCS-LIBS:.c=.o)
DEPS-LIBS := $(SRCS-LIBS:.c=.d)
OBJS-LIBS-INTERPOSE := $(SRCS-LIBS-INTERPOSE:.c=.o) $(OBJS-LIBS) $(OBJS-TAS)
DEPS-LIBS-INTERPOSE := $(SRCS-LIBS-INTERPOSE:.c=.d) $(DEPS-LIBS)

CFLAGS += -g3 -O3 -Wall -MD -MP -pthread -shared -fPIC
LDLIBS := -lm -pthread -lrt
STATICLIBS := $(LIBDIR)/util/libutil.a

LIB := libflextoe_sockets.so
LIB-INTERPOSE := libflextoe_interpose.so

all: $(LIB) $(LIB-INTERPOSE)

$(LIB): $(OBJS-LIBS)
	$(CC) -Wl,--whole-archive $(STATICLIBS) -Wl,--no-whole-archive -shared -fPIC $(LDLIBS) -o $@ $^ -ldl

$(LIB-INTERPOSE): $(OBJS-LIBS-INTERPOSE)
	$(CC) -Wl,--whole-archive $(STATICLIBS) -Wl,--no-whole-archive -shared -fPIC $(LDLIBS) -o $@ $^ -ldl

clean:
	rm -vf $(OBJS-LIBS) $(DEPS-LIBS) $(LIB) $(OBJS-LIBS-INTERPOSE) $(DEPS-LIBS-INTERPOSE) $(LIB-INTERPOSE)

-include $(DEPS-LIBS)
-include $(DEPS-LIBS-INTERPOSE)

.PHONY: all clean

