#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

 CONF_FILE="/conf/dio.conf"

#insert driver module
insmod /Test/dio_drv.ko

#make device node
mknod /dev/dio_drv c 244 0

#wait 1 seconds
sleep 1

dio=`/Test/dio_tst`
ECHO "DIO Check ...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO_CONF "Version 1.0"
ECHO_CONF "-----------------------------------------------"
ECHO_CONF "$dio"
ECHO_CONF "-----------------------------------------------"

#remove driver
rmmod dio_drv

if [ -f "$CONF_FILE" ]; then
    buf=`cat "$CONF_FILE"`
    if [ "$buf" == "$CONF" ]; then
        ECHO_SUMMARY "DIO Check PASS"
        exit 0
    else
        ECHO "The default is $buf"
        ECHO_SUMMARY "DIO Check FAIL"
        exit 1
    fi
else
    while [ 1 ]; do
        echo -e "\e[0;36mIs this version OK? or press 's' to save this to defaults (y/n/s)\e[m"
        read -n 1 x
        echo ""
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            ECHO "[NOTE] DIO Information has been confirmed by user"
            ECHO_SUMMARY "DIO Information check PASS"
            exit 0
        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "DIO Information check FAIL"
            exit 1
        elif [ "$x" == "s" ] || [ "$x" == "S" ]; then
            echo "$CONF" > $CONF_FILE
            sync
        fi
    done
fi

