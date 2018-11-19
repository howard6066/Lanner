
#include <stdio.h>
#ifdef DJGPP
/* For DOS djgpp */
#include <dos.h>
#include <inlines/pc.h>
#include <dpmi.h>
#include <sys/farptr.h>
#else
/* For Linux */
#include <sys/io.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>	// supoort MMIO
#include <fcntl.h>	// supoort MMIO
#define inportb(x)	inb(x)
#define outportb(x, y)	outb(y, x)
#define inportw(x)	inw(x)
#define outportw(x, y)	outw(y, x)
#define inportl(x)	inl(x)
#define outportl(x, y)	outl(y, x)
#define delay(x)	usleep(x*1000)
#define sleep(x)	usleep(x*1000000)
#endif


#define PCI_index	0xcf8
#define PCI_data	0xcfc

unsigned char Pwr_add[2]={0xB0,0xB2};

unsigned char Pwr_add_A[2]={0xA0,0xA2};
#define Pwr_EEP		0xA0
int 	smbase;
int	smb_err;
int	ich_iobase; //v0.3
//PMBUS COMMAND
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
#define MFR_SERIAL							0x9E		// (Read Block) 14 bytes
#define STATUS_MFR_SPECIFIC			0x80		// (Read Byte) AC/DC mode
#define MFR_FW_ID               0xD1    // Undocument

#define	debug		0
#define	Show_status_only	0x00//V0.4 If power supply support Serial number EEPROM, set 0
int block_d[256];
int	Byte1[11]={0,10,20,30,40,50,60,70,80,90,100};
int	Byte2=0x00;
int	PEC_B0[2]={0xea, 0x3c};
int	PEC_B2[2]={0x0a, 0xdc};
float 	lin5_11ToFloat1(int wordValue);
float	exponent[32]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8,4,2};
//===============================================================
//  GPIO's definitions/interfaces (MMIO - Memory Mapping I/O Addressing)
//===============================================================
#define PCH_PCR_BASE_ADDRESS			0xFD000000		// SBREG MMIO base address
#define PCH_PCR_MMIO_SIZE               0x01000000     	// < 16MB
#define SB_PCR_ADDRESS(Pid, Offset)    	( ((unsigned long)(Pid) << 16) | ((unsigned int)(Offset)) )
#define Port_ID_GPP_K		0x6b
#define GPP_K11_Offset 	0x6b0



#ifdef DJGPP 
__dpmi_meminfo                  _DmpiMemAccessIF;                       // DPMI interface/structure for accessing above 1MB memory.

unsigned int                          _PCR_MMIO_Selector = 0xFFFF;            // LDT selector for Skylake(SKL) GPIO(/MMIO) memory space.
 

void                    DPMI_AllocLdt_ForMmioAddrAbove1MB (unsigned long PhysicalAddr, unsigned long MemSize)
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

//------------------------------------------------------
void                    GpioBaseInit (void)
{
    DPMI_AllocLdt_ForMmioAddrAbove1MB( PCH_PCR_BASE_ADDRESS, PCH_PCR_MMIO_SIZE );

	return;
}

//---------------------------------------------------------------
unsigned long                  MmioRead32 (unsigned long Address)
{
	if ( _PCR_MMIO_Selector != 0xFFFF )
	{        
		return _farpeekl(_PCR_MMIO_Selector, Address);
	}
    else
	return 0xFFFFFFFFL;
}

//---------------------------------------------------------------
void                    MmioWrite32 (unsigned long Address, unsigned long Value)
{
	if ( _PCR_MMIO_Selector != 0xFFFF )
		_farpokel(_PCR_MMIO_Selector, Address, Value);
}
#else  /// DJGPP function 
// DIRECT_IO_ACCESS
unsigned int *get_mem_addr(void){
    	volatile void *gpio_addr =NULL;
    	int fd;
			fd = open("/dev/mem", O_RDWR|O_SYNC);
			
    	if (fd == -1)
    	{
        	exit (-1);
    	}
			gpio_addr = mmap(NULL, 0x1000000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PCH_PCR_BASE_ADDRESS);
    	close(fd);
	
      return (unsigned int *)gpio_addr;	
}

unsigned int MmioRead32(unsigned long offset)
{
    	unsigned int reg;
    	volatile unsigned int *gpio_oe_addr =NULL;
    	volatile void *gpio_addr =NULL;
    	//unsigned int  *gpio_addr =NULL;
			gpio_addr=get_mem_addr();
  		gpio_oe_addr = gpio_addr + offset;
			reg = *(gpio_oe_addr);

        return reg;
}



void MmioWrite32(unsigned long offset, unsigned long value)
{
    	volatile void *gpio_addr =NULL;
    	volatile unsigned int *gpio_oe_addr =NULL;
    	//unsigned int  *gpio_addr =NULL;
			gpio_addr=get_mem_addr();
    	gpio_oe_addr = gpio_addr + offset;
			*(volatile unsigned int *)(gpio_oe_addr)=value;
   	

        return;
}
#endif
//---------------------------------------------------------------
void 			SetGpioLvlSel (int PinOffset, int OutValue)
{
    unsigned long               GpioCfgRegVal = 0;
    GpioCfgRegVal = MmioRead32( SB_PCR_ADDRESS(Port_ID_GPP_K,PinOffset));
    if (OutValue)
	{
	GpioCfgRegVal |= 0x01;
    }
    else
	{
	GpioCfgRegVal &= ~(0x01);
    }
    MmioWrite32( SB_PCR_ADDRESS(Port_ID_GPP_K,PinOffset),GpioCfgRegVal );

}

//===============================================================
//v0.3-add
unsigned int get_gpiobase(void)
{
outportl(PCI_index,(0x80000000+(0<<16)+(31<<11)+(0<<8)+0x48));	//Intel LPC bridge
	return(inportl(PCI_data));

}
//v0.3-end
int get_smbase(void)
{
outportl(PCI_index,(0x80000000+(0<<16)+(31<<11)+(4<<8)+0x20));//smbase dev31 fun4 rx21-20
return ((inportw(PCI_data))&0xfffe);
}
//============================================================================================

int write_byte(int cslaveadd,int byte_c1,int byted1)
{
	int	i;
	smb_err=0x00; //clr status flag
	for(i=0x00;i<100;i++){	
	outportb(smbase,0xff);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==0x99){
	printf("Slave Address=%x  device smbus access timeout!!\n",cslaveadd);
	smb_err=0x01;}
	}    	
			outportb(smbase+2,(inportb(smbase+2)&0x7F));//disable pec first	
      outportb(smbase+3,byte_c1);//byte N
    	delay(5);
    	outportb(smbase+4,cslaveadd&0xfe);//slave address and write
    	delay(5);
    	outportb(smbase+5,byted1);//byte data access
    	delay(5);
    	outportb(smbase+2,0x48);//byte data access
    	delay(20);
    	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {
  	   #if debug
  	   printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	   #endif
  	   smb_err=0x02;
  	   }
  	    #if debug
  			if(smb_err!=0x00){printf("smbus access error exit!!!!\n");
  			return -1;
  			}
  		  #endif	   
 	return(0x00);  	   
}  
//=============================================================================================  
int read_byte(int cslaveadd,int byte_c)
{
	int	i;
	smb_err=0x00; //clr status flag
   for(i=0x00;i<100;i++){	
	outportb(smbase,0xff);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==0x99){
		printf("Slave Address=%x  device smbus access timeout!!\n",cslaveadd);
		smb_err=0x01;}
	}    
	outportb(smbase+2,(inportb(smbase+2)&0x7F));//disable pec first		
   // outportb(smbase+2,0x02);	
    outportb(smbase+3,byte_c);//byte N
    delay(5);
    outportb(smbase+4,cslaveadd|0x01);//slave address and raed
    delay(5);
    outportb(smbase+2,0x48);//byte data access
    delay(20);
     	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {
  	  #if debug
  	  	printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	  #endif
  	    smb_err=0x02;
  	   }
  	   #if debug
  			if(smb_err!=0x00){printf("smbus access error exit!!!!\n");
  			return -1;
  			}   	   
    		#endif
    return(inportb(smbase+5));//byte data access
}    	

int read_word(int cslaveadd,int byte_c)
{
	int	i;
	smb_err=0x00; //clr status flag
   for(i=0x00;i<100;i++){	
	outportb(smbase,0xff);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==0x99){smb_err=0x01;
		printf("Slave Address=%x  device smbus access timeout!!\n",cslaveadd);
		}
	}    
outportb(smbase+2,(inportb(smbase+2)&0x7F));//disable pec first		
   // outportb(smbase+2,0x02);	
    outportb(smbase+3,byte_c);//byte N
    delay(5);
    outportb(smbase+4,cslaveadd|0x01);//slave address and raed
    delay(5);
    outportb(smbase+2,0x4c);//byte data access
    delay(20);
     	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {
  	    #if debug
  	  	printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	    #endif 
  	    smb_err=0x02;
  	
  	   }
  		#if debug
  		if(smb_err!=0x00){printf("smbus access error exit!!!!\n");
  		return -1;}
  		#endif   	   
    return(inportw(smbase+5));//byte data access
}  

int read_block(int cslaveadd,int byte_c)
{
	int	i,byte_count=0x00, b_count=0;;
	smb_err=0x00; //clr status flag
   for(i=0x00;i<100;i++){	
	outportb(smbase,0xff);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==0x99){smb_err=0x01;
	  #if debug
		printf("Slave Address=%x  device smbus access timeout!!\n",cslaveadd);
		#endif
		}
	}   

    outportb(smbase+0x0d,0x02);//tina test
 
	inportb(smbase+2);//reset buffer		
    	outportb(smbase+3,byte_c);//byte N
   	 delay(5);
   	 outportb(smbase+4,cslaveadd|0x01);//slave address and raed
   	 delay(5);
   	 outportb(smbase+2,0x54);//byte data access
   	 delay(20);
     	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {	
  	    #if debug
  	  	printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	    #endif
  	    smb_err=0x02;
  	   }
   #if debug
   if(smb_err!=0x00){
   			printf("smbus access error exit!!!!\n");
   			return -1;
   			}   	   
    #endif
    	byte_count=inportb(smbase+5);//byte data access
    	b_count=0;
        do{
    
    	block_d[b_count]=(inportb(smbase+7));
	  	outportb(smbase,0xff);
	  	delay(10);
     	  b_count++;
	}   
    	while(b_count<byte_count);
            	
   	return byte_count;
} 

int write_word_PEC(int cslaveadd,int cmd, int byte1, int byte2 , int pec_value)

{
	int	i;
	smb_err=0x00; //clr status flag
   for(i=0x00;i<100;i++){	
	outportb(smbase,0xff);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==0x99){smb_err=0x01;
		printf("Slave Address=%x  device smbus access timeout!!\n",cslaveadd);
		}
	}    
    outportb(smbase+2,(inportb(smbase+2)|0x80));//enable pec
    outportb(smbase+8,pec_value);//pec crc data   	
    outportb(smbase+3,cmd);
    delay(5);
    outportb(smbase+4,cslaveadd & 0xfe);//slave address and write
    delay(5);
    outportb(smbase+5,byte1);
    outportb(smbase+6,byte2);
    delay(5);
            
    outportb(smbase+2,0x4c|0x80);//word data access with pec
    delay(20);
     	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  { 
  	    #if debug
  	    printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);
  	    #endif
  	    smb_err=0x02;
  	   }
  	#if debug
  	if(smb_err!=0x00){//printf("smbus access error exit!!!!\n");
  	return -1;
  	}
    #endif   	
  return 0x00;	   

}  

//===============================================================================================
void help()
{
    printf("****************************************************************\n");		
    printf("** Lanner Electronics Inc.  2018-04-26\n");
    printf("** PMBUS utility  Ver, 1.1 for NCB-5220\n");
    printf("****************************************************************\n");	
 }
//===========================================
//MAIN
//
//========================================== 
 
int main(int argc, char *argv[])
{	
#ifndef DJGPP
	iopl(3);
#endif
	int	SM_BP=0x00,SM_A0=0x00,SM_A2=0x00,SM_P1=0x00,SM_P2=0x00;	//V1.5
	float tmp_value=0x00; 
	int bWaring=0x00;
	unsigned int i,j,k, WordValue, ByteCnt, tmp;
	unsigned char ByteValue,Pwr_Module;

     	smb_err=0x00;
     	smbase=get_smbase(); 

	     	
     	help();
//write_byte(0xe4,0x00,0x80);      
if (argc==1){ 
//Set SMbus select pin GPP_K11 to Low
#ifdef DJGPP
		GpioBaseInit();
#endif
		//SetGpioLvlSel(GPP_K11_Offset, 0x00);
//-----------		  
		read_byte(0xB0,0x00);
	 	  if (smb_err!=0x00){
	  									printf("Power Module 1 PMBUS fail!\n");
	  									SM_P1=0x01;
	  									return -1;	
	  									}

	  
		read_byte(0xB2,0x00);
	 	  if (smb_err!=0x00){
	  									printf("Power Module 2 PMBUS fail!\n");
	  									SM_P2=0x01;
	  									return -1;	
	  									}

      			
//Powe Info

if (Show_status_only !=1){
 	printf("<<<<< Power Information: >>>>> \n");	
 	//use Command :D1, byte 9, 0x41(A):AC power,0x44(D):DC power

 	if(SM_P1==0x00){
 			printf("Power Module 1 Power Mode:");
 			tmp=read_block(Pwr_add[0], MFR_FW_ID);
 			if ( block_d[8] == 0x41)
		 		printf("AC Power Mode\n");
	    else if ( block_d[8] == 0x44)
	  	 	printf("DC Power Mode\n");
	     	else
		 			printf("Unknow AC/DC Power Mode\n");
		 			
      printf("Power Module 1 SN:");
      for ( ByteCnt= 0x2E; ByteCnt < 0x40; ByteCnt++ )
		  printf("%c", read_byte(Pwr_add_A[0], ByteCnt));
      printf("\n");                         
 	}
 	if(SM_P2==0x00){

 			printf("Power Module 1 Power Mode:");
 			tmp=read_block(Pwr_add[1], MFR_FW_ID);
 			if ( block_d[8] == 0x41)
		 		printf("AC Power Mode\n");
	    else if ( block_d[8] == 0x44)
	  	 	printf("DC Power Mode\n");
	     	else
		 			printf("Unknow AC/DC Power Mode\n");
     
      printf("Power Module 2 SN:");
      for ( ByteCnt= 0x2E; ByteCnt < 0x40; ByteCnt++ )
		  printf("%c", read_byte(Pwr_add_A[1], ByteCnt));
      printf("\n");

	}
 
										 	
}//if (Show_status_only !=1)

	for(Pwr_Module=0;Pwr_Module<2;Pwr_Module++){	
	if ((SM_P1!=0x00)&&(Pwr_Module==0x00)){continue;}
	if ((SM_P2!=0x00)&&(Pwr_Module==0x01)){continue;}
printf("\n================= Power Module %x  =================\n",Pwr_Module+1);
 //Get Status 	
// printf("\n<<<<< Power Module %d Status: >>>>> \n",Pwr_Module+1);
 	printf("Power Module Status:\n");	
 	WordValue=read_word(Pwr_add[Pwr_Module],STATUS_WORD);	
  	#if debug
	printf("%x  \n",WordValue);
	#endif
  if (WordValue==0x2000) printf("No Warning/Error Status\n");
  if (WordValue==0x2840) printf("Unit not providing power to the output.\n"); 
 
	if (WordValue&0xec7c){
  
		if(WordValue&0x04){ //temperature warning/error
  			ByteValue=read_byte(Pwr_add[Pwr_Module],STATUS_TEMPERATURE);	
  			#if debug
			printf("Temperature status : %x  \n",ByteValue);
			#endif
		 	if(ByteValue&0x80){ printf("Over Temperature Fault\n");}
		 	else if(ByteValue&0x40){ printf("Over Temperature Warning\n");}
		  }
		if(WordValue&0x08){ //Iuput under voltage
		  printf("Iuput under voltage Fault\n");
		  }
		if(WordValue&0x10){ //Output over current
		  printf("Output over current Fault\n");
		  }
		if(WordValue&0x20){ //Output over voltage
		  printf("Output over voltage Fault\n");
		  }
      
		if(WordValue&0x40){ //OFF 
		  printf("Unit not providing power to the output.\n");
		  }
      
		if(WordValue&0x400){ //Fan 
		  	ByteValue=read_byte(Pwr_add[Pwr_Module],STATUS_FANS_1_2);	
  			#if debug
			printf("FAN_1_2 status : %x  \n",ByteValue);
			#endif
		 	if(ByteValue&0x80){ printf("Fan 1 Fault\n");}
		 	else if(ByteValue&0x20){ printf("Fan 1 Warning\n");}
			else if(ByteValue&0x08){ printf("Fan 1 Overridden\n");}
		  	
		  }
		if(WordValue&0x800){ //power good 
		  	 printf("POWER_GOOD# present.\n");
		  	}
		if(WordValue&0x2000){ //Input current/voltage 
		  	ByteValue=read_byte(Pwr_add[Pwr_Module],STATUS_INPUT);	
  			#if debug
			printf("VIN status : %x  \n",ByteValue);
			#endif
 			if(ByteValue&0x40){ printf("VIN Overvoltage Warning\n");}
		 	else if(ByteValue&0x20){ printf("VIN Under Voltage Warning\n");}
		 	else if(ByteValue&0x10){ printf("VIN Under voltage Fault\n");}
		 	else if(ByteValue&0x80){ printf("Uinit is off for insufficient input voltage.\n");}
		  			  
		  }
		if(WordValue&0x4000){ //Output current/voltage 
			ByteValue=read_byte(Pwr_add[Pwr_Module],STATUS_IOUT);	
  			#if debug
			printf("Vout status : %x  \n",ByteValue);
			#endif
			
		 	if(ByteValue&0x80){ printf("IOUT Overcurrent Fault\n");}
		 	else if(ByteValue&0x20){ printf("IOUT Overcurrent Warning\n");}
		  			  
		  }
		if(WordValue&0x8000){ //Output voltage 
				
  			#if debug
			printf("Vout status : %x  \n",ByteValue);
			#endif

 			write_byte(Pwr_add[Pwr_Module],0x00,0x00);
		 	ByteValue=read_byte(Pwr_add[Pwr_Module],STATUS_VOUT);
		 	if(ByteValue&0x80){ printf("VOUT 12V Overvoltage Fault\n");}
		 	else if(ByteValue&0x40){ printf("VOUT 12V Overvoltage Warning\n");}
		 	else if(ByteValue&0x20){ printf("VOUT 12V Under voltage Warning\n");}
		 	else if(ByteValue&0x10){ printf("VOUT 12V Under voltage Fault\n");}

		  }
	 
   if (WordValue&0x40==0) {printf("No Warning/Error Status\n");}
    
	}
//	else {printf("No Warning/Error Status\n");}

if (Show_status_only !=1){ //v0.4
//printf("\n<<<<< Power Module %d Output Votlage: >>>>> \n",Pwr_Module+1);	
	write_byte(Pwr_add[Pwr_Module],0x00,0x00);
	//printf("12V: %.3f  \t\t", lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_VOUT)));
	tmp_value = lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_VOUT));
	if ((tmp_value <= 10) | (tmp_value>=14)) bWaring = 1;
  printf("12V: %.3f  \t\t", tmp_value);
  
//printf("\n<<<<< Power Module Output Current: >>>>> \n",Pwr_Module+1);	
	write_byte(Pwr_add[Pwr_Module],0x00,0x20);
	//printf("5Vsb: %.3f  \n", lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_VOUT)));	
		tmp_value = lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_VOUT));
	if ((tmp_value <= 3) | (tmp_value>=7)) bWaring = 1;
  printf("5Vsb: %.3f  \t\n", tmp_value);

 //Get FAN speed 
 		//printf("Fan Speed: %d  \t",(int)lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_FAN_SPEED_1)));	
 		tmp = (int)lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_FAN_SPEED_1));
 		if (tmp <= 0) bWaring = 1;
 		printf("Fan Speed: %d  \t", tmp);
 //printf("\n<<<<< Power Module %d Temperature: >>>>> \n",Pwr_Module+1);
 		//printf("Temp1: %d  \t",(int)lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_TEMPTURE_1)));	
 		tmp = (int)lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_TEMPTURE_1));
 		if (tmp <= 0) bWaring = 1;
 		printf("Temp1: %d  \t", tmp);
		//printf("Temp2: %d  \n",(int)lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_TEMPTURE_2)));
		tmp = (int)lin5_11ToFloat1(read_word(Pwr_add[Pwr_Module],READ_TEMPTURE_2));
		if (tmp <= 0) bWaring = 1;	
 		printf("Temp1: %d  \n", tmp);	
}//if (Show_status_only !=1){ //v0.4

    }	//for(Pwr_Module=0;Pwr_Module<2;Pwr_Module++)
		if (bWaring != 0) printf("PMBUS Value fail\n");	
		
		//SetGpioLvlSel(GPP_K11_Offset, 0x01);
} //argc==1
	
	

}



//===================================================================================================== 

 float lin5_11ToFloat1(int wordValue)
{
	float	d = (float)(wordValue & 0x03ff);
	if(wordValue&0x400) {d = -d;}	
	if (wordValue&0x8000){
		#if debug
		printf("%f / %f \n",d,exponent[(wordValue>>11)&0x001f]);
		#endif
		return (d / (exponent[(wordValue>>11)&0x001f]));
		}
	else{
		#if debug
		printf("%f * %f \n",d,exponent[(wordValue>>11)&0x001f]);
		#endif
		return (d *( exponent[(wordValue>>11)&0x001f]));
		}

}


