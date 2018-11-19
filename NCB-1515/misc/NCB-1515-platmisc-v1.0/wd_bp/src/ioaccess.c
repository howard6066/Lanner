/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Watchdog/Bypass program

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

#endif

#endif

/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))

#define sio_index	0x2E
#define sio_data	0x2F
#define PCI_index	0xcf8
#define PCI_data	0xcfc


void enter_SIO_config(void)
{
        outportb(sio_index, 0x87); // Must Do It Twice
        outportb(sio_index, 0x87);
        return;
}


void exit_SIO_config(void)
{
        outportb(sio_index, 0xAA);
        return;
}

unsigned char read_SIO_reg(int LDN, int reg)
{	
	outportb(sio_index, 0x07); //LDN register
        delay(5);
        outportb(sio_data, LDN);
        delay(5);
        outportb(sio_index, reg);
        delay(5);
        return(inportb(sio_data));
}


void write_SIO_reg(int LDN, int reg, int value)
{

        outportb(sio_index, 0x07); //LDN register
        delay(5);
        outportb(sio_data, LDN);
        delay(5);
        outportb(sio_index, reg);
        delay(5);
        outportb(sio_data, value);
        return;
}

void start_watchdog_timer(unsigned int watchdog_time)
{
	enter_SIO_config();
	//write_SIO_reg(0x8, 0xf6, watchdog_time); 					//LDN=8, CRF6, set watchdog timer time-out value
	do {write_SIO_reg(0x8, 0xf6, watchdog_time);}
	while (read_SIO_reg(0x08, 0xf6)!=watchdog_time);
	exit_SIO_config();
	return;	
}

void stop_watchdog_timer(void)
{
	enter_SIO_config();
	do {write_SIO_reg(0x8, 0xf6, 0x00);}
	while (read_SIO_reg(0x08, 0xf6)!=0x00);	
	exit_SIO_config();
	return;
}

void wd_gpio_init(void)
{
	enter_SIO_config();

  write_SIO_reg(0x8, 0x30, read_SIO_reg(0x8, 0x30)|0x05); //dev8 rx30[2,0]=1,1
	write_SIO_reg(0x8, 0xf7,0x00);//CR_f7=0	
	do {write_SIO_reg(0x8, 0xf6, 0x00);}
	while (read_SIO_reg(0x08, 0xf6)!=0x00);	
	write_SIO_reg(0x17,0xe5, read_SIO_reg(0x17, 0xe5) | 0x01); //dev17 rxe5[0]=1
	write_SIO_reg(0x17,0xe0, read_SIO_reg(0x17, 0xe0) & 0xfe); //dev17 rxe0[0]=0


	write_SIO_reg(0x8, 0x2C,read_SIO_reg(0x8, 0x2c)&0xFE);//CR_2C[0]=0	

	exit_SIO_config();    	

}



#endif
