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

#define SIO_IDX_PORT	    0x2E
#define SIO_DAT_PORT	    0x2F

//---------------------------------------------------------------
void            EnterSioConfigMode (void)
{
    outportb (SIO_IDX_PORT, 0x87);      // Must do it twice per spec. requested
    outportb (SIO_IDX_PORT, 0x87);
}

//---------------------------------------------------------------
void            ExitSioConfigMode (void)
{
    outportb (SIO_IDX_PORT, 0xAA);
}


//---------------------------------------------------------------
unsigned char ReadSioReg (int Ldn, int Reg)
{
    int       RetData = 0x00;

    EnterSioConfigMode();

    outportb (SIO_IDX_PORT, 0x07);      // Select to Logical Device Number(LDN) Register
    outportb (SIO_DAT_PORT, Ldn);       // Write Ldn to SIO
    outportb (SIO_IDX_PORT, Reg);       // Write regster/index for access to
    RetData = inportb (SIO_DAT_PORT);   // Get data byte from SIO's register

    ExitSioConfigMode();

    return RetData;
}

//---------------------------------------------------------------
void WriteSioReg (int Ldn, int Reg, int Value)
{
    EnterSioConfigMode();

    outportb (SIO_IDX_PORT, 0x07);      // Select to Logical Device Number(LDN) Register
    outportb (SIO_DAT_PORT, Ldn);       // Write Ldn to SIO
    outportb (SIO_IDX_PORT, Reg);       // Write regster/index for access to
    outportb (SIO_DAT_PORT, Value);     // Write data byte into SIO's register

    ExitSioConfigMode();
}

void start_watchdog_timer(unsigned int watchdog_time)
{

    WriteSioReg ( 0x07,  0xF0, 0x80 ); 
    WriteSioReg ( 0x07,  0xF5, 0x52 );	
	  WriteSioReg ( 0x07,  0xF6, watchdog_time );     //LDN=7, CRF6, set watchdog timer time-out value
	  WriteSioReg ( 0x07,  0xF5, 0x32 ); 
}

void stop_watchdog_timer(void)
{
    WriteSioReg ( 0x07, 0xF5, 0x40 );
}

void wd_gpio_init(void)
{
	WriteSioReg ( 0x07,  0x26, ReadSioReg(0x07, 0x26) & 0xF7 );
	WriteSioReg ( 0x07,  0x28, ReadSioReg(0x07, 0x28) & 0x5F );
	WriteSioReg ( 0x07,  0x30, ReadSioReg(0x07, 0x30) | 0x01 );   // Enable WDT
	WriteSioReg ( 0x07,  0xF6, 0x00 );
	WriteSioReg ( 0x07,  0xF5, ReadSioReg(0x07, 0xF5) | 0x40 );   // clear WDT Time-out status.

}



#endif
