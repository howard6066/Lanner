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
#include <dpmi.h>
#include <sys/farptr.h>
#else //DJGPP
/* For Linux */


#ifdef DIRECT_IO_ACCESS
/* For Linux direct io access code */
/* standard include file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>	//V1.2 supoort MMIO

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
#include <sys/lock.h>
#include <sys/mutex.h>
#undef delay
#define delay(x) DELAY(1000*x)
#endif

#endif

/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))


/*
 * Platform Depend GPIOs for Reset Button
 *------------------------------------------------------------------------------
 * The high/low definition is as below:
 *     --------------------------
 *      1       Button Pressed
 *      0       Button Released
 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : Intel ICH8-M chipset
*/


//===============================================================
//  GPIO's definitions/interfaces (MMIO - Memory Mapping I/O Addressing)
//===============================================================
#define PCH_PCR_BASE_ADDRESS			0xFD000000		// SBREG MMIO base address
#define PCH_PCR_MMIO_SIZE               0x01000000     	// < 16MB
#define SB_PCR_ADDRESS(Pid, Offset)    	( ((unsigned long)(Pid) << 16) | ((unsigned int)(Offset)) )

//----------------------------------------------------------

#ifdef DJGPP 
__dpmi_meminfo                  _DmpiMemAccessIF;                       // DPMI interface/structure for accessing above 1MB memory.

unsigned int                          _PCR_MMIO_Selector = 0xFFFF;            // LDT selector for Skylake(SKL) GPIO(/MMIO) memory space.
 

void                    DPMI_AllocLdt_ForMmioAddrAbove1MB (unsigned long PhysicalAddr, unsigned long MemSize)
{   
	if ( _PCR_MMIO_Selector == 0xFFFF )
	{
		// Map the physical device address to linear memory.
		_DmpiMemAccessIF.address = PhysicalAddr;

	    _DmpiMemAccessIF.size        = MemSize & 0xFFFFF000L;           // Must be a multiple of 4KB per spec.

		// Now _DmpiMemAccessIF.address holds the linear address.
		__dpmi_physical_address_mapping ( &_DmpiMemAccessIF );

		// Allocate an LDT descriptor and set it up to span the entire device on-board memory.
		_PCR_MMIO_Selector = __dpmi_allocate_ldt_descriptors (1);

		__dpmi_set_segment_base_address (_PCR_MMIO_Selector, _DmpiMemAccessIF.address);
		__dpmi_set_segment_limit                (_PCR_MMIO_Selector, _DmpiMemAccessIF.size - 1);
	}
}

void GpioBaseInit(void)
{
    DPMI_AllocLdt_ForMmioAddrAbove1MB( PCH_PCR_BASE_ADDRESS, PCH_PCR_MMIO_SIZE );

	return;
}

//---------------------------------------------------------------
unsigned long                  MmioRead32 (unsigned long Address)
{
	if ( _PCR_MMIO_Selector != 0xFFFF )
	{        
		return _farpeekl(_PCR_MMIO_Selector, Address);
	}
    else
	return 0xFFFFFFFFL;
}

//---------------------------------------------------------------
void                    MmioWrite32 (unsigned long Address, unsigned long Value)
{
	if ( _PCR_MMIO_Selector != 0xFFFF )
		_farpokel(_PCR_MMIO_Selector, Address, Value);
}
#else  /// DJGPP function 
#ifdef DIRECT_IO_ACCESS 
// DIRECT_IO_ACCESS
unsigned int *get_mem_addr(void){
    	volatile void *gpio_addr =NULL;
    	int fd;
			fd = open("/dev/mem", O_RDWR|O_SYNC);
			
    	if (fd == -1)
    	{
        	exit (-1);
    	}
			gpio_addr = mmap(NULL, 0x1000000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PCH_PCR_BASE_ADDRESS);
    	close(fd);
	
      return (unsigned int *)gpio_addr;	
}

unsigned int MmioRead32(unsigned long offset)
{
    	unsigned int reg;
    	volatile unsigned int *gpio_oe_addr =NULL;
    	volatile void *gpio_addr =NULL;
    	//unsigned int  *gpio_addr =NULL;
			gpio_addr=get_mem_addr();
  		gpio_oe_addr = gpio_addr + offset;
			reg = *(gpio_oe_addr);

        return reg;
}



void MmioWrite32(unsigned long offset, unsigned long value)
{
    	volatile void *gpio_addr =NULL;
    	volatile unsigned int *gpio_oe_addr =NULL;
    	//unsigned int  *gpio_addr =NULL;
			gpio_addr=get_mem_addr();
    	gpio_oe_addr = gpio_addr + offset;
			*(volatile unsigned int *)(gpio_oe_addr)=value;
   	

        return;
}
// DIRECT_IO_ACCESS
#else
void                    GpioBaseInit (void)
{

	return;
}


unsigned long                  MmioRead32 (unsigned long Address)
{
		return	ioread32(ioremap_nocache(PCH_PCR_BASE_ADDRESS + Address, 4));
}

//---------------------------------------------------------------
void                    MmioWrite32 (unsigned long Address, unsigned long Value)
{

			iowrite32(Value, ioremap(PCH_PCR_BASE_ADDRESS + Address, 4));
}
#endif


#endif// 
//---------------------------------------------------------------
char 		get_input (int PortID_GPP,int PinOffset)
{
    unsigned long               GpioCfgRegVal = 0;
    char	PinState;
    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(PortID_GPP,PinOffset));

    PinState = (GpioCfgRegVal & 0x02)>>1;

    return PinState;
}


unsigned char get_btn_status(void){
	
	char value=0x00;
	value|=get_input(0x6B,0x6A0);//GPP_E6 
	return value;
}

#endif
