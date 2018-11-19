//20180423 V1.0 Init mother board ID eeprom test utility. 

#include <stdio.h>
#ifdef DJGPP
/* For DOS djgpp */
#include <dos.h>
#include <inlines/pc.h>
#else
/* For Linux */
#include <sys/io.h>
#include <time.h>
#define inportb(x)	inb(x)
#define inportw(x)	inw(x)
#define outportb(x, y)	outb(y, x)
#define outportl(x, y)	outl(y, x)
#define delay(x)	usleep(x*100)
#endif

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

#define PCI_index	0xcf8
#define PCI_data	0xcfc
#define	sm_target_addr  0xae//eeprom address 
//int GPIO_PORT;
int	Item_Number=0x01;
static int	smb_err;
static int 	smbase; 



void help()
{

    printf("N1515MEP  w/r: write/read  EEPROM \n");

}    

unsigned short get_smbase(void)
{
outportl(PCI_index,(0x80000000+(0<<16)+(31<<11)+(4<<8)+0x20));//smbase dev31 fun4 rx20
return ((inportw(PCI_data))&0xfffe);
}

void Write_SMB_Byte(int reg, int value,int sm_addr)
{
	int	i;
	smb_err=0x00; //clr status flag
	for(i=0x00;i<100;i++){	
	outportb(smbase,0xfe);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==99){smb_err=0x01;}
	}    	
      outportb(smbase+3,reg);//byte N
    	delay(5);
    	outportb(smbase+4,sm_addr&0xfe);//slave address and write
    	delay(5);
    	outportb(smbase+5,value);//byte data access
    	delay(5);
    	outportb(smbase+2,0x48);//byte data access
    	delay(20);
    	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {//printf("Slave Address=%x  device not exist or smbus access error!!\n",sm_addr);//debug
  	   smb_err=0x02;
  	   }
 	return;  	   
}  
//=============================================================================================  
char Read_SMB_Byte(int byte_c,int cslaveadd)
{
	int	i;

	smb_err=0x00; //clr status flag
   for(i=0x00;i<100;i++){	
	outportb(smbase,0xfe);//clr status
    	delay(10);
	if ((inportb(smbase+0x00)&0x01)==0x00)
		{break;};
	if(i==99){smb_err=0x01;}
	}    
   // outportb(smbase+2,0x02);	
    	outportb(smbase+3,byte_c);//byte N
    	delay(5);
    	outportb(smbase+4,cslaveadd|0x01);//slave address and raed
    	delay(5);
    	outportb(smbase+2,0x48);//byte data access
    	delay(20);
     	if(((inportb(smbase))&0x04)==0x4)//check status fail?
  	  {//printf("Slave Address=%x  device not exist or smbus access error!!\n",cslaveadd);//debug
  	    smb_err=0x02;
  	   }
    return(inportb(smbase+5));//byte data access
}    	



//Read EEPROM data
void Read_EEPROM(void){
	int	i;
     #ifdef Item1_string
     printf("%s",Item1_string);
     #else
     printf("Item 1:");
     #endif
   	for(i=0;i<EEPRPM_SIZE;i++)
   	{ 
   		if (Read_SMB_Byte(i,sm_target_addr)=='#') {
    			break;
   			}
   		if (Read_SMB_Byte(i,sm_target_addr)=='@'){
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
   		printf("%c",Read_SMB_Byte(i,sm_target_addr)); 
		
   	}

}
//*********************************************
// Main start  
//*********************************************
int main(int argc, char *argv[])
{
#ifndef DJGPP
	iopl(3);
#endif	
if( argc != 2 )
    {
	help();
	return -1;
    }    




	smbase=get_smbase();
int	i;
	char ch,tempdata1[EEPRPM_SIZE]; 
#ifdef DJGPP
     clrscr();
#endif
     printf("*************************************************************\n");
     printf("**  Lanner Electronics Inc.  2018-04-23                     **\n");
     printf("**  MainBoard EEPROM test utility Ver,1.0 for NCB-1515    	**\n");  
     printf("*************************************************************\n");
    
     	FILE *fp;
   	if( strcmp(argv[1], "w") == 0 ){     //write eeprom
//read serial number from file     
    	if ((fp=fopen("N1515MEP.TXT","r+"))==NULL)
		  	{ printf("Please put N1515MEP.txt for read\n ");//file exist?
		   	perror("N1515MEP.TXT");
		   	return(1);}
     	printf("Read data from N1515MEP.txt:\n");
     
     	for(i=0;i<EEPRPM_SIZE;i++)	
     	{    	 	
     	 tempdata1[i]=getc(fp);
     	 printf("%c",tempdata1[i]);
     	 if (tempdata1[i]=='#')	break;
     	}
      	printf("\n");
	 fclose(fp);
     		  
		  
//write Serial number to eeprom 
    printf("Write data to MB EEPROM");

 	for(i=0;i<EEPRPM_SIZE;i++)
   	{ printf(".");
   	Write_SMB_Byte(i,tempdata1[i],sm_target_addr);
   	if (tempdata1[i]=='#')	break;
   	}
   	printf("finish\n");	 
   	
//read back from eeprom
     printf("<<<<Read back From MB EEPROM>>>>\n");	
	Read_EEPROM();

}	     

   if( strcmp(argv[1], "r") == 0 ){     //read eeprom
//read back from eeprom
     	printf("<<<<Read back From MB EEPROM>>>>\n");	
   	Read_EEPROM();

    } 
}