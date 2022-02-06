#include <rtc.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <irq.h>
// convert BCD para Decimal
#define convert_bcd(val)  ((val / 16) * 10 + (val & 0xf))

void rtc_handler(void);

struct tm *clock;


void disable_NMI(){

	char val = inportb(0x70);
	val |= 0x80;
    	outportb(0x70,val);

}

void enable_NMI(){

	char val = inportb(0x70);
	val &=~0x80;
    	outportb(0x70,val);
}

static int get_update_in_progress()
{
	outportb(0x70,0x0A);
	return (int)(inportb(0x71) &0x80);
}


void rtc_setup(){


	printf("RTC install...\\\\\n");
	

	clock = (struct tm*) malloc(sizeof(struct tm));
	memset(clock,0,sizeof(struct tm));
	
	clock->tm_utc = 1;
	
	fnvetors_handler[8] = &rtc_handler;

	disable_NMI();

    unsigned int status;
    // registro status B e desabilita NMI
    outportb(0x70,0x8B);
    // ler actual valor de status B  
    status = inportb(0x71);
    // registro status B 
    outportb(0x70,0x8B);
	
    //bit 6: Hablita interrupções periódica IRQ8,
    //bit 5 : hablita interrupção de alarme,
    //bit 2 : caledário em formato binário,
    //bit 1 : formato 24h
    outportb(0x71, status | 0x22); 

	
    enable_NMI();
}


// código c para o manipulador IRQ8 offset no IDT vetor 40
void rtc_handler(void){
	
	while( get_update_in_progress() );

	outportb(0x70,0);
	clock->tm_sec = convert_bcd(inportb(0x71)) &0x3f;

	outportb(0x70,2);
	clock->tm_min = convert_bcd(inportb(0x71)) &0x3f;


	outportb(0x70,4);
	clock->tm_hour = convert_bcd(inportb(0x71)) &0x1f;
	
	clock->tm_hour += clock->tm_utc;
	if(clock->tm_hour > 23) clock->tm_hour = 0;
	
	static unsigned char status;
	outportb(0x70,0x0C);
	// é importante ler do registro status C 
 	status = inportb(0x71);


	if(status);
	

}


unsigned int swait_init(int time){
    unsigned int status;
    while( get_update_in_progress() );

	outportb(0x70,0);
	int sec = convert_bcd(inportb(0x71)) &0x3f;
    outportb(0x70,2);
	int min = convert_bcd(inportb(0x71)) &0x3f;


    min += time/60;
    sec += time%60;
    sec += 1;
    min += sec/60;
    sec = sec%60;

    status = min << 8 & 0xFF00;
    status |= sec;

    return status;
}

int swait(unsigned int status){
    while( get_update_in_progress() );

	outportb(0x70,0);
	int sec = convert_bcd(inportb(0x71)) &0x3f;
    outportb(0x70,2);
	int min = convert_bcd(inportb(0x71)) &0x3f;

    if(sec >= (status&0xFF) && min >= (status >> 8 &0xFF) ){
        return 0;
    }

    return 1; 
}
