#include <mm.h>
#include <msr.h>
#include <io.h>
#include <apic.h>


#include <stdio.h>


#define null 0

unsigned long lapicbase = 0;


unsigned int getapicbase()
{
	unsigned int a,d;
	
	getmsr(IA32_APIC_BASE_MSR,&a,&d);
	
	return a;
}

int enableapic(unsigned int apicbase)
{
	unsigned int a = (apicbase &0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;
	unsigned int d = 0;
	
	setmsr(IA32_APIC_BASE_MSR,a,d);
	
	return 0;
	
	
}

void apic_eoi_register()
{
	*(unsigned int*)(lapicbase + APIC_EOI) = 0;
}


int setup_apic() {



	// mapear o endereço do Local APIC
	
	unsigned long local_apic_virtual_addr;
	
	mm_mp(IA32_LOCAL_APIC_BASE_ADDR, &local_apic_virtual_addr, 0x200000/*2MiB*/,0);
	
	lapicbase = local_apic_virtual_addr;
	unsigned int val;


	//Legacy PIC mask all off

	// Envia ICW1 reset
	outportb(0x20,0x11);	// reset PIC 1
	outportb(0xA0,0x11);	// reset PIC 2

	// Envia ICW2 start novo PIC 1 e 2
	outportb(0x21,0x20);	// PIC 1 localiza no IDT 39-32 
	outportb(0xA1,0x28);	// PIC 2 localiza no IDT 47-40

	// Envia ICW3
	outportb(0x21,0x04);	// IRQ2 conexao em cascata com o PIC 2
	outportb(0xA1,0x02);
	
	// Envia ICW4
	outportb(0x21,0x01);
	outportb(0xA1,0x01);

	// OCW1
	// Desabilita todas as interrupcoes
	outportb(0x21,0xFF);
	outportb(0xA1,0xFF);

	// Destination Format Register (DFR)
	*(volatile unsigned int*)(lapicbase + APIC_DFR) = 0xFFFFFFFF; // Value after reset

	// Logical Destination Register (LDR)
	val = *(volatile unsigned int*)(lapicbase + APIC_LDR) &0x00FFFFFF;
	val |= 1;
	*(volatile unsigned int*)(lapicbase + APIC_LDR) =  val;
	
	// Local APIC ID Register
	*(volatile unsigned int*)(lapicbase + APIC_ID) = APIC_ID_0 << 24;
	
	//Task Priority Register (TPR), to inhibit softint delivery
	*(volatile unsigned int*)(lapicbase + APIC_TPR) = 0; //0x20;
	
	// Timer interrupt vector, to disable timer interrupts
	*(volatile unsigned int*)(lapicbase + APIC_LVT_TIMER) = APIC_CONFIG_DATA_LVT(0,1/*Masked*/,0,0,0,null,0x20/*Vetor*/);
	
	// Performance counter interrupt, to disable performance counter interrupts
	*(volatile unsigned int*)(lapicbase + APIC_LVT_PERFORMANCE) = APIC_CONFIG_DATA_LVT(null,0/*No Masked*/,0,0,0,0,0x21/*Vetor*/);
	
	// Local interrupt 0, to enable normal external interrupts, Trigger Mode = Level
	*(volatile unsigned int*)(lapicbase + APIC_LVT_LINT0) = APIC_CONFIG_DATA_LVT(null,1/*Masked*/,null,null,1,7,0x22/*Vetor*/);
	
	// Local interrupt 1, to enable normal NMI processing
	*(volatile unsigned int*)(lapicbase + APIC_LVT_LINT1) = APIC_CONFIG_DATA_LVT(null,1/*Masked*/,null,null,0,4,0x23/*Vetor*/);
	
	// Error interrupt, to disable error interrupts
	*(volatile unsigned int*)(lapicbase + APIC_LVT_ERROR) = APIC_CONFIG_DATA_LVT(null,1/* Masked*/,null,null,null,0,0x24/*Vetor*/);
	
	// Spurious interrupt Vector Register, to enable the APIC and set
	// spurious vector to 15
	*(volatile unsigned int*)(lapicbase + APIC_S_INT_VECTOR) = 0x10f;
	
	
	// Global enable
	enableapic(getapicbase());
	
	printf("APIC Global enable 0x%x\n",getapicbase());
	
	apic_timer(lapicbase);
	
	
	return 0;
	
}

void apic_initial_count_timer(int value)
{
	*(volatile unsigned int*)(lapicbase + APIC_INITIAL_COUNT_TIMER ) = value;

}
int apic_timer(unsigned long _lapicbase)
{
	// Timer interrupt vector, to enable timer interrupts
	*(volatile unsigned int*)(_lapicbase + APIC_LVT_TIMER) = APIC_CONFIG_DATA_LVT\
	(0/*One-shot*/,1/*Masked*/,null,null,0,null,0x20/*Vetor*/);
	
	//Divide Configuration Register, to divide by 128
	*(volatile unsigned int*)(_lapicbase + APIC_DIVIDE_TIMER) = 0xA;

	// Initial Count Register
	*(volatile unsigned int*)(_lapicbase + APIC_INITIAL_COUNT_TIMER) = 123456;
	
	return 0;
	
}

void apic_timer_umasked(){

	unsigned int data = *(volatile unsigned int*)(lapicbase + APIC_LVT_TIMER);

	*(volatile unsigned int*)(lapicbase + APIC_LVT_TIMER) =  data &0xFFFEFFFF;

	//Divide Configuration Register, to divide by 128 0xA divide by 32 0x8
	*(volatile unsigned int*)(lapicbase + APIC_DIVIDE_TIMER) = 0xA;

	// Initial Count Register
	*(volatile unsigned int*)(lapicbase + APIC_INITIAL_COUNT_TIMER) = 12345;
	
}

void apic_timer_masked(){

	unsigned int data = *(volatile unsigned int*)(lapicbase + APIC_LVT_TIMER);
	*(volatile unsigned int*)(lapicbase + APIC_LVT_TIMER) = (data | 1 << 16);

}



// I/O APIC
IOAPIC_REDIR_TBL ioapic_redir_table[24];

unsigned long ioapic_base;

unsigned int read_ioapic_register(unsigned long _ioapic_base, int reg) {

	*(volatile unsigned char*)(_ioapic_base + IO_APIC_IND) = reg;
	
	return ( *(volatile unsigned int*)(_ioapic_base + IO_APIC_DAT));
}

void write_ioapic_register(unsigned long _ioapic_base, int reg, unsigned int val) {

	*(volatile unsigned char*)(_ioapic_base + IO_APIC_IND) = reg;

	*(volatile unsigned int*)(_ioapic_base + IO_APIC_DAT) = val;
}

int ioapic_masked(int n)
{

	unsigned long pointer = (unsigned long)&ioapic_redir_table;

	ioapic_redir_table[n].mask = 1;
	
	unsigned int *data = (unsigned int*) pointer;
	write_ioapic_register(ioapic_base,IO_APIC_REDIR_TBL(n),*(unsigned int*)(data + (n *2)));


	return 0;
}

int ioapic_umasked(int n)
{

	unsigned long pointer = (unsigned long)&ioapic_redir_table;

	ioapic_redir_table[n].mask = 0;
	
	unsigned int *data = (unsigned int*) pointer;
	write_ioapic_register(ioapic_base,IO_APIC_REDIR_TBL(n),*(unsigned int*)(data + (n *2)));


	return 0;
}


int set_ioapic_redir_table(	unsigned long _ioapic_base,
				int n,unsigned char vector,unsigned char delv_mode,
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

	write_ioapic_register(_ioapic_base,IO_APIC_REDIR_TBL(n),*(unsigned int*)(data + (n *2)));
	write_ioapic_register(_ioapic_base,IO_APIC_REDIR_TBL(n) + 1,*(unsigned int*)(data + (n *2) + 1));


	return 0;
}


int ioapic_cofiguration(unsigned long _ioapic_base)
{

	for(int n = 0; n < 24; n++) {
	
	set_ioapic_redir_table(_ioapic_base,n/*IRQn*/,0x40 + n/*vector*/,\
	0/*Delivery Mode*/,0/*Destination Mode*/,0/*RO*/,0/*Interrupt Input Pin Polarity*/,\
	0/*RO*/,0,1/* masked*/,APIC_ID_0);

	}

	return 0;

}

int setup_ioapic()
{
	unsigned long io_apic_base_addr;

	// Mapear o IO APIC BASE
	mm_mp( IO_APIC_BASE, &io_apic_base_addr, 0x1000/*4KiB*/,0);

	ioapic_base =  io_apic_base_addr;

	write_ioapic_register(ioapic_base,IO_APIC_ID,APIC_ID_0);

	ioapic_cofiguration(ioapic_base);
	
	return 0;
}

