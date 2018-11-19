#!/bin/bash
. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

#need to install according to the order driver
insmod /Test/cdc-wdm.ko
insmod /Test/usbnet.ko
insmod /Test/usbserial.ko
insmod /Test/usb_wwan.ko
insmod /Test/qcserial.ko
insmod /Test/GobiSerial.ko

mknod /dev/ttyUSB0 c 188 0
mknod /dev/ttyUSB1 c 188 1
mknod /dev/ttyUSB2 c 188 2

