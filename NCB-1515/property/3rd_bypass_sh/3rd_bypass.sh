#/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

ECHO "3rd_BYPASS Test for NCB-1510...`date "+%Y/%m/%d %H:%M:%S"`"
ECHO "Version 1.0"
ECHO "------------------------------------------"

CHECK_TEST_EQUIPMENT "3rd_BYPASS Test"

#install driver
rmmod i2c_i801
insmod /Test/bpwd_drv.ko
if [ ! -b /dev/bpwd_drv ]; then
    mknod /dev/bpwd_drv c 246 0
fi
sleep 1

ethx=`ls /sys/class/net/ | grep -v lo | grep -v tunl0 | grep -iv w`
for i in $ethx
do
   ifconfig $i up
done
#ifconfig eth0 up
#ifconfig eth1 up
#ifconfig eth2 up
#ifconfig eth3 up
#ifconfig eth4 up

while [ 1 ];
do
    ECHO "2. Runtime Bypass Enable"
    ECHO "------------------------"
    /Test/bpwd_tst -d 0x37 -c 0x12 -o 0x0f -w  > /dev/null
    /Test/bpwd_tst -d 0x36 -c 0x12 -o 0x0f -w  > /dev/null
    echo -e "\e[0;36mPress a key to continue\e[m"
    read -n 1 x


    ECHO "3. Runtime Bypass Disable"
    ECHO "-------------------------"
    /Test/bpwd_tst -d 0x37 -c 0x12 -o 0x00 -w  > /dev/null
    /Test/bpwd_tst -d 0x36 -c 0x12 -o 0x00 -w  > /dev/null
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
