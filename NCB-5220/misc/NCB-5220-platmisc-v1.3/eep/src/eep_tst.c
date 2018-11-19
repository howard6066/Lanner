/*******************************************************************************

  eep_tst.c: main application for Lanner platform EEPROM program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010 - 2011 Lanner Electronics Inc.
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
#include "../include/version.h"

/* standard include file */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DJGPP

/* For DOS DJGPP */
#include <dos.h>
#include <inlines/pc.h>

#ifndef DIRECT_IO_ACCESS
#error ***Error: define DIRECT_IO_ACCESS in config.h for DOS ***
#endif

#else

/* For Linux */
#if defined(LINUX_ENV)
#include <sys/io.h>
#include <sys/ioctl.h> 	//v1.2
#endif

#if defined(FreeBSD_ENV)
#define SET_IOPL() (iofl=open("/dev/io",000))
#define RESET_IOPL() close(iofl)
#include <machine/cpufunc.h>
#endif

#include <time.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "../include/eep_ioctl.h"

#define delay(x) usleep(x)
#define sleep(x) usleep(x*1000000)
#ifdef DIRECT_IO_ACCESS
#warning ***** Note: You build with DIRECT_IO_ACCESS defined *****
#warning ***** Note: undefine this to build for driver code *****
#endif

#endif

/* local include file */
#include "../include/ioaccess.h"


void help(void){
		fprintf(stdout, "Usage:\n");
		fprintf(stdout, "  eep_tst w/r (write/read EEPROM)\n\n");
		exit(1);
		}

void	convert(char *tempdata1){
	int i;
	int  Item_Number=0x01;

        #ifdef Item1_string
        printf("%s",Item1_string);
        #else
        printf("Item 1:");
        #endif
        for (i=0;*(tempdata1+i) !=  END ;i++){
                if (*(tempdata1+i)== SEPARATE ){
                        Item_Number+=1;
                        switch (Item_Number){
                                case 2:
                                     #ifdef Item2_string
                                        printf("\n%s",Item2_string);
                                     #else
                                        printf("\nItem %d:",Item_Number);
                                     #endif
                                        break;
                                case 3:
                                     #ifdef Item3_string
                                        printf("\n%s",Item3_string);
                                     #else
                                        printf("\nItem %d:",Item_Number);
                                     #endif
                                        break;
                                case 4:
                                     #ifdef Item4_string
                                        printf("\n%s",Item4_string);
                                     #else
                                        printf("\nItem %d:",Item_Number);
                                     #endif
                                        break;
                                case 5:
                                     #ifdef Item5_string
                                        printf("\n%s",Item5_string);
                                     #else
                                        printf("\nItem %d:",Item_Number);
                                     #endif
                                        break;                                                     
                                case 6:
                                     #ifdef Item6_string
                                       printf("\n%s",Item6_string);
                                     #else
                                        printf("\nItem %d:",Item_Number);
                                     #endif
                                        break;
                                }
                        continue;
                       }
               printf("%c",*(tempdata1+i));
        }
        printf("\n");
}


int main( int argc, char** argv )
{
        	int devfd;
        	int value, ret,i;
		char c,tempdata[EEPRPM_SIZE];
		FILE *fp;
		char tempdataA[EEPRPM_SIZE];
		char s[50] = "please put ";


#if ( defined(DIRECT_IO_ACCESS) && !defined(DJGPP) ) 
#if defined(LINUX_ENV)
	iopl(3);
#endif
#if (defined(FreeBSD_ENV))
       int iofl;
       SET_IOPL();
#endif
#endif

	fprintf(stdout, "=== Lanner platform miscellaneous utility ===\n");;
	fprintf(stdout, PLATFORM_NAME" EEPROM "CODE_VERSION"\n\n");;

	if (argc !=2){ help();}	
	
#ifdef DIRECT_IO_ACCESS
//==Dos mode==
	eep_gpio_init();	//v1.2
#if (defined(FreeBSD_ENV))
		RESET_IOPL();
#endif

#else
//==Linux Mode
        devfd = open("/dev/eep_drv", O_RDONLY);
        if(devfd == -1)
        {
                printf("Can't open /dev/eep_drv\n");
                return -1;
        }
#endif

//;slash-add
if (( !strcmp(argv[1], "w"))||( !strcmp(argv[1], "W"))) {	
//Read input file========================
if ((fp=fopen(FILENAME,"r+"))==NULL){
        		printf("%s\n", s);//file exist?
                	perror(FILENAME);
                	exit(1);
		}

        	printf("Read input file data:");
        
		for(i=0;i<EEPRPM_SIZE;i++)
        	{
         		tempdata[i]=getc(fp);
         		printf("%c",tempdata[i]);
         		if (tempdata[i]== END ) break;
        	}
        	printf("\n");
        	fclose(fp);


	#ifdef DIRECT_IO_ACCESS
	//Dos Mode
	smbus_ON();//enable smbus channel
		for(i = 0; i<EEPRPM_SIZE; i++){
			Write_SMB_Byte(i,tempdata[i],MB_EEPROM_ADD);
			if (tempdata[i]== END ) break;
		}

	smbus_OFF();//disable smbus channel
	#else
	//Linux <ode
		//==enable smbus channel==
  		value=0;
		ret = ioctl(devfd,IOCTL_EEP_SET_SMBUS_ON, &value);
		//==Write eeprom==     
		printf("Write EEPROM:");
		for(i = 0; i<EEPRPM_SIZE; i++){
			value = (i<<8)|(tempdata[i]);	
			//printf("Input Byte:%x  Data:%x\n",(value>>8)&0xff, value &0xff);//debug
		      	ret = ioctl(devfd, IOCTL_EEP_SET_OUTPUT, &value);
			printf(".");
			if (tempdata[i]== END ) break;
		}
	//==disable smbus channel==
		value=0;
        	ret = ioctl(devfd,IOCTL_EEP_SET_SMBUS_OFF, &value); 	
	#endif
	printf("Done\n");
}	
else if (( !strcmp(argv[1], "r"))||( !strcmp(argv[1], "R"))) {	
	#ifdef DIRECT_IO_ACCESS
	//Dos Mode	
	smbus_ON();//enable smbus channel
	//==Read eeprom== 
        	printf("<<<< Read EEPROM >>>>\n");
		for(i = 0; i<EEPRPM_SIZE; i++){
			
			//v1.2 tempdataA[i]=Read_SMB_Byte(i,MB_EEPROM_ADD);
			tempdataA[i]=Read_SMB_Byte(MB_EEPROM_ADD,i);	//v1.2
			// printf("Read Byte:%x  Data:%x\n",i, value);//debug
         			if (tempdataA[i]== END ) break;
		}		
		
	smbus_OFF();//disable smbus channel

	#else
  	//Linux Mode
	//==enable smbus channel==
  		value=0;
		ret = ioctl(devfd,IOCTL_EEP_SET_SMBUS_ON, &value);
//==Read eeprom== 
        	printf("<<<<Read back from EEPROM>>>>\n");
		for(i = 0; i<EEPRPM_SIZE; i++){
			value=i;
	     		ret = ioctl(devfd, IOCTL_EEP_GET_INPUT, &value);
			tempdataA[i]=value;
			// printf("Read Byte:%x  Data:%x\n",i, value);//debug
         			if (tempdataA[i]== END ) break;
		}		
		
		
	//==disable smbus channel==
		value=0;
        	ret = ioctl(devfd,IOCTL_EEP_SET_SMBUS_OFF, &value); 		
	#endif
	convert(tempdataA);//Print Read Back content
}
else{	help(); }
//;slash-end 
	
#ifdef DIRECT_IO_ACCESS

#if (defined(FreeBSD_ENV))
       RESET_IOPL();
#endif

#else
        close(devfd);
#endif
        return 0;

}

