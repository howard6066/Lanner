#!/bin/bash
. /usr/bin/common_log.sh
SKIP_BURNIN_TEST

insmod /Test/tpm.ko
insmod /Test/tpm_tis.ko


mknod /dev/tpm0 c 10 224
