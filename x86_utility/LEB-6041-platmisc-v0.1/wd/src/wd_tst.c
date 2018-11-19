/*******************************************************************************

  wd_tst.c: main application for Lanner platform Watchdog/Bypass program

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
#include <sys/ioctl.h>
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
#include "../include/wd_ioctl.h"

#define delay(x) usleep(x)

#ifdef DIRECT_IO_ACCESS
#warning ***** Note: You build with DIRECT_IO_ACCESS defined *****
#warning ***** Note: undefine this to build for driver code *****
#endif

#endif

/* local include file */
#include "../include/ioaccess.h"


void print_usage(char* argv0)
{

        printf("%s -swt xxx (Set Watchdog Timer 1-255 seconds and start to count-down)\n", argv0);
        printf("%s -stop (Stop Watchdog Timer)\n", argv0);
}


int main( int argc, char** argv )
{
        int devfd;
        unsigned int value;


	fprintf(stdout, "=== Lanner platform miscellaneous utility ===\n");;
	fprintf(stdout, PLATFORM_NAME" Watchdog/Bypass "CODE_VERSION"\n\n");;
        if ( argc < 2) {
                print_usage(argv[0]);
                return -1;
        }

#if ( defined(DIRECT_IO_ACCESS) && !defined(DJGPP) ) 
#if defined(LINUX_ENV)
	iopl(3);
#endif
#if (defined(FreeBSD_ENV))
       int iofl;
       SET_IOPL();
#endif
#endif

		if ( !strcmp(argv[1], "-swt")) {

#ifdef DIRECT_IO_ACCESS
			wd_gpio_init();
                	unsigned int tmp;
                	if (argc !=3) {
                        	printf("No timer input, program terminated\n");
                        	return -1;
                	}
	                tmp = atoi(argv[2]);
        	        if ( (tmp < 1) || (tmp > 255)) {
        	                printf("Wrong timer value, please input within (1~255)\n");
        	                return -1;
        	        }
		       	start_watchdog_timer(tmp);
            	
        	    
#else
        	devfd = open("/dev/wd_drv", O_RDONLY);
        	if(devfd == -1) {
                	printf("Can't open /dev/wd_drv\n");
                	return -1;
        	}

                	unsigned int tmp;
	                if (argc !=3) {
                        	printf("No timer input, program terminated\n");
                        	close(devfd);
                        	return -1;
                	}
                	tmp = atoi(argv[2]);
                	if ( (tmp < 1) || (tmp > 255)) {
                        	printf("Wrong timer value, please input within (1~255)\n");
                        	close(devfd);
                        	return -1;
                	}
		 ioctl(devfd, IOCTL_SET_WDTO_TIMER, &tmp); 
		
#endif
		printf("Start Watchdog Timer:%d sec\n",atoi(argv[2]));
		
	}

/******** Stop watchdog timer ********************************************/
        else if ( !strcmp(argv[1], "-stop")) {

#if defined(DIRECT_IO_ACCESS)
		wd_gpio_init();
	    	stop_watchdog_timer();

#else
        	devfd = open("/dev/wd_drv", O_RDONLY);
        	if(devfd == -1) {
                	printf("Can't open /dev/wd_drv\n");
                	return -1;
        	}
		value=0x00;
                ioctl(devfd, IOCTL_STOP_WDT_TIMER, &value); 
     
#endif
	  printf("Stop Watchdog Timer....\n");
        }

	else {
		printf("<<<Error: Unknown Parameter>>>\n");
                print_usage(argv[0]);
		return -1;
	}

#ifdef DIRECT_IO_ACCESS
#if (defined(FreeBSD_ENV))
       RESET_IOPL();
#endif
#else
        close(devfd);
#endif
        return 0;

}

