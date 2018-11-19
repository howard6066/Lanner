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
#define PCI_index			0xcf8
#define PCI_data			0xcfc

//LEB-6041--------------------------------------------------------

#define VCORE_ALARM_MAX   	1.0*1.2
#define VCORE_ALARM_MIN   	1.0*0.6         //(ApolloLake LEB-6041)
#define VDDR_ALARM_MAX   	1.3*1.05
#define VDDR_ALARM_MIN   	1.3*0.95
#define V5_ALARM_MAX    	5.0*1.1
#define V5_ALARM_MIN    	5.0*0.9
#define V3_3_ALARM_MAX  	3.3*1.1
#define V3_3_ALARM_MIN   	3.3*0.9
#define VSB3V3_ALARM_MAX  	3.3*1.1
#define VSB3V3_ALARM_MIN  	3.3*0.9
#define BAT_ALARM_MAX  	3.0*1.1
#define BAT_ALARM_MIN  	3.0*0.9
#define FAN_ALARM_MIN   1000
#define FAN_ALARM_MAX   10000

int sm_base;

int get_smbase(void)
{
outportl(PCI_index,(0x80000000+(0<<16)+(31<<11)+(4<<8)+0x20));//smbase dev31 fun4 rx21-20
return ((inportw(PCI_data))&0xfffe);
}




unsigned int read_sio_hwm(unsigned char bank, unsigned char addr)
{
    unsigned char data8;
    
    outportb(0xA26, bank);
    outportb(0xA25, addr);
    return inportb(0xA26);
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
    
     printf("*****************************************************\n");
     printf("**  Lanner Electronics Inc.                        **\n");
     printf("**  HardWare Monitor utility Ver, 0.1 for LEB-6041 **\n");  
     printf("*****************************************************\n");
     sleep(2);

     	sm_base=get_smbase();
          

      printf("\nLEB-6041 onboard Temperature sensor.....\n");
     	printf("--------------------------------------------------------\n");
	    printf("CPU temp:  %d C\n", read_sio_hwm(0x00, 0x72));	
	    printf("SYS temp:  %d C\n", read_sio_hwm(0x00, 0x74));
      
   	  printf("\nLEB-6041 onboard Voltage detecting.....\n");
   	  printf("--------------------------------------------------------\n");
	    //Read_Vol();						   //;Read Vol
		  temp3 = read_sio_hwm(0x00, 0x21) * 0.008;
	  	printf("VCORE: %.03f V ", temp3);
    	if((temp3 < VCORE_ALARM_MIN) || (temp3 > VCORE_ALARM_MAX))
      	printf("ALARM\n");
    	else
      	printf("\n");
    
    	temp3 = read_sio_hwm(0x00, 0x29) * 0.008 * 3;
	  	printf("VSB5V: %.03f V ", temp3);
    	if((temp3 < V5_ALARM_MIN) || (temp3 > V5_ALARM_MAX))
      	printf("ALARM\n");
    	else
      	printf("     \n");


    temp3 = read_sio_hwm(0x00, 0x28) * 0.008 * 2;
	  printf("VBAT : %.03f V ", temp3);     
    if((temp3 < BAT_ALARM_MIN) || (temp3 > BAT_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("       \n");

    temp3 = read_sio_hwm(0x00, 0x2A) * 0.008 * 2;
	  printf("3.3  : %.03f V ", temp3);     
    if((temp3 < V3_3_ALARM_MIN) || (temp3 > V3_3_ALARM_MAX))
      printf("ALARM\n");
    else
      printf("        \n");  
	 
}





