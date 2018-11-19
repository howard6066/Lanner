#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

#define inportb(x) inb_p(x)
#define outportb(x, y) outb_p(y, x)
#define inportl(x) inl_p(x)
#define outportl(x, y) outl_p(y, x)

#define INDEX_PORT	0x2E
#define DATA_PORT	0x2F
#define BANK_REG	0x07
#define LDN7		0x07
#define delay(x) usleep(x)

#define siogpio71 0x02
#define siogpio72 0x04
#define siogpio74 0x10
#define siogpio75 0x20
	
#define amberled siogpio71
#define redled siogpio72
#define alarmamberled siogpio74
#define alarmredled siogpio75


//;********************* Enter SIO *****************************************************
void Enter_Configuation(void)
{
    	outportb(INDEX_PORT, 0x87);  
        delay(1);
    	outportb(INDEX_PORT, 0x87); 
	return;
}

//;********************* Exit SIO *****************************************************
void Exit_Configuation(void)
{
    	outportb(INDEX_PORT, 0xaa);  						//Exit sio Configuration
	return;
}

//;********************* Read SIO *****************************************************
unsigned char read_sio_reg(int LDN, int reg)
{
        unsigned char tmp=0;	
	Enter_Configuation();		
        outportb(INDEX_PORT, BANK_REG); //LDN register
        delay(1);
        outportb(DATA_PORT, LDN);
        delay(1);
        outportb(INDEX_PORT, reg);
        delay(1);       
        tmp = inportb(DATA_PORT); //0x2F
	Exit_Configuation();         
	return tmp;
}

//;********************* Write SIO *****************************************************
void write_sio_reg(int LDN, int reg, int value)
{
	Enter_Configuation();		
        outportb(INDEX_PORT, BANK_REG); //LDN register
        delay(1);
        outportb(DATA_PORT, LDN);
        delay(1);
        outportb(INDEX_PORT, reg);
        delay(1);
        outportb(DATA_PORT, value);
	Exit_Configuation();        
        return;
}

void set_led_off(void)
{
        unsigned long tmp=0;
	         
	tmp = read_sio_reg(LDN7, 0xe1) & ~(amberled + redled); //siogpio71 0x02 + siogpio72 0x04
    //tmp = read_sio_reg(LDN7, 0xe1);
    //tmp = ~(amberled + redled);
	tmp |= (amberled + redled);	//;GPIO71,72 set 11b
	write_sio_reg(LDN7, 0xe1, tmp);
	printf("%ld\n",tmp);	

        return;
}

int main()
{
  iopl(3);
  set_led_off();
  //set_led_red();
  //set_led_amber();
  //printf("%ld\n",tmp);
  return 0;
}