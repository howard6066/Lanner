#/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

SLOT_LOCATION[0]="BOT 1"
SLOT_LOCATION[1]="BOT 2"
SLOT_LOCATION[2]="BOT 3"
SLOT_LOCATION[3]="BOT 4"
SLOT_LOCATION[4]="TOP 1"
SLOT_LOCATION[5]="TOP 2"
SLOT_LOCATION[6]="TOP 3"
SLOT_LOCATION[7]="TOP 4"

ECHO "3rd_BYPASS Test for NCB-5220...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "------------------------------------------"

CHECK_TEST_EQUIPMENT "3rd_BYPASS Test"


#install driver
#rmmod i2c_i801
#rmmod ipmi_devintf
#insmod /Test/bpwd_drv.ko
#if [ ! -b /dev/bpwd_drv ]; then
#    mknod /dev/bpwd_drv c 247 0
#fi
#sleep 1

ethx=`ls /sys/class/net | grep -v lo | grep -v tun10`
for i in $ethx 
do
  ifconfig $i up
done

while [ 1 ];
do
#    if [ -f "/Test/00a-smbus_off.sh" ]; then
#        /Test/icmd 00 20 b8 06 05
#    fi
     ECHO "1. Detect Controllor"
     ECHO "--------------------"
#    for i in $(seq 0 1)
     for i in 0 1 2
    do
#        buf=`/Test/bpwd_tst -r -d 0x37 -c 0x12`
#	     buf=`/Test/bpwd_tst -M MB-887X -r -d 0x3$i -c 0x12`
         buf=`echo "$buf" | grep "Command:0x12 ...OK"`
#        if [ -z "$buf" ]; then
#            ECHO "${SLOT_LOCATION[$i]} : NA"
#        else
#            ECHO "${SLOT_LOCATION[$i]} : Detected"
#        fi
    done

    ECHO "2. Runtime Bypass Enable"
    ECHO "------------------------"
    /Test/bpwd_tst -d 0x37 -c 0x12 -o 0x0f -w > /dev/null               #on board module bot
    /Test/bpwd_tst -M NCB-5220 -d 0x30 -c 0x12 -o 0x0f -w > /dev/null   #on board module top
    /Test/bpwd_tst -M NCB-5220 -d 0x31 -c 0x12 -o 0x0f -w > /dev/null   #RC52203A module 
    /Test/bpwd_tst -M NCB-5220 -d 0x32 -c 0x12 -o 0x0f -w > /dev/null   #RC52203A module 
    echo -e "\e[0;36mPress a key to continue\e[m"
    read -n 1 x


    ECHO "3. Runtime Bypass Disable"
    ECHO "-------------------------"
    /Test/bpwd_tst -d 0x37 -c 0x12 -o 0x00 -w > /dev/null               #on board module bot
    /Test/bpwd_tst -M NCB-5220 -d 0x30 -c 0x12 -o 0x0f -w > /dev/null   #on board module top
    /Test/bpwd_tst -M NCB-5220 -d 0x31 -c 0x12 -o 0x0f -w > /dev/null   #RC52203A module 
    /Test/bpwd_tst -M NCB-5220 -d 0x32 -c 0x12 -o 0x0f -w > /dev/null   #RC52203A module 
    echo -e "\e[0;36mPress a key to continue\e[m"
    read -n 1 x
    ECHO "------------------------------------------"

    while [ 1 ]; do
        echo -e "\e[0;36mIs this result OK? or press 'r' to retry (y/n/r)\e[m"
        read -n 1 x
        if [ "$x" == "y" ] || [ "$x" == "Y" ]; then
            ECHO_SUMMARY "3rd_BYPASS TEST PASS"
            rmmod bpwd_drv
            exit 0
        elif [ "$x" == "n" ] || [ "$x" == "N" ]; then
            ECHO_SUMMARY "3rd_BYPASS TEST FAIL"
            rmmod bpwd_drv
            exit 1
        elif [ "$x" == "r" ] || [ "$x" == "R" ]; then
            break
        fi
    done
done

ECHO_SUMMARY "3rd_BYPASS TEST FAIL"
#uninstall driver
rmmod bpwd_drv
exit 1
