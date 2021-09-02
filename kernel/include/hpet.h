#ifndef __HPET_H
#define __HPET_H

#define HPET_SHIFT_REV_ID >> 0
#define HPET_SHIFT_NUM_TIM_CAP >> 8
#define HPET_SHIFT_COUNT_SIZE_CAP >> 13
#define HPET_SHIFT_LEG_RT_CAP >> 15
#define HPET_SHIFT_VENDOR_ID >> 16
#define HPET_SHIFT_COUNTER_CLK_PERIOD >> 32

#define HPET_MASK_ENABLE_CNF 1
#define HPET_MASK_LEG_RT_CNF 2

#define HPET_MASK_T0_INT_STS 1
#define HPET_MASK_T1_INT_STS 2
#define HPET_MASK_T2_INT_STS 4

#define HPET_Tn_INT_TYPE_CNF 		1 << 1
#define HPET_Tn_INT_ENB_CNF  		1 << 2
#define HPET_Tn_TYPE_CNF		1 << 3
#define HPET_Tn_PER_INT_CAP		1 << 4
#define HPET_Tn_SIZE_CAP		1 << 5
#define HPET_Tn_VAL_SET_CNF		1 << 6
#define HPET_Tn_32MODE_CNF		1 << 8
#define HPET_SHIFT_Tn_INT_ROUTE_CNF	9
#define HPET_Tn_FSB_EN_CNF		1 << 14
#define HPET_Tn_FSB_INT_DEL_CAP 	1 << 15
#define HPET_SHIFT_Tn_INT_ROUTE_CAP	32

typedef struct _HPET_TIMER_REGISTER {

	unsigned long long config_and_cap_register;
	unsigned long long cmp_value_register;
	unsigned long long FSB_interrupt_route_register;
	unsigned char rsv[8];
	
}__attribute__ ((packed)) HPET_TIMER_REGISTER;


typedef struct _HPET_REGISTER {

	unsigned long long g_cap_and_id_register;
	unsigned char rsv1[8];
	unsigned long long g_config_register;
	unsigned char rsv2[8];
	unsigned long long g_interrupt_status_register;
	unsigned char rsv3[0xF0 - 0x28];
	unsigned long long main_Counter_ralue_register;
	unsigned char rsv4[8];
	
	HPET_TIMER_REGISTER timer[3];
	
	unsigned char rsv[0x400 - 0x160];

}__attribute__ ((packed)) HPET_REGISTER;

void setup_hpet(unsigned int hertz);
void hpet_sleep(unsigned long ms, int timer);

#endif
