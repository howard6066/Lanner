/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Reset Button program

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



#include "../include/config.h"


#ifdef DJGPP

/* standard include file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* For DOS DJGPP */
#include <dos.h>
#include <inlines/pc.h>

#else //DJGPP
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

#endif //MODULE

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

#define delay(x) DELAY(x*1000)

#endif

#endif

/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))


/*
 * Platform Depend GPIOs for Reset Button
 */

/*
 *------------------------------------------------------------------------------
 * LEB-3110 Version V 1.0
 *
 * The IO interface for Reset Button is connected to Intel ICH8-M GPIO 20.
 * Refer to Intel ICH8-M datasheet for details.
 * The high/low definition is as below:
 *      GPIO20  Button Status
 *     --------------------------
 *      1       Button Pressed
 *      0       Button Released
 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : Intel ICH8-M chipset
*/
#define INDEX_PORT (0x2e)
#define DATA_PORT   (0x2f)

void enter_SIO_config(void)
{
        outportb(INDEX_PORT, 0x87); // Must Do It Twice
        outportb(INDEX_PORT, 0x87);
        return;
}


void exit_SIO_config(void)
{
        outportb(INDEX_PORT, 0xAA);
        return;
}

unsigned char read_SIO_reg(int LDN, int reg)
{	
	      outportb(INDEX_PORT, 0x07); //LDN register
        delay(5);
        outportb(DATA_PORT, LDN);
        delay(5);
        outportb(INDEX_PORT, reg);
        delay(5);
        return(inportb(DATA_PORT));
}


void write_SIO_reg(int LDN, int reg, int value)
{

        outportb(INDEX_PORT, 0x07); //LDN register
        delay(5);
        outportb(DATA_PORT, LDN);
        delay(5);
        outportb(INDEX_PORT, reg);
        delay(5);
        outportb(DATA_PORT, value);
        return;
}


/*
 * Return value:
 * 	0: BUTTON Pressed
 * 	1: BUTTON Released
 */ 	
unsigned char get_btn_status(void)
{
    enter_SIO_config();
		return  (read_SIO_reg(0x06, 0x92) & 0x08);
    exit_SIO_config();
}

void btn_gpio_init(void)
{
        /* platform initial code */

        /* Reset button initial function */

        /* select GPIO function */
    enter_SIO_config();
  write_SIO_reg(0x06, 0x26, read_SIO_reg(0x06, 0x26) & 0xfe);
	write_SIO_reg(0x06, 0x2a, read_SIO_reg(0x06, 0x2a) | 0x08);
	write_SIO_reg(0x06, 0x26, read_SIO_reg(0x06, 0x26) | 0x01);
	write_SIO_reg(0x06, 0x2a, read_SIO_reg(0x06, 0x2a) & 0xfd);
	write_SIO_reg(0x06, 0x90, read_SIO_reg(0x06, 0x90) & 0xf7);
    exit_SIO_config();
	
	return;
}


#endif
