/*******************************************************************************

  drv_main.c : Main code of Linux driver for Lanner platform Watchdof/Bypass
                program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010 Lanner Electronics Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer,
     without modification.
  2. Redistributions in binary form must reproduce at minimum a disclaimer
     similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
     redistribution must be conditioned upon including a substantially
     similar Disclaimer requirement for further binary redistribution.
  3. Neither the names of the above-listed copyright holders nor the names
     of any contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  Alternatively, this software may be distributed under the terms of the
  GNU General Public License ("GPL") version 2 as published by the Free
  Software Foundation.

  NO WARRANTY
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

*******************************************************************************/


/* Standard in kernel modules */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <linux/delay.h>
//#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include "../include/ioaccess.h"
#include "../include/version.h"
#include "../include/wd_ioctl.h"
#include "../include/config.h"

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif


/*
 * Device Major Number
 */
//#define WD_MAJOR 241
// for Dymaic Major number
#if (LANNER_MAJOR_NUMBER != 0)
#define WD_MAJOR LANNER_MAJOR_NUMBER
#else
static unsigned int  WD_MAJOR = 0;
module_param(WD_MAJOR, uint, 0);
#endif

/*
 * Is the device opened right now?
 * Prevent to access the device in the same time
 */
static int Device_Open = 0;
//tadd++Start
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
static int wd_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long wd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
//tadd++End
{

	unsigned int value;


        switch(cmd)
        {
              
	/* Set the watchdog timer */
                case IOCTL_SET_WDTO_TIMER:
			if( copy_from_user(&value, (unsigned int *)arg, sizeof(value)))
			return -EFAULT;
                	start_watchdog_timer(value);
                       break;

	/* Stop watchdog timer */
                case IOCTL_STOP_WDT_TIMER:
                                                    
                        stop_watchdog_timer();
                        break;
                  
                default:
                        return -EOPNOTSUPP;
        }
        return 0;

}

/*
 * This function is called whenever a process attempts to
 * open the device file
 */
static int wd_open(struct inode * inode, struct file * file)
{
	/* we don't want to talk to two processes at the same time */
	if(Device_Open) return -EBUSY;
	Device_Open++;
	/* Make sure that the module isn't removed while the file
	 * is open by incrementing the usage count (the number of
	 * opened references to the module,if it's zero emmod will
	 * fail)
	 */
	return 0;
}

/*
 * This function is called when a process closes the device file.
 */
static int wd_release(struct inode * inode, struct file * file)
{
	/* ready for next caller */
	Device_Open--;
	/* Decrement the usage count, otherwise once you opened the file
	 * you'll never get rid of the module.
	 */
	return 0;
}

/*
 * This structure will hold the functions to be called
 * when a process does something to the device we created.
 * Since a pointer to this structure is kept in the
 * devices table, it can't be local to init_module.
 * NULL is for unimplemented functions.
 */
#if defined(OLDKERNEL)
static struct file_operations wd_fops = {
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
	ioctl:		wd_ioctl,
	open:		wd_open,
	release:	wd_release,
};
#else
static const struct file_operations wd_fops = {

	//tadd++Start
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
	.ioctl		= wd_ioctl,
#else
	.unlocked_ioctl		= wd_ioctl,
#endif
//tadd++end	
	.open		= wd_open,
	.release	= wd_release,
};
#endif

int wd_init(void)
{
#if (LANNER_MAJOR_NUMBER != 0)	//add for damyic major number select	
	/*
	 * Register the character device
	 */
	if(register_chrdev(WD_MAJOR, "wd_drv", &wd_fops))
	{
		printk("wd_drv : unable to get major %d\n", WD_MAJOR);
		return -EIO;
	}
	wd_gpio_init();
	printk("Lanner Platform <<%s>> Watchdog  Version %s -- loaded\n", PLATFORM_NAME, CODE_VERSION);
	return 0;
#else //add for damyic major number select >>
	int major;
	int ret = 0;

	major = register_chrdev(WD_MAJOR, "wd_drv", &wd_fops);
	
	if((WD_MAJOR > 0 && major != 0) ||
		(WD_MAJOR == 0 && major < 0) ||
		major < 0) {
		printk("%s driver registeration error.\n", "wd_drv");
		ret = major;
		goto error;
	}
	if (WD_MAJOR == 0){
		WD_MAJOR =major;
	}
	
	wd_gpio_init();
	
	printk("Watchdog driver(major %d) installed.\n", WD_MAJOR);
	
	return 0;

error:
	return(ret);	
#endif	//add for damyic major number select <<	
}

/*
 * Cleanup - unregister the appropriate file from /proc
 */
void wd_exit(void)
{
	/* Unregister the device */
	unregister_chrdev(WD_MAJOR, "wd_drv");
	/* If there's an error, report it */
	printk("Lanner Platform Watchdog/Bypass Driver -- Unloaded\n");
}

module_init(wd_init);
module_exit(wd_exit);

MODULE_AUTHOR("Lanner SW");
MODULE_DESCRIPTION("Lanner Platform Watchdog/Bypass Driver");
MODULE_LICENSE("Dual BSD/GPL");
