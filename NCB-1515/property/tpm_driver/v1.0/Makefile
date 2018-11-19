#
# Makefile for building the kernel tpm device drivers outside of the kernel
# Will build for the currently booted kernel and the drivers will remain in
# the current directory
#

#ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_TCG_TPM) += tpm.o
tpm-y := tpm-interface.o tpm-dev.o tpm-sysfs.o tpm-chip.o tpm2-cmd.o
#obj-$(CONFIG_TCG_NSC) += tpm_nsc.o
#obj-$(CONFIG_TCG_ATMEL) += tpm_atmel.o
obj-$(CONFIG_TCG_TIS) += tpm_tis.o

#else

#KDIR	:= /lib/modules/$(shell uname -r)/build
KDIR	:= /usr/src/linux
##KDIR	:= /home/factory_testbed/build/linux-3.15_3.15_x86_64.config
PWD	:= $(shell pwd)

CONFIG	= CONFIG_TCG_TPM=m \
	  CONFIG_TCG_TIS=m \
#	  CONFIG_TCG_NSC=m \
#	  CONFIG_TCG_ATMEL=m \
#	  CONFIG_TCG_INFINEON=m \

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) $(CONFIG) modules

clean:; $(RM) *.o *.ko *.mod.? modules.order Module.symvers

#endif

