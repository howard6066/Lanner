#! /bin/bash 

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

CONF_FILE="/conf/sierra_sim.conf"

function return_ATI_value()
{
    cat /dev/ttyUSB2  > /tmp/ttyusb2.log 2>&1 &
    stty -F /dev/ttyUSB2 -echo
	echo -e "ATI\r\n" > /dev/ttyUSB2
	sleep 0.5
	pkill cat 2>  /dev/null 
	pkill cat 2>  /dev/null 
    ATI_info=`cat /tmp/ttyusb2.log | grep -iv AT | grep -v OK | grep .[[:print:]] | grep "$1" | head -n 8`
    cat /tmp/ttyusb2.log > /tmp/jim1
    ECHO "$ATI_info"
    rm /tmp/ttyusb2.log
}


function return_USIM1_value()
{
        cat /dev/ttyUSB2  > /tmp/ttyusb2.log 2>&1 &
	echo -e "AT!ENTERCND=\"A710\"\r\n" > /dev/ttyUSB2         #Enable access to password-protected commands
	sleep 0.5
	echo -e "AT!UIMS=0\r\n" > /dev/ttyUSB2                     #Active USIM1
	sleep 0.5
	echo -e "AT!UIMS?\r\n" > /dev/ttyUSB2                     #Request USIM setting (USIM1 or USIM2)
	sleep 2
	#echo -e "AT+CIMI\r\n" > /dev/ttyUSB2                      #Request international mobile subscriber
	#sleep 1
        echo -e "AT+ICCID\r\n" > /dev/ttyUSB2
        sleep 2
	pkill cat 2>  /dev/null 
	pkill cat 2>  /dev/null 
        USIM1_info=`cat /tmp/ttyusb2.log | grep -iv AT | grep -v OK | grep -v ERR | grep -iv T | grep .[[:print:]] | uniq`
        cat /tmp/ttyusb2.log > /tmp/jim2
        ECHO_CONF "$USIM1_info"
        rm /tmp/ttyusb2.log
}

function return_USIM2_value()
{
        cat /dev/ttyUSB2  > /tmp/ttyusb2.log 2>&1 &
	echo -e "AT!ENTERCND=\"A710\"\r\n" > /dev/ttyUSB2         #Enable access to password-protected commands
	sleep 0.5
	echo -e "AT!CUSTOM=\"UIM2ENABLE\",1\r\n" > /dev/ttyUSB2   #Enable USIM2 identity
	sleep 0.5
	echo -e "AT!ENTERCND=\"A710\"\r\n" > /dev/ttyUSB2         #Enable access to password-protected commands
	sleep 0.5
	echo -e "AT!UIMS=1\r\n" > /dev/ttyUSB2                     #Active USIM2
	sleep 0.5
	echo -e "AT!UIMS?\r\n" > /dev/ttyUSB2                     #Request USIM setting (USIM1 or USIM2)
	sleep 2
	#echo -e "AT+CIMI\r\n" > /dev/ttyUSB2                      #Request international mobile subscriber
        #sleep 1
        echo -e "AT+ICCID\r\n"> /dev/ttyUSB2                      #Request international mobile subscriber
        sleep 2
	pkill cat 2>  /dev/null 
	pkill cat 2>  /dev/null 
        USIM2_info=`cat /tmp/ttyusb2.log | grep -iv AT | grep -v OK | grep -v ERR | grep -iv T | grep .[[:print:]] | uniq`
        cat /tmp/ttyusb2.log > /tmp/jim3
        ECHO_CONF "$USIM2_info"
        rm /tmp/ttyusb2.log
}

ECHO "Detect Sierra Wireless EM7455 sim card ...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO_CONF "Version 1.0"
ECHO_CONF "-----------------------------------------------"
ECHO "--------------ATI INFO-------------------------"
ECHO "$ATI_INFO"
return_ATI_value
ECHO "-----------------------------------------------"

ECHO_CONF "-------------SIM1 INFO-------------------------"
return_USIM1_value
ECHO_CONF "-----------------------------------------------"

ECHO_CONF "-------------SIM2 INFO-------------------------"
return_USIM2_value
ECHO_CONF "-----------------------------------------------"

if [ -f "$CONF_FILE" ]; then
    buf=`cat $CONF_FILE`
    if [ "$buf" == "$CONF" ]; then
        ECHO_SUMMARY "Detect Sierra Wireless EM7455 sim card PASS"
        exit 0
    else
        ECHO "The Default is $buf"
        ECHO_SUMMARY "Detect Sierra Wireless EM7455 sim card FAIL"
        exit 1
    fi
else
    while [ 1 ]; do
        echo -e "\e[0;36mIs this conf OK? or press 's' to save this to defaults (y/n/s)\e[m"
        read -n 1 x
        echo ""
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            ECHO_SUMMARY "Detect Sierra Wireless EM7455 sim card PASS"
            exit 0
        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "Detect Sierra Wireless EM7455 sim card Fail"
            exit 1
        elif [ "$x" == "s" ] || [ "$x" == "S" ]; then
            echo "$CONF" > $CONF_FILE
            sync
        fi
    done
fi


