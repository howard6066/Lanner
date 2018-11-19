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


void print_usage(void)
{
        printf("\nhelp message:\n");
        printf("a=Super Io, b=lower card, c=upper card,d=show all comport status\n");
        printf("0~1=Super Io number, 0~7=card number\n");
        printf("0=rs232, 1=rs485, 2=rs422/485\n");
        printf("0=Termination disable, 1=Termination enable\n");		
        printf("for example: [cp_tst --a 1 1 0] represent:\n");
        printf("Super Io number is 2,  transmission mode is rs485, Termination is disable\n");
        printf("for example: [cp_tst --c 6 2 1] represent:\n");
        printf("upper card number is 6,  transmission mode is rs422/485, Termination is enable\n");
        printf("for example: [cp_tst --c 2] represent: show mode and termination of upper card port 2 \n");
        printf("for example: [cp_tst --a 1] represent: show mode and termination of super io port 1 \n");		
}


int main( int argc, char** argv )
{
        int devfd;
        int value = 0;
        int tmp = 0;
        char *ptr = NULL; 
        int type = 0;
        int port = 0; 
        int RiserSts_flag = 0;


	fprintf(stdout, "=== Lanner platform miscellaneous utility ===\n");;
	fprintf(stdout, PLATFORM_NAME" Com Port "CODE_VERSION"\n");;
        if ( (argc != 5) && (argc != 3)&&(argc != 2)) {
                print_usage();
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

#ifdef DIRECT_IO_ACCESS
	cp_gpio_init();
#else
        devfd = open("/dev/cp_drv", O_RDONLY);
        if(devfd == -1)
        {
                printf("Can't open /dev/cp_drv\n");
                return -1;
        }
#endif

//************************* check input parameter **************************/
         if(argc == 5) {
	    if ( !strcmp(argv[1], "--a")) {
	        if(argv[2][0] < '0' || argv[2][0] > '1') {
                     print_usage();
		   return -1;			 
	        	}
	        if(argv[3][0]<'0' || argv[3][0]>'2') {
	            print_usage();
                     return -1; 
	        	}
	        if(argv[4][0]<'0' || argv[4][0]>'1') {
		   print_usage();
		   return -1;
	        	}
	    } else if (!strcmp(argv[1], "--b")){
                 if(argv[2][0] < '0' || argv[2][0] > '7') {
                     print_usage();
		   return -1;			 
                 	}
	        if(argv[3][0]<'0' || argv[3][0]>'2') {
	            print_usage();
		   return -1;		
	        	}
	        if(argv[4][0]<'0' || argv[4][0]>'1') {
	            print_usage();
		   return -1;		
	        	}
             } else if (!strcmp(argv[1], "--c")){
                 if(argv[2][0] < '0' || argv[2][0] > '7') {
                     print_usage();
                     return -1;  
                 	}			 
	        if(argv[3][0]<'0' || argv[3][0]>'2') {
	            print_usage();
                     return -1;  
	        	}
	        if(argv[4][0]<'0' || argv[4][0]>'1') {
		    print_usage(); 
                      return -1;
	        	}
             } else { 
	        print_usage();
	        return -1;	
             }
         }	 

         if(argc == 3) {
	    if ( !strcmp(argv[1], "--a")) {
	        if(argv[2][0] < '0' || argv[2][0] > '1') {
                     print_usage();
                     return -1; 
	        	}   
	    } else if (!strcmp(argv[1], "--b")){
                 if(argv[2][0] < '0' || argv[2][0] > '7') {
                     print_usage();
                     return -1;
                 	}
             } else if (!strcmp(argv[1], "--c")){
                 if(argv[2][0] < '0' || argv[2][0] > '7') {
                     print_usage();
                     return -1; 
                 	}
             } else { 
	        print_usage();
	        return -1;	
             }
         }

	if(argc == 2){
             if(strcmp(argv[1], "--d")){
                 print_usage();
	        return -1;   			 
             }
	}
//********************fill content of value with input parameter ******************/

         if(argc == 5) {
             value = argv[1][2] - 'a';
             value |= (argv[2][0] - '0') << 2;
             value |= (argv[3][0] - '0') << 5;
	    value |= (argv[4][0] - '0') << 7;
         }
	if(argc == 3) {
             value = argv[1][2] - 'a';
             value |= (argv[2][0] - '0') << 2;
	}
//**************************set com port***********************************/	
                      
#if defined(DIRECT_IO_ACCESS)
    if(argc == 5) {
        SetComPort(value);
        printf("OK\n");		
    }		
    if(argc == 3) {
        tmp = ShowComPort(value);		
        if((tmp & 0x3) == 0x0)
            ptr = "rs232";
        if((tmp & 0x3) == 0x1)
            ptr = "rs485";
        if((tmp & 0x3) == 0x2)
            ptr = "rs422/485";
        printf("mode = %s", ptr);

        tmp = tmp >> 2;
        if((tmp & 0x1) == 0x1)
            ptr = "enable\n";
        if((tmp & 0x1) == 0x0)
            ptr = "disable\n";
        printf("  termination = %s", ptr);	
    }
    if(argc == 2) {

        RiserSts_flag = ReturnRiserStsFlag();

        if(RiserSts_flag == 1){
			
            type = 'a'; 
	   for(port = 0 ; port <=1 ; port++){
                    value = type - 'a';
                    value |= (port - '0') << 2;
		  tmp = ShowComPort(value);

		  printf("super io port %d : ", port);													
		  if((tmp & 0x3) == 0x0)
                        ptr = "rs232";
                    if((tmp & 0x3) == 0x1)
                        ptr = "rs485";
                    if((tmp & 0x3) == 0x2)
                        ptr = "rs422/485";
                    printf("mode = %s", ptr);
                    tmp = tmp >> 2;
                    if((tmp & 0x1) == 0x1)
                        ptr = "enable\n";
                    if((tmp & 0x1) == 0x0)
                       ptr = "disable\n";
                    printf(" termination = %s", ptr);		
             }

	    type = 'b';
	    for(port = 0 ; port <=7 ; port++){
                    value = type - 'a';
                    value |= (port - '0') << 2;
		  tmp = ShowComPort(value);

		  printf("lower card port %d : ", port);													
		  if((tmp & 0x3) == 0x0)
                        ptr = "rs232";
                    if((tmp & 0x3) == 0x1)
                        ptr = "rs485";
                    if((tmp & 0x3) == 0x2)
                        ptr = "rs422/485";
                    printf("mode = %s", ptr);
                    tmp = tmp >> 2;
                    if((tmp & 0x1) == 0x1)
                        ptr = "enable\n";
                    if((tmp & 0x1) == 0x0)
                       ptr = "disable\n";
                    printf(" termination = %s", ptr);		
            }
        }	
        
        if(RiserSts_flag == 2){
            type = 'a'; 
	   for(port = 0 ; port <=1 ; port++){
                    value = type - 'a';
                    value |= (port - '0') << 2;
		  tmp = ShowComPort(value);

		  printf("super io port %d : ", port);													
		  if((tmp & 0x3) == 0x0)
                        ptr = "rs232";
                    if((tmp & 0x3) == 0x1)
                        ptr = "rs485";
                    if((tmp & 0x3) == 0x2)
                        ptr = "rs422/485";
                    printf("mode = %s", ptr);
                    tmp = tmp >> 2;
                    if((tmp & 0x1) == 0x1)
                        ptr = "enable\n";
                    if((tmp & 0x1) == 0x0)
                       ptr = "disable\n";
                    printf(" termination = %s", ptr);		
            }

            type = 'c';      
	   for(port = 0 ; port <=7 ; port++){
                    value = type - 'a';
                    value |= (port - '0') << 2;
		  tmp = ShowComPort(value);

		  printf("upper card port %d : ", port);													
		  if((tmp & 0x3) == 0x0)
                        ptr = "rs232";
                    if((tmp & 0x3) == 0x1)
                        ptr = "rs485";
                    if((tmp & 0x3) == 0x2)
                        ptr = "rs422/485";
                    printf("mode = %s", ptr);
                    tmp = tmp >> 2;
                    if((tmp & 0x1) == 0x1)
                        ptr = "enable\n";
                    if((tmp & 0x1) == 0x0)
                       ptr = "disable\n";
                    printf(" termination = %s", ptr);		
                }
            }
       
        if(RiserSts_flag == 3){ 		
            for(type = 'a' ; type <= 'c' ; type++){
                if(type == 'a'){			
                    for(port = 0 ; port <=1 ; port++){
                        value = type - 'a';
                        value |= (port - '0') << 2;
		      tmp = ShowComPort(value);

		      printf("super io port %d : ", port);													
		      if((tmp & 0x3) == 0x0)
                            ptr = "rs232";
                        if((tmp & 0x3) == 0x1)
                            ptr = "rs485";
                        if((tmp & 0x3) == 0x2)
                            ptr = "rs422/485";
                        printf("mode = %s", ptr);
                        tmp = tmp >> 2;
                       if((tmp & 0x1) == 0x1)
                           ptr = "enable\n";
                       if((tmp & 0x1) == 0x0)
                           ptr = "disable\n";
                       printf(" termination = %s", ptr);		
                    }
                }

	       if(type == 'b'){			
                    for(port = 0 ; port <=7 ; port++){
                        value = type - 'a';
                        value |= (port - '0') << 2;
		      tmp = ShowComPort(value);

		      printf("lower card port %d : ", port);													
		      if((tmp & 0x3) == 0x0)
                            ptr = "rs232";
                        if((tmp & 0x3) == 0x1)
                            ptr = "rs485";
                        if((tmp & 0x3) == 0x2)
                            ptr = "rs422/485";
                        printf("mode = %s", ptr);
                        tmp = tmp >> 2;
                        if((tmp & 0x1) == 0x1)
                            ptr = "enable\n";
                        if((tmp & 0x1) == 0x0)
                           ptr = "disable\n";
                        printf(" termination = %s", ptr);		
                    }
                }

	       if(type == 'c'){			
                    for(port = 0 ; port <=7 ; port++){
                        value = type - 'a';
                        value |= (port - '0') << 2;
		      tmp = ShowComPort(value);

		      printf("upper card port %d : ", port);													
		      if((tmp & 0x3) == 0x0)
                            ptr = "rs232";
                        if((tmp & 0x3) == 0x1)
                            ptr = "rs485";
                        if((tmp & 0x3) == 0x2)
                            ptr = "rs422/485";
                        printf("mode = %s", ptr);
                        tmp = tmp >> 2;
                        if((tmp & 0x1) == 0x1)
                            ptr = "enable\n";
                        if((tmp & 0x1) == 0x0)
                           ptr = "disable\n";
                        printf(" termination = %s", ptr);		
                    }
                }
            }
        }
    }
#else
    if(argc == 5) { 
        if( ioctl(devfd, IOCTL_SET_COMPORT, &value) != 0)
            printf("Fail\n");
        else
            printf("OK !!\n");
    }
    if(argc == 3) {
        tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);
        if((tmp & 0x3) == 0x0)
            ptr = "rs232";
        if((tmp & 0x3) == 0x1)
            ptr = "rs485";
        if((tmp & 0x3) == 0x2)
            ptr = "rs422/485";
        printf("mode = %s", ptr);

        tmp = tmp >> 2;
        if((tmp & 0x1) == 0x1)
            ptr = "enable\n";
        if((tmp & 0x1) == 0x0)
	   ptr = "disable\n";
        printf("  termination = %s", ptr);				
    }

    if(argc == 2) { 	

        RiserSts_flag = ioctl(devfd, IOCTL_RISER_FLAG, &value);

        if(RiserSts_flag == 1){
            type = 'a';
            for(port = 0 ; port <=1 ; port++){
                value = type - 'a';
                value |= (port - '0') << 2;
                tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

                printf("super io port %d : ", port);													
	       if((tmp & 0x3) == 0x0)
                    ptr = "rs232";
                if((tmp & 0x3) == 0x1)
                    ptr = "rs485";
                if((tmp & 0x3) == 0x2)
                    ptr = "rs422/485";
                printf("mode = %s", ptr);
                tmp = tmp >> 2;
                if((tmp & 0x1) == 0x1)
                    ptr = "enable\n";
                if((tmp & 0x1) == 0x0)
                    ptr = "disable\n";
                printf(" termination = %s", ptr);		
            }
              
	   type = 'b';			
            for(port = 0 ; port <=7 ; port++){
                value = type - 'a';
                value |= (port - '0') << 2;
                tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

	       printf("lower card port %d : ", port);													
	       if((tmp & 0x3) == 0x0)
                    ptr = "rs232";
                if((tmp & 0x3) == 0x1)
                    ptr = "rs485";
                if((tmp & 0x3) == 0x2)
                    ptr = "rs422/485";
                printf("mode = %s", ptr);
                tmp = tmp >> 2;
                if((tmp & 0x1) == 0x1)
                    ptr = "enable\n";
                if((tmp & 0x1) == 0x0)
                    ptr = "disable\n";
                printf(" termination = %s", ptr);	
            }		
        }		

        if(RiserSts_flag == 2){
            type = 'a';
            for(port = 0 ; port <=1 ; port++){
                value = type - 'a';
                value |= (port - '0') << 2;
                tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

                printf("super io port %d : ", port);													
	       if((tmp & 0x3) == 0x0)
                    ptr = "rs232";
                if((tmp & 0x3) == 0x1)
                    ptr = "rs485";
                if((tmp & 0x3) == 0x2)
                    ptr = "rs422/485";
                printf("mode = %s", ptr);
                tmp = tmp >> 2;
                if((tmp & 0x1) == 0x1)
                    ptr = "enable\n";
                if((tmp & 0x1) == 0x0)
                    ptr = "disable\n";
                printf(" termination = %s", ptr);		
            }

	   type = 'c'; 
	   for(port = 0 ; port <=7 ; port++){
                    value = type - 'a';
                    value |= (port - '0') << 2;
		  tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

		  printf("upper card port %d : ", port);													
		  if((tmp & 0x3) == 0x0)
                        ptr = "rs232";
                    if((tmp & 0x3) == 0x1)
                        ptr = "rs485";
                    if((tmp & 0x3) == 0x2)
                        ptr = "rs422/485";
                    printf("mode = %s", ptr);
                    tmp = tmp >> 2;
                    if((tmp & 0x1) == 0x1)
                        ptr = "enable\n";
                    if((tmp & 0x1) == 0x0)
                       ptr = "disable\n";
                    printf(" termination = %s", ptr);		
                }
        }

        if(RiserSts_flag == 3){		
            for(type = 'a' ; type <= 'c' ; type++){
                if(type == 'a'){			
                    for(port = 0 ; port <=1 ; port++){
                        value = type - 'a';
                        value |= (port - '0') << 2;
		      tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

		      printf("super io port %d : ", port);													
		      if((tmp & 0x3) == 0x0)
                            ptr = "rs232";
                        if((tmp & 0x3) == 0x1)
                            ptr = "rs485";
                        if((tmp & 0x3) == 0x2)
                            ptr = "rs422/485";
                        printf("mode = %s", ptr);
                        tmp = tmp >> 2;
                        if((tmp & 0x1) == 0x1)
                            ptr = "enable\n";
                        if((tmp & 0x1) == 0x0)
                           ptr = "disable\n";
                        printf(" termination = %s", ptr);		
                    }
                }

	       if(type == 'b'){			
                    for(port = 0 ; port <=7 ; port++){
                        value = type - 'a';
                        value |= (port - '0') << 2;
		      tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

		      printf("lower card port %d : ", port);													
		      if((tmp & 0x3) == 0x0)
                            ptr = "rs232";
                        if((tmp & 0x3) == 0x1)
                            ptr = "rs485";
                        if((tmp & 0x3) == 0x2)
                            ptr = "rs422/485";
                        printf("mode = %s", ptr);
                        tmp = tmp >> 2;
                        if((tmp & 0x1) == 0x1)
                            ptr = "enable\n";
                        if((tmp & 0x1) == 0x0)
                           ptr = "disable\n";
                        printf(" termination = %s", ptr);	
                    }
                }

	       if(type == 'c'){			
                    for(port = 0 ; port <=7 ; port++){
                        value = type - 'a';
                        value |= (port - '0') << 2;
		      tmp = ioctl(devfd, IOCTL_SHOW_COMPORT, &value);

		      printf("upper card port %d : ", port);													
		      if((tmp & 0x3) == 0x0)
                            ptr = "rs232";
                        if((tmp & 0x3) == 0x1)
                            ptr = "rs485";
                        if((tmp & 0x3) == 0x2)
                            ptr = "rs422/485";
                        printf("mode = %s", ptr);
                        tmp = tmp >> 2;
                        if((tmp & 0x1) == 0x1)
                            ptr = "enable\n";
                        if((tmp & 0x1) == 0x0)
                           ptr = "disable\n";
                        printf(" termination = %s", ptr);		
                    }
                }
            }
        }
    }
#endif



#ifdef DIRECT_IO_ACCESS
#if (defined(FreeBSD_ENV))
       RESET_IOPL();
#endif
#else
        close(devfd);
#endif
        return 0;

}

