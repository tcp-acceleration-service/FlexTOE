# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

SUBDIRS_CORE = util kernel lib user firmware
SUBDIRS_NOFIRMWARE = util kernel lib user
SUBDIRS = $(SUBDIRS_CORE)
.PHONY: $(SUBDIRS) all core nofirmware clean

core: $(SUBDIRS_CORE)
nofirmware: $(SUBDIRS_NOFIRMWARE)
all: $(SUBDIRS)

$(SUBDIRS):
		$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
