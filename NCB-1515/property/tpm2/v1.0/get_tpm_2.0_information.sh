#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

CONF_FILE="/conf/tpm_information.conf"


if [ ! -e "/dev/tpm0" ];then
    ECHO "No such tpm device /dev/tpm0 "
    ECHO_SUMMARY "TPM test FAIL, Need to bios options tpm enable or install tpm driver"
    exit 1

fi

ECHO "Get TPM 2.0 information test...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO_CONF "-----------------------------------------------"

get_tpm_version=`/Test/lSkCmdxx -getversion`

tpm_selftestfull=`/Test/lSkCmdxx -selftest`

#NTC_GetTpmStatus=`/Test/lSkCmdxx -gettpmstatus2`

TPM_GetTestResult=`/Test/lSkCmdxx -gettestresult`

alarm1=`echo "$get_tpm_version" | grep "Command Passed"`
alarm2=`echo "$tpm_selftestfull" | grep "Command Passed"`
#alarm3=`echo "$NTC_GetTpmStatus" | grep "Command Passed"`
alarm4=`echo "$TPM_GetTestResult" | grep "Command Passed"`

ECHO_CONF "GET TPM VERSION"
ECHO_CONF "============================"
ECHO_CONF "$get_tpm_version"

ECHO_CONF "TPM SELFTEST FULL"
ECHO_CONF "============================"
ECHO_CONF "$tpm_selftestfull"

ECHO_CONF ""
ECHO_CONF "TPM GetTestResult"
ECHO_CONF "============================"
ECHO_CONF "$TPM_GetTestResult"

ECHO_CONF "-----------------------------------------------"

if [ -f "$CONF_FILE" ]; then
    buf=`cat "$CONF_FILE"`
    if [ "$buf" == "$CONF" ]; then
        ECHO_SUMMARY "Get TPM information check PASS"
        exit 0
    else
        ECHO "The default is $buf"
        ECHO_SUMMARY "Get TPM information check FAIL"
        exit 1
    fi
else
    while [ 1 ]; do
        echo -e "\e[0;36mIs this conf OK? or press 's' to save this to defaults (y/n/s)\e[m"
        read -n 1 x
        echo ""
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            if [ -n "$alarm1" ] && [ -n "$alarm2" ]  && [ -n "$alarm4" ]; then
                 ECHO "[NOTE] TPM conf has been confirmed by user"
                 ECHO_SUMMARY "Get TPM information check PASS"
                 exit 0
            else
                 echo "[DENY] Fail detected..."
                 echo "GET TPM VERSION"
                 echo "============================"
                 echo "$get_tpm_version"

                 echo "TPM SELFTEST FULL"
                 echo "============================"
                 echo "$tpm_selftestfull"

                 echo ""
                 echo "TPM GetTestResult"
                 echo "============================"
                 echo "$TPM_GetTestResult"
                 echo "-----------------------------------------------"
            fi

        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "Get TPM information check FAIL"
            exit 1
        elif [ "$x" == "s" ] || [ "$x" == "S" ]; then
            echo "$CONF" > $CONF_FILE
            sync
        fi
    done
fi





