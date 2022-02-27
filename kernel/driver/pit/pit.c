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


void pit_prepare_sleep(int hz)
{
    // Initialize PIT Ch 2 in one-shot mode
    // waiting 1 sec could slow down boot time considerably,
	// so we'll wait 1/100 sec, and multiply the counted ticks

    int val = 0;

    unsigned int divisor = 1193182/hz;
    
    val = (inportb(0x61) & 0xfd) | 0x1;
    outportb(0x61, val);
    outportb(0x43, 0xb2);

    outportb(0x42,(unsigned char)(divisor & 0xFF));		// LSB
    inportb(0x60); // Short delay
	outportb(0x42,(unsigned char)(divisor >> 8) & 0xFF); // MSB

}

void pit_perform_sleep(){
    
    // Reset PIT one-short counter (start counting)
    int val = 0;
    val = (inportb(0x61) & 0xfe);
    outportb(0x61, val); // gate low
    val = val | 1;
    outportb(0x61, val); // gate high

    // Now wait until PIT counter reaches zero
    while((inportb(0x61) & 0x20) == 0){}
}
