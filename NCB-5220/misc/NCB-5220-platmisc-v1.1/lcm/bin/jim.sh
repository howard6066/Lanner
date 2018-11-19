#! /bin/bash 

rmmod lcm_drv
insmod lcm_drv.ko 
./lcm_tst -show


