/*******************************************************************************

  ioaccess.h : function declaration file for Lanner platform EEPROM program 

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


#ifndef DJGPP

#ifdef __FreeBSD__

#define inportb(x) inb(x)
#define outportb(x, y) outb(x, y)
#define inportw(x) inw(x)
#define outportw(x, y) outw(x, y)
#define inportl(x) inl(x)
#define outportl(x, y) outl(x, y)

#else

#define inportb(x) inb_p(x)
#define outportb(x, y) outb_p(y, x)
#define inportw(x) inw_p(x)
#define outportw(x, y) outw_p(y, x)
#define inportl(x) inl_p(x)
#define outportl(x, y) outl_p(y, x)

#endif
#endif

#define		EEPRPM_SIZE 256
#define FILENAME 		"MBEEP.TXT"
//// FILENAME.txt file format 
//// AAAAAA'SEPARATE'BBBBBBBB'SEPARATE'CCCCCC'SEPARATE'DDDDDD....'END'
//// item1   	     item2             item3           item4 ...
////  :SEPARATE mark
////  :END mark

#define SEPARATE		'@'
#define END			'#'
#define Item1_string   		"Project Name:" 
#define Item2_string   		"Serial Number:" 
#define 		MB_EEPROM_ADD	0xAE

//#define Item3_string   i	"Date:"
//#define Item4_string   	:Week:"


void eep_gpio_init(void);
unsigned	int get_smbase(void);
void	Write_SMB_Byte(int reg, int value,int sm_addr);
char Read_SMB_Byte(int cslaveadd,int byte_c);
void wait_for_IBF_clear(int delay_count);
void clear_OBF(void);
//v1.2 int 	Detect_IPMI(void);
void Detect_IPMI(void);
void eep_set_output(unsigned int REG_DATA);
char eep_get_input(unsigned int REG);
void smbus_ON(void);
void smbus_OFF(void);

