/*******************************************************************************

  lcm_tst.c: main application for Lanner platform LCM program

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2010 - 2017 Lanner Electronics Inc.
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
#include "../include/lcm_ioctl.h"
#include <sys/ioctl.h>

#define delay(x) usleep(x)
#define sleep(x) usleep(x*1000000)
#ifdef DIRECT_IO_ACCESS
#warning ***** Note: You build with DIRECT_IO_ACCESS defined *****
#warning ***** Note: undefine this to build for driver code *****
#endif

#endif

/* local include file */
#include "../include/ioaccess.h"


int main( int argc, char** argv )
{
        int devfd, i;
        int value, ret;
				//char text[16]; 
				char text1[]="Lanner-LCM--Test"; 
				char text2[]="1234567890ABCDEF"; 
        
#if ( defined(DIRECT_IO_ACCESS) && !defined(DJGPP) ) 
#if (defined(LINUX_ENV))
	iopl(3);
#endif
#if (defined(FreeBSD_ENV))
	int iofl;
	SET_IOPL();
#endif
#endif
	fprintf(stdout, "=== Lanner platform miscellaneous utility ===\n");;
	fprintf(stdout, PLATFORM_NAME" LCM "CODE_VERSION"\n\n");;
if( argc != 2 )
    {
//	help(argv[0]);
    printf("LCM TEST Function\n");
    printf("%s -show (Show Text on LCM module screen)\n", argv[0]);
    printf("%s -btn (Test LCM module button function)\n", argv[0]);
		printf("%s -clr (Clean LCM module Screen)\n", argv[0]);
		
	return -1;
    }    
#ifdef DIRECT_IO_ACCESS

	//int read;

	LCM_init();
	if(!strcmp(argv[1], "-show")){
		  //printf("Please enter content to show on line1\n");
		  //scanf("%s",text);
		  // for simple test direct display text on line1
		  printf("Line 1 display : %s\n", text1);
		  //printf("Line 2 display : %s\n", text2);
		  LCM_CLR(); //clear screen
			LCM_SHOW(text1);
			//printf("Please enter content to show on line2\n");
			//scanf("%s",text);
			// for simple test direct display text on line2
			printf("Line 2 display : %s\n", text2);
			LCM_INS();
			LCM_SHOW(text2);
		}
	else if(!strcmp(argv[1], "-btn")){
			printf("Press LCM module button to check button function\n");
			//printf("Press CTRL+C to Stop\n");		
			value = LCM_BTN();
		switch(value)
		{ 
		case 0x41:
			printf("Got A key\n");
			break;
		case 0x42:
			printf("Got B key\n");
			break;
		case 0x43:
			printf("Got C key\n");
			break;
		case 0x44:
			printf("Got D key\n");
			break;
		}		
		}	
	else if(!strcmp(argv[1], "-clr")){
			LCM_CLR();
		}
	else{	
		printf("input = %s\n", argv[1]);
		printf("Wrong command\n");
	}	

#if (defined(FreeBSD_ENV))
	RESET_IOPL();
#endif

#else

        devfd = open("/dev/lcm_drv", O_RDONLY);
        if(devfd == -1)
        {
                printf("Can't open /dev/lcm_drv\n");
                return -1;
        }
        
        value = LCM_BASE_ADDRESS;
        ret = ioctl(devfd, IOCTL_LCM_INIT, &value);
        
				if(!strcmp(argv[1], "-show")){
                                                //printf("Please enter content to show on line1\n");
                                                //scanf("%s",text);
                                                // for simple test direct display text on line1
						printf("Line 1 display : %s\n", text1);
						ret = ioctl(devfd, IOCTL_LCM_CLR, &value);
                                                for(i=0;i<16;i++)
                                                {
                                                      value = 0;
                                                      ret=ioctl(devfd, IOCTL_LCM_SHOW, &text1[i]);
                                                      if (text1[i]== ' ' ) break;
                                                }
                                                //printf("%s\n",text);
						ret = ioctl(devfd, IOCTL_LCM_INS, &value);
                                                //printf("Please enter content to show on line2\n");
                                                //scanf("%s",text);
                                                printf("Line 1 display : %s\n", text1);
                                                for(i=0;i<16;i++)
                                                {
                                                      value = 0;
                                                      ret=ioctl(devfd, IOCTL_LCM_SHOW, &text2[i]);
                                                      if (text2[i]== ' ' ) break;
                                                }
                                                //printf("%s\n",text);
					}
				else if(!strcmp(argv[1], "-btn")){
						printf("Press LCM module button to check button function\n");
						//printf("Press CTRL+C to Stop\n");							
						ret = ioctl(devfd, IOCTL_LCM_BTN, &value);
						switch(ret)
						{
							case 0x41: //A ASCII
								printf("Got A key\n");
								break;
							case 0x42: //B ASCII
								printf("Got B key\n");
								break;
							case 0x43: //C ASCII
								printf("Got C key\n");
								break;
							case 0x44: //D ASCII
								printf("Got D key\n");
								break;
							default:																				
								break;
						}
					}	
				else if(!strcmp(argv[1], "-clr")){
						ret = ioctl(devfd, IOCTL_LCM_CLR, &value);
					}
				else{	
					printf("input = %s\n", argv[1]);
					printf("Wrong command\n");
				}	        
        
        
    

        printf("Test completed\n");
        close(devfd);
        return 0;


#endif //DIRECT_IO_ACCESS

	return 0;
}
