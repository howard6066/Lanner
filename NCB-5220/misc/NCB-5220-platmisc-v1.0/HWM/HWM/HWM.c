//; Copyright (c) 2015 Lanner Electronic Inc. All rights reserved.
//; HW monitor utility for NCB-5210 & NCA-4210 & LS-5210Z


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
					  "***        NCB-4210  HW monitor utility   Ver 1.5 2016/03/08 ***\n"\
					  "****************************************************************\n" },

					{ "SignOnMessage:NCB-5210",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***        NCB-5210  HW monitor utility   Ver 1.1 2015/12/18 ***\n"\
					  "****************************************************************\n" },

					{ "SignOnMessage:LB-5210Z",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***      LB-5210Z  HW monitor utility  Ver 1.0 2016/07/25    ***\n"\
					  "****************************************************************\n" },
					  
					{ "SignOnMessage:NCB-4210WG",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***    NCB-4210WG  HW monitor utility  Ver 1.0 2016/08/11    ***\n"\
					  "****************************************************************\n" },
					  
					{ "SignOnMessage:LB-4210Z",
					  "****************************************************************\n"\
					  "***          Lanner platform miscellaneous utility           ***\n"\
					  "***      LB-4210Z  HW monitor utility  Ver 1.0 2016/09/02    ***\n"\
					  "****************************************************************\n" } };

//-----------------------------------------------------------------------------

//NCB-5210--------------------------------------------------------
#define CPU_ALARM_TEMP   	95
#define SYS1_ALARM_TEMP   	65
#define SYS2_ALARM_TEMP   	65
#define VCORE_ALARM_MAX   	1.0*1.2
#define VCORE_ALARM_MIN   	1.0*0.8
#define V12_0_ALARM_MAX   	12*1.05
#define V12_0_ALARM_MIN   	12*0.95
#define V5_0_ALARM_MAX   	5*1.05
#define V5_0_ALARM_MIN   	5*0.95
#define V3_3_ALARM_MAX   	3.3*1.05
#define V3_3_ALARM_MIN   	3.3*0.95
//#define V3_3_ALARM_MIN   	3.5*0.9 //debug test
#define VPVDD_ALARM_MAX  	1.2*1.05
#define VPVDD_ALARM_MIN   	1.2*0.95
#define FAN_ALARM_MIN  		1000

#define FAN_CONFIG  		0x04//For NCB-5210B
//#define FAN_CONFIG  		0x08//For NCB-5210?

//NCB-4210--------------------------------------------------------
#define FAN_ALARM_MIN  	1000
#define VCORE_ALARM_MAX   	1.0*1.2
#define VCORE_ALARM_MIN   	1.0*0.8
#define V15_0_ALARM_MAX   	1.2*1.1
#define V15_0_ALARM_MIN   	1.2*0.9
#define V12_0_ALARM_MAX_1  	12*1.05
#define V12_0_ALARM_MIN_1  	12*0.95
#define V1_0_ALARM_MAX   	1*1.1
#define V1_0_ALARM_MIN   	1*0.9
#define V5_0_ALARM_MAX_1   	5.0*1.1
#define V5_0_ALARM_MIN_1   	5.0*0.9
#define V3_3_ALARM_MAX_1   	3.3*1.1
#define V3_3_ALARM_MIN_1   	3.3*0.9
#define V5_5_ALARM_MAX   	5.5*1.1
#define V5_5_ALARM_MIN   	5.5*0.9
#define VBAT_ALARM_MAX  	3.0*1.1
#define VBAT_ALARM_MIN   	3.0*0.9

//==============================================================
void wait_for_IBF_clear(int delay_count)
{
	int i=0;
	for(i=0;i<delay_count;i++)
	{
		if ((inportb(0xca3)&0x02)!=0x02)break;	
		delay(20);
	}	
}
void clear_OBF(void)
{
	int i=0;
	i=inportb(0xca2);
}

int opma_smbus(int value)
{
	wait_for_IBF_clear(100);
	clear_OBF();
	//Set write start
	outportb(0xca3,0x61);
	wait_for_IBF_clear(100);
	clear_OBF();
	 //Set netfn 
	outportb(0xca2,0xb8);
	wait_for_IBF_clear(100);
	clear_OBF();
	//Set I2C OFF
	outportb(0xca2,0x06);
	wait_for_IBF_clear(100);
	clear_OBF();
	//Set write end
	outportb(0xca3,0x62);
	wait_for_IBF_clear(100);
	clear_OBF();
	//wait some delay for IPMI finish program
	outportb(0xca2,value);
	wait_for_IBF_clear(100);
	clear_OBF();
	sleep(2);
	return(0);
}

//========================================================
//NCB-4210 parameter---------------------------------start
//;===================Read temp.==========================
Read_Temperature()
{
    unsigned long tempT;
/********System temp.********/
//    tempT = read_HM_reg(0x00, 0x73);
//    printf("CPU0 Temperature: %d %cC %s\n", tempT, 0xf8, tempT >= CPU_ALARM_TEMP ? "ALARM" : "     ");
	printf("SYS1 temp:  %d C          \n", read_HM_reg(0x00, 0x73));	//;Super IO (bank 0 offset 0x73) for system temp.

/********CPU temp.********/
   // tempT = read_HM_reg(0x07, 0x20);
    printf("CPU1 temp:  %d C           \n", (read_HM_reg(0x07, 0x20)& 0x7F));
	//printf("CPU temp.:     %d C          \n", read_HM_reg(0x00, 0x75));	//;Super IO (bank 0 offset 0x75) for CPU temp.
	return 0;
}

//;===================Read System FAN RPM==========================
Read_FAN()
{
/********CPUFAN********/
    unsigned long temp1;

/********AUXFAN1********/
	  temp1 = (read_HM_reg(0x06, 0x58) << 8) | read_HM_reg(0x06, 0x59);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN1 Speed: - NA(ALARM) - \n");
    else
      printf("FAN1 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********CPUFAN********/
	  temp1 = (read_HM_reg(0x06, 0x5A) << 8) | read_HM_reg(0x06, 0x5B);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN2 Speed: - NA(ALARM) - \n");
    else
      printf("FAN2 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********SYSFAN********/
	  temp1 = (read_HM_reg(0x06, 0x5C) << 8) | read_HM_reg(0x06, 0x5D);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN3 Speed: - NA(ALARM) - \n");
    else
      printf("FAN3 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********AUXFAN0********/
	  temp1 = (read_HM_reg(0x06, 0x56) << 8) | read_HM_reg(0x06, 0x57);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN4 Speed: - NA(ALARM) - \n");
    else
      printf("FAN4 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

	return 0;
}

//;===================Read System Voltage==========================
Read_Vol()
{
    float temp3;

     temp3 = read_HM_reg(0x00, 0x20) * 0.008;
	  printf("CPU1 VCORE: %.03f V ", temp3);
    if((temp3 < VCORE_ALARM_MIN) || (temp3 > VCORE_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("\t");

    temp3 = read_HM_reg(0x00, 0x21) * 0.008 * ((0xb+1)/1);
	  printf("12V:  %.03f V ", temp3);
    if((temp3 < V12_0_ALARM_MIN) || (temp3 > V12_0_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

    temp3 = read_HM_reg(0x00, 0x24) * 0.008 * ((0x28+0xa)/0xa);
	  printf("5V:      %.03f V ", temp3);
    if((temp3 < V5_0_ALARM_MIN) || (temp3 > V5_0_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("     \t");

    temp3 = read_HM_reg(0x00, 0x25) * 0.008 ;
	  printf("VDDQ:  %.03f V ", temp3);
	if((temp3 < V15_0_ALARM_MIN) || (temp3 > V15_0_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

    temp3 = read_HM_reg(0x00, 0x26) * 0.008 * ((0x28+0xa)/0xa);
	  printf("VSB5V:   %.03f V ", temp3);
	if((temp3 < V5_5_ALARM_MIN) || (temp3 > V5_5_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("     \t");

    temp3 = read_HM_reg(0x00, 0x23) * 0.008 * 2;
	  printf("VCC3:  %.03f V ", temp3);
	if((temp3 < V3_3_ALARM_MIN) || (temp3 > V3_3_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

    temp3 = read_HM_reg(0x05, 0x50) * 0.008 * 2;
	  printf("VSB3.3V: %.03f V ", temp3);
	if((temp3 < V3_3_ALARM_MIN) || (temp3 > V3_3_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("     \t");

    temp3 = read_HM_reg(0x05, 0x51) * 0.008 * 2;
	  printf("VBAT:  %.03f V ", temp3);
	if((temp3 < VBAT_ALARM_MIN) || (temp3 > VBAT_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

	return 0;
}
//========================================================
//NCB-4210 parameter-----------------------------------end
//========================================================

//========================================================
//LB-5210Z parameter-------------------------------start
//;==================Read System FAN RPM==================
Read_LB5210Z_FAN()
{
/********CPUFAN********/
    unsigned long temp1;

/********AUXFAN1********/
	  temp1 = (read_HM_reg(0x06, 0x58) << 8) | read_HM_reg(0x06, 0x59);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN1 Speed: - NA(ALARM) - \n");
    else
      printf("FAN1 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********CPUFAN********/
	  temp1 = (read_HM_reg(0x06, 0x5A) << 8) | read_HM_reg(0x06, 0x5B);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN2 Speed: - NA(ALARM) - \n");
    else
      printf("FAN2 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********SYSFAN********/
	  temp1 = (read_HM_reg(0x06, 0x5C) << 8) | read_HM_reg(0x06, 0x5D);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN3 Speed: - NA(ALARM) - \n");
    else
      printf("FAN3 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********AUXFAN0********/
	  temp1 = (read_HM_reg(0x06, 0x56) << 8) | read_HM_reg(0x06, 0x57);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN4 Speed: - NA(ALARM) - \n");
    else
      printf("FAN4 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");     

	return 0;
}
//========================================================
//LB-5210Z parameter---------------------------------end
//========================================================

//========================================================
//NCB-4210WG parameter-------------------------------start
//;==================Read System FAN RPM==================
Read_4210WG_FAN()
{
/********CPUFAN********/
    unsigned long temp1;

/********AUXFAN1********/
	  temp1 = (read_HM_reg(0x06, 0x58) << 8) | read_HM_reg(0x06, 0x59);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN1 Speed: - NA(ALARM) - \n");
    else
      printf("FAN1 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********CPUFAN********/
	  temp1 = (read_HM_reg(0x06, 0x5A) << 8) | read_HM_reg(0x06, 0x5B);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN2 Speed: - NA(ALARM) - \n");
    else
      printf("FAN2 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********SYSFAN********/
	  temp1 = (read_HM_reg(0x06, 0x5C) << 8) | read_HM_reg(0x06, 0x5D);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN3 Speed: - NA(ALARM) - \n");
    else
      printf("FAN3 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********AUXFAN0********/
/*	  temp1 = (read_HM_reg(0x06, 0x56) << 8) | read_HM_reg(0x06, 0x57);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN4 Speed: - NA(ALARM) - \n");
    else
      printf("FAN4 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");     
*/
	return 0;
}
//========================================================
//NCB-4210WG  parameter---------------------------------end
//========================================================

INT16           main (INT16 argc, char** argv )
{
	
     char tempdata1[EEPRPM_SIZE]; 
     FILE *fp;
//   char *pstr = (char *) MK_FP(0xF000, 0xF58B);
	 unsigned int 	pStrLen;
     int  Flag,j;
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
*/
//	 Project_name();
/*
     if ( Flag )
		 printf("%s", ProjectInfo[j].strShowUtilityInfo);
	 else
		 printf("Error : Lanner no this platform name...\n");
*/
j=0;
//========================================================
//NCB-5210 
//========================================================
if( j == 1 )
{
	      unsigned long temp1;
          unsigned long temp2;
          float temp3;
          int alarm_flag=0;
     // char label1[60]="--Auto scan all sensor every 5 sec, Press any key to exit--";
//         clrscr();
//         sleep(2);
	 //disable OPMA SMBUS host
//	     opma_smbus(0x05);
	 //get smbase
	_SmbBaseIoAddr  = GetSmbBaseIoAddr();
		 
//         sleep(2);
//         clrscr();
     //ScreenPutString(label1, 05, 0, 0);
     //Read temperature
         alarm_flag=0;
         printf("\nNCB-5210 onboard Temperature sensor.....\n");
         printf("--------------------------------------------------------\n");
	     write_NCT7904_reg(0xff,0x00);//bank0
   	 //NCT7904 Temp1 (Bank0 rxa0) = CPU1 Temp
   	     temp1=read_NCT7904_reg(0xa0);
    	 printf("CPU1 Temperature    :%d %cC %s \n",temp1, 0xf8,temp1>=CPU_ALARM_TEMP?" ALARM":" ");
         if(temp1>=CPU_ALARM_TEMP)
	     {
			alarm_flag|=0x01;
		 }

    /* //NCT7904 Temp2(Bank0 rxa2) = CPU2 Temp
         temp1=read_NCT7904_reg(0xa2);
 	     printf("CPU2 Temperature    :%d %cC  %s\n",temp1, 0xf8,temp1>=CPU_ALARM_TEMP?" ALARM":" ");
       	 if(temp1>=CPU_ALARM_TEMP)
		 {
			alarm_flag|=0x01;
		 }*/

	 //NCT7904 sysTemp1(Bank0 rx42) = System1 Temp
	     temp1=read_NCT7904_reg(0x42);
    	 printf("Sys1 Temperature    :%d %cC  %s\t",temp1, 0xf8,temp1>=SYS1_ALARM_TEMP?" ALARM":" ");
    	 if(temp1>=SYS1_ALARM_TEMP)
		 {
			alarm_flag|=0x01;
		 }

     //NCT7904 sysTemp1(Bank0 rx46) = System2 Temp
	     temp1=read_NCT7904_reg(0x46);
    	 printf("Sys2 Temperature    :%d %cC  %s\n",temp1, 0xf8,temp1>=SYS2_ALARM_TEMP?" ALARM":" ");
   	     if(temp1>=SYS2_ALARM_TEMP)
		 {
			alarm_flag|=0x01;
		 }
     //printf("\nalarm_flag:%x\n",alarm_flag);


     //Read Fan Speed
         printf("\nNCB-5210 onboard FAN Speed detecting.....\n");
         printf("--------------------------------------------------------\n");
#if FAN_CONFIG==0x04
	     temp1=(read_NCT7904_reg(0x80)<<5)|(read_NCT7904_reg(0x81)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
			{
				printf("FAN1 Speed:  - NA(ALARM) -\n");
      	 		alarm_flag|=0x02;
      	    }
      	 else
      	 {
      	 temp2=1350000/temp1;
	     printf("FAN1 Speed:%d RPM %s\n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	        if (temp2<FAN_ALARM_MIN)
		    {
				alarm_flag|=0x02;
		    }
      	 }
		 
	     temp1=(read_NCT7904_reg(0x84)<<5)|(read_NCT7904_reg(0x85)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
				printf("FAN2 Speed:  - NA(ALARM) -\n");
				alarm_flag|=0x02;
      	 }
      	 else
      	 {
      	 temp2=1350000/temp1;
	     printf("FAN2 Speed:%d RPM %s \n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	        if (temp2<FAN_ALARM_MIN)
		    {
				alarm_flag|=0x02;
		    }
      	 }

         temp1=(read_NCT7904_reg(0x88)<<5)|(read_NCT7904_reg(0x89)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			 printf("FAN3 Speed:  - NA(ALARM) -\n");
      	 	 alarm_flag|=0x02;
      	 }

      	 else
      	 {
      	     temp2=1350000/temp1;
	         printf("FAN3 Speed:%d RPM %s\n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	         if (temp2<FAN_ALARM_MIN)
			 {
				alarm_flag|=0x02;}
         	 }

		 temp1=(read_NCT7904_reg(0x8C)<<5)|(read_NCT7904_reg(0x8D)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			 printf("FAN4 Speed:  - NA(ALARM) -\n");
      	 	 alarm_flag|=0x02;
      	 }
       	 else
      	 {
      	     temp2=1350000/temp1;
	         printf("FAN4 Speed:%d RPM %s\n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	         if (temp2<FAN_ALARM_MIN)
			 {
				 alarm_flag|=0x02;}
      	     }
#endif //FAN_CONFIG==0x04
#if FAN_CONFIG==0x08
	     temp1=(read_NCT7904_reg(0x80)<<5)|(read_NCT7904_reg(0x81)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			 printf("FAN1A Speed:  - NA(ALARM) -  \t\t");
      	 	 alarm_flag|=0x02;
      	 }
      	 else
      	 {
      	     temp2=1350000/temp1;
	         printf("FAN1A Speed:%d RPM %s\t\t",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     "); 
      	     if (temp2<FAN_ALARM_MIN)
		     {
				alarm_flag|=0x02;
		     }
      	 }

	     temp1=(read_NCT7904_reg(0x82)<<5)|(read_NCT7904_reg(0x83)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			 printf("FAN1B Speed:  - NA(ALARM) -\n");
      	 	 alarm_flag|=0x02;
      	 }
      	 else
      	 {
      	     temp2=1350000/temp1;
	         printf("FAN1B Speed:%d RPM %s\n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	         if (temp2<FAN_ALARM_MIN)
			 {
				alarm_flag|=0x02;
			 }
      	 }

	     temp1=(read_NCT7904_reg(0x84)<<5)|(read_NCT7904_reg(0x85)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			 printf("FAN2A Speed:  - NA(ALARM) -\t\t");
      	 	 larm_flag|=0x02;
      	 }
      	 else
      	 {
      	     temp2=1350000/temp1;
	         printf("FAN2A Speed:%d RPM %s\t\t",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	         if (temp2<FAN_ALARM_MIN)
			 {
				alarm_flag|=0x02;
			 }
      	 }

	     temp1=(read_NCT7904_reg(0x86)<<5)|(read_NCT7904_reg(0x87)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			printf("FAN2B Speed:  - NA(ALARM) -\n");
			alarm_flag|=0x02;
      	 }
      	 else
      	 {
			temp2=1350000/temp1;
			printf("FAN2B Speed:%d RPM %s \n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	        if (temp2<FAN_ALARM_MIN)
			{
				alarm_flag|=0x02;
		    }
      	 }

	     temp1=(read_NCT7904_reg(0x88)<<5)|(read_NCT7904_reg(0x89)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			printf("FAN3A Speed:  - NA(ALARM) -\t\t");
			alarm_flag|=0x02;
		 }

      	 else
      	 {
			temp2=1350000/temp1;
			printf("FAN3A Speed:%d RPM %s\t\t",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
			if (temp2<FAN_ALARM_MIN)
			{
				alarm_flag|=0x02;
			}
		 }

	     temp1=(read_NCT7904_reg(0x8A)<<5)|(read_NCT7904_reg(0x8B)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			printf("FAN3B Speed:  - NA(ALARM) -\n");
      	 	alarm_flag|=0x02;
         }

      	 else
      	 {
      	    temp2=1350000/temp1;
	        printf("FAN3B Speed:%d RPM %s\n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
	        if (temp2<FAN_ALARM_MIN)
			{
				alarm_flag|=0x02;
			}
     	 }

	     temp1=(read_NCT7904_reg(0x8C)<<5)|(read_NCT7904_reg(0x8D)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			printf("FAN4A Speed:  - NA(ALARM) -\t\t");
			alarm_flag|=0x02;
      	 }
      	 else
      	 {
			temp2=1350000/temp1;
			printf("FAN4A Speed:%d RPM %s\t\t",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
			if (temp2<FAN_ALARM_MIN)
			{
				alarm_flag|=0x02;
			}
      	 }

	     temp1=(read_NCT7904_reg(0x8E)<<5)|(read_NCT7904_reg(0x8F)&0x1f);
      	 if((temp1==0x1FFF)|(temp1==0x00))
		 {
			printf("FAN4B Speed:  - NA(ALARM) -\n");
			alarm_flag|=0x02;
      	 }
      	 else
      	 {
			temp2=1350000/temp1;
			printf("FAN4B Speed:%d RPM %s\n",temp2, temp2<FAN_ALARM_MIN?" ALARM":"     ");
			if (temp2<FAN_ALARM_MIN)
			{
			alarm_flag|=0x02;
			}
      	 }
#endif //FAN_CONFIG==0x08

    //   printf("\nalarm_flag:%x\n",alarm_flag);

    //read Voltage
          printf("\nNCB-5210 Voltage detecting.....\n");
          printf("--------------------------------------------------------\n");
	      temp3= (((read_NCT7904_reg(0x4C))*8)+((read_NCT7904_reg(0x4D))& 0x07))*0.002;
	      printf("VDDR:    %.03f V ", temp3);
	      if((temp3 < VPVDD_ALARM_MIN) || (temp3 > VPVDD_ALARM_MAX))
              printf("ALARM\t");
          else
              printf("     \t");

   	      temp3= (((read_NCT7904_reg(0x54))*8)+((read_NCT7904_reg(0x55))& 0x07))*0.002;
	      printf("CPU1 VCORE: %.03f V ", temp3);
	      if((temp3 < VCORE_ALARM_MIN) || (temp3 > VCORE_ALARM_MAX))	  
              printf("ALARM\n");
          else
              printf("     \n");

	      temp3= ((((read_NCT7904_reg(0x56))*8)+((read_NCT7904_reg(0x57))& 0x07))*0.002)*3;
	      printf("VSB5V:   %.03f V ", temp3);
	      if((temp3 < V5_0_ALARM_MIN_1) || (temp3 > V5_0_ALARM_MAX_1))
              printf("ALARM\t");
          else
              printf("     \t");

	      temp3= ((((read_NCT7904_reg(0x58))*8)+((read_NCT7904_reg(0x59))& 0x07))*0.002)*3;
	      printf("        5V: %.03f V ", temp3);
	      if((temp3 < V5_0_ALARM_MIN_1) || (temp3 > V5_0_ALARM_MAX_1))
              printf("ALARM\n");
          else
              printf("     \n");

	      temp3= ((((read_NCT7904_reg(0x5a))*8)+((read_NCT7904_reg(0x5b))& 0x07))*0.002)*0x0c;
	      printf("12V:    %.03f V ", temp3);
	      if((temp3 < V12_0_ALARM_MIN_1) || (temp3 > V12_0_ALARM_MAX_1))
              printf("ALARM\t");
          else
              printf("     \t");

	      temp3= ((((read_NCT7904_reg(0x5C))*8)+((read_NCT7904_reg(0x5D))& 0x07))*0.006)*0x01;
	      printf("      3.3V: %.03f V ", temp3);
	      if((temp3 < V3_3_ALARM_MIN_1) || (temp3 > V3_3_ALARM_MAX_1))
              printf("ALARM\n");
          else
              printf("     \n");

	      temp3= ((((read_NCT7904_reg(0x60))*8)+((read_NCT7904_reg(0x61))& 0x07))*0.006)*0x01;
	      printf("VSB3.3V: %.03f V ", temp3);
	      if((temp3 < V3_3_ALARM_MIN_1) || (temp3 > V3_3_ALARM_MAX_1))
              printf("ALARM\t");
          else
              printf("     \t");

	      temp3= ((((read_NCT7904_reg(0x5E))*8)+((read_NCT7904_reg(0x5F))& 0x07))*0.006)*0x01;
	      printf("      VBAT: %.03f V ", temp3);
	      if((temp3 < V3_3_ALARM_MIN_1) || (temp3 > V3_3_ALARM_MAX_1))
              printf("ALARM\n");
          else
              printf("     \n");

	// sleep(5);

    //enable OPMA SMBUS host
//	      opma_smbus(0x04);

 	      printf("*************************  Alarm Status  *************************\n");	   	   	 
          if(alarm_flag)
          {
             printf("  *** Temperature:%s, FanSpeed:%s, Voltage:%s  *** \n",(alarm_flag&0x01)?"Alarm ":"Pass ",(alarm_flag&0x02)?"Alarm ":"Pass ",(alarm_flag&0x04)?"Alarm ":"Pass " );
          return 1;
          }
          else
          {
             printf(" No Alarm status \n");
             return 0;
          }
	      
}
	
//========================================================
//NCB-4210 ,NCB-4210WG
//========================================================
	if( j == 0 )
	{

//	sleep(1);
	//disable OPMA SMBUS host
//	opma_smbus(0x05);
	//get smbase
	_SmbBaseIoAddr  = GetSmbBaseIoAddr();

//    do
    {
        sleep(2);
//      clrscr();

       	printf("\nNCB-5220 onboard Temperature sensor.....\n");
     	printf("--------------------------------------------------------\n");
	    Read_Temperature();				   //;Read temp.
      	printf("\nNCB-5220 onboard FAN Speed detecting.....\n");
     	printf("--------------------------------------------------------\n");
	    Read_FAN();						   //;Read FAN RPM
   	    printf("\nNCB-5220 onboard Voltage detecting.....\n");
   	    printf("--------------------------------------------------------\n");
	    Read_Vol();						   //;Read Vol
		
	    printf("              *** Press Any key to quit this program ***\n");
	}

//        while(kbhit()==0);
//enable OPMA SMBUS host
//	    opma_smbus(0x04);
	}
	
	
//========================================================
//LB-5210Z ,LB-4210Z
//========================================================
	if( j == 2 || j==4 ) 
	{
		
//	sleep(1);
	//disable OPMA SMBUS host
//	opma_smbus(0x05);
	//get smbase
	_SmbBaseIoAddr  = GetSmbBaseIoAddr();

//    do
    {
        sleep(2);
//      clrscr();	
       	printf("MB-LS5210Z onboard Temperature sensor.....\n");
     	printf("--------------------------------------------------------\n");
	    Read_Temperature();				   //;Read temp.
      	printf("\nMB-LS5210Z onboard FAN Speed detecting.....\n");
     	printf("--------------------------------------------------------\n");
	    Read_LB5210Z_FAN();				   //;Read FAN RPM
   	    printf("\nMB-LS5210Z onboard Voltage detecting.....\n");
   	    printf("--------------------------------------------------------\n");
	    Read_Vol();						   //;Read Vol
		
	    printf("              *** Press Any key to quit this program ***\n");

	}

//       while(kbhit()==0);
//enable OPMA SMBUS host
//	    opma_smbus(0x04);

	}

	
//========================================================
//NCB-4210WG 
//========================================================
	if( j == 3 ) 
	{
		
//	sleep(1);
	//disable OPMA SMBUS host
//	opma_smbus(0x05);
	//get smbase
	_SmbBaseIoAddr  = GetSmbBaseIoAddr();

//    do
    {
        sleep(2);
//      clrscr();	
       	printf("MB-LS5210Z onboard Temperature sensor.....\n");
     	printf("--------------------------------------------------------\n");
	    Read_Temperature();				   //;Read temp.
      	printf("\nMB-LS5210Z onboard FAN Speed detecting.....\n");
     	printf("--------------------------------------------------------\n");
	    Read_4210WG_FAN();				   //;Read FAN RPM
   	    printf("\nMB-LS5210Z onboard Voltage detecting.....\n");
   	    printf("--------------------------------------------------------\n");
	    Read_Vol();						   //;Read Vol
		
	    printf("              *** Press Any key to quit this program ***\n");

	}

//        while(kbhit()==0);
//enable OPMA SMBUS host
//	    opma_smbus(0x04);

	}
}
	
