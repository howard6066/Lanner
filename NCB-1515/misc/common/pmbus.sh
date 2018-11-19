#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

CONF_FILE="/conf/pmbus.conf"

ECHO "PMBUS Check...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO_CONF "Version 2.0"
ECHO_CONF "-----------------------------------------------"

buf=`/Test/pms`
ECHO "$buf"
alarm=`echo "$buf" | grep -e "Alarm" -e "ERR"`
if [ ! -z "$alarm" ]; then
    ECHO_SUMMARY "PMBUS CHECK FAIL"
    exit 1
fi

buf2=`echo "$buf" | grep -v -e "Temperature 1" -e "Temperature 2" -e "Fan:"`
CONF=`echo -e "$CONF\n$buf2"`

if [ -f "$CONF_FILE" ]; then
    buf=`cat "$CONF_FILE"`
    if [ "$buf" != "$CONF" ]; then
        ECHO "The default is $buf"
        ECHO_SUMMARY "PMBUS CHECK FAIL"
        exit 1
    fi
else
    while [ 1 ]; do
        echo -e "\e[0;36mIs this result OK? or press 's' to save this to defaults (y/n/s)\e[m"
        read -n 1 x
        echo ""
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            ECHO "[NOTE] module information has been confirmed by user"
            break
        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "PMBUS CHECK FAIL"
            exit 1
        elif [ "$x" == "s" ] || [ "$x" == "S" ]; then
            echo "$CONF" > $CONF_FILE
            sync
        fi
    done
fi


ECHO_SUMMARY "PMBUS CHECK PASS"
exit 0
