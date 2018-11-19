#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/lcm_drv.ko

#make device node
mknod /dev/lcm_drv c 245 0 

#wait 1 seconds
sleep 1

echo ""
ECHO "Check StatusLCM...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#run lcm_tst -show
lcm_show="`$TEST_PATH/lcm_tst -show`"
ECHO "$lcm_show"

i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] StatusLCM has been confirmed by user"
        ECHO "LCM printf char check PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "LCM printf char check FAIL"
        exit 1
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        #run lcm_tst
        $TEST_PATH/lcm_tst -show
	echo ""
    fi
done
#run lcm_tst -show

 
ECHO ""
ECHO "Check LCM clear Status...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#run lcm_tst -clr
lcm_clr="`$TEST_PATH/lcm_tst -clr`"
ECHO "$lcm_clr"

j=1
while [ $j == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] StatusLCM has been confirmed by user"
        ECHO "LCM clear check PASS"
        j=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "LCM clear check FAIL"
        exit 1
        j=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        #run lcm_tst
        $TEST_PATH/lcm_tst -clr
        echo ""
    fi
done
#run lcm_tst -clr


ECHO ""
ECHO "Check LCM button Status...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

lcm_btn1="`$TEST_PATH/lcm_tst -btn`"
ECHO "$lcm_btn1"
lcm_btn2="`$TEST_PATH/lcm_tst -btn`"
ECHO "$lcm_btn2"
lcm_btn3="`$TEST_PATH/lcm_tst -btn`"
ECHO "$lcm_btn3"
lcm_btn4="`$TEST_PATH/lcm_tst -btn`"
ECHO "$lcm_btn4"

k=1
while [ $k == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] StatusLCM has been confirmed by user"
        ECHO "LCM button check PASS"
        k=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "LCM button check FAIL"
        exit 1
        k=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        #run lcm_tst
        $TEST_PATH/lcm_tst -btn
        $TEST_PATH/lcm_tst -btn
        $TEST_PATH/lcm_tst -btn
        $TEST_PATH/lcm_tst -btn
        echo ""
    fi
done

ECHO ""
ECHO_SUMMARY "LCM CHECK PASS"

#remove driver
rmmod lcm_drv

#wait 1 seconds
sleep 1

