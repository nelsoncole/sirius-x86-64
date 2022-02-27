#include <mm.h>
#include <msr.h>
#include <io.h>
#include <apic.h>


#include <stdio.h>

#define null 0
unsigned int apic_timer_ticks;

void apic_initial_count_timer(int value)
{
	local_apic_write_command( APIC_INITIAL_COUNT_TIMER, value);

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

    while((inportb(0x61) & 0x20) == 0){}
}

int apic_timer()
{
    // Map APIC timer to an interrupt, and by that enable it in one-shot mode
    unsigned int val = APIC_CONFIG_DATA_LVT(0/*one-shot mode*/,0/*umasked*/,null,null,0,null,0x20/*vetor*/);
    local_apic_write_command( APIC_LVT_TIMER, val);

	//Divide Configuration Register, to divide by 16
	local_apic_write_command( APIC_DIVIDE_TIMER, 0x3);

   
    pit_prepare_sleep(100);

    // Rest APIC Timer (set counter to -1)
    local_apic_write_command( APIC_INITIAL_COUNT_TIMER, 0xFFFFFFFF);

    // Now wait until PIT counter reaches zero
    pit_perform_sleep();

    // Stop APIC Timer
	local_apic_write_command( APIC_LVT_TIMER, 1 << 16);

    // Now do the math...
    // Get current counter value
    apic_timer_ticks = local_apic_read_command( APIC_CURRENT_COUNT_TIMER);
    // It is counted down from -1, make it positive
    apic_timer_ticks = 0xFFFFFFFF - apic_timer_ticks;
    apic_timer_ticks++;

    printf("apic_timer_ticks %d\n", apic_timer_ticks);

    
    // Finally re-enable timer in periodic mode
	val = APIC_CONFIG_DATA_LVT(1/*periodic mode*/,1/*masked*/,null,null,0,null,0x20/*vetor*/);
    local_apic_write_command( APIC_LVT_TIMER, val);
        
    // Setting divide value register again not needed by the manuals
    // Although I have found buggy hardware that required it
    local_apic_write_command( APIC_DIVIDE_TIMER, 0x3);

    // Now eax holds appropriate number of ticks, use it as APIC timer counter initializer
    local_apic_write_command( APIC_INITIAL_COUNT_TIMER, apic_timer_ticks);
	
	return 0;
	
}

void apic_timer_umasked(){

	unsigned int data = local_apic_read_command(APIC_LVT_TIMER);
    local_apic_write_command( APIC_LVT_TIMER, data &0xFFFEFFFF);

	//Divide Configuration Register, to divide by 128 0xA divide by 32 0x8
	local_apic_write_command( APIC_DIVIDE_TIMER, 0xA);

	// Initial Count Register
	local_apic_write_command( APIC_INITIAL_COUNT_TIMER, 12345);
	
}

void apic_timer_masked(){

	unsigned int data = local_apic_read_command(APIC_LVT_TIMER);
	local_apic_write_command( APIC_LVT_TIMER, data | 1 << 16);

}

