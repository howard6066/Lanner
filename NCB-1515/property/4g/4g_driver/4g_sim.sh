#! /bin/bash 

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

CONF_FILE="/conf/3g_sim.conf"


ECHO "Detect 3G sim card ...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO_CONF "Version 1.0"
ECHO_CONF "-----------------------------------------------"

iccid=`qmicli -d /dev/cdc-wdm0 --dms-uim-get-iccid`

ECHO_CONF "$iccid"

if [ -f "$CONF_FILE" ]; then
    buf=`cat $CONF_FILE`
    if [ "$buf" == "$CONF" ]; then
        ECHO_SUMMARY "Detect 3G sim card PASS"
        exit 0
    else
        ECHO "The Default is $buf"
        ECHO_SUMMARY "Detect 3G sim card FAIL"
        exit 1
    fi
else
    while [ 1 ]; do
        echo -e "\e[0;36mIs this conf OK? or press 's' to save this to defaults (y/n/s)\e[m"
        read -n 1 x
        echo ""
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            ECHO_SUMMARY "Detect 3G sim card PASS"
            exit 0
        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "Detect 3G sim card Fail"
            exit 1
        elif [ "$x" == "s" ] || [ "$x" == "S" ]; then
            echo "$CONF" > $CONF_FILE
            sync
        fi
    done
fi


