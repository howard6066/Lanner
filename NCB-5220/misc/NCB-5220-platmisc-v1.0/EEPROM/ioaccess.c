/*******************************************************************************
  ioaccess.c: Lanner's common GPIOs, SIO and SMBus interfaces on Skylake(SKL-H) platform

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

// 20150811: (Rev 1.0)

//#if defined(HEADER_FILE_IN_ROOT)
//#define  DIRECT_IO_ACCESS
  #include "ioaccess.h"
//#else
  //#include "..\include\config.h"
  //#include "..\include\ioaccess.h"
//#endif

//=======================================================================================
//  Project_name
//=======================================================================================
// For Skylake(SKL) PCH-H platform
//---------------------------------------------------------------

/*
  void * MK_FP (unsigned short seg, unsigned short ofs)
  {
    if ( !(_crt0_startup_flags & _CRT0_FLAG_NEARPTR) )
      if (!__djgpp_nearptr_enable ())
        return (void *)0;
    return (void *) (seg*16 + ofs + __djgpp_conventional_base);
  }
    void 	Get_Project_Strname (char strName[])
  {
	 char *name;
     char *name2;
     char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 int Idx, strLen;

	    name=strstr(pstr,":")+1;
	    name2 = strstr(name," ") - 2;
		strLen = name2 - name + 1;
		//printf("Start:%X, End:%X, Len:%X\n", name, name2, strLen);
        strncpy(strName, name, strLen);
        strName[strLen]	=0x00;
		//printf("%s\n", strName);

  }

    void 	Get_Strsku (char strSku[])
  {
	 char *sku;
	 char *sku2;
     char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 int Idx;

	     sku = strstr(pstr," ");
		 sku2=(sku-1);
		 //printf("%c\n", *sku2);

  }
    void 	Get_Strver (char strVer[])
  {
	 char *ver;
     char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 int Idx;

	     ver = strstr(pstr,".");
		 Idx = 1;
		 while ( ver[Idx] && ver[Idx]!=' ' )
		 {
			 strVer[Idx-1] = ver[Idx];
			 Idx ++;
		 }
		 strVer[Idx-1] = 0x00;
		 //printf("%s\n", strVer);

  }
    void 	Get_Strdate (char strDate[])
  {

     char *Day;
	 char *test;
     char *pstr = (char *) MK_FP(0xF000, 0xF58B);
     int Idx;

	     test = strstr(pstr,".");
	     Day =  strstr(test," ");
		 Idx = 1;
		 while ( Day[Idx] && Day[Idx]!=' ' )
		 {
			 strDate[Idx-1] = Day[Idx];
			 Idx ++;
		 }
		 strDate[Idx-1] = 0x00;
		 //printf("%s\n", strDate);
  }

  UINT8 	Project_name (VOID)
  {
	 char str3[ ]="          ";
     char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 char str2[15]="SignOnMessage:";
	 int  j,k;
	
	 k=sizeof(str2)-1;
	 j=strncmp(pstr,str2,k);

	 if (j==0)
	 {
	     strncpy(str3,pstr+k,9);
         printf("%sThis is only Lanner Skylake platform",str3);
     }
         printf("\n");
  }

//===============================================================================================
#if defined(DIRECT_IO_ACCESS) || (defined(MODULE) || defined(KLD_MODULE))
*/
//=======================================================================================
//  SIO Interface   (I/O Base Addressing)
//=======================================================================================
// For Skylake(SKL) PCH-H platform
//---------------------------------------------------------------
VOID            EnterSioConfigMode (VOID)
{
    outportb (SIO_IDX_PORT, 0x87);      // Must do it twice per spec. requested
    outportb (SIO_IDX_PORT, 0x87);
}

//---------------------------------------------------------------
VOID            ExitSioConfigMode (VOID)
{
    outportb (SIO_IDX_PORT, 0xAA);
}

//---------------------------------------------------------------
UINT8           ReadSioReg (UINT8 Ldn, UINT8 Reg)
{
    UINT8       RetData = 0x00;

    EnterSioConfigMode();

    outportb (SIO_IDX_PORT, 0x07);      // Select to Logical Device Number(LDN) Register
    outportb (SIO_DAT_PORT, Ldn);       // Write Ldn to SIO
    outportb (SIO_IDX_PORT, Reg);       // Write regster/index for access to
    RetData = inportb (SIO_DAT_PORT);   // Get data byte from SIO's register

    ExitSioConfigMode();

    return RetData;
}

//---------------------------------------------------------------
VOID            WriteSioReg (UINT8 Ldn, UINT8 Reg, UINT8 Value)
{
    EnterSioConfigMode();

    outportb (SIO_IDX_PORT, 0x07);      // Select to Logical Device Number(LDN) Register
    outportb (SIO_DAT_PORT, Ldn);       // Write Ldn to SIO
    outportb (SIO_IDX_PORT, Reg);       // Write regster/index for access to
    outportb (SIO_DAT_PORT, Value);     // Write data byte into SIO's register

    ExitSioConfigMode();
}

//=======================================================================================
//  HWM Interface   (I/O Base Addressing)(NCT7904D)
//=======================================================================================
// For Skylake(SKL) PCH-H platform
//---------------------------------------------------------------
int write_NCT7904_reg(int reg, int value)
{
	int	i,j;		
 	for(j=0;j<10;j++){ 	
		for(i=0;i<10;i++){	
			outportb((_SmbBaseIoAddr+0x00),0xfe);//clr status
  			delay(10);	
  			if (check_bsy_err(5)==0x00){break;}			
		   }
  	
  	outportb((_SmbBaseIoAddr+0x03),reg);//offset
  	delay(5);
  	outportb((_SmbBaseIoAddr+0x05),value);//value
  	delay(5);  
  	outportb((_SmbBaseIoAddr+0x04),(NCT7904_SlaveAdd& 0xfe));
  	delay(5);  
  	outportb((_SmbBaseIoAddr+0x02),0x48);//byte write cmd
  	delay(20);//more delay for cmd finish 
    	if (check_bsy_err(5)==0x00){break;}			
 	}				
  	outportb((_SmbBaseIoAddr+0x00),0xfe);//clr status
  	delay(10);
	return(0);
}
int read_NCT7904_reg(int reg)
{
	int	i,j;		
 	for(j=0;j<10;j++){ 
		for(i=0;i<10;i++){	
			outportb((_SmbBaseIoAddr+0x00),0xfe);//clr status
  			delay(10);	
  			if (check_bsy_err(5)==0x00){break;}			
	    	}	
	  	    	  		
  		outportb((_SmbBaseIoAddr+0x03),reg);//offset
  		delay(2);
  		outportb((_SmbBaseIoAddr+0x04),(NCT7904_SlaveAdd|0x01));
  		delay(2);  
  		outportb((_SmbBaseIoAddr+0x02),0x48);//byte write cmd
  		delay(10);//more delay for cmd finish 
 	    	if (check_bsy_err(5)==0x00){break;}			
  	    }				
  	outportb((_SmbBaseIoAddr+0x00),0xfe);//clr status
  	delay(10);
	return inportb(_SmbBaseIoAddr+0x05);//value
  	
}
//busy_count = 0.1x sec time out
check_bsy_err(int busy_count){
int	k;	
if(inportb(_SmbBaseIoAddr+0x00)&0x04){ //error?
//   	 printf("\n Read W792 device error!!\n");
//	 exit(1);
	 return(1);
  	}
for(k=0;k<busy_count;k++){  	
	if(inportb(_SmbBaseIoAddr+0x00)&0x09){ delay(1000);}
	else{ return(0);}
}
if(inportb(_SmbBaseIoAddr+0x00)&0x01){
	 printf("\n Read NCT7904 device busy time out!!\n");
 	 return(2);
	}
if(inportb(_SmbBaseIoAddr+0x00)&0x08){
	 printf("\n smbus bus error  time out!!\n");
 	 return(3);
	}
}	

//=======================================================================================
//  HWM Interface   (I/O Base Addressing)(NCT6776F)
//=======================================================================================
// For Skylake(SKL) PCH-H platform
//---------------------------------------------------------------
UINT8           read_HM_reg (UINT8 hmbank, UINT8 hmreg)
{
	UINT8       RetData = 0x00;

    outportb (HM_INDEX_PORT, 0x4E);      
    outportb (HM_DATA_PORT , hmbank);     
    outportb (HM_INDEX_PORT, hmreg);       
    RetData = inportb (HM_DATA_PORT);   

    return RetData;
}

//---------------------------------------------------------------
VOID            write_HM_reg (UINT8 hmbank, UINT8 hmreg, UINT8 Value)
{

    outportb (HM_INDEX_PORT, 0x4E);      
    outportb (HM_DATA_PORT , hmbank);       
    outportb (HM_INDEX_PORT, hmreg);      
    outportb (HM_DATA_PORT , Value);    

}

//=======================================================================================
//  SMBUS Interface (I/O Base Addressing)
//=======================================================================================
// For Skylake(SKL) PCH-H platform
//----------------------------------------------------------
UINT16          _SmbBaseIoAddr;             // SMB/HC I/O base address which is located in B00:D31:F04:R20h
UINT8           _SmbError;                  // Global variable that indicates the status of SMB transmission.
UINT8           _BlockBuf[32];              // Global buffer for block transmission
UINT8           _ErrStrBuf[256] = { 0x00 }; // Global buffer for error message output before terminate this program.

//----------------------------------------------------------
UINT16          GetSmbBaseIoAddr (VOID)
{
    outportl(PCI_IDX_REG, PCH_SMBHOST_BASEIO_PFA);

    return (inportw(PCI_DAT_REG) & 0xFFFE);     // Get Smbus Controller Base I/O Address
}

//----------------------------------------------------------------------------
// Return:
//      TRUE := SMBC is in busy state or somehow in controller
//      FALSE:= SMBC is in avaliable state
//
BOOLEAN         IsSmbcBusy (VOID)
{
    INT16       u16TimeOutLoop = 100;   // 100ms timeout time totally

	_SmbError = 0x00;       // Assume no error on SMB tranmission

    while ( u16TimeOutLoop-- && (inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS) & SMBC_F_HBSY) )
	delay(10);      // delay 1 ms

    if ( !u16TimeOutLoop )
    { // It's timeout then force to stop current transaction
	outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, SMBC_F_KILL);
	delay(100);     // delay 10 ms
	outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, 0x00);
	delay(100);     // delay 10 ms
    };

    // Clear SMBC Status Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);
    delay(10);          // delay 1 ms

    _SmbError = inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS) & (SMBC_F_ERRS | SMBC_F_HBSY);
    
    return _SmbError? TRUE: FALSE;
}

//----------------------------------------------------------------------------
// Return:
//      TRUE := Successful
//      FALSE:= Failed to do this transaction
//
BOOLEAN         IsCmdCompleted (VOID)
{
    INT16       u16TimeOutLoop = 50 * 10;   // 50ms(500 * 100us) timeout time totally

	_SmbError = 0x00;       // Assume no error on SMB tranmission

    while ( u16TimeOutLoop-- && (inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS) & SMBC_F_HBSY) )
	delay(1);   // delay 100 us

    _SmbError = inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS) & (SMBC_F_ERRS | SMBC_F_HBSY);

    return _SmbError? FALSE: TRUE;
}

//----------------------------------------------------------------------------
VOID            ShowErrCode (const UINT8 *ErrMessage)
{
    //printf(ErrMessage == NULL? _ErrStrBuf: ErrMessage);
    //printf("        The content of Host Status Register : %02X\n", _SmbError);

}

//----------------------------------------------------------------------------
VOID            ShowErrCodeMessage (const UINT8 *ErrMessage, BOOLEAN bClrIUS)
{   

    printf("PCH SMBus/Host Controller IO Base Address : %04X\n", _SmbBaseIoAddr);

    ShowErrCode(ErrMessage);

    if ( bClrIUS == TRUE )
    {
		
	// Clear SMBC Status Register
	outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, 0xFF);    //0x00

	// Disable/Clear CRC function, error flag and 32 Byte Buffer
	outportb(_SmbBaseIoAddr + PCH_SMBC_REG_AUXS, SMBC_F_CRCE);  //0x0C   bit 0
	outportb(_SmbBaseIoAddr + PCH_SMBC_REG_AUXC, 0x00);   //0x0D    0x00

    }
	
}

//----------------------------------------------------------------------------
VOID            SMB_WriteByte (UINT8 SlaveAddr, UINT8 SlaveByteOfs, UINT8 SlaveData)
{
  #define EXIT_ERRDMP_SMB_WRITEBYTE \
       {ShowErrCodeMessage(NULL, TRUE); \
	    sprintf(_ErrStrBuf, "  SMB_WriteByte (%02X, %02X, %02X)\n", SlaveAddr, SlaveByteOfs, SlaveData); \
	    assert(FALSE);}

    if ( IsSmbcBusy() == TRUE )
	EXIT_ERRDMP_SMB_WRITEBYTE;

    // Write slave address with <Write> protocol
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_TSA,  SlaveAddr & (~BIT0));

    // Write SMB protocol settings
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, SMB_PROTOCOL_BYTEDATA);

    // Set index byte for write DATA0 into device
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCMD, SlaveByteOfs);

    // Write data byte to PCH/SMB Data0 Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HDO, SlaveData);

    // Clear SMBC Status Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);
    
    // Enable SMBC starting transaction
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL) | SMBC_F_START);

    if ( IsCmdCompleted() == FALSE )
	EXIT_ERRDMP_SMB_WRITEBYTE;
 }  

//----------------------------------------------------------------------------
VOID            SMB_WriteByte_PEC (UINT8 SlaveAddr, UINT8 SlaveByteOfs, UINT8 SlaveData, UINT8 SlavePEC)
{
  #define EXIT_ERRDMP_SMB_WRITEBYTE_PEC \
       {ShowErrCodeMessage(NULL, TRUE); \
	    sprintf(_ErrStrBuf, "  SMB_WriteByte_PEC (%02X, %02X, %02X, %02X)\n", SlaveAddr, SlaveByteOfs, SlaveData, SlavePEC); \
	    assert(FALSE);}

    if ( IsSmbcBusy() == TRUE )
	EXIT_ERRDMP_SMB_WRITEBYTE_PEC;

    // Write slave address with <Write> protocol
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_TSA,  SlaveAddr & (~BIT0));

    // Write SMB's PEC and protocol settings
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, SMBC_F_PEC_EN | SMB_PROTOCOL_BYTEDATA);

    // Set index byte for write DATA0 into device
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCMD, SlaveByteOfs);

    // Write data byte to PCH/SMB Data0 Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HDO, SlaveData);

    // Enable Automatically Append CRC
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_AUXC, SMBC_F_AAC);

    // Write PEC data byte to PCH/SMB Packet Error Check Data Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_PEC, SlavePEC);

    // Clear SMBC Status Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);

    // Enable SMBC starting transaction
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL) | SMBC_F_START);

    if ( IsCmdCompleted() == FALSE )
	EXIT_ERRDMP_SMB_WRITEBYTE_PEC;

    // Disable/Clear CRC function, error flag and 32 Byte Buffer
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_AUXS, SMBC_F_CRCE);
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_AUXC, 0x00);
}

//----------------------------------------------------------------------------
UINT8           SMB_ReadByte (UINT8 SlaveAddr, UINT8 SlaveByteOfs)
{
  #define EXIT_ERRDMP_SMB_READBYTE \
       {ShowErrCodeMessage(NULL, TRUE); \
	    sprintf(_ErrStrBuf, "  SMB_ReadByte (%02X, %02X)\n", SlaveAddr, SlaveByteOfs); \
	    assert(FALSE);}

  #define ERRDMP_SMB_READBYTE \
       {sprintf(_ErrStrBuf, "  SMB_ReadByte (%02X, %02X)\n", SlaveAddr, SlaveByteOfs); \
	    ShowErrCode(NULL);}

    if ( IsSmbcBusy() == TRUE )
	EXIT_ERRDMP_SMB_READBYTE;

    // Write slave address with <Read> protocol
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_TSA,  SlaveAddr | BIT0);// 1 = read, 0 = write   0x04

    // Write SMB protocol settings
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, SMB_PROTOCOL_BYTEDATA);//  0x02  bit3   010b

    // Set index byte for read a byte from device	
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCMD, SlaveByteOfs);//  0x03  0x00

    // Clear SMBC Status Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);   //0x00   10111111
  
    // Enable SMBC starting transaction
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL) | SMBC_F_START);//0x02   01000000

    if ( IsCmdCompleted() == FALSE )
	ERRDMP_SMB_READBYTE;

    // Read data byte from PCH/SMB Data0 Register
    return _SmbError? 0xFF : inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HDO);  //0x05
}

//----------------------------------------------------------------------------
UINT16          SMB_ReadWord (UINT8 SlaveAddr, UINT8 SlaveByteOfs)
{
  #define EXIT_ERRDMP_SMB_READWORD \
       {ShowErrCodeMessage(NULL, TRUE); \
	    sprintf(_ErrStrBuf, "  SMB_ReadByte (%02X, %02X)\n", SlaveAddr, SlaveByteOfs); \
	    assert(FALSE);}

  #define ERRDMP_SMB_READWORD \
       {sprintf(_ErrStrBuf, "  SMB_ReadByte (%02X, %02X)\n", SlaveAddr, SlaveByteOfs); \
	    ShowErrCode(NULL);}

    if ( IsSmbcBusy() == TRUE )
	EXIT_ERRDMP_SMB_READWORD;

    // Write slave address with <Read> protocol
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_TSA,  SlaveAddr | BIT0);

    // Write SMB protocol settings
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, SMB_PROTOCOL_WORDDATA);

    // Set index byte for read a word from device
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCMD, SlaveByteOfs);

    // Clear SMBC Status Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);
    
    // Enable SMBC starting transaction
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL) | SMBC_F_START);

    if ( IsCmdCompleted() == FALSE )
	ERRDMP_SMB_READWORD;

    // Read data word from PCH/SMB Data0 and Data1 Registers
    return _SmbError? 0xFFFF : inportw(_SmbBaseIoAddr + PCH_SMBC_REG_HDO);
}

//----------------------------------------------------------------------------
UINT8           SMB_ReadBlock (UINT8 SlaveAddr, UINT8 SlaveByteNums)
{
  #define EXIT_ERRDMP_SMB_READBLOCK \
       {ShowErrCodeMessage(NULL, TRUE); \
	    sprintf(_ErrStrBuf, "  SMB_ReadByte (%02X, %02X)\n", SlaveAddr, SlaveByteNums); \
	    assert(FALSE);}

  #define ERRDMP_SMB_READBLOCK \
       {sprintf(_ErrStrBuf, "  SMB_ReadByte (%02X, %02X)\n", SlaveAddr, SlaveByteNums); \
	    ShowErrCode(NULL);}

    INT16       DataByteIdx, DataByteNums;

    if ( IsSmbcBusy() == TRUE )
	EXIT_ERRDMP_SMB_READBLOCK;

    // Write slave address with <Read> protocol
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_TSA,  SlaveAddr | BIT0);

    // Write SMB protocol settings
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, SMB_PROTOCOL_BLOCK);

    // How many BYTEs need to be read from device
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCMD, SlaveByteNums);

    // Clear SMBC Status Register
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);
    
    // Enable SMBC starting transaction
    outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL, inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HCTL) | SMBC_F_START);

// Voss(DBG)++ Not sure the actual behavior of READ/WRITE block protocol of the PCH SMBC/HW state machine,
//              Need to be confirmed later.
    if ( IsCmdCompleted() == FALSE )
	ERRDMP_SMB_READBLOCK;

    DataByteNums = inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HDO);
    for (DataByteIdx= 0; DataByteIdx < DataByteNums; DataByteIdx++ )
    {
		delay(5);   // delay 500 us

	if ( inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS) & SMBC_F_BDS )
	    _BlockBuf[DataByteIdx] = inportb(_SmbBaseIoAddr + PCH_SMBC_REG_HBD);
	else
	    _BlockBuf[DataByteIdx] = 0xFF;

	// Clear SMBC Status Register
		outportb(_SmbBaseIoAddr + PCH_SMBC_REG_HSTS, ~SMBC_F_IUS);

    #if ENABLE_DEBUG_OUTPUT
		printf(" %02X", _BlockBuf[DataByteIdx]);
	#endif
    };
  #if ENABLE_DEBUG_OUTPUT
    printf("\n");
  #endif
// Voss(DBG)-- 

    return DataByteNums;
}

//===============================================================
//  EEPROM related definition/declaration (I/O Base Addressing)
//===============================================================
// For Skylake(SKL) PCH-H platform
//----------------------------------------------------------
//Read EEPROM data
void Read_EEPROM(void)
{
	int	i;
     #ifdef Item1_string
     printf("%s",Item1_string);
     #else
     printf("Item 1:");
     #endif
   	for(i=0;i<EEPRPM_SIZE;i++)
   	{ 
   		if (SMB_ReadByte(sm_target_addr,i)=='#') 
		{
    			break;
   		}
   		if (SMB_ReadByte(sm_target_addr,i)=='@')
		{
    			Item_Number+=1;
    			switch (Item_Number)
				{
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
   		printf("%c",SMB_ReadByte(sm_target_addr,i)); 
   	}
}

// For [ WatchGuard ] Skylake(SKL) PCH-H platform
//----------------------------------------------------------
//Read EEPROM data
void Read_EEPROM_WG(void)
{
	int	i;
     #ifdef Item1_string
     printf("%s",Item1_string);
     #else
     printf("Item 1:");
     #endif
   	for(i=0;i<EEPRPM_SIZE;i++)
   	{ 
   		if (SMB_ReadByte(sm_target_addr_WG,i)=='#') 
		{
    			break;
   		}
   		if (SMB_ReadByte(sm_target_addr_WG,i)=='@')
		{
    			Item_Number+=1;
    			switch (Item_Number)
				{
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
   		printf("%c",SMB_ReadByte(sm_target_addr_WG,i)); 
   	}
}

//=======================================================================================
//  GPIO Interface  (MMIO - Memory Mapping I/O Addressing)
//=======================================================================================
// For Skylake(SKL) PCH-H platform
AMI_GPIO_GROUP_INFO SbHGpioGroupInfo[] =
{
  {PID_GPIOCOM0, R_PCH_H_PCR_GPIO_GPP_A_PAD_OWN,  R_PCH_PCR_GPIO_GPP_A_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_A_GPI_IS,    R_PCH_PCR_GPIO_GPP_A_GPI_IE,    R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_A_SMI_STS,  R_PCH_PCR_GPIO_GPP_A_SMI_EN,   R_PCH_PCR_GPIO_GPP_A_NMI_STS,  R_PCH_PCR_GPIO_GPP_A_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET,    V_PCH_GPIO_GPP_A_PAD_MAX},    //SKL PCH-H GPP_A
  {PID_GPIOCOM0, R_PCH_H_PCR_GPIO_GPP_B_PAD_OWN,  R_PCH_PCR_GPIO_GPP_B_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_B_GPI_IS,    R_PCH_PCR_GPIO_GPP_B_GPI_IE,    R_PCH_PCR_GPIO_GPP_B_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_B_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_B_SMI_STS,  R_PCH_PCR_GPIO_GPP_B_SMI_EN,   R_PCH_PCR_GPIO_GPP_B_NMI_STS,  R_PCH_PCR_GPIO_GPP_B_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET,    V_PCH_GPIO_GPP_B_PAD_MAX},    //SKL PCH-H GPP_B
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_C_PAD_OWN,  R_PCH_PCR_GPIO_GPP_C_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_C_GPI_IS,    R_PCH_PCR_GPIO_GPP_C_GPI_IE,    R_PCH_PCR_GPIO_GPP_C_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_C_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_C_SMI_STS,  R_PCH_PCR_GPIO_GPP_C_SMI_EN,   R_PCH_PCR_GPIO_GPP_C_NMI_STS,  R_PCH_PCR_GPIO_GPP_C_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET,    V_PCH_GPIO_GPP_C_PAD_MAX},    //SKL PCH-H GPP_C
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_D_PAD_OWN,  R_PCH_PCR_GPIO_GPP_D_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_D_GPI_IS,    R_PCH_PCR_GPIO_GPP_D_GPI_IE,    R_PCH_PCR_GPIO_GPP_D_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_D_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_D_SMI_STS,  R_PCH_PCR_GPIO_GPP_D_SMI_EN,   R_PCH_PCR_GPIO_GPP_D_NMI_STS,  R_PCH_PCR_GPIO_GPP_D_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET,    V_PCH_GPIO_GPP_D_PAD_MAX},    //SKL PCH-H GPP_D
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_E_PAD_OWN,  R_PCH_PCR_GPIO_GPP_E_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_E_GPI_IS,    R_PCH_PCR_GPIO_GPP_E_GPI_IE,    R_PCH_PCR_GPIO_GPP_E_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_E_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_E_SMI_STS,  R_PCH_PCR_GPIO_GPP_E_SMI_EN,   R_PCH_PCR_GPIO_GPP_E_NMI_STS,  R_PCH_PCR_GPIO_GPP_E_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET,    V_PCH_H_GPIO_GPP_E_PAD_MAX},  //SKL PCH-H GPP_E
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_F_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_F_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_F_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_F_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_F_SMI_STS,R_PCH_H_PCR_GPIO_GPP_F_SMI_EN, R_PCH_H_PCR_GPIO_GPP_F_NMI_STS,R_PCH_H_PCR_GPIO_GPP_F_NMI_EN, R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_F_PADCFG_OFFSET,  V_PCH_GPIO_GPP_F_PAD_MAX},    //SKL PCH-H GPP_F
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_G_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_G_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_G_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_G_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_G_SMI_STS,R_PCH_H_PCR_GPIO_GPP_G_SMI_EN, R_PCH_H_PCR_GPIO_GPP_G_NMI_STS,R_PCH_H_PCR_GPIO_GPP_G_NMI_EN, R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_G_PADCFG_OFFSET,  V_PCH_H_GPIO_GPP_G_PAD_MAX},  //SKL PCH-H GPP_G
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_H_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_H_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_H_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_H_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_H_SMI_STS,R_PCH_H_PCR_GPIO_GPP_H_SMI_EN, R_PCH_H_PCR_GPIO_GPP_H_NMI_STS,R_PCH_H_PCR_GPIO_GPP_H_NMI_EN, R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_H_PADCFG_OFFSET,  V_PCH_H_GPIO_GPP_H_PAD_MAX},  //SKL PCH-H GPP_H
  {PID_GPIOCOM3, R_PCH_H_PCR_GPIO_GPP_I_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_I_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_I_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_I_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_I_SMI_STS,R_PCH_H_PCR_GPIO_GPP_I_SMI_EN, R_PCH_H_PCR_GPIO_GPP_I_NMI_STS,R_PCH_H_PCR_GPIO_GPP_I_NMI_EN, R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_I_PADCFG_OFFSET,  V_PCH_H_GPIO_GPP_I_PAD_MAX},  //SKL PCH-H GPP_I
  {PID_GPIOCOM2, R_PCH_H_PCR_GPIO_GPD_PAD_OWN,    R_PCH_PCR_GPIO_GPD_HOSTSW_OWN,      R_PCH_PCR_GPIO_GPD_GPI_IS,      R_PCH_PCR_GPIO_GPD_GPI_IE,      R_PCH_PCR_GPIO_GPD_GPI_GPE_STS,     R_PCH_PCR_GPIO_GPD_GPI_GPE_EN,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_H_PCR_GPIO_GPD_PADCFGLOCK,    R_PCH_H_PCR_GPIO_GPD_PADCFGLOCKTX,    R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET,      V_PCH_GPIO_GPD_PAD_MAX}       //SKL PCH-H GPD
};

//---------------------------------------------------------------
//__dpmi_meminfo                  _DmpiMemAccessIF;                       // DPMI interface/structure for accessing above 1MB memory.

UINT16                          _PCR_MMIO_Selector = 0xFFFF;            // LDT selector for Skylake(SKL) GPIO(/MMIO) memory space.
 
//---------------------------------------------------------------
/*
VOID                    DPMI_AllocLdt_ForMmioAddrAbove1MB (UINT32 PhysicalAddr, UINT32 MemSize)
{   
	if ( _PCR_MMIO_Selector == 0xFFFF )
	{
		// Map the physical device address to linear memory.
		_DmpiMemAccessIF.address = PhysicalAddr;

	    _DmpiMemAccessIF.size        = MemSize & 0xFFFFF000L;           // Must be a multiple of 4KB per spec.

		// Now _DmpiMemAccessIF.address holds the linear address.
		__dpmi_physical_address_mapping ( &_DmpiMemAccessIF );

		// Allocate an LDT descriptor and set it up to span the entire device on-board memory.
		_PCR_MMIO_Selector = __dpmi_allocate_ldt_descriptors (1);

		__dpmi_set_segment_base_address (_PCR_MMIO_Selector, _DmpiMemAccessIF.address);
		__dpmi_set_segment_limit                (_PCR_MMIO_Selector, _DmpiMemAccessIF.size - 1);
	}
}
*/
//---------------------------------------------------------------
UINT32                  MmioRead32 (UINT32 Address)
{
	if ( _PCR_MMIO_Selector != 0xFFFF )
	{        
	//	return _farpeekl(_PCR_MMIO_Selector, Address);
	}
    else
	return 0xFFFFFFFFL;
}

//---------------------------------------------------------------
VOID                    MmioWrite32 (UINT32 Address, UINT32 Value)
{
//	if ( _PCR_MMIO_Selector != 0xFFFF )
//		_farpokel(_PCR_MMIO_Selector, Address, Value);
}

//---------------------------------------------------------------
UINT32                  ConvertGpioGroupPad (AMI_OEM_GPIO Gpio)
{
    UINT32              ConvertValue = 0;

    if (Gpio.Group < Gpio_Group_Max)
    {
	ConvertValue = (UINT32) (Gpio.Group << 16);
    } 
    else
	{
	ConvertValue = 0xFFFFFFFF;
    };
    
    if (ConvertValue != 0xFFFFFFFF)
	{
	ConvertValue |= (UINT32) Gpio.PinNum;
    }
    
    return ConvertValue;
}

//---------------------------------------------------------------
// Input : PinNum:   GPIO Pin#
//         GpioMode: TRUE == GPIO MODE
//
VOID                    SbProgGpioUseSel (AMI_OEM_GPIO Gpio, BOOLEAN GpioMode, UINT8 MulFunc)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32              GpioGroupPad = 0;
    UINT32      GroupIndex;
    UINT32      GpioCfgRegVal = 0;

    GpioGroupPad = ConvertGpioGroupPad(Gpio);

    GpioGroupInfo = SbHGpioGroupInfo;

    GroupIndex = (Gpio.Group) & 0xFF;

    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
					       GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum ) );
    
    if (GpioMode)
	{
	GpioCfgRegVal &= ~(BIT10 | BIT11);
	MmioWrite32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
				    GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum),
		     GpioCfgRegVal );
    }
}

//---------------------------------------------------------------
// Input : PinNum = GPIO Pin#
//         InputMode = TRUE : Input Mode
//                     FALSE : Output Mode
//
VOID                    SbSetGpioIoSel (AMI_OEM_GPIO Gpio, BOOLEAN InputMode)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32              GpioGroupPad = 0;
    UINT32      GroupIndex;
    UINT32      GpioCfgRegVal = 0;
    
    GpioGroupPad = ConvertGpioGroupPad(Gpio);

    GpioGroupInfo = SbHGpioGroupInfo;

    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
					       GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum) );
   
    if(GpioCfgRegVal == 0xFFFFFFFF)
	return;
    
    GpioCfgRegVal &= ~(BIT8 | BIT9);
    if (InputMode)
	{
	GpioCfgRegVal |= BIT8;
    }
    else
	{
	GpioCfgRegVal |= BIT9;
    };
    
    MmioWrite32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
				GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum),
		 GpioCfgRegVal );
}

//---------------------------------------------------------------
// Input : PinNum: GPIO Pin#
// Output: TRUE = High
//         FALSE= Low
//
// Note  : Please be noted that the returned value of 
//         SbGetGpioLvlSel() is unexpected if the GPIO 
//         pin is neither set to "GPI" nor "GPO".
//
BOOLEAN                 SbGetGpioLvlSel (AMI_OEM_GPIO Gpio)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GpioGroupPad = 0;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    GpioGroupPad = ConvertGpioGroupPad(Gpio);

    GpioGroupInfo = SbHGpioGroupInfo;

    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
					       GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum) );
    
    if ( !(GpioCfgRegVal & BIT8) )
	{ // Gpio Output
	if (GpioCfgRegVal & BIT0)
		{ // Output High
	    return TRUE;
	} else
	{ // Output Low
	    return FALSE;
	};
    }
    
    if ( !(GpioCfgRegVal & BIT9) )
	{ // Gpio Input
	if (GpioCfgRegVal & BIT1)
		{ // Output High
	    return TRUE;
	}
	else
		{ // Output Low
	    return FALSE;
	};
    }
    
    // Unexpected state
    return FALSE;
}

//---------------------------------------------------------------
// Input : PinNum:   GPIO Pin#
//         High:     TRUE = High
//                   FALSE= Low
//
VOID                    SbSetGpioLvlSel (AMI_OEM_GPIO Gpio, BOOLEAN  High)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GpioGroupPad = 0;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    GpioGroupPad = ConvertGpioGroupPad(Gpio);

    GpioGroupInfo = SbHGpioGroupInfo;

    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
				GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum) );
    if (High)
	{
	GpioCfgRegVal |= BIT0;
    }
    else
	{
	GpioCfgRegVal &= ~BIT0;
    }
    
    MmioWrite32( SB_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community,
					GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum),
		 GpioCfgRegVal );
}

VOID                    SetGpioLvlSel (UINT32 Community, UINT32 Offset, BOOLEAN  High)
{

    UINT32               GpioCfgRegVal = 0;
    
    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(Community, Offset) );
    if (High)
	{
	GpioCfgRegVal |= BIT0;
    }
    else
	{
	GpioCfgRegVal &= ~BIT0;
    }
    
    MmioWrite32( SB_PCR_ADDRESS(Community, Offset),GpioCfgRegVal );
}
//=============================================================================
UINT32                  GetGpioBaseMemory (VOID)
{
	return PCH_PCR_BASE_ADDRESS;            // GPIO MMIO base address
}

//------------------------------------------------------
VOID                    GpioBaseInit (VOID)
{
//    DPMI_AllocLdt_ForMmioAddrAbove1MB( GetGpioBaseMemory(), PCH_PCR_MMIO_SIZE );

	return;
}

//===============================================================================================
//#endif //#if defined(DIRECT_IO_ACCESS) || (defined(MODULE) || defined(KLD_MODULE

