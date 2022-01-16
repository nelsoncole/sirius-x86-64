#include <pit.h>
#include <io.h>
#include <irq.h>
#include <ethernet.h>
unsigned long timer_ticks;
unsigned long eticks;

extern void rtc_handler(void);

static void pit_handler()
{	
	++timer_ticks;
	if(eticks)--eticks;
	
	rtc_handler();
    //handler_ethernet_package_received();
}

void pit_set_frequencia(unsigned int freq)
{

	unsigned int divisor = 1193182/freq;

	cli();
	
    	// Control uint16_t register
	// bits 7-6 = 0 - Set counter 0 (counter divisor),bit 5-4=11 LSB/MSB 16-bit
	// bit 3-1=x11 Square wave generator, bit 0 =0 Binary counter 16-bit
	outportb(0x43,0x36);
	outportb(0x40,(unsigned char)(divisor & 0xFF));		
	outportb(0x40,(unsigned char)(divisor >> 8) & 0xFF);
	

}


void pit_enable(int control){

	cli();
	
	eticks = 0;
	timer_ticks = 0;
	
	fnvetors_handler[2] = &pit_handler;
	
	if(control)
	{
		// Set mode 2 (rate generator)
		pit_set_frequencia( FREQ );
	
	} else {
	
		// Set mode 1 (hardware re-triggerable one-shot)
		outportb(0x43, 0x32 );
		
	}

}

void timer_wait(unsigned long  ticks)
{

	eticks = ticks;

	while(eticks){
		__asm__ __volatile__("pause;":::"memory");
	}

}
