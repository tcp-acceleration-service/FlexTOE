# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

SUBDIRS = tas sockets

.PHONY: subdirs $(SUBDIRS) all clean

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
		$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
