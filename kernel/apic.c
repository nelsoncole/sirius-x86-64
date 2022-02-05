#include <mm.h>
#include <msr.h>
#include <io.h>
#include <apic.h>


#include <stdio.h>


#define null 0

unsigned int localId;
unsigned long lapicbase;

unsigned int local_apic_read_command(unsigned short addr){
    return *( (volatile unsigned int *)(lapicbase + addr));
}

void local_apic_write_command(unsigned short addr,unsigned int val){
    *( (volatile unsigned int *)(lapicbase + addr)) = val;
}


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
	local_apic_write_command( APIC_EOI, 0);
}


int setup_apic() {

	// mapear o endereço do Local APIC
	unsigned long local_apic_virtual_addr;
	
	mm_mp(IA32_LOCAL_APIC_BASE_ADDR, &local_apic_virtual_addr, 0x200000/*2MiB*/,0);
	
	lapicbase = local_apic_virtual_addr;

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
	*(volatile unsigned int*)(lapicbase + APIC_DFR) = 0xFFFFFFFF; // Value after reset, flat mode

	// Logical Destination Register (LDR)
	*(volatile unsigned int*)(lapicbase + APIC_LDR) = 0x01000000; // All cpus use logical id 1
	
	// Local APIC ID Register
	//*(volatile unsigned int*)(lapicbase + APIC_ID) = APIC_ID_0 << 24;

    localId = local_apic_get_id();
	
	//Task Priority Register (TPR), to inhibit softint delivery
	*(volatile unsigned int*)(lapicbase + APIC_TPR) = 0; //0x20;
	
	// Timer interrupt vector, to disable timer interrupts
	*(volatile unsigned int*)(lapicbase + APIC_LVT_TIMER) = APIC_CONFIG_DATA_LVT(0,1/*Masked*/,0,0,0,null,0x20/*Vetor*/);
	
	// Performance counter interrupt, to disable performance counter interrupts
	*(volatile unsigned int*)(lapicbase + APIC_LVT_PERFORMANCE) = APIC_CONFIG_DATA_LVT(null,1/*No Masked*/,0,0,0,0,0x21/*Vetor*/);
	
	// Local interrupt 0, to enable normal external interrupts, Trigger Mode = Level
	*(volatile unsigned int*)(lapicbase + APIC_LVT_LINT0) = APIC_CONFIG_DATA_LVT(null,1/*Masked*/,null,null,1,7,0x22/*Vetor*/);
	
	// Local interrupt 1, to enable normal NMI processing
	*(volatile unsigned int*)(lapicbase + APIC_LVT_LINT1) = APIC_CONFIG_DATA_LVT(null,1/*Masked*/,null,null,0,4,0x23/*Vetor*/);
	
	// Error interrupt, to disable error interrupts
	*(volatile unsigned int*)(lapicbase + APIC_LVT_ERROR) = APIC_CONFIG_DATA_LVT(null,1/* Masked*/,null,null,null,0,0x24/*Vetor*/);
	
	// Spurious interrupt Vector Register, to enable the APIC and set
	// spurious vector to 255
	*(volatile unsigned int*)(lapicbase + APIC_S_INT_VECTOR) = 0x1ff;
	
	
	// Global enable
	enableapic(getapicbase());
	
	printf("APIC Global enable 0x%x\n",getapicbase());
	
	apic_timer(lapicbase);
	
	
	return 0;
	
}

unsigned int local_apic_get_id()
{
    return (local_apic_read_command(LAPIC_ID) >> 24) &0xFF;
}

void local_apic_send_init(unsigned int apic_id)
{
    local_apic_write_command(LAPIC_ICRHI, apic_id << 24);

    local_apic_write_command(LAPIC_ICRLO,
    ICR_INIT | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    while (local_apic_read_command(LAPIC_ICRLO) & ICR_SEND_PENDING);
}

void local_apic_send_startup(unsigned int apic_id, unsigned int vector)
{
    local_apic_write_command(LAPIC_ICRHI, apic_id << 24);

    local_apic_write_command(LAPIC_ICRLO,
    vector | ICR_STARTUP | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    while (local_apic_read_command(LAPIC_ICRLO) & ICR_SEND_PENDING);
}
