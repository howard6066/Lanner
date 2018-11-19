/*******************************************************************************

  drv_main.c : Main code of Linux driver for Lanner EEPROM program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010 - 2011 Lanner Electronics Inc.
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
#include <linux/spinlock.h>
//v1.2 #include <asm/uaccess.h>
#include <linux/uaccess.h>
#include "../include/eep_ioctl.h"
#include "../include/ioaccess.h"
#include "../include/version.h"
#include "../include/config.h"

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif


/*
 * Device Major Number
 */
//#define EEP_MAJOR 247
// for Dymaic Major number
#if (LANNER_MAJOR_NUMBER != 0)
#define EEP_MAJOR LANNER_MAJOR_NUMBER
#else
static unsigned int  EEP_MAJOR = 0;
module_param(EEP_MAJOR, uint, 0);
#endif

/*
 * Is the device opened right now?
 * Prevent to access the device in the same time
 */
static int Device_Open = 0;
static DEFINE_SPINLOCK(eep_lock);
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
static int eep_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long eep_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif

{
	int value;
	int tmp;
        switch(cmd)
        {


                case IOCTL_EEP_SET_SMBUS_ON:

			smbus_ON();
                        break;


                case IOCTL_EEP_SET_SMBUS_OFF:

			smbus_OFF();
                        break;

              

                case IOCTL_EEP_SET_OUTPUT:
			if( copy_from_user(&value, (int *)arg, sizeof(value)))
				return -EFAULT;
			eep_set_output(value);
                        break;
                case IOCTL_EEP_GET_INPUT:
			if( copy_from_user(&value, (int *)arg, sizeof(value)))
				return -EFAULT;
			tmp=value;
			value = eep_get_input(tmp);
			 if ( copy_to_user((int *)arg, &value, sizeof(value)))
				return -EFAULT;
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
static int eep_open(struct inode * inode, struct file * file)
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
static int eep_release(struct inode * inode, struct file * file)
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
static struct file_operations eep_fops = {
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
	ioctl:		eep_ioctl,
	open:		eep_open,
	release:	eep_release,
};
#else
static const struct file_operations eep_fops = {
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
	.ioctl		= eep_ioctl,
#else
	.unlocked_ioctl		= eep_ioctl,
#endif
	.open		= eep_open,
	.release	= eep_release,
};
#endif

int eep_init(void)
{
#if (LANNER_MAJOR_NUMBER != 0)	//add for damyic major number select	
	/*
	 * Register the character device
	 */
	if(register_chrdev(EEP_MAJOR, "eep_drv", &eep_fops))
	{
		printk("eep_drv : unable to get major %d\n", EEP_MAJOR);
		return -EIO;
	}
	eep_gpio_init();
	spin_lock_init(&eep_lock);
	printk("Lanner Platform <<%s>> EEPROM Driver Version %s -- loaded\n", PLATFORM_NAME, CODE_VERSION);
	return 0;
#else //add for damyic major number select >>
	int major;
	int ret = 0;

	major = register_chrdev(EEP_MAJOR, "eep_drv", &eep_fops);
	
	if((EEP_MAJOR > 0 && major != 0) ||
		(EEP_MAJOR == 0 && major < 0) ||
		major < 0) {
		printk("%s driver registeration error.\n", "eep_drv");
		ret = major;
		goto error;
	}
	if (EEP_MAJOR == 0){
		EEP_MAJOR =major;
	}
	
	eep_gpio_init();
	spin_lock_init(&eep_lock);
	
	printk("EEPROM driver(major %d) installed.\n", EEP_MAJOR);
	
	return 0;

error:
	return(ret);	
#endif	//add for damyic major number select <<	
}

/*
 * Cleanup - unregister the appropriate file from /proc
 */
void eep_exit(void)
{
	/* Unregister the device */
	unregister_chrdev(EEP_MAJOR, "eep_drv");
	/* If there's an error, report it */
	printk("Lanner Platform EEPROM Driver -- Unloaded\n");
}

module_init(eep_init);
module_exit(eep_exit);

MODULE_AUTHOR("Lanner SW");
MODULE_DESCRIPTION("Lanner Platform EEPROM Driver");
MODULE_LICENSE("Dual BSD/GPL");
