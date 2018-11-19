#!/bin/bash

. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

insmod /Test/wd_drv.ko
mknod /dev/wd_drv c 241 0

sleep 2

/Test/wd_tst --stop

