#include <stdio.h>
#ifdef DJGPP
/* For DOS djgpp */
#include <dos.h>
#include <inlines/pc.h>
#else
/* For Linux */
#include <sys/io.h>
#include <time.h>
#include <unistd.h>
#define inportb(x)	inb(x)
#define inportw(x)	inw(x)
#define inportl(x)	inl(x)
#define outportb(x, y)	outb(y, x)
#define outportw(x, y)	outw(y, x)
#define outportl(x, y)	outl(y, x)
#define delay(x)	usleep(x*100)
#define sleep(x)	usleep(x*1000000)
#endif
#define sio_index   	0x2e
#define sio_data    	0x2f
#define siohm_index 	0xa15
#define siohm_data  	0xa16
#define PCI_index	0xcf8
#define PCI_data		0xcfc
#define NCT7904_SlaveAdd   	0x5c
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

#define V15_0_ALARM_MAX   	1.2*1.1
#define V15_0_ALARM_MIN   	1.2*0.9
#define V5_5_ALARM_MAX   		5*1.1
#define V5_5_ALARM_MIN   		5*0.9
#define VBAT_ALARM_MAX   		3.0*1.1
#define VBAT_ALARM_MIN   		3.0*0.9
int sm_base;

int get_smbase(void)
{
outportl(PCI_index,(0x80000000+(0<<16)+(31<<11)+(4<<8)+0x20));//smbase dev31 fun4 rx21-20
return ((inportw(PCI_data))&0xfffe);
}

void wait_for_IBF_clear(int delay_count){
	int i=0;
	for(i=0;i<delay_count;i++){
		if ((inportb(0xca3)&0x02)!=0x02)break;	
		delay(20);
	}	
}
void clear_OBF(void){
	int i=0;
	i=inportb(0xca2);
}

int opma_smbus(int value){
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
//busy_count = 0.1x sec time out
int check_bsy_err(int busy_count){
int	k;	
if(inportb(sm_base+0x00)&0x04){ //error?
//   	 printf("\n Read W792 device error!!\n");
//	 exit(1);
	 return(1);
  	}
for(k=0;k<busy_count;k++){  	
	if(inportb(sm_base+0x00)&0x09){ delay(1000);}
	else{ return(0);}
}
if(inportb(sm_base+0x00)&0x01){
	 printf("\n Read NCT7904 device busy time out!!\n");
 	 return(2);
	}
if(inportb(sm_base+0x00)&0x08){
	 printf("\n smbus bus error  time out!!\n");
 	 return(3);
	}
}


int write_NCT7904_reg(int reg, int value)
{
	int	i,j;		
 	for(j=0;j<10;j++){ 	
		for(i=0;i<10;i++){	
			outportb((sm_base+0x00),0xfe);//clr status
  			delay(10);	
  			if (check_bsy_err(100)==0x00){break;}			
		   }
  	
  	outportb((sm_base+0x03),reg);//offset
  	delay(5);
  	outportb((sm_base+0x05),value);//value
  	delay(5);  
  	outportb((sm_base+0x04),(NCT7904_SlaveAdd& 0xfe));
  	delay(5);  
  	outportb((sm_base+0x02),0x48);//byte write cmd
  	delay(20);//more delay for cmd finish 
    	if (check_bsy_err(100)==0x00){break;}			
 	}				
  	outportb((sm_base+0x00),0xfe);//clr status
  	delay(10);
	return(0);
}
int read_NCT7904_reg(int reg)
{
	int	i,j;		
 	for(j=0;j<10;j++){ 
		for(i=0;i<10;i++){	
			outportb((sm_base+0x00),0xfe);//clr status
  			delay(10);	
  			if (check_bsy_err(100)==0x00){break;}			
	    	}	
	  	    	  		
  		outportb((sm_base+0x03),reg);//offset
  		delay(2);
  		outportb((sm_base+0x04),(NCT7904_SlaveAdd|0x01));
  		delay(2);  
  		outportb((sm_base+0x02),0x48);//byte write cmd
  		delay(10);//more delay for cmd finish 
 	    	if (check_bsy_err(100)==0x00){break;}			
  	    }				
  	outportb((sm_base+0x00),0xfe);//clr status
  	delay(10);
	return inportb(sm_base+0x05);//value
  	
}


void sio_hwm_init(void)
{
    outportb(0x2E, 0x87);
    outportb(0x2E, 0x87);

    outportb(0x2E, 0x07);
    outportb(0x2F, 0x0B);

    outportb(0x2E, 0x60);
    outportb(0x2F, 0x0A);

    outportb(0x2E, 0x61);
    outportb(0x2F, 0x10);

    outportb(0x2E, 0x30);
    outportb(0x2F, 0x01);
}

unsigned int 
read_sio_hwm(unsigned char bank, unsigned char addr)
{
    unsigned char data8;
    
    outportb(0xA15, 0x4E);
    data8 = inportb(0xA16);
    data8 &= 0xF8;
    data8 |= bank;
    outportb(0xA16, data8);
    
    outportb(0xA15, addr);
    return inportb(0xA16);
}

int main()
{
#ifndef DJGPP
	iopl(3);
#endif		
     unsigned int temp1;
     unsigned int temp2;
     float temp3;
     int alarm_flag=0;
     int cpu0temp, cpu1temp, sys0temp;
    // char label1[60]="--Auto scan all sensor every 5 sec, Press any key to exit--";
    // clrscr();
     printf("*****************************************************\n");
     printf("**  Lanner Electronics Inc.                        **\n");
     printf("**  HardWare Monitor utility Ver, 1.2 for NCB-5220 **\n");  
     printf("*****************************************************\n");
     sleep(2);
	//disable OPMA SMBUS host
	//	 opma_smbus(0x05);
	//get smbase     
     	sm_base=get_smbase();
      sio_hwm_init();        

  //  clrscr();
      printf("\nNCB-5220 onboard Temperature sensor.....\n");
     	printf("--------------------------------------------------------\n");
	    //JJDEBUG printf("SYS1 temp:  %d C\n", read_sio_hwm(0x00, 0x73));	//;Super IO (bank 0 offset 0x73) for system temp.
	    printf("CPU1 temp:  %d C\n", (read_sio_hwm(0x07, 0x20)& 0x7F));
		printf("TS1 temp:  %d C\n", read_sio_hwm(0x01, 0x50));	//;Super IO (bank 1 offset 0x50) for CPUTIN (SMIOVT2 temperature). //JJDEBUG
	    printf("TS2 temp:  %d C\n", read_sio_hwm(0x00, 0x73));	//;Super IO (bank 0 offset 0x73) for SYSTIN. //JJDEBUG
      printf("\nNCB-5220 onboard FAN Speed detecting.....\n");
     	printf("--------------------------------------------------------\n");
	    /********AUXFAN1********/
	  temp1 = (read_sio_hwm(0x06, 0x58) << 8) | read_sio_hwm(0x06, 0x59);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN1 Speed: - NA(ALARM) - \n");
    else
      printf("FAN1 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********CPUFAN********/
	  temp1 = (read_sio_hwm(0x06, 0x5A) << 8) | read_sio_hwm(0x06, 0x5B);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN2 Speed: - NA(ALARM) - \n");
    else
      printf("FAN2 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********SYSFAN********/
	  temp1 = (read_sio_hwm(0x06, 0x5C) << 8) | read_sio_hwm(0x06, 0x5D);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN3 Speed: - NA(ALARM) - \n");
    else
      printf("FAN3 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");

/********AUXFAN0********/
	  temp1 = (read_sio_hwm(0x06, 0x56) << 8) | read_sio_hwm(0x06, 0x57);
    //0x00A4 = 0 RPM
    if(temp1 == 0xA4)
      printf("FAN4 Speed: - NA(ALARM) - \n");
    else
      printf("FAN4 Speed: %d RPM %s\n", temp1, temp1 < FAN_ALARM_MIN ? "ALARM" : "     ");
       
   	  printf("\nNCB-5220 onboard Voltage detecting.....\n");
   	  printf("--------------------------------------------------------\n");
	    //Read_Vol();						   //;Read Vol
		       temp3 = read_sio_hwm(0x00, 0x20) * 0.008;
	  printf("VCORE:   %.03f V ", temp3);
    //if((temp3 < VCORE_ALARM_MIN) || (temp3 > VCORE_ALARM_MAX))
    //  printf("ALARM\t");
    //else
      printf("\t");

    temp3 = read_sio_hwm(0x00, 0x21) * 0.008 * ((0xb+1)/1);
	  printf("12V:  %.03f V ", temp3);
    if((temp3 < V12_0_ALARM_MIN) || (temp3 > V12_0_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

    temp3 = read_sio_hwm(0x00, 0x24) * 0.008 * ((0x28+0xa)/0xa);
	  printf("5V:      %.03f V ", temp3);
    if((temp3 < V5_0_ALARM_MIN) || (temp3 > V5_0_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("     \t");

    temp3 = read_sio_hwm(0x00, 0x25) * 0.008 ;
	  printf("VDDQ:  %.03f V ", temp3);
	if((temp3 < V15_0_ALARM_MIN) || (temp3 > V15_0_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

    temp3 = read_sio_hwm(0x00, 0x26) * 0.008 * ((0x28+0xa)/0xa);
	  printf("VSB5V:   %.03f V ", temp3);
	if((temp3 < V5_5_ALARM_MIN) || (temp3 > V5_5_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("     \t");

    temp3 = read_sio_hwm(0x00, 0x23) * 0.008 * 2;
	  printf("VCC3:  %.03f V ", temp3);
	if((temp3 < V3_3_ALARM_MIN) || (temp3 > V3_3_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");

    temp3 = read_sio_hwm(0x05, 0x50) * 0.008 * 2;
	  printf("VSB3.3V: %.03f V ", temp3);
	if((temp3 < V3_3_ALARM_MIN) || (temp3 > V3_3_ALARM_MAX))
      printf("ALARM\t");
    else
      printf("     \t");

    temp3 = read_sio_hwm(0x05, 0x51) * 0.008 * 2;
	  printf("VBAT:  %.03f V ", temp3);
	if((temp3 < VBAT_ALARM_MIN) || (temp3 > VBAT_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("     \n");
	//while(kbhit()==0);
  //enable OPMA SMBUS host
	//opma_smbus(0x04); 
		
// 	printf("\n\n*************************  Alarm Status  *************************\n");		 	   	   	 
// if(alarm_flag){
//   printf("  *** Temperature:%s, FanSpeed:%s, Voltage:%s  *** \n",(alarm_flag&0x01)?"Alarm ":"Pass ",(alarm_flag&0x02)?"Alarm ":"Pass ",(alarm_flag&0x04)?"Alarm ":"Pass " );
//    return 1;
//    
//}else{
//    printf(" No Alarm status \n");
//    return 0;
//}
 
}





