#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

ECHO "check StatusLCM...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/lcm_drv.ko

#make device node
mknod /dev/lcm_drv c 245 0 

#wait 1 seconds
sleep 1

#run lcm_tst
$TEST_PATH/lcm_tst -show
$TEST_PATH/lcm_tst -btn
$TEST_PATH/lcm_tst -clr

i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] StatusLCM has been confirmed by user"
        ECHO_SUMMARY "StatusLCM check PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "StatusLCM check FAIL"
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        #run lcm_tst
        $TEST_PATH/lcm_tst -show
	echo ""
    fi
done

#remove driver
rmmod lcm_drv

#wait 1 seconds
sleep 1

