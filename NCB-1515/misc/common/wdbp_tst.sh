#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

function RUN_TEST(){
    daemon.sh "tail -n 0 -f $LOG" pid_wdbp &
    sleep 1
    echo "-----------------------------------" >> $LOG
    echo "Set runtime bypass enable on Pair 1" >> $LOG
#    echo "Press anykey to continue..." >> $LOG
    echo -e "\e[0;36mPress a key to continue\e[m" >> $LOG
    read -n 1
    sync

    #run watchdog bypass test
    $TEST_PATH/wd_tst --srbe 1 >> $LOG

    echo "Set runtime bypass disbale on Pair 1" >> $LOG
#    echo "Press anykey to continue..." >> $LOG
    echo -e "\e[0;36mPress a key to continue\e[m" >> $LOG
    read -n 1
    sync

    #run watchdog bypass test
    $TEST_PATH/wd_tst --srbd 1 >> $LOG
    echo "-----------------------------------" >> $LOG
    sync

    sleep 2
    pkill tail
}

ECHO "Watchdog Bypass Check...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"

CHECK_TEST_EQUIPMENT "WDBP check Test"

#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/wd_drv.ko

#make device node
mknod /dev/wd_drv c 241 0 

#wait 1 seconds
sleep 2

RUN_TEST

i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] WDBP has been confirmed by user"
        ECHO_SUMMARY "WDBP check PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "WDBP check FAIL"
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        RUN_TEST
    fi
done

#remove driver
rmmod wd_drv

#wait 1 seconds
sleep 1
