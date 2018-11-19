#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

ECHO "check StatusLED...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/sled_drv.ko

#make device node
mknod /dev/sled_drv c 240 0 

#wait 1 seconds
sleep 1

#run sled_tst
$TEST_PATH/sled_tst

i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] StatusLED has been confirmed by user"
        ECHO_SUMMARY "StatusLED check PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "StatusLED check FAIL"
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        #run sled_tst
        $TEST_PATH/sled_tst
	echo ""
    fi
done

#remove driver
rmmod sled_drv

#wait 1 seconds
sleep 1

