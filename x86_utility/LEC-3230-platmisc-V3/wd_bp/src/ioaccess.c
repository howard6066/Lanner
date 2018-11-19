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
#undef delay
#define delay(x) DELAY(1000*x)

#endif

#endif

/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))


/*
 * Platform Depend GPIOs Interface for Watchdog and Lan bypass
 */
/*
 *------------------------------------------------------------------------------



 *------------------------------------------------------------------------------




 *------------------------------------------------------------------------------
 */
/*
 */

#define INDEX_PORT	0x2E
#define DATA_PORT	0x2F
#define SIO_GPIO_22_BIT	0x04
#define SIO_GPIO_23_BIT	0x08
#define SIO_GPIO_24_BIT	0x10
#define SIO_GPIO_27_BIT	0x80
#define SIO_GPIO_30_BIT	0x01
#define SIO_GPIO_31_BIT	0x02
#define SIO_GPIO_60_BIT	0x01


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


/*Runtime bypass definitions */
#define RUNTIME_BYPASS_PAIR1_LDN	(9)
#define RUNTIME_BYPASS_PAIR1_REG	(0xe5)
#define RUNTIME_BYPASS_PAIR1_BIT	(SIO_GPIO_24_BIT)
#define RUNTIME_BYPASS_PAIR1_ENABLE	(0)
#define RUNTIME_BYPASS_PAIR1_DISABLE	(SIO_GPIO_24_BIT)

#define RUNTIME_BYPASS_PAIR2_LDN	(8)
#define RUNTIME_BYPASS_PAIR2_REG	(0xe5)
#define RUNTIME_BYPASS_PAIR2_BIT	(SIO_GPIO_60_BIT)
#define RUNTIME_BYPASS_PAIR2_ENABLE	(0)
#define RUNTIME_BYPASS_PAIR2_DISABLE	(SIO_GPIO_60_BIT)

/*Offmode bypass definitions */
#define OFFMODE_BYPASS_PAIR1_LDN	(9)
#define OFFMODE_BYPASS_PAIR1_REG	(0xe5)
#define OFFMODE_BYPASS_PAIR1_BIT	(SIO_GPIO_22_BIT | SIO_GPIO_23_BIT)
#define OFFMODE_BYPASS_PAIR1_ENABLE	SIO_GPIO_22_BIT
#define OFFMODE_BYPASS_PAIR1_DISABLE	SIO_GPIO_23_BIT

#define OFFMODE_BYPASS_PAIR2_LDN	(7)
#define OFFMODE_BYPASS_PAIR2_REG	(0xe1)
#define OFFMODE_BYPASS_PAIR2_BIT	(SIO_GPIO_30_BIT | SIO_GPIO_31_BIT)
#define OFFMODE_BYPASS_PAIR2_ENABLE	SIO_GPIO_30_BIT
#define OFFMODE_BYPASS_PAIR2_DISABLE	SIO_GPIO_31_BIT


void start_watchdog_timer(int watchdog_time)
{
	enter_SIO_config();
	write_SIO_reg(0x7, 0xf6, watchdog_time); 					//LDN=8, CRF6, set watchdog timer time-out value
	delay(5);
	write_SIO_reg(0x7, 0xFA, 0x01);//enabel WDRST# output	
	write_SIO_reg(0x7, 0xf5, ((read_SIO_reg(0x7, 0xf5)& 0x40)|0x20)); //enabel counter
	exit_SIO_config();
	return;	
}

void stop_watchdog_timer(void)
{
	enter_SIO_config();
	/* stop timer */
	write_SIO_reg(0x7, 0xf5, ((read_SIO_reg(0x7, 0xf5)& 0x40)&0xDF)); //disable counter
	exit_SIO_config();
	return;
}

int wd_gpio_init(void)
{
	enter_SIO_config();
	int ret=0;
	write_SIO_reg(0x7, 0xFA, 0x00);//disabel WDRST# output
	delay(5);
	write_SIO_reg(0x7, 0x2b, (read_SIO_reg(0x7, 0x2b)& 0xdF)); //2b[5]=0 , pin select
  delay(5);
	write_SIO_reg(0x7, 0xf5, 0x40); //active low, 1sec time step, clr status
  delay(5);
	exit_SIO_config();    	
	return ret;

}

void set_bypass_enable_when_system_off(unsigned long pair_no)
{

	int reg_no, ldn_no;
	unsigned char bit_mask;
	unsigned char en_data;
	unsigned char tmp;

	reg_no=ldn_no=bit_mask=en_data=tmp=0;
	switch(pair_no) {
		case BYPASS_PAIR_1:
			ldn_no = OFFMODE_BYPASS_PAIR1_LDN;
			reg_no = OFFMODE_BYPASS_PAIR1_REG;
			bit_mask = OFFMODE_BYPASS_PAIR1_BIT;
			en_data = OFFMODE_BYPASS_PAIR1_ENABLE;
			break;
		case BYPASS_PAIR_2:
			ldn_no = OFFMODE_BYPASS_PAIR2_LDN;
			reg_no = OFFMODE_BYPASS_PAIR2_REG;
			bit_mask = OFFMODE_BYPASS_PAIR2_BIT;
			en_data = OFFMODE_BYPASS_PAIR2_ENABLE;
			break;
		default:
			/*un-support pair no, return */
			return;
	}
	tmp=read_SIO_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask) ;
	tmp |= en_data;
	write_SIO_reg(ldn_no, reg_no, tmp);

	return;
}


void set_bypass_disable_when_system_off(unsigned long pair_no)
{

	int reg_no, ldn_no;
	unsigned char bit_mask;
	unsigned char en_data;
	unsigned char tmp;

	reg_no=ldn_no=bit_mask=en_data=tmp=0;
	switch(pair_no) {
	case BYPASS_PAIR_1:
			ldn_no = OFFMODE_BYPASS_PAIR1_LDN;
			reg_no = OFFMODE_BYPASS_PAIR1_REG;
			bit_mask = OFFMODE_BYPASS_PAIR1_BIT;
			en_data = OFFMODE_BYPASS_PAIR1_DISABLE;
			break;
	case BYPASS_PAIR_2:
			ldn_no = OFFMODE_BYPASS_PAIR2_LDN;
			reg_no = OFFMODE_BYPASS_PAIR2_REG;
			bit_mask = OFFMODE_BYPASS_PAIR2_BIT;
			en_data = OFFMODE_BYPASS_PAIR2_DISABLE;
			break;
		default:
			/*un-support pair no, return */
			return;
	}
	tmp=read_SIO_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask) ;
	tmp |= en_data;
	write_SIO_reg(ldn_no, reg_no, tmp);

	return;
}

void set_runtime_bypass_enable(unsigned long pair_no)
{
	int reg_no, ldn_no;
  unsigned char tmp, bit_mask, en_data;

	reg_no=ldn_no=bit_mask=en_data=tmp=0;
/*      Note: To sete runtime bypass mode, user need to set off-mode bypass
 *            enabled in order to let function activity.
 */
  set_bypass_enable_when_system_off(pair_no);

  switch(pair_no) {
  case BYPASS_PAIR_1:
			ldn_no = RUNTIME_BYPASS_PAIR1_LDN;
			reg_no = RUNTIME_BYPASS_PAIR1_REG;
      bit_mask = RUNTIME_BYPASS_PAIR1_BIT;
      en_data = RUNTIME_BYPASS_PAIR1_ENABLE;
  break;
  case BYPASS_PAIR_2:
			ldn_no = RUNTIME_BYPASS_PAIR2_LDN;
			reg_no = RUNTIME_BYPASS_PAIR2_REG;
      bit_mask = RUNTIME_BYPASS_PAIR2_BIT;
      en_data = RUNTIME_BYPASS_PAIR2_ENABLE;
  break;
	default:
			/*un-support pair no, return */
			return;

        }
	tmp=read_SIO_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask) ;
	tmp |= en_data;
	write_SIO_reg(ldn_no, reg_no, tmp);

	return;
}

void set_runtime_bypass_disable(unsigned long pair_no)
{

	int reg_no, ldn_no;
	unsigned char tmp, bit_mask, en_data;

	reg_no=ldn_no=tmp=bit_mask=en_data=0;

	switch(pair_no) {
	case BYPASS_PAIR_1:
		ldn_no = RUNTIME_BYPASS_PAIR1_LDN;
		reg_no = RUNTIME_BYPASS_PAIR1_REG;
		bit_mask = RUNTIME_BYPASS_PAIR1_BIT;
		en_data = RUNTIME_BYPASS_PAIR1_DISABLE;
	break;
	case BYPASS_PAIR_2:
		ldn_no = RUNTIME_BYPASS_PAIR2_LDN;
		reg_no = RUNTIME_BYPASS_PAIR2_REG;
		bit_mask = RUNTIME_BYPASS_PAIR2_BIT;
		en_data = RUNTIME_BYPASS_PAIR2_DISABLE;
	break;
  }
	tmp=read_SIO_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask) ;
	tmp |= en_data;
	write_SIO_reg(ldn_no, reg_no, tmp);

	return;
}

void set_wdto_state_system_reset(void)
{
//	unsigned char tmp;
//
//	/* set GPIO27=1 for reset mode */
//	tmp=read_SIO_reg(0x9, 0xe5);
//	tmp |= SIO_GPIO_27_BIT;
//	write_SIO_reg(0x9, 0xe5, tmp);

	return;
}

void set_wdto_state_system_bypass(void)
{
//	unsigned char tmp;
//
//	/* set GPIO27=0 for bypass mode */
//	tmp=read_SIO_reg(0x9, 0xe5);
//	tmp &= ~SIO_GPIO_27_BIT;
//	write_SIO_reg(0x9, 0xe5, tmp);

	return;
}

#endif
