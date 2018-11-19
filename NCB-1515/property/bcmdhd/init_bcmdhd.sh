#!/bin/bash
. /usr/bin/common_log.sh
SKIP_BURNIN_TEST
cd /Test
tar -xvf firmware1.tgz
cp -a /Test/firmware1 /lib/
insmod /Test/cfg80211.ko
insmod /Test/bcmdhd.ko firmware_path=/lib/firmware1/fw_bcm4356a2_pcie_ag.bin nvram_path=/lib/firmware1/nvram_ap12356.txt
