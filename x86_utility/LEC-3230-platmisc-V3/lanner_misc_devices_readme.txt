/* Lanner platform misc devices readme */

This package provides driver/test program for developer to realize Lanner misc
devices. Due to various platform embedded with specific functions, following
just describes general purpose of each individual sub-directory. Some of 
sub-directory may not present since platform may not be capabled of that:

sub-directory	Descriptions
==========================================================================

wd_bp		Watchdog(/Lan-bypass) demo program
		Linux pre-defined node number:	char device, major number=241

sled		Single Led demo program
		Linux pre-defined node number:	char device, major number=240

sw_btn		Software Button demo program
		Linux pre-defined node number:	char device, major number=242
		
led		Eight led demo program
		Linux pre-defined node number:	char device, major number=246		

Note:
Developer SHOULD combine individual function into one integrated program
to prevent conflict IO accessing across tasks.

