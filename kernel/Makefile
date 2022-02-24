# SPDX-License-Identifier: BSD 3-Clause License
# Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin

KSRC ?= /lib/modules/`uname -r`/build

all:
	make -C $(KSRC)/ M=$$PWD

clean:
	make -C $(KSRC)/ M=$$PWD clean
	rm -rf .tmp_versions Module.symvers *.o.ur-safe
