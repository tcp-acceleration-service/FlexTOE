#!/bin/bash

# SPDX-License-Identifier: BSD-3-Clause

set -e

# Load nfp module with params
echo " - Reload nfp..."
rmmod nfp || true
modprobe nfp nfp_dev_cpp=1 nfp_pf_netdev=0 || exit 1
echo "done"

# Load igb_uio DPDK-capable driver
if lsmod | grep "igb_uio" &> /dev/null ; then
  echo "igb_uio already loaded!"
else

  if modprobe "igb_uio" &> /dev/null ; then
    echo "done"
  else
    if ! [ -f dpdk-kmods/linux/igb_uio/igb_uio.ko ]; then
      echo " - Fetch..."
      git clone https://dpdk.org/git/dpdk-kmods

      echo " - Build..."
      cd dpdk-kmods/linux/igb_uio
      make || exit 1
    else
      cd dpdk-kmods/linux/igb_uio
    fi

    echo " - Load..."
    modprobe uio || exit 1
    insmod igb_uio.ko

    echo "done"
  fi
fi
