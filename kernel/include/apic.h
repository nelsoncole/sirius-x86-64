#ifndef __APIC_H
#define __APIC_H
 
 
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800
 
 // Local APIC
 
#define IA32_LOCAL_APIC_BASE_ADDR   0xFEE00000
#define LAPIC_ID                    0x0020  // Local APIC ID
#define APIC_ID 			        0x20
#define APIC_TPR 			        0x80 
#define APIC_EOI			        0xb0
#define APIC_LDR			        0xd0
#define APIC_DFR			        0xe0
#define APIC_S_INT_VECTOR 		    0xf0
#define LAPIC_ICRLO                 0x0300  // Interrupt Command
#define LAPIC_ICRHI                 0x0310  // Interrupt Command [63:32]
#define APIC_LVT_TIMER 		        0x320
#define APIC_LVT_PERFORMANCE		0x340
#define APIC_LVT_LINT0			    0x350
#define APIC_LVT_LINT1			    0x360
#define APIC_LVT_ERROR			    0x370
#define APIC_INITIAL_COUNT_TIMER 	0x380
#define APIC_CURRENT_COUNT_TIMER	0x390
#define APIC_DIVIDE_TIMER		    0x3e0

// Interrupt Command Register
// Delivery Mode
#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

// Destination Mode
#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

// Delivery Status
#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

// Level
#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

// Trigger Mode
#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

// Destination Shorthand
#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000

#define APIC_CONFIG_DATA_LVT(TimerMode,Mask,TriggerMode,Remote,InterruptInput,DeliveryMode,Vector)(\
(((unsigned int)(TimerMode) &0x3 ) << 17) |\
(((unsigned int)(Mask) &0x1 ) << 16) |\
(((unsigned int)(TriggerMode) &0x1 ) << 15) |\
(((unsigned int)(Remote) &0x1 ) << 14) |\
(((unsigned int)(InterruptInput) &0x1 ) << 13) |\
(((unsigned int)(DeliveryMode) &0x7 ) << 8) |\
((unsigned int)(Vector) &0xff )\
)

extern unsigned int localId;

unsigned int local_apic_read_command(unsigned short addr);
void local_apic_write_command(unsigned short addr,unsigned int val);
unsigned int local_apic_get_id();
void apic_eoi_register();
void apic_initial_count_timer(int value);
int apic_timer();
int setup_apic();
void local_apic_send_init(unsigned int apic_id);
void local_apic_send_startup(unsigned int apic_id, unsigned int vector);

void apic_timer_umasked();
void apic_timer_masked();

/*  I/O APIC
 *
 *
 *
 */


#define IO_APIC_BASE 	0xFEC00000
#define IO_APIC_IND 	0
#define IO_APIC_DAT 	(IO_APIC_IND +0x10)
#define IO_APIC_EOIR	(IO_APIC_IND +0x40)

#define IO_APIC_ID 		0x0
#define IO_APIC_VER 	0x1
#define IO_APIC_ARB		0x2
#define IO_APIC_REDIR_TBL(n) 	(0x10 + 2 * n) //REDIR_TBL[0:23] 64 bits 


struct __IOAPIC_REDIR_TBL {
	unsigned long long vector	: 8;
	unsigned long long delv_mode 	: 3;
	unsigned long long dest_mode	: 1;
	unsigned long long delv_status	: 1;
	unsigned long long pin_polarity	: 1;
	unsigned long long remote_IRR	: 1;
	unsigned long long trigger_mode	: 1;
	unsigned long long mask		: 1;
	unsigned long long rsv		: 39;
	unsigned long long destination	: 8;
	
}__attribute__((packed));

int setup_ioapic();
int ioapic_masked(int n);
int ioapic_umasked(int n);

 
 
#endif






























