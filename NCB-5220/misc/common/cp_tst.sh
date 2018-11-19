#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST
 

ECHO "CASE OPEN Check...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"

#absolute path in USB
TEST_PATH=/Test

#insert driver module
insmod $TEST_PATH/cp_drv.ko

#make device node
mknod /dev/cp_drv c 243 0 

#wait 1 seconds
sleep 1

#run cp_tst

while [ 1 ]; do
       buf=`$TEST_PATH/cp_tst h a`
       ECHO "$buf"
       pass=`echo "$buf" | grep opened`
       if [ ! -z "$pass" ]; then
            break
       fi
       ECHO "Please open case open switch !!"
       sleep 2
done


ECHO "Test clear the case open startus and case open switch"
ECHO "Please press case open switch 10 seconds"


while [ 1 ]; do
       buf=`$TEST_PATH/cp_tst f k`
       ECHO "$buf"
       pass=`echo "$buf" | grep close`
       if [ ! -z "$pass" ]; then
            break
       fi
       ECHO "Please press case open switch !!"
       sleep 2
done
  
buf=`$TEST_PATH/cp_tst f k`
ECHO "$buf"


i=1
while [ $i == 1 ]; do
    echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO "[NOTE] This function has been confirmed by tester\n"
        ECHO_SUMMARY "CASE OPEN CHECK PASS"
        i=0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "CASE OPEN CHECK FAIL"
        i=0
    elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
        buf=`$TEST_PATH/cp_tst h k`
        ECHO "$buf"
        ECHO "Test clear the case open startus and case open switch"
        ECHO "Please press case open switch 10 seconds"
        
        buf=`$TEST_PATH/cp_tst f k`
        ECHO "$buf"
        
	echo ""
    elif [ "$x" == "c" ] || [ "$x" == "C" ]; then
        #run sled_tst
        buf=`$TEST_PATH/cp_tst y`
        ECHO "$buf"
	echo ""
    fi
done

#remove driver
rmmod cp_drv

#wait 1 seconds
sleep 1


