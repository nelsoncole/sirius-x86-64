#include <mm.h>
#include <msr.h>
#include <io.h>
#include <apic.h>


#include <stdio.h>

#define null 0

void apic_initial_count_timer(int value)
{
	local_apic_write_command( APIC_INITIAL_COUNT_TIMER, value);

}
int apic_timer()
{
    // Map APIC timer to an interrupt, and by that enable it in one-shot mode
    unsigned int val = APIC_CONFIG_DATA_LVT(0/*one-shot mode*/,0/*umasked*/,null,null,0,null,0x20/*vetor*/);
    local_apic_write_command( APIC_LVT_TIMER, val);

	//Divide Configuration Register, to divide by 16
	local_apic_write_command( APIC_DIVIDE_TIMER, 0x3);

    // Initialize PIT Ch 2 in one-shot mode
    // waiting 1 sec could slow down boot time considerably,
	// so we'll wait 1/100 sec, and multiply the counted ticks
    unsigned char byte = inportb(0x61);
    byte = byte &0xfd;
    byte = byte | 1;
    outportb(0x61, byte);

    byte = 0b10110010;
    outportb(0x43, byte);

    // 1193180/100 Hz = 11931 = 0x2e9b
    outportb(0x42, 0x9b); // LSB
    byte = inportb(0x60); // Short delay
    outportb(0x42, 0x2e); // MSB

    // Reset PIT one-short counter (start counting)
    byte = inportb(0x61);
    byte = byte &0xfe;
    outportb(0x61, byte); // gate low
    byte = byte | 1;
    outportb(0x61, byte); // gate high

    // Rest APIC Timer (set counter to -1)
    local_apic_write_command( APIC_INITIAL_COUNT_TIMER, 0xFFFFFFFF);

    // Now wait until PIT counter reaches zero
    while(inportb(0x61)&0x20){}

    // Stop APIC Timer
    apic_timer_masked();

    // Now do the math...
    // Get current counter value
    unsigned int counter = local_apic_read_command( APIC_CURRENT_COUNT_TIMER);

    // It is counted down from -1, make it positive
    counter = 0xFFFFFFFF - counter;
    counter ++;
    // We used divide value different than 1, so now we have to multiply the result by 16
    counter = counter*16;// >> 4; // *16

    // Moreover, PIT did not wait a whole sec, only a fraction, so multiply by that too
    counter = counter * 100; // *PITHz
    // -----edx:eax now holds the CPU bus frequency-----
    // now calculate timer counter value of your choice
	// this means that tasks will be preempted 1000 times in a second. 100 is popular too.
    counter = counter / 1000;

    // Again, we did not use divide value of 1
    counter = counter/16;  //<< 4; // /16

    // Sanity check, min 16
    if(counter != 0x10){
        counter = 0x10;
    }
    
    // Now eax holds appropriate number of ticks, use it as APIC timer counter initializer
    local_apic_write_command( APIC_INITIAL_COUNT_TIMER, counter);

    // Finally re-enable timer in periodic mode
	val = APIC_CONFIG_DATA_LVT(1/*periodic mode*/,1/*masked*/,null,null,0,null,0x20/*vetor*/);
    local_apic_write_command( APIC_LVT_TIMER, val);
        
    // Setting divide value register again not needed by the manuals
    // Although I have found buggy hardware that required it
    local_apic_write_command( APIC_DIVIDE_TIMER, 0x3);
	
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

