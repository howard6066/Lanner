/* Lanner platform misc devices readme */

This package provides driver/test program for developer to realize Lanner misc
devices. Due to various platform embedded with specific functions, following
just describes general purpose of each individual sub-directory. Some of 
sub-directory may not present since platform may not be capabled of that:

sub-directory	Descriptions
==========================================================================
sled 		Status LED demo program
		Linux pre-defined node number:	char device, major number=240
		
wd_bp 		watchdog
		Linux pre-defined node number:	char device, major number=241

sw_btn		Software Reset Button
		Linux pre-defined node number:	char device, major number=242

case_op		Case Open (Intruder) Status
		Linux pre-defined node number:	char device, major number=243
		
Dio		Digital IO
		Linux pre-defined node number:	char device, major number=244		

lcm		LCM 
		Linux pre-defined node number:	char device, major number=245
				
eep		EEPROM 
		Linux pre-defined node number:	char device, major number=247
Note:
Developer SHOULD combine individual function into one integrated program
to prevent conflict IO accessing across tasks.

