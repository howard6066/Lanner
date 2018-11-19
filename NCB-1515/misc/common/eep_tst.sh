#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

ECHO "EEPROM TEST Check...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "-----------------------------------------------"
echo "NCB-1515A V0.1@SN:20180423-123456789#" > N1515MEP.TXT

#absolute path in USB
TEST_PATH=/Test

#insert driver module
#insmod $TEST_PATH/eep_drv.ko

#make device node
#mknod /dev/eep_drv c 246 0


       buf1=`/Test/N1515MEP w`
       buf2=`/Test/N1515MEP r`
       ECHO "$buf1"
       ECHO "-----------------------------------------------"
       ECHO "$buf2"
while [ 1 ]; do



    echo -e "\e[0;36mIs this result OK? or press 's' to program the data(y/n/s)\e[m"
    read -n 1 x
    echo ""
    if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
        ECHO_SUMMARY "R & W eeprom CHECK PASS"
        exit 0
    elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
        ECHO_SUMMARY "R & W eeprom CHECK FAIL"
        exit 1
    elif [ "$x" == "s" ] || [ "$x" == "S" ]; then
        while [ 1 ];
        do
            echo -e "\e[0;36mInput Serial Number: \e[m\c"
            read sNum
	    break
	done
	echo "NCB-1515A V0.1@""$sNum""#" > N1515MEP.TXT
	/Test/N1515MEP w
	ECHO "-----------------------------------------------"
	/Test/N1515MEP r
	ECHO ""
	rm -rf N1515MEP.TXT
       
    fi
done
