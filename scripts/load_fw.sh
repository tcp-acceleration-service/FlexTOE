#!/bin/bash

# Copyright (c) 2016 Netronome Systems, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause

set -e

NFP_SDK_DIR=${NFP_SDK_DIR:-/opt/netronome}
export PATH=${NFP_SDK_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${NFP_SDK_DIR}/lib:$LD_LIBRARY_PATH

OUTPUT=/dev/null
FIRMWARE=firmware/fastpath.fw
DEVICE=$1

verify() {
    test -d "/sys/module/igb_uio" || (echo "igb_uio not loaded. run setup_mods.sh"; exit 1)
    test -d "/sys/module/nfp" || (echo "nfp not loaded. run setup_mods.sh"; exit 1)
    test "$(cat /sys/module/nfp/parameters/nfp_dev_cpp)" == "1" || (echo "incorrect nfp module params. run setup_mods.sh"; exit 1)
    test "$(cat /sys/module/nfp/parameters/nfp_pf_netdev)" == "N" || (echo "incorrect nfp module params. run setup_mods.sh"; exit 1)

    test -f $FIRMWARE || (echo "$FIRMWARE not found"; exit 1)
}

verify
./devbind.py -u $DEVICE

echo "Starting FW:"
./devbind.py -b nfp $DEVICE

echo -n " - Unload FW..."
nfp-nffw unload
echo "done"

echo -n " - Reset Islands..."
nfp-power nbi=reset nbi=on
nfp-power fpc=reset fpc=on
nfp-power imu=reset imu=on
nfp-power ila=reset ila=on
nfp-power crp=reset crp=on
nfp-power pci_0.core=reset pci_0.core=on
nfp-power pci_0.meg0=reset pci_0.meg0=on
nfp-power pci_0.meg1=reset pci_0.meg1=on
sleep 5
echo "done"

echo -n " - Init MAC..."
nfp-nsp -M &> $OUTPUT || exit 1
echo "done"

echo -n " - Reset NSP..."
nfp-nsp -R &> $OUTPUT || exit 1
echo "done"

# temporary fix
echo -n " - Reset PCIe..."
nfp-reg pcie:i4.PcieInternalTargets.DMAController.DMAQStatToPCI0.HighQueueStop=0
nfp-reg pcie:i4.PcieInternalTargets.DMAController.DMAQStatToPCI0.MediumQueueStop=0
nfp-reg pcie:i4.PcieInternalTargets.DMAController.DMAQStatToPCI0.LowQueueStop=0
nfp-reg pcie:i4.PcieInternalTargets.DMAController.DMAQStatToPCI0.DmaError=0
echo "done"

echo -n " - Load FW..."
nfp-nffw $FIRMWARE load --no-start &> $OUTPUT || exit 1
echo "done"

echo -n " - Enable RX..."
nfp -m mac -e set port rx 0 0 enable &> $OUTPUT || exit 1
nfp -m mac -e set port rx 0 4 enable &> $OUTPUT || exit 1
echo "done"

echo -n " - Set EgressPrependEnable..."
nfp-reg xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacCsr.EgCmdPrependEn0Lo.EgCmdPrependEn=0xf000f
echo "done"

sleep 1

echo -n " - Start MEs..."
nfp-nffw start || exit 1
echo "done"

./devbind.py -b igb_uio $DEVICE

echo ""
exit 0

