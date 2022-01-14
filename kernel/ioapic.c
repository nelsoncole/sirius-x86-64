#include <mm.h>
#include <msr.h>
#include <io.h>
#include <apic.h>


#include <stdio.h>


#define null 0

// I/O APIC
IOAPIC_REDIR_TBL ioapic_redir_table[24];

unsigned long ioapic_base;

unsigned int read_ioapic_register(int reg) {

	*(volatile unsigned char*)(ioapic_base + IO_APIC_IND) = reg;
	
	return ( *(volatile unsigned int*)(ioapic_base + IO_APIC_DAT));
}

void write_ioapic_register(int reg, unsigned int val) {

	*(volatile unsigned char*)(ioapic_base + IO_APIC_IND) = reg;

	*(volatile unsigned int*)(ioapic_base + IO_APIC_DAT) = val;
}

int ioapic_masked(int n)
{

	unsigned long pointer = (unsigned long)&ioapic_redir_table;

	ioapic_redir_table[n].mask = 1;
	
	unsigned int *data = (unsigned int*) pointer;
	write_ioapic_register(IO_APIC_REDIR_TBL(n),*(unsigned int*)(data + (n *2)));


	return 0;
}

int ioapic_umasked(int n)
{

	unsigned long pointer = (unsigned long)&ioapic_redir_table;

	ioapic_redir_table[n].mask = 0;
	
	unsigned int *data = (unsigned int*) pointer;
	write_ioapic_register(IO_APIC_REDIR_TBL(n),*(unsigned int*)(data + (n *2)));


	return 0;
}


int set_ioapic_redir_table(int n,unsigned char vector,unsigned char delv_mode,
				unsigned char dest_mode,unsigned char delv_status,
				unsigned char pin_polarity,unsigned char remote_IRR,
				unsigned char trigger_mode,unsigned char mask,
				unsigned char destination)
{
	
	unsigned long pointer = (unsigned long)&ioapic_redir_table;

	unsigned int *data = (unsigned int*) pointer;

	ioapic_redir_table[n].vector 		= vector;
	ioapic_redir_table[n].delv_mode 	= delv_mode;
	ioapic_redir_table[n].dest_mode 	= dest_mode;
	ioapic_redir_table[n].delv_status	= delv_status;
	ioapic_redir_table[n].pin_polarity	= pin_polarity;
	ioapic_redir_table[n].remote_IRR	= remote_IRR;
	ioapic_redir_table[n].trigger_mode	= trigger_mode;
	ioapic_redir_table[n].mask		= mask;
	ioapic_redir_table[n].destination	= destination;

	write_ioapic_register(IO_APIC_REDIR_TBL(n),*(unsigned int*)(data + (n *2)));
	write_ioapic_register(IO_APIC_REDIR_TBL(n) + 1,*(unsigned int*)(data + (n *2) + 1));


	return 0;
}


int ioapic_cofiguration(int count)
{

	for(int n = 0; n < count; n++) {
	
	set_ioapic_redir_table(n/*IRQn*/,0x40 + n/*vector*/,\
	0/*Delivery Mode*/,0/*Destination Mode*/,0/*RO*/,0/*Interrupt Input Pin Polarity*/,\
	0/*RO*/,0,1/* masked*/, localId);

	}

	return 0;

}

int setup_ioapic()
{
	unsigned long io_apic_base_addr;
	// Mapear o IO APIC BASE
	mm_mp( IO_APIC_BASE, &io_apic_base_addr, 0x1000/*4KiB*/,0);

	ioapic_base =  io_apic_base_addr;

    // maximum redirection entry
    int count  = ((read_ioapic_register(IO_APIC_VER) >> 16) & 0xff) + 1;

	write_ioapic_register(IO_APIC_ID, localId);

	ioapic_cofiguration(count);
	
	return 0;
}

// timer

void apic_initial_count_timer(int value)
{
	local_apic_write_command( APIC_INITIAL_COUNT_TIMER, value);

}
int apic_timer()
{
	// Timer interrupt vector, to enable timer interrupts
	unsigned int val = APIC_CONFIG_DATA_LVT(0/*One-shot*/,1/*Masked*/,null,null,0,null,0x20/*Vetor*/);
    local_apic_write_command( APIC_LVT_TIMER, val);
	
	//Divide Configuration Register, to divide by 128
	local_apic_write_command( APIC_DIVIDE_TIMER, 0xA);

	// Initial Count Register
	local_apic_write_command( APIC_INITIAL_COUNT_TIMER, 123456);
	
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

