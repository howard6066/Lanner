#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

#define inportb(x) inb_p(x)
#define outportb(x, y) outb_p(y, x)
#define inportw(x) inw_p(x)
#define outportw(x, y) outw_p(y, x)
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

int main(int argc, char const *argv[])
{
    /* code */
    iopl(3);
    unsigned char tmp=0;
    unsigned char a=0;
     unsigned char b=0;
    outportb(INDEX_PORT, BANK_REG); //LDN register
    delay(1);
    outportb(DATA_PORT, LDN7);
    delay(1);
    outportb(INDEX_PORT, 0xe1); //reg=0xe1
    delay(1);  	
    tmp = inportb(DATA_PORT); //0x2F
    a=~0x06;
    printf("%x\n",a);	
    printf("%x\n",tmp);	
    return 0;
}
