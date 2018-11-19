/* Lanner platform misc devices readme */

This package provides driver/test program for developer to realize Lanner misc
devices. Due to various platform embedded with specific functions, following
just describes general purpose of each individual sub-directory. Some of 
sub-directory may not present since platform may not be capabled of that:

sub-directory	Descriptions
==========================================================================
sled		Status LED demo program
		Linux pre-defined node number:	char device, major number=240

wd		Watchdog demo program
		Linux pre-defined node number:  char device, major number=241	
		

Note:
1. Developer SHOULD combine individual function into one integrated program
to prevent conflict IO accessing across tasks.
2. Almost major device numbers are statically assigned to the most common devices.
Developer can be use dynamic major number assign for device driver.

