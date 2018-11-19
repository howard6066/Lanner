#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

function RUN_TEST(){
    $TEST_PATH/wd_tst -swt 10
    for i in 10 9 8 7 6 5 4 3 2 1
    do
        echo -e ".$i\c"
        sleep 1
	if
	   [ $i == 5 ]; then
		$TEST_PATH/wd_tst -stop
		break
	fi
    done
    echo "Waiting the result"
    for i in 7 6 5 4 3 2 1
    do
        echo -e ".$i\c"
        sleep 1
    done

    echo ""
}
ECHO "check Watchdog...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"


#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/wd_drv.ko

#make device node
mknod /dev/wd_drv c 241 0

#wait 1 seconds
sleep 1

#run watchdog
RUN_TEST

i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] WATCHDOG has been confirmed by user"
        ECHO_SUMMARY "WATCHDOG check PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "WATCHDOG check FAIL"
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        RUN_TEST
    fi
done
