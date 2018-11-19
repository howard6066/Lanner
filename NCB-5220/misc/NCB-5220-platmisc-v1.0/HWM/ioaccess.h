/*******************************************************************************
  ioaccess.h: Lanner's common GPIOs, SIO and SMBus definitions on Skylake(SKL-H) platform

  Lanner Platform Miscellaneous Utility
  Copyright(c) 2015 Lanner Electronics Inc.
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

/* standard include file */
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
//#include <sys/nearptr.h>
//#include <crt0.h>
#define delay(x) usleep(x*1000)

//#include "../include/config.h"

#ifdef DJGPP
  /* For DOS DJGPP */
  #include <dpmi.h>
  #include <sys/farptr.h>

#else
  /* For Linux */
  #include <sys/io.h>
  #include <time.h>

  #ifdef __FreeBSD__
    #define inportb(x)          inb(x)
    #define outportb(x, y)      outb(y, x)
    #define inportw(x)          inw(x)
    #define outportw(x, y)      outw(y, x)
    #define inportl(x)          inl(x)
    #define outportl(x, y)      outl(y, x)

  #else
	#define inportb(x)          inb_p(x)
	#define outportb(x, y)      outb_p(y, x)
	#define inportw(x)          inw_p(x)
	#define outportw(x, y)      outw_p(y, x)
	#define inportl(x)          inl_p(x)
	#define outportl(x, y)      outl_p(y, x)

  #endif //#ifdef __FreeBSD__

#endif //#ifdef DJGPP

//===============================================================================
// Common CONST Definition
#define PCI_IDX_REG		0xCF8
#define PCI_DAT_REG		0xCFC

#define BIT0     		0x00000001
#define BIT1     		0x00000002
#define BIT2     		0x00000004
#define BIT3     		0x00000008
#define BIT4     		0x00000010
#define BIT5     		0x00000020
#define BIT6     		0x00000040
#define BIT7     		0x00000080
#define BIT8     		0x00000100
#define BIT9     		0x00000200
#define BIT10    		0x00000400
#define BIT11    		0x00000800
#define BIT12    		0x00001000
#define BIT13    		0x00002000
#define BIT14    		0x00004000
#define BIT15    		0x00008000
#define BIT16    		0x00010000
#define BIT17    		0x00020000
#define BIT18    		0x00040000
#define BIT19    		0x00080000
#define BIT20    		0x00100000
#define BIT21    		0x00200000
#define BIT22    		0x00400000
#define BIT23    		0x00800000
#define BIT24    		0x01000000
#define BIT25    		0x02000000
#define BIT26    		0x04000000
#define BIT27    		0x08000000
#define BIT28    		0x10000000
#define BIT29    		0x20000000
#define BIT30    		0x40000000
#define BIT31    		0x80000000

#define VOID			void
#define BOOLEAN			bool
#define IN
#define OUT
#define	TRUE			1
#define	FALSE			0

#define	UINT8 			unsigned char
#define	UINT16			unsigned int
#define	UINT32			unsigned long
#define	INT8 			signed char
#define	INT16			int
#define	INT32			long


//===============================================================================
//
// Declare GPIO, SIO,HWM and SMBus related definitions for Skylake(SKL-H) platform
//
//===============================================================================

//===============================================================
//  SIO related definition/declaration (I/O Base Addressing)
//===============================================================
#define SIO_IDX_PORT	    0x2E
#define SIO_DAT_PORT	    0x2F

VOID            EnterSioConfigMode      (VOID);
VOID            ExitSioConfigMode       (VOID);
UINT8           ReadSioReg              (UINT8 Ldn, UINT8 Reg);
VOID            WriteSioReg             (UINT8 Ldn, UINT8 Reg, UINT8 Value);


//===============================================================
//  HWM related definition/declaration (I/O Base Addressing)(NCT7904D)
//===============================================================
#define NCT7904_SlaveAdd   	0x5c

//===============================================================
//  HWM related definition/declaration (I/O Base Addressing)(NCT6776F)
//===============================================================
#define HM_INDEX_PORT	0xA15
#define HM_DATA_PORT	0xA16	

UINT8           read_HM_reg             (UINT8 hmbank, UINT8 hmreg);
VOID            write_HM_reg            (UINT8 hmbank, UINT8 hmreg, UINT8 Value);

//===============================================================
//  EEPROM related definition/declaration (I/O Base Addressing)
//===============================================================

//.txt file format 
// AAAAAA@BBBBBBBB@CCCCCC@DDDDDD....#
//  item1   item2   item3  item4 ...
// @ :separate mark
// # :end mark
#define	 	Item1_string	"Module Name:" 
#define	 	Item2_string	"Serial Number:" 
//#define	 	Item3_string	"Module Name:" 
//#define	 	Item4_string	"Serial Number:"
//#define	 	Item5_string	"Module Name:" 
//#define	 	Item6_string	"Serial Number:"

#define EEPRPM_SIZE  	256//EEPROM SIZE Bytes
#define PCA9548_addr    0xE6
#define PCA9545_addr    0xE6
#define PCA9545_IoBoard_Channe3  	0x08//IO board eeprom
#define	sm_target_addr  	0xAE//eeprom address 
#define	sm_target_addr_WG  	0xAC//NCB-WG4210 eeprom address
int	Item_Number=0x01;

//===============================================================
//  SMBus related definition/declaration (I/O Base Addressing)
//===============================================================
//    B00, D31, F04, R20h
#define PCH_SMBHOST_BASEIO_PFA  (0x80000000L + (0<<16) + (31<<11) + (4<<8) + 0x20) //Bus0  Device 31 Function 4 Registers20

//----------------------------------------------------------
// PCH/SMBus Host Controller Registers
//
#define PCH_SMBC_REG_HSTS       0x00    // Host Status Register
  #define SMBC_F_BDS              BIT7      // 1:= When the host controller has received a byte for Block Read commands or 
                                            //       if it has completed transmission of a byte for Block Write commands when
                                            //         the 32-byte buffer is not being used.
  #define SMBC_F_IUS              BIT6      // 1:= Other App. is owened the SMBus
  #define SMBC_F_SMSTS            BIT5      // 1:= Detected SMBAlert# signal
  #define SMBC_F_FAIL             BIT4      // 1:= A failed bus transaction 
  #define SMBC_F_BERR             BIT3      // 1:= A failed bus collision
  #define SMBC_F_DERR             BIT2      // 1:= Device timeout, CRC error
  #define SMBC_F_HBSY             BIT0      // 1:= Host is running a command
  #define SMBC_F_ERRS             (SMBC_F_SMSTS | SMBC_F_FAIL | SMBC_F_BERR | SMBC_F_DERR)

#define PCH_SMBC_REG_HCTL       0x02    // Host Control Register
  #define SMBC_F_PEC_EN           BIT7      // 1:= Enabled PEC tranaction
  #define SMBC_F_START            BIT6      // 1:= Start transaction
  #define SMBC_F_CMD_B2           BIT4      ///
  #define SMBC_F_CMD_B1           BIT3      /// To determine which command protocol will be used.
  #define SMBC_F_CMD_B0           BIT2      ///
  #define SMBC_F_KILL             BIT1      // 1:= Kills the current host transaction

#define PCH_SMBC_REG_HCMD       0x03    // Host Command Register
#define PCH_SMBC_REG_TSA        0x04    // Transmit Slave Address Register
#define PCH_SMBC_REG_HDO        0x05    // Data 0 Register
#define PCH_SMBC_REG_HD1        0x06    // Data 1 Register
#define PCH_SMBC_REG_HBD        0x07    // Host Block Data
#define PCH_SMBC_REG_PEC        0x08    // Packet Error Check Data Register

#define PCH_SMBC_REG_AUXS       0x0C    // Auxiliary Status
  #define SMBC_F_CRCE             BIT0      // 1:= When received message contained a CRC error

#define PCH_SMBC_REG_AUXC       0x0D    // Auxiliary Control
  #define SMBC_F_E32B             BIT1      // 1:= The Host Block Data register is a pointer into a 32-byte buffer
  #define SMBC_F_AAC              BIT0      // 1:= PCH will automatically apeend the CRC

//----------------------------------------------------------
// PCH/SMBus protocol definition
//
//#define SMB_PROTOCOL_QUICK    0               // 000b, For read/write Quick Byte.
#define SMB_PROTOCOL_BYTE       (SMBC_F_CMD_B0) // 001b, Uses the transmit slave address and command registers.
#define SMB_PROTOCOL_BYTEDATA   (SMBC_F_CMD_B1) // 010b, Uses the transmit slave address, command, and DATA0 registers.
#define SMB_PROTOCOL_WORDDATA   (SMBC_F_CMD_B1 | SMBC_F_CMD_B0) // 011b, Uses the transmit slave address, command, DATA0 and DATA1 registers.
#define SMB_PROTOCOL_BLOCK      (SMBC_F_CMD_B2 | SMBC_F_CMD_B0) // 101b, Uses the transmit slave address, command, DATA0 registers, and the Block Data Byte register.

//----------------------------------------------------------
// PCH/SMBus Global Variables and Functions declaration
//
UINT16          _SmbBaseIoAddr;             // SMB/HC I/O base address which is located in B00:D31:F04:R20h
UINT8           _SmbError;                  // Global variable that indicates the status of SMB transmission.
UINT8           _BlockBuf[32];              // Global buffer for block transmission
UINT8           _ErrStrBuf[256];            // Global buffer for error message output before terminate this program.

UINT16          GetSmbBaseIoAddr        (VOID);
BOOLEAN         IsSmbcBusy              (VOID);
BOOLEAN         IsCmdCompleted          (VOID);
VOID            ShowErrCode             (const UINT8 *ErrMessage);
VOID            ShowErrCodeMessage      (const UINT8 *ErrMessage, BOOLEAN bClrIUS);
VOID            SMB_WriteByte           (UINT8 SlaveAddr, UINT8 SlaveByteOfs, UINT8 SlaveData);
VOID            SMB_WriteByte_PEC       (UINT8 SlaveAddr, UINT8 SlaveByteOfs, UINT8 SlaveData, UINT8 SlavePEC);
UINT16          SMB_ReadWord            (UINT8 SlaveAddr, UINT8 SlaveByteOfs);
UINT8           SMB_ReadBlock           (UINT8 SlaveAddr, UINT8 SlaveByteNums);


//===============================================================
//  GPIO's definitions/interfaces (MMIO - Memory Mapping I/O Addressing)
//===============================================================
#define PCH_PCR_BASE_ADDRESS			0xFD000000		// SBREG MMIO base address
#define PCH_PCR_MMIO_SIZE               0x01000000     	// < 16MB
#define SB_PCR_ADDRESS(Pid, Offset)    	( ((UINT32)(Pid) << 16) | ((UINT16)(Offset)) )

#define V_PCH_GPIO_GPP_A_PAD_MAX            24    //skylake spec 804page
#define V_PCH_GPIO_GPP_B_PAD_MAX            24
#define V_PCH_GPIO_GPP_C_PAD_MAX            24
#define V_PCH_GPIO_GPP_D_PAD_MAX            24
#define V_PCH_H_GPIO_GPP_E_PAD_MAX          13
#define V_PCH_GPIO_GPP_F_PAD_MAX            24
#define V_PCH_H_GPIO_GPP_G_PAD_MAX          24
#define V_PCH_H_GPIO_GPP_H_PAD_MAX          24
#define V_PCH_H_GPIO_GPP_I_PAD_MAX          11
#define V_PCH_GPIO_GPD_PAD_MAX              12

// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
#define NO_REGISTER_FOR_PROPERTY 			(0xFFFFFFFFL)

// GPIO Community 0 Private Configuration Registers
// SKL PCH-H
#define R_PCH_H_PCR_GPIO_GPP_A_PAD_OWN        	0x20
#define R_PCH_H_PCR_GPIO_GPP_B_PAD_OWN        	0x30//joe fix
#define R_PCH_H_PCR_GPIO_GPP_A_GPI_VWM_EN     	0x70
#define R_PCH_H_PCR_GPIO_GPP_B_GPI_VWM_EN     	0x74
#define R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCK     	0x90
#define R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCKTX   	0x94
#define R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCK     	0x98
#define R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCKTX   	0x9C
#define R_PCH_PCR_GPIO_GPP_A_HOSTSW_OWN     	0xD0
#define R_PCH_PCR_GPIO_GPP_B_HOSTSW_OWN     	0xD4

#define R_PCH_PCR_GPIO_GPP_A_GPI_IS         	0x0100
#define R_PCH_PCR_GPIO_GPP_B_GPI_IS         	0x0104
#define R_PCH_PCR_GPIO_GPP_A_GPI_IE         	0x0120

#define R_PCH_PCR_GPIO_GPP_B_GPI_IE         	0x0124
#define R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS    	0x0140
#define R_PCH_PCR_GPIO_GPP_B_GPI_GPE_STS    	0x0144
#define R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN     	0x0160
#define R_PCH_PCR_GPIO_GPP_B_GPI_GPE_EN     	0x0164
#define R_PCH_PCR_GPIO_GPP_A_SMI_STS        	0x0180
#define R_PCH_PCR_GPIO_GPP_B_SMI_STS        	0x0184
#define R_PCH_PCR_GPIO_GPP_A_SMI_EN         	0x01A0
#define R_PCH_PCR_GPIO_GPP_B_SMI_EN         	0x01A4
#define R_PCH_PCR_GPIO_GPP_A_NMI_STS        	0x01C0
#define R_PCH_PCR_GPIO_GPP_B_NMI_STS        	0x01C4
#define R_PCH_PCR_GPIO_GPP_A_NMI_EN         	0x01E0
#define R_PCH_PCR_GPIO_GPP_B_NMI_EN         	0x01E4
#define R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET  	0x400
#define R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET  	0x4C0

// GPIO Community 1 Private Configuration Registers
//SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_C_PAD_OWN        	0x20
#define R_PCH_H_PCR_GPIO_GPP_D_PAD_OWN        	0x2C
#define R_PCH_H_PCR_GPIO_GPP_E_PAD_OWN        	0x38
#define R_PCH_H_PCR_GPIO_GPP_F_PAD_OWN        	0x40
#define R_PCH_H_PCR_GPIO_GPP_G_PAD_OWN        	0x4C
#define R_PCH_H_PCR_GPIO_GPP_H_PAD_OWN        	0x58

#define R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCK     	0x90
#define R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCKTX   	0x94
#define R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCK     	0x98
#define R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCKTX   	0x9C
#define R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCK     	0xA0
#define R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCKTX   	0xA4
#define R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCK     	0xA8
#define R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCKTX   	0xAC
#define R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCK     	0xB0
#define R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCKTX   	0xB4
#define R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCK     	0xB8
#define R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCKTX   	0xBC

#define R_PCH_PCR_GPIO_GPP_C_HOSTSW_OWN       	0xD0
#define R_PCH_PCR_GPIO_GPP_D_HOSTSW_OWN       	0xD4
#define R_PCH_PCR_GPIO_GPP_E_HOSTSW_OWN       	0xD8
#define R_PCH_H_PCR_GPIO_GPP_F_HOSTSW_OWN     	0xDC
#define R_PCH_H_PCR_GPIO_GPP_G_HOSTSW_OWN     	0xE0
#define R_PCH_H_PCR_GPIO_GPP_H_HOSTSW_OWN     	0xE4

#define R_PCH_PCR_GPIO_GPP_C_GPI_IS           	0x0100
#define R_PCH_PCR_GPIO_GPP_D_GPI_IS           	0x0104
#define R_PCH_PCR_GPIO_GPP_E_GPI_IS           	0x0108
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_IS         	0x010C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_IS         	0x0110
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_IS         	0x0114
#define R_PCH_PCR_GPIO_GPP_C_GPI_IE           	0x0120
#define R_PCH_PCR_GPIO_GPP_D_GPI_IE           	0x0124
#define R_PCH_PCR_GPIO_GPP_E_GPI_IE           	0x0128
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_IE         	0x012C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_IE         	0x0130
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_IE         	0x0134

#define R_PCH_PCR_GPIO_GPP_C_GPI_GPE_STS      	0x0140
#define R_PCH_PCR_GPIO_GPP_D_GPI_GPE_STS      	0x0144
#define R_PCH_PCR_GPIO_GPP_E_GPI_GPE_STS      	0x0148
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_STS    	0x014C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_STS    	0x0150
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_STS    	0x0154
#define R_PCH_PCR_GPIO_GPP_C_GPI_GPE_EN       	0x0160
#define R_PCH_PCR_GPIO_GPP_D_GPI_GPE_EN       	0x0164
#define R_PCH_PCR_GPIO_GPP_E_GPI_GPE_EN       	0x0168
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_EN     	0x016C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_EN     	0x0170
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_EN     	0x0174

#define R_PCH_PCR_GPIO_GPP_C_SMI_STS          	0x0180
#define R_PCH_PCR_GPIO_GPP_D_SMI_STS          	0x0184
#define R_PCH_PCR_GPIO_GPP_E_SMI_STS          	0x0188
#define R_PCH_H_PCR_GPIO_GPP_F_SMI_STS        	0x018C
#define R_PCH_H_PCR_GPIO_GPP_G_SMI_STS        	0x0190
#define R_PCH_H_PCR_GPIO_GPP_H_SMI_STS        	0x0194
#define R_PCH_PCR_GPIO_GPP_C_SMI_EN           	0x01A0
#define R_PCH_PCR_GPIO_GPP_D_SMI_EN           	0x01A4
#define R_PCH_PCR_GPIO_GPP_E_SMI_EN           	0x01A8
#define R_PCH_H_PCR_GPIO_GPP_F_SMI_EN         	0x01AC
#define R_PCH_H_PCR_GPIO_GPP_G_SMI_EN         	0x01B0
#define R_PCH_H_PCR_GPIO_GPP_H_SMI_EN         	0x01B4

#define R_PCH_PCR_GPIO_GPP_C_NMI_STS          	0x01C0
#define R_PCH_PCR_GPIO_GPP_D_NMI_STS          	0x01C4
#define R_PCH_PCR_GPIO_GPP_E_NMI_STS          	0x01C8
#define R_PCH_H_PCR_GPIO_GPP_F_NMI_STS        	0x01CC
#define R_PCH_H_PCR_GPIO_GPP_G_NMI_STS        	0x01D0
#define R_PCH_H_PCR_GPIO_GPP_H_NMI_STS        	0x01D4
#define R_PCH_PCR_GPIO_GPP_C_NMI_EN           	0x01E0
#define R_PCH_PCR_GPIO_GPP_D_NMI_EN           	0x01E4
#define R_PCH_PCR_GPIO_GPP_E_NMI_EN           	0x01E8
#define R_PCH_H_PCR_GPIO_GPP_F_NMI_EN         	0x01EC
#define R_PCH_H_PCR_GPIO_GPP_G_NMI_EN         	0x01F0
#define R_PCH_H_PCR_GPIO_GPP_H_NMI_EN         	0x01F4

#define R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET    	0x400
#define R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET    	0x4C0
#define R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET    	0x580
#define R_PCH_H_PCR_GPIO_GPP_F_PADCFG_OFFSET  	0x5E8
#define R_PCH_H_PCR_GPIO_GPP_G_PADCFG_OFFSET  	0x6A8
#define R_PCH_H_PCR_GPIO_GPP_H_PADCFG_OFFSET	0x768

// GPIO Community 2 Private Configuration Registers
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPD_PAD_OWN          	0x20
#define R_PCH_H_PCR_GPIO_GPD_PADCFGLOCK       	0x90
#define R_PCH_H_PCR_GPIO_GPD_PADCFGLOCKTX		0x94
#define R_PCH_PCR_GPIO_GPD_HOSTSW_OWN       	0xD0
#define R_PCH_PCR_GPIO_GPD_GPI_IS           	0x0100
#define R_PCH_PCR_GPIO_GPD_GPI_IE           	0x0120
#define R_PCH_PCR_GPIO_GPD_GPI_GPE_STS      	0x0140
#define R_PCH_PCR_GPIO_GPD_GPI_GPE_EN       	0x0160
#define R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET    	0x400

// GPIO Community 3 Private Configuration Registers
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_I_PAD_OWN        	0x20
#define R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCK     	0x90
#define R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCKTX   	0x94
#define R_PCH_H_PCR_GPIO_GPP_I_HOSTSW_OWN     	0xD0
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_IS         	0x0100
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_IE         	0x0120
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_STS    	0x0140
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_EN		0x0160
#define R_PCH_H_PCR_GPIO_GPP_I_SMI_STS        	0x0180
#define R_PCH_H_PCR_GPIO_GPP_I_SMI_EN         	0x01A0
#define R_PCH_H_PCR_GPIO_GPP_I_NMI_STS        	0x01C0
#define R_PCH_H_PCR_GPIO_GPP_I_NMI_EN         	0x01E0
#define R_PCH_H_PCR_GPIO_GPP_I_PADCFG_OFFSET  	0x400

//----------------------------------------------------------
// Definition for SBI PID
// The PCH_SBI_PID defines the PID for PCR MMIO programming and PCH SBI programming as well.
typedef enum
{
  PID_GPIOCOM0		= 0xAF,
  PID_GPIOCOM1      = 0xAE,
  PID_GPIOCOM2      = 0xAD,
  PID_GPIOCOM3      = 0xAC,
} PCH_SBI_PID;

typedef struct
{
  UINT32			Community;
  UINT32 			PadOwnOffset;
  UINT32 			HostOwnOffset;
  UINT32 			GpiIsOffset;
  UINT32 			GpiIeOffset;
  UINT32 			GpiGpeStsOffset;
  UINT32 			GpiGpeEnOffset;
  UINT32 			SmiStsOffset;
  UINT32 			SmiEnOffset;
  UINT32 			NmiStsOffset;
  UINT32 			NmiEnOffset;
  UINT32 			PadCfgLockOffset;
  UINT32 			PadCfgLockTxOffset;
  UINT32 			PadCfgOffset;
  UINT32 			PadPerGroup;
} AMI_GPIO_GROUP_INFO;

typedef enum
{
  H_Gpio_GPP_A 		= 0x0100,
  H_Gpio_GPP_B,
  H_Gpio_GPP_C,
  H_Gpio_GPP_D,
  H_Gpio_GPP_E,
  H_Gpio_GPP_F,
  H_Gpio_GPP_G,
  H_Gpio_GPP_H,
  H_Gpio_GPP_I,
  H_Gpio_GPD,
  Gpio_Group_Max
} AMI_GPIO_GROUP_TYPE;

typedef struct
{
  AMI_GPIO_GROUP_TYPE	Group;//4
  UINT8					PinNum;//1
} AMI_OEM_GPIO;

//----------------------------------------------------------
VOID			GpioBaseInit            (VOID);
VOID			SbProgGpioUseSel        (AMI_OEM_GPIO Gpio, BOOLEAN GpioMode, UINT8 MulFunc);
VOID 			SbSetGpioIoSel          (AMI_OEM_GPIO Gpio, BOOLEAN InputMode);
VOID			SbSetGpioLvlSel         (AMI_OEM_GPIO Gpio, BOOLEAN High);
BOOLEAN			SbGetGpioLvlSel         (AMI_OEM_GPIO Gpio);


