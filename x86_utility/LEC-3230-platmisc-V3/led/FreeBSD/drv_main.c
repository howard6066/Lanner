/*******************************************************************************

  drv_main.c : Main code of FreeBSD driver for Lanner platform Status LED 
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


#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/conf.h>
#include <sys/module.h>

#include <sys/time.h>
#include <sys/proc.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
//#include <sys/ioctl.h>
#include <sys/ioccom.h>
#include <sys/sysctl.h>
#include <sys/signalvar.h>

#include <sys/uio.h>
#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/rman.h>
#include "../include/led_ioctl.h"
#include "../include/ioaccess.h"
#include "../include/version.h"


static struct cdev *led_dev;
static int isopen;

static	d_open_t	led_open;
static	d_close_t	led_close;
static	d_ioctl_t	led_ioctl;

static struct cdevsw led_cdevsw = {
	.d_version	= D_VERSION,
	.d_name		= "led",
	.d_flags	= D_NEEDGIANT,
	.d_open		= led_open,  /* open */
	.d_close	= led_close, /* close */
	.d_ioctl	= led_ioctl  /* ioctl */
};

static int led_open(struct cdev *dev, int flags, int devtype, struct thread *td) {
	if(isopen != 0) {
		return (EBUSY);
	}
	isopen = 1;
	return 0;
}

static int led_close(struct cdev *dev, int flags, int fmt, struct thread *td) {
	
 	isopen=0;
		
	return 0;
}

static int led_ioctl(struct cdev *dev, u_long cmd, caddr_t data,
	int fflag, struct thread *td)
{
	int *arg = (int*)data;
	
  switch(cmd)
  {
      case IOCTL_LED_SET_STATUS:
      switch(*arg)
      {
          case LED_SET_STATUS_1:
               set_led_status_1();
          break;                                
                        
          case LED_SET_STATUS_2:   
               set_led_status_2();
          break; 
								
          case LED_SET_STATUS_3:
               set_led_status_3(); 
          break;  
                   
          case LED_SET_STATUS_4:
               set_led_status_4();
          break;                                

          default:
                return -ENODEV;
      }
      break;
      default:
        return -ENODEV;
  }
  return 0;

}

static int
led_loader(struct module *m, int what, void *arg)
{
    int err = 0;

    switch (what) {
    case MOD_LOAD:                /* kldload */
        led_dev = make_dev(&led_cdevsw,
            0,
            UID_ROOT,
            GID_WHEEL,
            0600,
            "led_drv");
        /* kmalloc memory for use by this driver */
        isopen = 0;
	      led_gpio_init();
	      printf("Lanner "PLATFORM_NAME" Status LED Driver "CODE_VERSION" loaded\n");
        break;
    case MOD_UNLOAD:
        destroy_dev(led_dev);
	      printf("Lanner "PLATFORM_NAME" Status LED Driver "CODE_VERSION" unloaded\n");
        break;
    default:
        err = EOPNOTSUPP;
        break;
    }
    return(err);
}

DEV_MODULE(led, led_loader, NULL);

