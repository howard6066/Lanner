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




 
#define   INDEX_PORT	       (0x2E)
#define   DATA_PORT	       (0x2F)
#define   LDN_INDEX                (0x07)
#define   GPIO_INDEX              (0x06)
#define   F75111_ADD_9C       (0x9c)
#define   F75111_ADD_6E       (0x6e)
#define   PCA9545_ADD_E6    (0xe6)
#define   UPPER_CARD             (0x02)
#define   LOWER_CARD            (0x01)
#define   PCI_INDEX               (0xcf8)
#define   PCI_DATA                 (0xcfc)
#define GPIO1X_OUTPUT_CONTROL_REG  (0x10)
#define GPIO1X_OUTPUT_DATA_REG         (0x11)
#define GPIO2X_OUTPUT_CONTROL_REG  (0x20)
#define GPIO2X_OUTPUT_DATA_REG         (0x21) 

static int smbase;
static char RiserSts_flag = 0;
unsigned long  gComPortBase1 = 0;   //upper io card
unsigned long  gComPortBase2 = 0;   //lower io card

#if defined(LINUX_ENV)
void SetRS485AutoEn(unsigned char ComNo, unsigned char En, unsigned char IoCard)
{
    if(RiserSts_flag <3 && En == 1){
        outportb(0xb3, ComNo);
        outportb(0xb2, 0x70);	
    }
    if(RiserSts_flag < 3 && En == 0){
        outportb(0xb3, ComNo);
        outportb(0xb2, 0x71);	
    }
    if(RiserSts_flag == 3 && En == 1){
            if(IoCard == UPPER_CARD){
		outportb(0xb3, ComNo);
                  outportb(0xb2, 0x70);
            }else{
                  outportb(0xb3, ComNo);
                  outportb(0xb2, 0x72);
            }
    }
     if(RiserSts_flag == 3 && En == 0){
            if(IoCard == UPPER_CARD){
		outportb(0xb3, ComNo);
                  outportb(0xb2, 0x71);
            }else{
                  outportb(0xb3, ComNo);
                  outportb(0xb2, 0x73);
            }	
    }		 
    	
}
#endif

unsigned short get_smbase(void)
{
		outportl(PCI_INDEX,(0x80000000+(0<<16)+(31<<11)+(3<<8)+0x20));//smbase dev31 fun3 rx21-20		  	
		return ((inportw(PCI_DATA))&0xfffe);
}
//============================================================================================
void write_byte(int cslaveadd,int byte_c1,int byted1)
{
		outportb(smbase,0xff);//clr status
		delay(5);
		outportb(smbase+3,byte_c1);//byte N
		delay(5);
		outportb(smbase+4,cslaveadd&0xfe);//slave address and write
		delay(5);
		outportb(smbase+5,byted1);//byte data access
		delay(5);
		outportb(smbase+2,0x48);//byte data access
		delay(10);
		if(((inportb(smbase))&0x04)==0x4)//check status fail?
		{//printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	   // exit(0);
		}
}  
//=============================================================================================  
unsigned char read_byte(int cslaveadd,int byte_c)
{
		outportb(smbase,0xff);//clr status
		delay(5);
		// outportb(smbase+2,0x02);	
		outportb(smbase+3,byte_c);//byte N
		delay(5);
		outportb(smbase+4,cslaveadd|0x01);//slave address and raed
		delay(5);
		outportb(smbase+2,0x48);//byte data access
		delay(10);
		if(((inportb(smbase))&0x04)==0x4)//check status fail?
		{//printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
		 // exit(0);
		}
		return(inportb(smbase+5));//byte data access
}    	

void set_PCA9548_channel(unsigned char add){
		write_byte(PCA9545_ADD_E6,0x00, add);
}	

unsigned char read_f75111_6E_reg(unsigned char reg){
		return (read_byte(F75111_ADD_6E,reg));
}

void write_f75111_6E_reg(unsigned char reg, unsigned char val){
		write_byte(F75111_ADD_6E,reg, val);
}


unsigned char read_f75111_9C_reg1(unsigned char reg){
		return (read_byte(F75111_ADD_9C,reg));
}

void write_f75111_9C_reg1(unsigned char reg, unsigned char val){
		write_byte(F75111_ADD_9C,reg, val);
}	

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

unsigned char read_SIO_reg(int reg)
{	
        outportb(INDEX_PORT, LDN_INDEX); //LDN register
        delay(5);
        outportb(DATA_PORT, GPIO_INDEX);
        delay(5);
        outportb(INDEX_PORT, reg);
        delay(5);
        return(inportb(DATA_PORT));
}


void write_SIO_reg(int reg, int value)
{

        outportb(INDEX_PORT, LDN_INDEX); //LDN register
        delay(5);
        outportb(DATA_PORT, GPIO_INDEX);
        delay(5);
        outportb(INDEX_PORT, reg);
        delay(5);
        outportb(DATA_PORT, value);
        return;
}

void SetSuperIoMultiFunction(void)
{
    int value=0;

    value = read_SIO_reg(0x26);
    value &= 0xfe;
    write_SIO_reg(0x26, value);
    value = read_SIO_reg(0x2a);
    value |= 0x08;
    write_SIO_reg(0x2a, value);
    value = read_SIO_reg(0x26);
    value |= 0x01;
    write_SIO_reg(0x26, value);
    value = read_SIO_reg(0x2a);
    value &= 0x87;	
    write_SIO_reg(0x2a, value);	
}

void SetF75111MultiFunctionAndGPIO(void)
{
    unsigned char	       SMdata;

//set f75111 (0x9c) multifunction
	    SMdata = read_f75111_9C_reg1(0x04);
	    SMdata &= 0x61;
	    write_f75111_9C_reg1(0x04, SMdata);
	    SMdata = read_f75111_9C_reg1(0x05);
	    SMdata &= 0xf0;
	    write_f75111_9C_reg1(0x05, SMdata);
//set f75111 (0x9c) gpio  17 , 20~27 to output	
             SMdata = read_f75111_9C_reg1(GPIO1X_OUTPUT_CONTROL_REG );
             SMdata |= 0x80;
	    write_f75111_9C_reg1(GPIO1X_OUTPUT_CONTROL_REG, SMdata);
	    SMdata = 0xff;
	    write_f75111_9C_reg1(GPIO2X_OUTPUT_CONTROL_REG, SMdata);
//set f75111 (0x6e) multifunction	
             SMdata = read_f75111_6E_reg(0x03);
	    SMdata &= 0xe0;
	    write_f75111_6E_reg(0x03, SMdata);
	    SMdata = read_f75111_6E_reg(0x04);
	    SMdata &= 0x00;
	    write_f75111_6E_reg(0x04, SMdata);
	    SMdata = read_f75111_6E_reg(0x05);
	    SMdata &= 0xf0;
	    write_f75111_6E_reg(0x05, SMdata);
//set f75111 (0x6e) gpio 10~13 , 14~17 , 20~27 to output
             SMdata = read_f75111_6E_reg(GPIO1X_OUTPUT_CONTROL_REG );
             SMdata |= 0xf7;
	    write_f75111_6E_reg(GPIO1X_OUTPUT_CONTROL_REG, SMdata);
	    SMdata = 0xff;
	    write_f75111_6E_reg(GPIO2X_OUTPUT_CONTROL_REG, SMdata); 
}

unsigned long DetectRiserCard(unsigned char bus, unsigned char dev, unsigned char fun)
{
    outportl(0xcf8,(0x80000000+(bus<<16)+(dev<<11)));
    return( inportl(0xcfc));
}

unsigned long ReturnRiserStsFlag(void)
{
    return  RiserSts_flag;
}

int cp_gpio_init(void)
{
         int                             value=0;
	

	enter_SIO_config();
//enable SuperIo gpio
         write_SIO_reg(0x30, 0x01);
//set Super Io MultiFunction	
         SetSuperIoMultiFunction();
//set Super Io gpio 50~55 to output mode
	value = read_SIO_reg(0xa0);
	value |= 0x3f;
	write_SIO_reg(0xa0, value);
	exit_SIO_config();  
      
         smbase = get_smbase();
	
//detect how many Io card
         if(DetectRiserCard(LOWER_CARD, 0, 0) == 0x035813a8)
             RiserSts_flag |= 1;
	if(DetectRiserCard(UPPER_CARD, 0, 0) == 0x035813a8)	 
             RiserSts_flag |= 2;

// no io card
         if(RiserSts_flag == 0)           
		return 0;		
         else if(RiserSts_flag == 1){		 //lower io card	
             write_byte(PCA9545_ADD_E6, 0x00, 0x02);
	    SetF75111MultiFunctionAndGPIO();
         } else if(RiserSts_flag == 2){          //upper io card
             write_byte(PCA9545_ADD_E6, 0x00, 0x04);
	    SetF75111MultiFunctionAndGPIO();
         } else {                                                  //both upper card and lower card have be plug in  
             write_byte(PCA9545_ADD_E6, 0x00, 0x02);
	    SetF75111MultiFunctionAndGPIO();
	    write_byte(PCA9545_ADD_E6, 0x00, 0x04);
	    SetF75111MultiFunctionAndGPIO();	
         }
	write_byte(PCA9545_ADD_E6, 0x00, 0x00);	 	  	

	return 0;

}		

unsigned long SetComPort(int value)
{
    int Type = 0;
    int port = 0;
    int mode = 0;
    int termination = 0;
    unsigned short data;	

    Type = (value & 0x03);
    port = (value & 0x1c) >> 2;
    mode = (value & 0x60) >>  5;
    termination = (value & 0x80) >> 7;

//set setup variable
    outportb(0xb3, value);
    outportb(0xb2, 0x74);

    switch(Type)
    {
//super io    
         case 0:
	    enter_SIO_config();	 	
	    switch(port)
	    {
                  case 0:
		    switch(mode)
		    {
                           case 0:
			    write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xfc) | 0x01);
		             break;
			case 1:
                               write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xfc) | 0x02);
		             break;	
		         case 2:
                               write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xfc) | 0x03);
		             break;			 
		    } 
                      switch(termination)
                      {
                           case 0:
			    write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xfb) | 0x00);
			    break;
			case 1:
			    write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xfb) | 0x04);
			    break;				
                      } 
		    break;  //port 0			  
                  case 1:
		    switch(mode)
		    {
                           case 0:
			    write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xe7) | 0x08);
		             break;
			case 1:
                               write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xe7) | 0x10);
		             break;	
		         case 2:
                               write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xe7) | 0x18);
		             break;			 
		    }
                      switch(termination)
                      {
                           case 0:
			    write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xdf) | 0x00);
			    break;
			case 1:
			    write_SIO_reg(0xa1, (read_SIO_reg(0xa1) & 0xdf) | 0x20);
			    break;	
                      }
		    break; //port 1			  
	    }
	    exit_SIO_config();	
	    break;
//lower card		
	case 1:
	    set_PCA9548_channel(0x02);
	    switch(port)
	    {
                 case 0:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(0, 0, LOWER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x80;
			    write_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfe;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	
		             break;
			case 1:
#if defined(LINUX_ENV)
                               SetRS485AutoEn(0, 1, LOWER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG);
			    data &= 0x7f;
			    write_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x01;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	
		             break;	
		         case 2:
#if defined(LINUX_ENV)				 	
                               SetRS485AutoEn(0, 1, LOWER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x80;
			    write_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x01;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
		             break;			 
		    } 
                      switch(termination)
                      {
                           case 0:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfd;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
			    break;
			case 1:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x02;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
			    break;				
                      } 		 	
		    break;  // port 0
	        case 1:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)
			    SetRS485AutoEn(1, 0, LOWER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xf7;
			    data |= 0x04;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	   	
                               break; 
			case 1:
#if defined(LINUX_ENV)				
			    SetRS485AutoEn(1, 1, LOWER_CARD); 
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfb;
			    data |= 0x08;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
                               break;
			case 2:
#if defined(LINUX_ENV)				
			    SetRS485AutoEn(1, 1, LOWER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x0c;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	
			    break;	
		    }
	             switch(termination)
	             {
                           case 0:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xef;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);				   	
			    break;
			case 1:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x10;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
	             }
		    break; // port 1		    
	        case 2:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(2, 0, LOWER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xbf;
			    data |= 0x20;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);			   	
			    break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(2, 1, LOWER_CARD);
#endif
                               data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xdf;
			    data |= 0x40;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(2, 1, LOWER_CARD);
#endif
                               data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x60;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);						
			    break;	
		    }
		    switch(termination)
		    {
                           case 0:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0x7f;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x80;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }
		    break;  //port 2
	        case 3:
		    switch(mode)
		    {
		        case 0:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(3, 0, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xfd;
		             data |= 0x01;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			     break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(3, 1, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xfe;
		             data |= 0x02;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(3, 1, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
		             data |= 0x03;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);				
			    break;	
		    }
		     switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data &= 0xfb;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x04;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);		
			    break;	
		    }	
		    break; //port3
	        case 4:
		    switch(mode)
		    {
		         case 0:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(4, 0, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xdf;
		             data |= 0x10;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			     break;
		        case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(4, 1, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xef;
		             data |= 0x20;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    break;
		        case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(4, 1, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
		             data |= 0x30;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);				
			    break;	
		    }
		     switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data &= 0xbF;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x40;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);		
			    break;	
		    }			
		    break; //port4
	        case 5:
		    switch(mode)
		    {
		         case 0:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(5, 0, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data |= 0x80;
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);	
		             data &= 0xfe;		 
	                      write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);
			     break;
		        case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(5, 1, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0x7f;
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);	
		             data |= 0x01;		 
	                      write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);
			    break;
		        case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(5, 1, LOWER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data |= 0x80;
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);	
		             data |= 0x01;		 
	                      write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);			
			    break;	
		    }
		     switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfd;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x02;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }					
		    break; //port 5
	        case 6:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(6, 0, LOWER_CARD);
#endif
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xf7;
			    data |= 0x04;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);			   	
			    break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(6, 1, LOWER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfb;
			    data |= 0x08;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(6, 1, LOWER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x0c;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);						
			    break;	
		    }
		    switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xef;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x10;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }		
		    break;  //port 6
	        case 7:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(7, 0, LOWER_CARD);
#endif
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xbf;
			    data |= 0x20;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);			   	
			    break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(7, 1, LOWER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xdf;
			    data |= 0x40;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(7, 1, LOWER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x60;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);						
			    break;
		    }
		    switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0x7f;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x80;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }				
		    break;			
	    }	    
	    break;
//upper card		
	case 2:
	    set_PCA9548_channel(0x04);
	    switch(port)
	    {
                 case 0:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(0, 0, UPPER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x80;
			    write_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfe;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	
		             break;
			case 1:
#if defined(LINUX_ENV)
                               SetRS485AutoEn(0, 1, UPPER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG);
			    data &= 0x7f;
			    write_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x01;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	
		             break;	
		         case 2:
#if defined(LINUX_ENV)				 	
                               SetRS485AutoEn(0, 1, UPPER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x80;
			    write_f75111_9C_reg1(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x01;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
		             break;			 
		    } 
                      switch(termination)
                      {
                           case 0:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfd;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
			    break;
			case 1:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x02;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
			    break;				
                      } 		 	
		    break;  // port 0
	        case 1:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(1, 0, UPPER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xf7;
			    data |= 0x04;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	   	
                               break; 
			case 1:
#if defined(LINUX_ENV)				
			    SetRS485AutoEn(1, 1, UPPER_CARD); 
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfb;
			    data |= 0x08;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);
                               break;
			case 2:
#if defined(LINUX_ENV)				
			    SetRS485AutoEn(1, 1, UPPER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x0c;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);	
			    break;	
		    }
	             switch(termination)
	             {
                           case 0:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xef;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);				   	
			    break;
			case 1:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x10;
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
	             }
		    break; // port 1		    
	        case 2:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(2, 0, UPPER_CARD);
#endif
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xbf;
			    data |= 0x20;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);			   	
			    break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(2, 1, UPPER_CARD);
#endif
                               data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xdf;
			    data |= 0x40;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(2, 1, UPPER_CARD);
#endif
                               data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x60;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);						
			    break;	
		    }
		    switch(termination)
		    {
                           case 0:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0x7f;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x80;	
			    write_f75111_9C_reg1(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }
		    break;  //port 2
	        case 3:
		    switch(mode)
		    {
		        case 0:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(3, 0, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xfd;
		             data |= 0x01;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			     break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(3, 1, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xfe;
		             data |= 0x02;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(3, 1, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
		             data |= 0x03;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);				
			    break;	
		    }
		     switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data &= 0xfb;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x04;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);		
			    break;	
		    }	
		    break; //port3
	        case 4:
		    switch(mode)
		    {
		         case 0:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(4, 0, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xdF;
		             data |= 0x10;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			     break;
		        case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(4, 1, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0xef;
		             data |= 0x20;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    break;
		        case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(4, 1, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
		             data |= 0x30;		 
	                      write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);				
			    break;	
		    }
		     switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data &= 0xbf;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
			    data |= 0x40;	
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);		
			    break;	
		    }			
		    break; //port4
	        case 5:
		    switch(mode)
		    {
		         case 0:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(5, 0, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data |= 0x80;
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);	
		             data &= 0xfe;		 
	                      write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);
			     break;
		        case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(5, 1, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data &= 0x7f;
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);	
		             data |= 0x01;		 
	                      write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);
			    break;
		        case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(5, 1, UPPER_CARD);
#endif				
                               data = read_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG);
	                      data |= 0x80;
			    write_f75111_6E_reg(GPIO1X_OUTPUT_DATA_REG, data);
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);	
		             data |= 0x01;		 
	                      write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);			
			    break;	
		    }
		     switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfd;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x02;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }					
		    break; //port 5
	        case 6:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(6, 0, UPPER_CARD);
#endif
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xf7;
			    data |= 0x04;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);			   	
			    break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(6, 1, UPPER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xfb;
			    data |= 0x08;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(6, 1, UPPER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x0c;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);						
			    break;	
		    }
		    switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xef;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x10;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }		
		    break;  //port 6
	        case 7:
		    switch(mode)
		    {
                           case 0:
#if defined(LINUX_ENV)						   	
			    SetRS485AutoEn(7, 0, UPPER_CARD);
#endif
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xbf;
			    data |= 0x20;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);			   	
			    break;
			case 1:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(7, 1, UPPER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0xdf;
			    data |= 0x40;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;
			case 2:
#if defined(LINUX_ENV)	
			    SetRS485AutoEn(7, 1, UPPER_CARD);
#endif
                               data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x60;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);						
			    break;
		    }
		    switch(termination)
		    {
                           case 0:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data &= 0x7f;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);					   	
			    break;
			case 1:
			    data = read_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG);
			    data |= 0x80;	
			    write_f75111_6E_reg(GPIO2X_OUTPUT_DATA_REG, data);		
			    break;	
		    }				
		    break;	//port 7		
	    }	    
	    break;	
    }
    write_byte(PCA9545_ADD_E6, 0x00, 0x00);	
    return 0;	
}

unsigned long ShowComPort(int value)
{
    outportb(0xb3, value);
    outportb(0xb2, 0x76);

    value = inportb(0xb3);
    outportb(0x80, value);	
    return value;
}

#endif
