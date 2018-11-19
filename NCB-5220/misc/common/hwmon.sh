#!/bin/bash

. /usr/bin/common_log.sh
ALLOW_BURN_TEST

GET_PACKAGE_PARAMETER "hwmon"
GET_SPECIFIC_SUBPARAMETER "verbose"
if [ ! -z "$SUBPARAMETER" ]; then
    VERBOSE="$SUBPARAMETER"
else
    VERBOSE="no"
fi

function RUN_TEST(){
    sensors="`/Test/HWM`"
    ECHO "$sensors"
    alarm=`echo "$sensors" | grep -i "ALARM"`
    ECHO "-----------------------------------------------"
}

while [ 1 ]
do
    ECHO "check HW Monitor of NCB-5220...`date "+%Y/%m/%d %H:%M:%S"`"
    ECHO "Version 1.1"
    ECHO "-----------------------------------------------"
    ECHO "[Parameter] verbose=$VERBOSE"

    RUN_TEST

    if [ -z "$alarm" ];then
        if [ -f $BURNIN_FLAG ]; then
            ECHO_SUMMARY "HWMON_NCB-5220 Check PASS"
            if [ "$BURNIN_MODE" != "BURNIN TEST v2.0" ]; then
                exit 0
            fi
        else
            while [ 1 ]; do
                echo -e "\e[0;36mIs this result OK? or press 'r' to re-try (y/n/r)\e[m"
                read -n 1 x
                echo ""
                if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
                    ECHO_SUMMARY "HWMON_NCB-5220 Check PASS"
                    exit 0
                elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
                    ECHO_SUMMARY "HWMON_NCB-5220 FAIL"
                    exit 1
                elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
                    echo "" > $LOG
                    sync
                    break
                fi
            done
        fi
    else
        ECHO_SUMMARY "HWMON_NCB-5220 Check FAIL"
        if [ "$BURNIN_MODE" == "BURNIN TEST v2.0" ]; then
            buf=`echo $0 | cut -d "/" -f 3 | cut -d "." -f 1`
            if [ "$VERBOSE" == "yes" ]; then
                cat /tmp/daemon/${buf}.cap >> $LOG
            else
                cp -f /tmp/daemon/${buf}.cap $LOG
            fi
            sync
            cp "/tmp/daemon/${buf}.cap" "/tmp/daemon/${buf}.fail"
        fi
        exit 1
    fi

    buf=`echo $0 | cut -d "/" -f 3 | cut -d "." -f 1`
    if [ -f "$BURNIN_ENDING" ]; then
        if [ "$VERBOSE" == "yes" ]; then
            cat /tmp/daemon/${buf}.cap >> $LOG
        else
            cp -f /tmp/daemon/${buf}.cap $LOG
        fi
        sync
        cp "/tmp/daemon/${buf}.cap" "/tmp/daemon/${buf}.pass"
        exit 0
    fi
    if [ "$VERBOSE" == "yes" ]; then
        cat /tmp/daemon/${buf}.cap >> $LOG
    fi
    if [ -f $BURNIN_FLAG ]; then
        echo "" > "/tmp/daemon/${buf}.cap"
        sleep 180
    fi
done

