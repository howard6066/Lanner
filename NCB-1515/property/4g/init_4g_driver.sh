#!/bin/bash
. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

insmod /Test/cdc-wdm.ko
insmod /Test/mii.ko
insmod /Test/usbnet.ko
insmod /Test/cdc_ncm.ko
insmod /Test/cdc_mbim.ko
insmod /Test/qmi_wwan.ko
mknod /dev/cdc-wdm0 c 180 176
tar -C /Test -xvf /Test/4g_driver.tgz
rpm=`ls /Test/4g_driver | grep rpm`
for i in $rpm
do
   rpm -i /Test/4g_driver/$i
done
