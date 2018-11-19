/*******************************************************************************

  pmb_tst.c: main application for Lanner platform - show power supply HWM, Seial number and redundant status.

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

// Refer to PSU FSP550-50ERS spec.

//#if defined(HEADER_FILE_IN_ROOT)
  #define  DIRECT_IO_ACCESS
  #include "../ioaccess.c"
//#else
  //#include "..\include\config.h"
  //#include "..\include\ioaccess.h"
//#endif

//----------------------------------------------------------
#define ENABLE_DEBUG_OUTPUT         0   // 1 := Enable extra debug info. output
#define SHOW_SERIAL_EEPROM_INFO     1   // 1 := If Power Supply Unit(PSU) is supported Serial EEPROM.
//-----------------------------------------------------------------------------
struct	
{
	char 	strProjectName[36];
	char    *strShowUtilityInfo;
} ProjectInfo[] = { { "SignOnMessage:NCB-4210",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***     NCB-4210  Power PMBUS utility   V1.1 2015-12-16      ***\n"\
					  "****************************************************************\n" },		  

					{ "SignOnMessage:NCB-5210",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***     NCB-5210  Power PMBUS utility   V1.0 2015-12-09      ***\n"\
					  "****************************************************************\n" }, 				  

					{ "SignOnMessage:LB-5210Z",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***     LB-5210Z  Power PMBUS utility   V1.0 2016-07-25      ***\n"\
					  "****************************************************************\n" },

					{ "SignOnMessage:LB-4210Z",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***     LB-4210Z  Power PMBUS utility   V1.0 2016-09-02      ***\n"\
					  "****************************************************************\n" } };

//-----------------------------------------------------------------------------

//----------------------------------------------------------
// PSU/PMBUS command definitons
//
#define VOUT_MODE               0x20    // (Read Byte)
#define FAN_CONFIG_1_2          0x3A    // (Read Byte) Only supported for module
#define FAN_COMMAND_1           0x3B    // (Read Word) Only supported for module
#define STATUS_WORD             0x79    // (Read Word)
#define STATUS_VOUT             0x7A    // (Read Byte)
#define STATUS_IOUT             0x7B    // (Read Byte) Page member
#define READ_VOUT               0x8B    // (Read Word) Page member
#define READ_IOUT               0x8C    // (Read Word) Page member
#define READ_TEMPTURE_1         0x8D    // (Read Word)
#define PMBUS_REVISION          0x98    // (Read Byte) Value 22h

#define STATUS_INPUT            0x7C    // (Read Byte) Only supported for module
#define STATUS_TEMPERATURE      0x7D    // (Read Byte) Only supported for module
#define STATUS_FANS_1_2         0x81    // (Read Byte) Only supported for module
#define READ_VIN                0x88    // (Read Word) Only supported for module
#define READ_TEMPTURE_2         0x8E    // (Read Word) Only supported for module
#define READ_FAN_SPEED_1        0x90    // (Read Word) Only supported for module
#define READ_PIN                0x97    // (Read Word) Only supported for module

#define MFR_FW_ID               0xD1    // Undocument

//----------------------------------------------------------
const UINT8     _PSU_BpSlaveAddr        =   0xAC;
const UINT8     _PSMC_SlaveAddr[2]      = { 0xB0, 0xB2 };   // Power Supply Management Controller
const UINT8     _PSU_EepSlaveAddr[2]    = { 0xA0, 0xA2 };   // Embedded EEPROM inside power supply

const UINT8     _WB_00_PEC[2]           = { 0xEA, 0x3C };
const UINT8     _WB_20_PEC[2]           = { 0x0A, 0xDC };

float  exponent[32]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8,4,2};

//----------------------------------------------------------
// Convert a LinearFloat5_11 formatted word into a floating point value
//
float           L11_to_float (UINT16 input_val)
{
  float  d=(float)(input_val & 0x03FF);
  if(input_val&0x400)
  {
	  d = -d;
  }
  if(input_val&0x8000)
  {
    #if debug
    printf("%f / %f \n",d,exponent[(input_val>>11)&0x001F]);
    #endif
    return (d / (exponent[(input_val>>11)&0x001F]));
  }
  else
  {
    #if debug
    printf("%f * %f \n",d,exponent[(input_val>>11)&0x001F]); 
    #endif
    return (d *(exponent[(input_val>>11)&0x001F]));
  }
}
//----------------------------------------------------------

INT16    main (INT16 argc, char** argv )
{
iopl(3);
     //char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 //unsigned int 	pStrLen;
     //int  Flag,j;

     //pStrLen = (unsigned long) (strchr(pstr, ' ') - pstr);

	 //Flag = 0;
	 //for ( j = 0; j < (sizeof(ProjectInfo)/sizeof(ProjectInfo[0])); j++ )
	 //{
	 //	 if ( !strncmp(pstr, ProjectInfo[j].strProjectName, pStrLen) )
	 //	 {
	 //		 Flag = 1;
	 //		 break;
	 //	 }
	 //}

//	 Project_name();

     //if ( Flag )
	//	 printf("%s", ProjectInfo[j].strShowUtilityInfo);
	 //else
	//	 printf("Error : Lanner no this platform name...\n");

         printf("NCA-5220A Power PMBUS utility\n");

         GpioBaseInit();
         //AMI_OEM_GPIO    G11 = { H_Gpio_GPP_G,11};
         //SbSetGpioIoSel (G11, FALSE);
         //SbSetGpioLvlSel(G11, FALSE);
		 SetGpioLvlSel(0x6B,0x6B0,0);
//----------------------------------------------------------

  #define EXIT_ERRDMP_MAIN \
       { ShowErrCodeMessage(NULL, FALSE);\
	       sprintf(_ErrStrBuf, "  main (...), Other App. is used the SMBus now...\n"); \
	        assert(FALSE);}

  #define EXIT_ERRDMP2_MAIN \
	     { ShowErrCodeMessage(NULL, TRUE);\
	       sprintf(_ErrStrBuf, "  main (...), Cannot recover SMB Controller state. (Might be caused by incorrect SMBus I/O base address)\n"); \
	       assert(FALSE);}


     BOOLEAN         SM_BP = TRUE;       // Assume PSU Serial Number Info. is supported.
     BOOLEAN         SM_P1 = TRUE;       // Assume PSU Controller#1 is supported.
     BOOLEAN         SM_P2 = TRUE;       // Assume PSU Controller#2 is supported.
     BOOLEAN         SM_A0 = TRUE;       // Assume PSU Eeprom#1 is supported.
     BOOLEAN         SM_A2 = TRUE;       // Assume PSU Eeprom#2 is supported.
     BOOLEAN         bWarning = FALSE;

     INT8            ByteNums, ByteCnt, PwrModule, ByteValue;
     UINT16          WordValue;

     _SmbBaseIoAddr  = GetSmbBaseIoAddr();
	 //printf("_SmbBaseIoAddr=%x\n",_SmbBaseIoAddr);
	 
/*  if ( (_SmbError = inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS)) & SMBC_F_IUS )
	// SMBus controller is owned by other App.
	EXIT_ERRDMP_MAIN; 
   
    if ( IsSmbcBusy() == TRUE )
	// Error Handler
	EXIT_ERRDMP2_MAIN;

//----------------------------------------------------------

     SMB_ReadByte(_PSU_BpSlaveAddr, 0x00);
     if ( _SmbError )
     {
	 printf("Power BP read fail!\n");
	 SM_BP = FALSE;
     }*/

     SMB_ReadByte(_PSMC_SlaveAddr[0], 0x00);
     if ( _SmbError )
     {
	 printf("Power Module 1 PMBUS fail!\n");
	 SM_P1 = FALSE;
     }

     SMB_ReadByte(_PSMC_SlaveAddr[1], 0x00);
     if ( _SmbError )
     {
	 printf("Power Module 2 PMBUS fail!\n");
	 SM_P2 = FALSE;
     }

     SMB_ReadByte(_PSU_EepSlaveAddr[0], 0x00);
     if ( _SmbError )
     {
	 printf("Power Module 1 EEPROM read fail!\n");
	 SM_A0 = FALSE;
     }

     SMB_ReadByte(_PSU_EepSlaveAddr[1], 0x00);
     if ( _SmbError )
     {
	 printf("Power Module 2 EEPROM read fail!\n");
	 SM_A2 = FALSE;
     }

//----------------------------------------------------------
     // Show EEPROM info. in PSU.
     if ( SHOW_SERIAL_EEPROM_INFO )
     {
	 printf("<<<<<   The EEPROM Info. in Power Supply Unit   >>>>> \n");
	 // Use Command 0xD1, byte 9, 0x41(A):AC power,0x44(D):DC power
	 if ( SM_P1 )
	 {
	     printf("Power Module 1 Power Mode:");

	     ByteNums = SMB_ReadBlock(_PSMC_SlaveAddr[0], MFR_FW_ID);
	     if ( _BlockBuf[8] == 0x41 )
		 printf("AC Power Mode\n");
	     else if ( _BlockBuf[8] == 0x44 )
		 printf("DC Power Mode\n");
	     else
		 printf("Unknow AC/DC Power Mode\n");

	#if ENABLE_DEBUG_OUTPUT
	     for ( ByteCnt= 0; ByteCnt < ByteNums; ByteCnt++ )
		 printf("%c", _BlockBuf[ByteCnt]);
	     printf("\n");
	#endif //ENABLE_DEBUG_OUTPUT
	 }

	 if ( SM_P2 )
	 {
	     printf("Power Module 2 Power Mode:");

	     ByteNums = SMB_ReadBlock(_PSMC_SlaveAddr[1], MFR_FW_ID);
	     if (_BlockBuf[8] == 0x41)
		 printf("AC Power Mode\n");
	     else if (_BlockBuf[8] == 0x44)
	  	 printf("DC Power Mode\n");
	     else
		 printf("Unknow AC/DC Power Mode\n");

	#if ENABLE_DEBUG_OUTPUT
	     for ( ByteCnt= 0; ByteCnt < ByteNums; ByteCnt++ )
		 printf("%c", _BlockBuf[ByteCnt]);
	     printf("\n");
	#endif //ENABLE_DEBUG_OUTPUT
	 }

	//----------------------------------------------------
	/* printf("Power BP SN:");
	 if ( SM_BP )
	 {
	     for ( ByteCnt= 0x2E; ByteCnt < 0x40; ByteCnt++ )
		 printf("%c", SMB_ReadByte(_PSU_BpSlaveAddr, ByteCnt));
	     printf("\n");
	 }
	 else
	     printf("read fail!\n");*/

	 printf("Power Module1 SN:");
	 if ( SM_A0 )
	 {
	     for ( ByteCnt= 0x2E; ByteCnt < 0x40; ByteCnt++ )
		 printf("%c", SMB_ReadByte(_PSU_EepSlaveAddr[0], ByteCnt));
	     printf("\n");
	 }
	 else
	     printf("read fail!\n");

	 printf("Power Module2 SN:");
	 if ( SM_A2 )
	 {
	     for ( ByteCnt= 0x2E; ByteCnt < 0x40; ByteCnt++ )
		 printf("%c", SMB_ReadByte(_PSU_EepSlaveAddr[1], ByteCnt));
	     printf("\n");
	 }
	 else
	     printf("read fail!\n");
     }//if ( SHOW_SERIAL_EEPROM_INFO )

//----------------------------------------------------------
     for ( PwrModule= 0; PwrModule < 2; PwrModule++ )
     {
	 if ((SM_P1 == FALSE) && (PwrModule == 0x00))
	     continue;

	 if ((SM_P2 == FALSE) && (PwrModule == 0x01))
	     continue;

	 printf("\n================= Power Module %x  =================\n", PwrModule + 1);
	 printf("Power Module Status:");

	 WordValue = SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], STATUS_WORD);
    #if ENABLE_DEBUG_OUTPUT
	 printf("%X  \n", WordValue);
    #endif //ENABLE_DEBUG_OUTPUT

	 if ( WordValue & 0xEC7C )
	 {
	     if ( WordValue & BIT2 )
	     { // Temperature Warnning / Error
		 ByteValue = SMB_ReadByte(_PSMC_SlaveAddr[PwrModule], STATUS_TEMPERATURE);
	#if ENABLE_DEBUG_OUTPUT
		 printf("Temperature status : %X  \n", ByteValue);
	#endif //ENABLE_DEBUG_OUTPUT
		 if ( ByteValue & BIT7 )
		     printf("Over Temperature Fault\n");
		 else if ( ByteValue & BIT6 )
		     printf("Over Temperature Warning\n");
		 }

	     if ( WordValue & BIT3 )
	     { // Iuput Under Voltage
		 printf("Iuput under voltage Fault\n");
	     }

	     if ( WordValue & BIT4 )
	     { // Output Over Current
		 printf("Output over current Fault\n");
	     }

	     if ( WordValue & BIT5 )
	     { // Output Over Voltage
		 printf("Output over voltage Fault\n");
	     }

	     if ( WordValue & BIT6 )
	     { // OFF
		 printf("Unit not provide power to output.\n");
	     }

		 if ( WordValue & BIT10 )
	     { // Fan
		 ByteValue = SMB_ReadByte(_PSMC_SlaveAddr[PwrModule], STATUS_FANS_1_2);
	#if ENABLE_DEBUG_OUTPUT
		 printf("FAN_1_2 status : %X  \n", ByteValue);
	#endif //ENABLE_DEBUG_OUTPUT
		 if ( ByteValue & BIT7 )
		      printf("Fan 1 Fault\n");
		 else if ( ByteValue & BIT5 )
		      printf("Fan 1 Warning\n");
		 else if ( ByteValue & BIT3 )
		      printf("Fan 1 Overridden\n");
	     }

	     if ( WordValue & BIT11 )
	     { // Power good
		 printf("POWER_GOOD# present.\n");
	     }

	     if ( WordValue & BIT13 )
	     { // Input Current / Voltage
		 ByteValue = SMB_ReadByte(_PSMC_SlaveAddr[PwrModule], STATUS_INPUT);
	#if ENABLE_DEBUG_OUTPUT
		 printf("VIN status : %x  \n", ByteValue);
	#endif //ENABLE_DEBUG_OUTPUT
		 if ( ByteValue & BIT6 )
		      printf("VIN Over Voltage Warning\n");
		 else if ( ByteValue & BIT5 )
		      printf("VIN Under Voltage Warning\n");
		 else if ( ByteValue & BIT4 )
		      printf("VIN Under Voltage Fault\n");
		 else if ( ByteValue & BIT7 )
		      printf("Unit is off for insufficient input voltage.\n");
	     }

	     if ( WordValue & BIT14 )
	     { // Output Current / Voltage
		 ByteValue = SMB_ReadByte(_PSMC_SlaveAddr[PwrModule], STATUS_IOUT);
	#if ENABLE_DEBUG_OUTPUT
		 printf("Vout status : %x  \n", ByteValue);
	#endif //ENABLE_DEBUG_OUTPUT
		 if ( ByteValue & BIT7 )
		      printf("IOUT Overcurrent Fault\n");
		 else if ( ByteValue & BIT5 )
		      printf("IOUT Overcurrent Warning\n");
	     }

	     if ( WordValue & BIT15 )
	     { // Output Voltage
	#if ENABLE_DEBUG_OUTPUT
		 printf("Vout status : %x  \n",ByteValue);
	#endif //ENABLE_DEBUG_OUTPUT
		 SMB_WriteByte(_PSMC_SlaveAddr[PwrModule], 0x00, 0x00);
		 ByteValue = SMB_ReadByte(_PSMC_SlaveAddr[PwrModule], STATUS_VOUT);
		 if ( ByteValue & BIT7 )
		      printf("VOUT 12V Over Voltage Fault\n");
		 else if ( ByteValue & BIT6 )
		      printf("VOUT 12V Over Voltage Warning\n");
		 else if ( ByteValue & BIT5 )
		      printf("VOUT 12V Under Voltage Warning\n");
		 else if ( ByteValue & BIT4 )
		      printf("VOUT 12V Under Voltage Fault\n");

		 SMB_WriteByte_PEC(_PSMC_SlaveAddr[PwrModule], 0x00, 0x20, _WB_20_PEC[PwrModule]);
		 ByteValue = SMB_ReadByte(_PSMC_SlaveAddr[PwrModule], STATUS_VOUT);
		 if ( ByteValue & BIT7 )
		      printf("VOUT 5V Over Voltage Fault\n");
		 else if ( ByteValue & BIT6 )
		      printf("VOUT 5V Over Voltage Warning\n");
		 else if ( ByteValue & BIT5 )
		      printf("VOUT 5V Under Voltage Warning\n");
		 else if ( ByteValue & BIT4 )
		      printf("VOUT 5V Under Voltage Fault\n");
	     }
	 }
	 else
	     printf("No Warning/Error Status\n");

	 if ( SHOW_SERIAL_EEPROM_INFO )
	 {
	      SMB_WriteByte(_PSMC_SlaveAddr[PwrModule], 0x00, 0x00);
	#if ENABLE_DEBUG_OUTPUT
	      printf("page 0 READ_VOUT: %X  \t", SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT));
	#endif //ENABLE_DEBUG_OUTPUT
	      printf("12V: %.3f  \t\t", L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT)));
	      if ( L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT)) <= 10 )
		       bWarning = TRUE;
	      if ( L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT)) >= 14 )
		       bWarning = TRUE;

	           SMB_WriteByte_PEC(_PSMC_SlaveAddr[PwrModule], 0x00, 0x20, _WB_20_PEC[PwrModule]);
	#if ENABLE_DEBUG_OUTPUT
	           printf("page 0x20 READ_VOUT: %x  \t", SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT));
	#endif //ENABLE_DEBUG_OUTPUT
	           printf("5Vsb: %.3f  \n", L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT)));
	      if ( L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT)) <= 3 )
		       bWarning = TRUE;
	      if ( L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_VOUT)) >=7 )
		       bWarning = TRUE;

	           SMB_WriteByte(_PSMC_SlaveAddr[PwrModule], 0x00, 0x00);
	#if ENABLE_DEBUG_OUTPUT
	           printf("page 0 READ_IOUT: %x  \t", SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_IOUT));
	#endif //ENABLE_DEBUG_OUTPUT

	           SMB_WriteByte_PEC(_PSMC_SlaveAddr[PwrModule], 0x00, 0x20, _WB_00_PEC[PwrModule]);
	#if ENABLE_DEBUG_OUTPUT
	           printf("page 0x20 READ_IOUT: %x  \t", SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_IOUT));
	#endif //ENABLE_DEBUG_OUTPUT
	           printf("Fan Speed: %u  \t", (int) L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_FAN_SPEED_1)));
	      if ( L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_FAN_SPEED_1)) <= 0 )
		       bWarning = TRUE;

	           printf("Temp1: %d  \t", (int) L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_TEMPTURE_1)));
	           printf("Temp2: %d  \n", (int) L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_TEMPTURE_2)));
	      if (L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_TEMPTURE_1)) <= 0)
		      bWarning = TRUE;
	      if (L11_to_float(SMB_ReadWord(_PSMC_SlaveAddr[PwrModule], READ_TEMPTURE_2)) <= 0)
		      bWarning = TRUE;
	 }//if ( SHOW_SERIAL_EEPROM_INFO )
     }//for ( PwrModule= 0; PwrModule <  2; PwrModule++ )

     if ( bWarning )
	      printf("PMBUS Value fail\n");
//----------------------------------------------------------
		 SetGpioLvlSel(0x6B,0x6B0,1);
	     //SbSetGpioLvlSel(G11, TRUE);

}

