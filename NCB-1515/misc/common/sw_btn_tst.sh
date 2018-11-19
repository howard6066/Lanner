#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

function RUN_TEST(){
    daemon.sh "$TEST_PATH/btn_tst" pid_btn &

    for i in 10 9 8 7 6 5 4 3 2 1
    do
        echo -e ".$i\c"
        sleep 1
        if [ ! -f "/tmp/pid_btn" ]; then
            break
        fi
    done

    if [ -f "/tmp/pid_btn" ]; then
        echo -e "\e[0;31mTIMEOUT\e[m"
        pid=`cat /tmp/pid_btn`
        kill $pid
        pkill btn_tst
        result=1
    else
        result=0
    fi
}

ECHO "check SW_Button...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/btn_drv.ko

#make device node
mknod /dev/btn_drv c 242 0 

#wait 1 seconds
sleep 1

#run sled_tst
RUN_TEST

if [ "$result" == 0 ]; then
    ECHO_SUMMARY "SW_BUTTON check PASS"
else
    while [ 1 ];
    do
        echo -e "\e[0;36mDo you want to try again (y/n)\e[m"
        read -n 1 x
        echo ""
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            RUN_TEST
            if [ "$result" == 0 ]; then
                ECHO_SUMMARY "SW_BUTTON check PASS"
                break
            fi
        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "SW_BUTTON check FAIL"
            break
        fi
    done
fi

#remove driver
rmmod btn_drv

#wait 1 seconds
sleep 1

exit $result
