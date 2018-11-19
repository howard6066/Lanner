
//#if defined(HEADER_FILE_IN_ROOT)
  #define  DIRECT_IO_ACCESS
  #include "../ioaccess.c"
//#else
  //#include "..\include\config.h"
  //#include "..\include\ioaccess.h"
//#endif

//-----------------------------------------------------------------------------
struct	
{
	char 	strProjectName[36];
	char    *strShowUtilityInfo;
} ProjectInfo[] = { { "SignOnMessage:NCB-4210",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***      NCB-4210  EEPROM test utility  Ver1.0 2015/12/09    ***\n"\
					  "****************************************************************\n" },				  
					  
					{ "SignOnMessage:NCB-5210",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***      NCB-5210  EEPROM test utility  Ver1.0 2015/12/09    ***\n"\
					  "****************************************************************\n" },				
					  
					{ "SignOnMessage:LB-5210Z",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***      LB-5210Z  EEPROM test utility  Ver1.0 2016/07/25    ***\n"\
					  "****************************************************************\n" },
					  
					{ "SignOnMessage:NCB-4210WG",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***    NCB-4210WG  EEPROM test utility  Ver1.0 2016/08/11    ***\n"\
					  "****************************************************************\n" },
					  
					{ "SignOnMessage:LB-4210Z",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***      LB-4210Z  EEPROM test utility  Ver1.0 2016/09/02    ***\n"\
					  "****************************************************************\n" } };
					  
//-----------------------------------------------------------------------------					  
	
void help()
{
    printf("MEP w/r: write/read  EEPROM \n");
}    

int  main(int argc, char *argv[])
{
iopl(3);
     char tempdata1[EEPRPM_SIZE]; 
     FILE *fp;
     //char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 unsigned int 	pStrLen;
     int  Flag,i,j;
/*
     pStrLen = (unsigned long) (strchr(pstr, ' ') - pstr);

	 Flag = 0;
	 for ( j = 0; j < (sizeof(ProjectInfo)/sizeof(ProjectInfo[0])); j++ )
	 {
	 	 if ( !strncmp(pstr, ProjectInfo[j].strProjectName, pStrLen) )
	 	 {
	 		 Flag = 1;
	 		 break;
	 	 }
	 }

//	 Project_name();

     if ( Flag )
		 printf("%s", ProjectInfo[j].strShowUtilityInfo);
	 else
		 printf("Error : Lanner no this platform name...\n");	 
*/	 
	 
   	 if( argc != 2 )
     {
	     help();
	     return -1;
     }
	 
	 _SmbBaseIoAddr  = GetSmbBaseIoAddr();
	 j=0;
	 switch(j)
	 {
		 case 0 :
			SMB_WriteByte(PCA9545_addr,0x00, 0x04);   	//enable smbus switch
			break;
		 
		 case 1 :
			SMB_WriteByte(PCA9548_addr,0x00, 0x20);   	//enable smbus switch
			break;
			
		 case 2 :
			SMB_WriteByte(PCA9548_addr,0x00, 0x20);   	//enable smbus switch
			break;
			
		 case 3 :
			SMB_WriteByte(PCA9545_addr,0x00, 0x48);   	//enable smbus switch
			break;
			
		 case 4 :
			SMB_WriteByte(PCA9545_addr,0x00, 0x48);   	//enable smbus switch
			break;
		
	 }

         if( strcmp(argv[1], "w") == 0 ) //write eeprom
	     {    
//read serial number from file     
    	 if ((fp=fopen("MEP.TXT","r+"))==NULL)
		 { 
		      printf("Please put MEP.txt for read\n ");//file exist?
		   	  perror("MEP.TXT");
		   	  return(1);
		 }
     	      printf("Read data from MEP.txt:\n");
     
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
     	 { 
             printf(".");
			 if( j == 3 )
			 {
				SMB_WriteByte(sm_target_addr_WG,i,tempdata1[i]);
			 }
			 else
			 {
				SMB_WriteByte(sm_target_addr,i,tempdata1[i]);				 
			 }
    	     if (tempdata1[i]=='#')	break;
    	 }
         	 printf("finish\n");	 
   	
//read back from eeprom
             printf("<<<<Read back From MB EEPROM>>>>\n");
			 if( j == 3 )
			 {
				 Read_EEPROM_WG();
			 }
			 else
			 {
				Read_EEPROM();				 
			 }

	       	 SMB_WriteByte(0xE6,0x00, 0x00);	 //disable all smbus channel 
         }	     

         if( strcmp(argv[1], "r") == 0 )//read eeprom
	     {     
//read back from eeprom
     	     printf("<<<<Read back From MB EEPROM>>>>\n");	
			 if( j == 3 )
			 {
				 Read_EEPROM_WG();
			 }
			 else
			 {
				Read_EEPROM();				 
			 }
			 SMB_WriteByte(0xE6,0x00, 0x00);	//disable all smbus channel 

         }

}


