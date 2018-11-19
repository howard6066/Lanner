#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

rmmod ath10k_pci   #remove ath10k driver
rmmod ath10k_core
rmmod ath

/Test/init_bcmdhd.sh 

ETHACT="wlan0"

ESSID="fon1"

KEY="no"
ESKEY="86926060lanne"

SET_CHANNEL="no"
CHANNEL=7

GET_PACKAGE_PARAMETER "wifi"
GET_SPECIFIC_SUBPARAMETER "connect_timeout"
if [ ! -z "$SUBPARAMETER" ]; then
    declare -i CONNECT_TIMEOUT=$SUBPARAMETER
else
    declare -i CONNECT_TIMEOUT=5
fi
GET_SPECIFIC_SUBPARAMETER "dhcp_timeout"
if [ ! -z "$SUBPARAMETER" ]; then
    declare -i DHCP_TIMEOUT=$SUBPARAMETER
else
    declare -i DHCP_TIMEOUT=5
fi
GET_SPECIFIC_SUBPARAMETER "ping_number"
if [ ! -z "$SUBPARAMETER" ]; then
    declare -i PING_NUMBER=$SUBPARAMETER
else
    declare -i PING_NUMBER=5
fi


##Set the number of times
##declare -i CONNECT_TIMEOUT=5
  
##Set the time    
##declare -i DHCP_TIMEOUT=5

##Set the number of times
##declare -i PING_NUMBER=5


/sbin/ifconfig $ETHACT up
ECHO "WIFI CARD TEST ...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "[Parameter] connect_timeout=$CONNECT_TIMEOUT, dhcp_timeout=$DHCP_TIMEOUT, ping_number=$PING_NUMBER"

ECHO "------------------------SCAN WIFI AP-----------------------" 

scan_wifi_ap=`iwlist $ETHACT scan | grep -A 6 "$ESSID"`

ECHO "$scan_wifi_ap"
if [ -z "$scan_wifi_ap" ]; then
    	ECHO "No such WIFI ap" 
    	ECHO_SUMMARY "Wifi card test FAIL"
    	ifconfig $ETHACT down
	exit 1
fi
ECHO "-----------------------------------------------------------"

while [ 1 ] ; do
	declare -i COUNT=1
	buf=`iwlist $ETHACT scan | grep "$ESSID"`
	EPID=`echo $buf | cut -d":" -f2 | sed 's/"//g'`

	if [ "$ESSID" == "$EPID" ]; then
		if [ "$KEY" == "yes" ]; then
			iwconfig $ETHACT essid "$ESSID" key s:${ESKEY}
			sleep 1
		else
			iwconfig $ETHACT essid "$ESSID"
			sleep 1
		fi

		if [ "$SET_CHANNEL" == "yes" ]; then
			iwconfig $ETHACT channel $CHANNEL
			sleep 1
		fi
	fi

	alarm=`iwconfig $ETHACT | grep "Quality" | wc -l`
	if [ $alarm -gt 0 ] ; then
   		echo " connect successful. "
   		break
	else
		if [ $COUNT -le $CONNECT_TIMEOUT ]; then
   			echo "-----------------------------------------------"
   			echo " retry connect wireless AP...."
   			echo "-----------------------------------------------"
   			echo "-Linit the number of times = $CONNECT_TIMEOUT -"
   			echo "-The current number of times = $COUNT -"
   			echo "-----------------------------------------------"
   			sleep 1
			COUNT=`expr $COUNT + 1 `
		else 
			ECHO "FAIL!! Can't conect Wireless AP!!"
		        ECHO_SUMMARY "FAIL!! Can't connect wireless AP!!"
		    	ifconfig $ETHACT down
			exit 1
	
		fi
	fi
done


/sbin/ifconfig $ETHACT up
echo -e "\033[34m connect WiFi-AP , search dhcp server, wait....\033[0m"
n=1
declare -i j=1

while [ $n == 1 ] ; do
	udhcpc -i $ETHACT -t 3 -R -n | tail -n 1 | cut -d" " -f3 > /tmp/cliip.txt
      	grep "fail" /tmp/cliip.txt >/dev/null
      	if [ $? -gt 0 ] ; then
        	IPADDR=`head -n 1 /tmp/cliip.txt`
         	echo " configuration $ETHACT ip : $IPADDR"
        	n=0
      	else
         	echo " search dhcp server, wait...."
         	echo "   or press 'c' to exit.  "
         	read -t 5 -n 1 xn
         	if [ "$xn" == "c" ] ; then
            		n=0
			ECHO "FAIL!! Cancel!! Can't get DHCP!!"
		        ECHO_SUMMARY "FAIL!! Cancel!! Can't get DHCP!!"
		    	ifconfig $ETHACT down
            		exit 1
         	fi
      	fi
	j=`expr $j + 1 `
      	if [ $j -gt $DHCP_TIMEOUT ] ; then
        	n=0
		ECHO "FAIL!! Timeout!! Can't get DHCP!!"
	        ECHO_SUMMARY "FAIL!! Timeout!! Can't get DHCP!!"
	    	ifconfig $ETHACT down
         	exit 1
      	fi
done

IPADDR=`head -n 1 /tmp/cliip.txt`
sleep 2

ifconfig $ETHACT $IPADDR netmask 255.255.255.0 up
SVRIP=`echo $IPADDR | awk -F"." '{print $1"."$2"."$3}'`
echo "$SVRIP" >/tmp/testip.txt
SVRIP="${SVRIP}.1"
#route add default gw ${SVRIP} dev $ETHACT

echo "nameserver 0.0.0.0" > /etc/resolv.conf

sleep 2

ECHO ""
ECHO "------------------------SHOW Signal level------------------" 
wifi_lan_info=`iwconfig $ETHACT`
ECHO "$wifi_lan_info"

signal_level=`iwconfig $ETHACT | grep "Signal level" | cut -d " " -f 15 | cut -d "-" -f 2`
if [ "$signal_level" == "0" ]; then
  	ECHO "Signal level error" 
    	ECHO_SUMMARY "Wifi card test FAIL"
    	ifconfig $ETHACT down
    	exit 1
fi
ECHO "-----------------------------------------------------------"
ECHO ""

ECHO "------------------------Ping test--------------------------" 
#ping_test_info=`ping 8.8.8.8 -c 3`    #ping google server
ping_test_info=`ping $SVRIP -c $PING_NUMBER`    #ping google server 
ECHO "$ping_test_info"
ping_test=`echo "$ping_test_info" | grep "0% packet loss,"`
ECHO "-----------------------------------------------------------"
ECHO ""

if [ -z "$ping_test" ]; then
    	ECHO "Ping Test fail"
    	ECHO_SUMMARY "Wifi card test FAIL"
	ifconfig $ETHACT down
    	exit 1
fi

ECHO_SUMMARY "Wifi card test PASS"
ifconfig $ETHACT down
exit 0


