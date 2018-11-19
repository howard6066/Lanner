#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

function RUN_TEST(){
    #run pwr_tst
    buf=`$TEST_PATH/pwr_tst`
    ECHO "$buf"
    ECHO "-----------------------------------------------"
}

#absolute path in USB
TEST_PATH=/Test

ECHO "PWR Check...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#insert driver module
insmod $TEST_PATH/pwr_drv.ko

#make device node
mknod /dev/pwr_drv c 243 0 

#wait 1 seconds
sleep 1

RUN_TEST

i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] Redundant Power has been confirmed by user"
        ECHO_SUMMARY "Redundant Power Status check PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "Redundant Power Status check FAIL"
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        RUN_TEST
	echo ""
    fi
done

#remove driver
rmmod pwr_drv

