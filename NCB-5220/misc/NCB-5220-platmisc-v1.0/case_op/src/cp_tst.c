/*******************************************************************************

  btn_tst.c: main application for Lanner platform Software Button program

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
#include "../include/cp_ioctl.h"

#define delay(x) usleep(x)

#ifdef DIRECT_IO_ACCESS
#warning ***** Note: You build with DIRECT_IO_ACCESS defined *****
#warning ***** Note: undefine this to build for driver code *****
#endif

#endif

/* local include file */
#include "../include/ioaccess.h"


int main( int argc, char** argv )
{
        int devfd;
        int value, ret;
				char c;

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
	fprintf(stdout, PLATFORM_NAME" Case Open "CODE_VERSION"\n\n");;
   printf("Case Open test Tool......\n");
#ifdef DIRECT_IO_ACCESS
//	btn_gpio_init();
#else
        devfd = open("/dev/cp_drv", O_RDONLY);
        if(devfd == -1)
        {
                printf("Can't open /dev/cp_drv\n");
                return -1;
        }
#endif
	while(1) {
#ifdef DIRECT_IO_ACCESS
		value = get_intruder_status();
		if(value) {
			printf("the case have been opened\n");
			printf("To clear the case open status(y/n)?");
			c=getchar();
				
			}	
		else{
				printf("the case is close");
				//return 0;	
			}
		if(c=='y')
			clear_intruder_status();
		
#else
		value = 0;	
     ret=ioctl(devfd, IOCTL_CASEOPEN_GET_STATUS, &value);
      
     if(value) {
			printf("the case have been opened\n");
			printf("To clear the case open status(y/n)?");
			c=getchar();
				
			}	
		else{
				printf("the case is close\n");
				//return 0;	
			}
		if(c=='y')
       ret=ioctl(devfd, IOCTL_CASEOPEN_CLEAR_STATUS, &value);   
#endif
		
			
			break;
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

