#!/bin/sh
#insert driver module
insmod btn_drv.ko

#make device node
mknod /dev/btn_drv c 242 0 

#wait 1 seconds
sleep 1

#run btn_tst
./btn_tst
