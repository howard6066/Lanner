/*******************************************************************************

  ioaccess.c: IO access code for Lanner Platfomr EEPROM program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010- 2011 Lanner Electronics Inc.
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
//v1.2 #define delay(x) usleep(x)
#define delay(x) usleep(x*100)		//v1.2
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
#undef sleep
#define sleep(x) delay(1000*x)
#endif

#endif



/* local include file */
#include "../include/ioaccess.h"

#if (defined(MODULE) || defined(DIRECT_IO_ACCESS) || defined(KLD_MODULE))



/*
 * Device Depend Definition : 
*/
#define 		SIO_INDEX	0x2E
#define 		SIO_DATA	0x2F
#define 		PCI_INDEX	0xcf8
#define 		PCI_DATA	0xcfc
#define 		IPMI_CMD_DELAY	100
#define 		KCS_IOPORT		0xCA2
#define 		MB_EEPROM_CH		0x10
#define			MB_PCA9548_ADD  	0xE4//Mother Board PCA9548 smbus switch slave address 
#define			PCA9545_addr    0xE6
static int		smb_err;
static int 	smbase; 
static int 	IpmiPresent; 
//define wich GPI


unsigned int get_smbase(void)
{
outportl(PCI_INDEX,(0x80000000+(0<<16)+(31<<11)+(4<<8)+0x20));//smbase dev31 fun4 rx21-20
return ((inportw(PCI_DATA))&0xfffe);
}


//v1.2 int Detect_IPMI(void){
void Detect_IPMI(void){
	outportb(SIO_INDEX,0x87);//Enter SIO 
	outportb(SIO_INDEX,0x87);
	outportb(SIO_INDEX,0x07);//LDN 7
	outportb(SIO_DATA,0x07);	
	outportb(SIO_INDEX,0xE1);//RX_E1
	if ((inportb(SIO_DATA)&0x10)==0x10){IpmiPresent=0x00;}
	else{IpmiPresent=0x01;}
	outportb(SIO_INDEX,0xAA);//exit SIO

}


void eep_gpio_init(void)
{	smbase=get_smbase();
	Detect_IPMI();
        return;
}


//============================================================================================
void Write_SMB_Byte(int reg, int value,int sm_addr)
{
	int	i;
	smb_err=0x00; //clr status flag
	for(i=0x00;i<100;i++){	
	outportb(smbase,0xfe);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==99){smb_err=0x01;}
	}    	
      outportb(smbase+3,reg);//byte N
    	delay(5);
    	outportb(smbase+4,sm_addr&0xfe);//slave address and write
    	delay(5);
    	outportb(smbase+5,value);//byte data access
    	delay(5);
    	outportb(smbase+2,0x48);//byte data access
    	delay(20);
    	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {//printf("Slave Address=%x  device not exist or smbus access error!!\n",sm_addr);
  	   smb_err=0x02;
  	   }
 	return;  	   
}  
//=============================================================================================  
char Read_SMB_Byte(int cslaveadd,int byte_c)
{
	int	i;
	smb_err=0x00; //clr status flag
   for(i=0x00;i<100;i++){	
	outportb(smbase,0xfe);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==99){smb_err=0x01;}
	}    
   // outportb(smbase+2,0x02);	
    	outportb(smbase+3,byte_c);//byte N
    	delay(5);
    	outportb(smbase+4,cslaveadd|0x01);//slave address and raed
    	delay(5);
    	outportb(smbase+2,0x48);//byte data access
    	delay(20);
     	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {//printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	    smb_err=0x02;
  	   }
    return(inportb(smbase+5));//byte data access
}    	


void wait_for_IBF_clear(int delay_count){
	int i=0;
	for(i=0;i<delay_count;i++){
		if ((inportb(KCS_IOPORT+1)&0x02)!=0x02)break;	
		delay(20);
	}	
}
void clear_OBF(void){
	int i=0;
	i=inportb(KCS_IOPORT);
}
void	opma_smbus(int value){
	if(IpmiPresent==0x00){return ;}//check IPMI present
	wait_for_IBF_clear(IPMI_CMD_DELAY);
	clear_OBF();
	//Set write start
	outportb(KCS_IOPORT+1,0x61);
	wait_for_IBF_clear(IPMI_CMD_DELAY);
	clear_OBF();
	 //Set netfn 
	outportb(KCS_IOPORT,0xb8);
	wait_for_IBF_clear(IPMI_CMD_DELAY);
	clear_OBF();
	//Set I2C OFF
	outportb(KCS_IOPORT,0x06);
	wait_for_IBF_clear(IPMI_CMD_DELAY);
	clear_OBF();
	outportb(KCS_IOPORT,value);
	wait_for_IBF_clear(IPMI_CMD_DELAY);
	clear_OBF();
	//Set write end
	outportb(KCS_IOPORT+1,0x62);
	wait_for_IBF_clear(IPMI_CMD_DELAY);
	clear_OBF();
	//wait some delay for IPMI finish program 
	delay(500);//wait for ipmi stable
	return ;
} 

void smbus_ON(void){
	opma_smbus(0x05);//tell ipmi do not touch smbus
	delay(500);//wait for ipmi stable
 	Write_SMB_Byte(0x00,0x04,PCA9545_addr); //enable PCA9548 ch3 for F75111

}

void smbus_OFF(void){
	Write_SMB_Byte(0x00,0x00,PCA9545_addr); //Disable PCA9548 all ch
	opma_smbus(0x04);//

}


void eep_set_output(unsigned int REG_DATA)
{
	

	//opma_smbus(0x05);//tell ipmi do not touch smbus
	//delay(500);//wait for ipmi stable
   
    	Write_SMB_Byte((REG_DATA>>8)&0xff,REG_DATA&0xff,MB_EEPROM_ADD); 
	//Write_SMB_Byte(0x00,0x00,MB_PCA9548_ADD); //Disable PCA9548 all ch
	//opma_smbus(0x04);//

        return;  
}

char eep_get_input(unsigned int REG)
{
	int	Input_Value=0x00;
	//opma_smbus(0x05);//tell ipmi do not touch smbus
//	delay(500);//wait for ipmi stable
 //   	Write_SMB_Byte(0x00,0x10,MB_PCA9548_ADD); //enable PCA9548 ch4 for EEPROM
    	Input_Value=Read_SMB_Byte(MB_EEPROM_ADD,REG);

  //  	Write_SMB_Byte(0x00,0x00,MB_PCA9548_ADD); //Disable PCA9548 all ch
//	opma_smbus(0x04);//
        return Input_Value;
}


//===============================================================================================
#endif
