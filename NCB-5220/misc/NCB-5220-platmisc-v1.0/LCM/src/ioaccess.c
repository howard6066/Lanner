/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr Status LED program

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
#include <sys/mman.h>

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
 * Platform Depend GPIOs for Status LEDs
 *------------------------------------------------------------------------------

 *------------------------------------------------------------------------------
*/

/*
 * Device Depend Definition : 
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

//------------------------------------------------------
void                    GpioBaseInit (void)
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


#endif// 
//---------------------------------------------------------------


#define   BIT0 0x1
#define   BIT1 0x2
#define   BIT2 0x4
#define   BIT3 0x8
#define   BIT4 0x10
#define   BIT5 0x20
#define   BIT6 0x40
#define   BIT7 0x80

// 16550 UART register offsets and bitfields
#define R_UART_RXBUF          0
#define R_UART_TXBUF          0
#define R_UART_BAUD_LOW       0
#define R_UART_BAUD_HIGH      1
#define R_UART_FCR            2
#define   B_UART_FCR_FIFOE    BIT0
#define   B_UART_FCR_FIFO64   BIT5
#define R_UART_LCR            3
#define   B_UART_LCR_DLAB     BIT7
#define R_UART_MCR            4
#define   B_UART_MCR_DTRC     BIT0
#define   B_UART_MCR_RTS      BIT1
#define R_UART_LSR            5
#define   B_UART_LSR_RXRDY    BIT0
#define   B_UART_LSR_TXRDY    BIT5
#define   B_UART_LSR_TEMT     BIT6
#define R_UART_MSR            6
#define   B_UART_MSR_CTS      BIT4
#define   B_UART_MSR_DSR      BIT5
#define   B_UART_MSR_RI       BIT6
#define   B_UART_MSR_DCD      BIT7

// Pcd
#define   PcdSerialFifoControl 0x7
#define   PcdSerialExtendedTxFifoSize 64
#define   PcdSerialLineControl 0x3

int 
SerialPortReadRegister (
  int  Base,
  int  Offset
  )
{

    return inportb (Base + Offset);
  
}

void 
SerialPortWriteRegister (
  int  Base,
  int  Offset,
  int  Value
  )
{

    return outportb (Base + Offset, Value);
  
}

int
SerialPortPoll (
  void
  )
{
  int         SerialRegisterBase;
  
  SerialRegisterBase = LCM_BASE_ADDRESS;

  // Read the serial port status
  if ((SerialPortReadRegister (SerialRegisterBase, R_UART_LSR) & B_UART_LSR_RXRDY) != 0) {

    return 1;
  }    
  
  return 0;
}

int
SerialPortRead (
  int     *Buffer,
  int     NumberOfBytes
  )
{
  int  SerialRegisterBase;
  int  Result;
  int  Mcr;

  if (NULL == Buffer) {
    return 0;
  }
  
  SerialRegisterBase = LCM_BASE_ADDRESS;

  
  for (Result = 0; NumberOfBytes-- != 0; Result++, Buffer++) {

    // Read byte from the receive buffer.
    *Buffer = SerialPortReadRegister (SerialRegisterBase, R_UART_RXBUF);
  }
  
  return Result;
}

int 
GetSerialData (
    int*    ReadData 
)
{
	int    Result;
    float    MaxDelay = 0x1046A;

    while(!SerialPortPoll() && MaxDelay)
        MaxDelay--;

    if ( MaxDelay <= 0 ) {
        return 0;
    }
	
    Result = SerialPortRead(ReadData, 0x01);

    if ( Result < 0x01 ) {
        return 0;
    } else {
        return 1;
    }
}

void 
SendSerialData (
    int    data
)
{
    //SerialPortWrite(&data, 0x01 );
}

int
SerialPortWrite (
  int    *Buffer,
  int     NumberOfBytes
  )
{
  int  SerialRegisterBase;
  int  Result;
  int  Index;
  int  FifoSize;
  float i = 0;

  if (Buffer == NULL) {
    return 0;
  }

  SerialRegisterBase = LCM_BASE_ADDRESS;

  
  if (NumberOfBytes == 0) {
    while ((SerialPortReadRegister (SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)){
        if (++i == 0xffff) return 0;
    }

    return 0;
  }

  // Compute the maximum size of the Tx FIFO
  FifoSize = 1;
  if ((PcdSerialFifoControl & B_UART_FCR_FIFOE) != 0) {
    if ((PcdSerialFifoControl & B_UART_FCR_FIFO64) == 0) {
      FifoSize = 16;
    } else {
      FifoSize = PcdSerialExtendedTxFifoSize;
    }
  }

  Result = NumberOfBytes;
  while (NumberOfBytes != 0) {

    i = 0;
    while ((SerialPortReadRegister (SerialRegisterBase, R_UART_LSR) & B_UART_LSR_TEMT) == 0){
        if (++i == 0xffff) return 0;
    }


    // Fill then entire Tx FIFO
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--, Buffer++) {

      // Write byte to the transmit buffer.
      SerialPortWriteRegister (SerialRegisterBase, R_UART_TXBUF, *Buffer);
    }
  }
  return Result;
}

void LCM_init(void)
{

  int   SerialRegisterBase;
  int   Divisor;
  int   CurrentDivisor;
  int   Initialized;

  Divisor = 0x06; //19200


  // Get the base address of the serial port in either I/O or MMIO space
  SerialRegisterBase = LCM_BASE_ADDRESS;

  // See if the serial port is already initialized
  Initialized = 1;
  if ((SerialPortReadRegister (SerialRegisterBase, R_UART_LCR) & 0x3F) != (PcdSerialLineControl & 0x3F)) {
    Initialized = 0;
  }
  SerialPortWriteRegister (SerialRegisterBase, R_UART_LCR, (int)(SerialPortReadRegister (SerialRegisterBase, R_UART_LCR) | B_UART_LCR_DLAB));
  CurrentDivisor =  SerialPortReadRegister (SerialRegisterBase, R_UART_BAUD_HIGH) << 8;
  CurrentDivisor |= (int) SerialPortReadRegister (SerialRegisterBase, R_UART_BAUD_LOW);
  SerialPortWriteRegister (SerialRegisterBase, R_UART_LCR, (int)(SerialPortReadRegister (SerialRegisterBase, R_UART_LCR) & ~B_UART_LCR_DLAB));
  if (CurrentDivisor != Divisor) {
    Initialized = 0;
  }
  if (Initialized) {
	//printf("Initialized\n");
    return;
  }

  // Wait for the serial port to be ready.
  // Verify that both the transmit FIFO and the shift register are empty.
  while ((SerialPortReadRegister (SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY));

  // Configure baud rate
  SerialPortWriteRegister (SerialRegisterBase, R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister (SerialRegisterBase, R_UART_BAUD_HIGH, (int) (Divisor >> 8));
  SerialPortWriteRegister (SerialRegisterBase, R_UART_BAUD_LOW, (int) (Divisor & 0xff));

  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from PcdSerialLineControl
  SerialPortWriteRegister (SerialRegisterBase, R_UART_LCR, (int)(PcdSerialLineControl & 0x3F));

  // Enable and reset FIFOs
  // Strip reserved bits from PcdSerialFifoControl
  SerialPortWriteRegister (SerialRegisterBase, R_UART_FCR, 0x00);
  SerialPortWriteRegister (SerialRegisterBase, R_UART_FCR, (int)(PcdSerialFifoControl & (B_UART_FCR_FIFOE | B_UART_FCR_FIFO64)));

  // Put Modem Control Register(MCR) into its reset state of 0x00. 
  SerialPortWriteRegister (SerialRegisterBase, R_UART_MCR, 0x00);
  
  //printf("success\n");
  return;
}

void LCM_SHOW(char *text)
{
/********************Show Text on LCM module screen********************************/
//int ascii = 0x41,i = 0;

	int i=0;
// Send ASCII to COM port
//        printk("%s   ioaccess debug text:\n", text);
//        printk("%d   ioaccess debug input\n", input);
	while(text[i] != 0x00 && i < 16){
  	//outportb(input, (int)text[i]); //
	SendSerialData((int)text[i]);
  	//delay(10); 	
  	i++;
	}   
	
}
int LCM_BTN(void){
/********************Test LCM module button function******************************/
	int read;
	while(1){

		GetSerialData(&read);
		
		if(read == 0x41){ //A
			//printf("Got A key\n");
			break;
		}
		else if(read == 0x42){ //B
			//printf("Got B key\n");
			break;
		}
		else if(read == 0x43){ //C
			//printf("Got C key\n");
			break;
		}
		else if(read == 0x44){ //D
			//printf("Got D key\n");
			break;
		}	
		
	}
}
void LCM_CLR(void){
/********************Clear LCM Screen******************************/	
  /*outportb(input, 0xFE ); //clear screen
  delay(10);
  outportb(input, 0x58 ); //
  delay(10);
  outportb(input, 0xFD ); //
  delay(10); */
  
  SendSerialData(0xFE);
  SendSerialData(0x58);
  SendSerialData(0xFD);
}
void LCM_INS(void){
/********************Set text insertion point***********************/	
/*	outportb(input, 0xFE ); //Set text insertion point
  delay(10);
  outportb(input, 0x47 ); //
  delay(10);
  outportb(input, 0x00 ); //col posion
  delay(10);
  outportb(input, 0x01 ); //row posion
  delay(10);    
  outportb(input, 0xFD ); //
  delay(10); */
  SendSerialData(0xFE);
  SendSerialData(0x47);
  SendSerialData(0x00);
  SendSerialData(0x01);
  SendSerialData(0xFD);
  
}
#endif
