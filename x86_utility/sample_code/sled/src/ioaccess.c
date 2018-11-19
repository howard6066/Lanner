/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Status LED program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010 Lanner Electronics Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided tha
#include <errno.h>t the following conditions
  are met:
#include <errno.h>
  1. Redistributions of source code must r
#include <errno.h>etain the above copyright
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

#include "../include/config.h"

/* DJGPP */
#ifdef DJGPP
/* standard include file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* For DOS DJGPP */
#include <dos.h>
#include <inlines/pc.h>
#else 
/* DJGPP */

/* For Linux */
#ifdef DIRECT_IO_ACCESS
/* For Linux direct io access code */
/* standard include file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(LINUX_ENV)
#include <sys/io.h>
#endif

#if defined(FreeBSD_ENV)
#include <machine/cpufunc.h>
#endif

#include <time.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define delay(x) usleep(x)
#endif

/* MODULE */
#ifdef MODULE
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/delay.h>
#undef delay
#define delay(x) mdelay(x)
#undef fprintf
#define fprintf(S, A)  printk(A)
#endif 
/* MODULE */

/* KLD_MODULE */
#ifdef KLD_MODULE
#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/errno.h>
#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#undef delay
#define delay(x) DELAY(1000*x)
#endif
/* KLD_MODULE */

#endif

/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))
/*
 * Platform Depend GPIOs for Status LEDs
 *------------------------------------------------------------------------------

 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : 
*/
#define INDEX_PORT      0x2E
#define DATA_PORT       0x2F
#define PCH_IOBASE      0x500

void enter_io_config(void)
{
        outportb(INDEX_PORT, 0x87); // Must Do It Twice
        outportb(INDEX_PORT, 0x87);
        return;
}

void exit_io_config(void)
{
        outportb(INDEX_PORT, 0xAA);
        return;
}

int read_io_reg(int LDN, int reg)
{
        outportb(INDEX_PORT, 0x07); //LDN register
        delay(5);
        outportb(DATA_PORT, LDN);
        delay(5);
        outportb(INDEX_PORT, reg);
        delay(5);
        return inportb(DATA_PORT);
}
int write_io_reg(int LDN, int reg, int value)
{	
        outportb(INDEX_PORT, 0x07); //LDN register
        delay(5);
        outportb(DATA_PORT, LDN);
        delay(5);
        outportb(INDEX_PORT, reg);
        delay(5);
        outportb(DATA_PORT, value);
        return 0;
}

#define gpio71 0x02
#define gpio72 0x04
#define gpio73 0x08
#define gpio74 0x10
#define gpio75 0x20

void sled_gpio_init(void)
{
//If any init code can put here!
   		enter_io_config();	
         	write_io_reg(0x0, 0x27,(read_io_reg(0x0, 0x27) & 0xFF)|0x80);    		
         	write_io_reg(0x09, 0x30,(read_io_reg(0x09, 0x30) & 0xFF)|0x80);   
         	write_io_reg(0x07, 0xe0,(read_io_reg(0x07, 0xe0) & 0xF3)|0x00); 
         	write_io_reg(0x0F, 0xE6,(read_io_reg(0x0F, 0xE6) & 0xF3)|0x00);  
  		exit_io_config();
   		return;
}

//;On board LED status
  void set_led_status_1(void)
  {
   		enter_io_config();	
      write_io_reg(0x07, 0xe1,(read_io_reg(0x7, 0xe1) & ~(gpio72 + gpio73)) | gpio73); //rxe1[3,2]=10
	    //GPIO72,73 set 10b	
  		exit_io_config();
  		return;
  }
  
  void set_led_status_2(void)
  {
  		enter_io_config();
 		  write_io_reg(0x7, 0xe1,(read_io_reg(0x7, 0xe1) & ~(gpio72 + gpio73)) | gpio72); //rxe1[3,2]=01
	    //GPIO72,73 set 01b		  	 	
   		exit_io_config();
   		return;		
  }
  
 void set_led_status_3(void)
 {
 	 	  enter_io_config();
 		  write_io_reg(0x7, 0xe1,(read_io_reg(0x7, 0xe1) & ~(gpio72 + gpio73)) | 0x00);  //rxe1[3,2]=00 
      //GPIO72,73 set 00		  	 	
   		exit_io_config();   			
   		return;
 }

#endif