DIRS = case_op dio eep hwm lcm pmbus sled sw_btn wd_bp 


all: gen_makefile

	for i in $(DIRS); do $(MAKE) KDIR=/usr/src/linux -C $$i $@; done


clean: gen_makefile

	for i in $(DIRS); do $(MAKE) -C $$i $@; done


gen_makefile:

	for i in $(DIRS);do cd $$i; cp -af Makefile.linux Makefile; cd ..; done


.PHONY: all clean gen_makefile

