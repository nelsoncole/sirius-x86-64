#include <acpi.h>
#include <hpet.h>
#include <mm.h>
#include <io.h>


#include <stdio.h>
#include <string.h>

#include <irq.h>
#include <rtc.h>



// 1000hertz = 1Khertz = 1000fs = 1ms
// 1s = 1000ms = 1000000000ns
// 1ms = 1000000ns
//
// Nanohertz 	= 10^15
// Microhertz 	= 10^12
// Milihertz 	= 1000000000
// Hertz	= 1000000
// Kilohertz	= 10000
// Megahertz	= 1
// 05F5E100h  (10 ^ 8 femptossegundos = 100 nanossegundos)
// Calculate HPET frequency (f = 10^15 / period)
//
// Tempo em milessegundos

HPET_REGISTER *hpet_register;
unsigned long period;
static unsigned long timer_tick[32];

static unsigned long  hpet_freq(unsigned long period, unsigned int hertz)
{
	unsigned long long time = 1000000000000000; // 10^15 femtossegundos por segundo
	
	period = period *hertz;
	time += period >> 1;
	time = time / period;
	
	return time;
}

static void hpet_handler0()
{	unsigned long *count = timer_tick;

	unsigned long status = hpet_register->g_interrupt_status_register;
	
	hpet_register->g_interrupt_status_register = status | 1;
	
	if(*count) *count -= 1;
	
		
}

static void hpet_handler1()
{
	unsigned long status = hpet_register->g_interrupt_status_register;
	
	hpet_register->g_interrupt_status_register = status | 2;
	//rtc_handler();
	
	
	//printf("IRQ8 ");	
}

void setup_hpet(unsigned int hertz) {

	unsigned long virt;
	*timer_tick = 0;
	printf("HPET_Setup ... \\\\ \n");
	for(;;);
	
	cli();
	fnvetors_handler[2] = &hpet_handler0;
	fnvetors_handler[8] = &hpet_handler1;
	//return; // deixa o pit
	
	 
	hpet = (ACPI_TABLE_HPET*) acpi_probe(rsdt, xsdt, "HPET");
	
	if(hpet){

		
		printf("HPET was found, ");
		printf("Address %x, minimum_tick %d\n",hpet->address.address,hpet->minimum_tick);
		// Converter o endereço fisico para virtual
		
		
		mm_mp( hpet->address.address, &virt, 0x1000/*4KiB*/,0);
		
		hpet_register = (HPET_REGISTER *) virt;
		
		period = hpet_register->g_cap_and_id_register >> 32 &0xFFFFFFFF;
		
		// Timer 0 periodic mode
		hpet_register->g_config_register = HPET_MASK_ENABLE_CNF | HPET_MASK_LEG_RT_CNF;
		
		hpet_register->main_Counter_ralue_register = 0;
		
		// Timer0
		hpet_register->timer[0].config_and_cap_register = (2 << HPET_SHIFT_Tn_INT_ROUTE_CNF)\
		| HPET_Tn_INT_ENB_CNF | HPET_Tn_TYPE_CNF | HPET_Tn_VAL_SET_CNF;
		
		// TODO: a configuração  deste bit trava no Oracle Virtual Box
		//hpet_register->timer[0].config_and_cap_register |= HPET_Tn_INT_TYPE_CNF;  
		hpet_register->timer[0].cmp_value_register = hpet_freq(period, hertz);
		
		
		// Timer1
		// 1Hz = 1s
		hpet_register->timer[1].config_and_cap_register = (8 << HPET_SHIFT_Tn_INT_ROUTE_CNF)\
		| HPET_Tn_INT_ENB_CNF | HPET_Tn_TYPE_CNF | HPET_Tn_VAL_SET_CNF;
		
		// TODO: a configuração  deste bit trava no Oracle Virtual Box
		//hpet_register->timer[1].config_and_cap_register |= HPET_Tn_INT_TYPE_CNF;  
		hpet_register->timer[1].cmp_value_register = hpet_freq(period, 1);
		

	
	
	}else {
	
		printf("HPET not found, the chipset must be old...\n");
	}
	

}


void hpet_sleep(unsigned long ms, int timer)
{

	unsigned long *count = timer_tick;

	switch(timer) {
		case 0:
			*count = ms/10;
			while(*count) __asm__ __volatile__("hlt");
		break;
	}
	
}


