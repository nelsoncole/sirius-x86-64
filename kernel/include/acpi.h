#ifndef __ACPI_H
#define __ACPI_H


// BIOS Data Area (BDA)
#define BDA 0x400
#define EBDA (BDA + 0xE)


typedef struct _ACPI_GENERIC_ADDRESS {
    
    	unsigned char space_id;    	// 0 - system memory, 1 - system I/O
    	unsigned char bit_width;	// Size in bits of given register
    	unsigned char bit_offset;	// Bit offset within the register 
    	unsigned char access_width;  	// Minimum Access size (ACPI 3.0)
    	unsigned long long address;	// 64-bit address of struct or register

}__attribute__ ((packed)) ACPI_GENERIC_ADDRESS;


typedef struct _ACPI_TABLE_RSDP {

	// ACPI Version 1.0
	char signature[8];
   	unsigned char checksum;
   	char oem_id[6];
   	unsigned char revision;
   	unsigned int rsdt_addr;
   	
   	// ACPI Version 2.0
   	unsigned int length;
 	unsigned long long xsdt_addr;
 	unsigned char extended_check_sum;
 	unsigned char reserved[3];
   	
}__attribute__ ((packed)) ACPI_TABLE_RSDP;

typedef struct _RSDT {

	// ACPI Version 1.0
	char signature[4];
	unsigned int length;
	unsigned char revision;
   	unsigned char checksum;
   	char oem_id[6];
   	char oem_table_id[8];
    	unsigned int oem_revision;
    	unsigned int creator_id;
    	unsigned int creator_revision;
    	
    	unsigned int *entry; // 4*n
   	
   	
}__attribute__ ((packed)) ACPI_TABLE_RSDT;


typedef struct _ACPI_TABLE_XSDT {

	// ACPI Version 1.0
	char signature[4];
	unsigned int length;
	unsigned char revision;
   	unsigned char checksum;
   	char oem_id[6];
   	char oem_table_id[8];
    	unsigned int oem_revision;
    	unsigned int creator_id;
    	unsigned int creator_revision;
    	
    	unsigned long long *entry; // 8*n
   	
   	
}__attribute__ ((packed)) ACPI_TABLE_XSDT;

typedef struct _ACPI_TABLE_FADT {
	// Header
    	char signature[4];
    	unsigned int length;
    	unsigned char major_version;
    	unsigned char checksum;
    	char oem_id[6];
    	char oem_table_id[8];
    	unsigned int oem_revision;
    	unsigned int creator_id;
    	unsigned int creator_revision;
    	unsigned int FIRMWARE_CTRL;
    	unsigned int  DSDT;
   	unsigned char unneded2[48 - 44];
   	unsigned int  SMI_CMD;
   	unsigned char ACPI_ENABLE;
  	unsigned char ACPI_DISABLE;
   	unsigned char unneded3[64 - 54];
   	unsigned int  PM1a_CNT_BLK;
   	unsigned int  PM1b_CNT_BLK;
   	
  	unsigned int		Pm2ControlBlock;    /* 32-bit port address of Power Mgt 2 Control Reg Blk */
   	unsigned int		PmTimerBlock;       /* 32-bit port address of Power Mgt Timer Ctrl Reg Blk */
    	unsigned int		Gpe0Block;          /* 32-bit port address of General Purpose Event 0 Reg Blk */
   	unsigned int		Gpe1Block;          /* 32-bit port address of General Purpose Event 1 Reg Blk */
    	unsigned char		Pm1EventLength;     /* Byte Length of ports at Pm1xEventBlock */
   	unsigned char		Pm1ControlLength;   /* Byte Length of ports at Pm1xControlBlock */
    	unsigned char		Pm2ControlLength;   /* Byte Length of ports at Pm2ControlBlock */
    	unsigned char		PmTimerLength;      /* Byte Length of ports at PmTimerBlock */
    	unsigned char		Gpe0BlockLength;    /* Byte Length of ports at Gpe0Block */
    	unsigned char		Gpe1BlockLength;    /* Byte Length of ports at Gpe1Block */
   	unsigned char		Gpe1Base;           /* Offset in GPE number space where GPE1 events start */
    	unsigned char		CstControl;         /* Support for the _CST object and C-States change notification */
    	unsigned short		C2Latency;          /* Worst case HW latency to enter/exit C2 state */
    	unsigned short		C3Latency;          /* Worst case HW latency to enter/exit C3 state */
    	unsigned short		FlushSize;          /* Processor memory cache line width, in bytes */
    	unsigned short		FlushStride;        /* Number of flush strides that need to be read */
    	unsigned char		DutyOffset;         /* Processor duty cycle index in processor P_CNT reg */
   	unsigned char		DutyWidth;          /* Processor duty cycle value bit width in P_CNT register */
    	unsigned char		DayAlarm;           /* Index to day-of-month alarm in RTC CMOS RAM */
   	unsigned char		MonthAlarm;         /* Index to month-of-year alarm in RTC CMOS RAM */
   	unsigned char		Century;            /* Index to century in RTC CMOS RAM */
    	unsigned short		BootFlags;          /* IA-PC Boot Architecture Flags (see below for individual flags) */
    	unsigned char		Reserved;           /* Reserved, must be zero */
    	unsigned int		Flags;              /* Miscellaneous flag bits (see below for individual flags) */
    	ACPI_GENERIC_ADDRESS  	ResetRegister;      /* 64-bit address of the Reset register */
   	unsigned char		ResetValue;         /* Value to write to the ResetRegister port to reset the system */
    	unsigned short		ArmBootFlags;       /* ARM-Specific Boot Flags (see below for individual flags) (ACPI 5.1) */
   	unsigned char		 MinorRevision;      /* FADT Minor Revision (ACPI 5.1) */
    	unsigned long long	XFacs;              /* 64-bit physical address of FACS */
    	unsigned long long	XDsdt;              /* 64-bit physical address of DSDT */
    	ACPI_GENERIC_ADDRESS   XPm1aEventBlock;    /* 64-bit Extended Power Mgt 1a Event Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XPm1bEventBlock;    /* 64-bit Extended Power Mgt 1b Event Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XPm1aControlBlock;  /* 64-bit Extended Power Mgt 1a Control Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XPm1bControlBlock;  /* 64-bit Extended Power Mgt 1b Control Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XPm2ControlBlock;   /* 64-bit Extended Power Mgt 2 Control Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XPmTimerBlock;      /* 64-bit Extended Power Mgt Timer Ctrl Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XGpe0Block;         /* 64-bit Extended General Purpose Event 0 Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	XGpe1Block;         /* 64-bit Extended General Purpose Event 1 Reg Blk address */
    	ACPI_GENERIC_ADDRESS  	SleepControl;       /* 64-bit Sleep Control register (ACPI 5.0) */
    	ACPI_GENERIC_ADDRESS  	SleepStatus;        /* 64-bit Sleep Status register (ACPI 5.0) */
    	unsigned long long	HypervisorId;       /* Hypervisor Vendor ID (ACPI 6.0) */
       
}__attribute__ ((packed)) ACPI_TABLE_FADT;


typedef struct _ACPI_TABLE_DSDT {
	// Header
    	char signature[4];
    	unsigned int length;
    	unsigned char rersion;
    	unsigned char checksum;
    	char oem_id[6];
    	char oem_table_id[8];
    	unsigned int oem_revision;
    	unsigned int creator_id;
    	unsigned int creator_revision;
    	unsigned int definition_block;
       
}__attribute__ ((packed)) ACPI_TABLE_DSDT;


typedef struct _ACPI_TABLE_HPET {
	// Header
    	char signature[4];
    	unsigned int length;
    	unsigned char rersion;
    	unsigned char checksum;
    	char oem_id[6];
    	char oem_table_id[8];
    	unsigned int oem_revision;
    	unsigned int creator_id;
    	unsigned int creator_revision;
    	// Hardware ID of Event Timer Block
    	unsigned char hardware_rev_id;
    	unsigned char comparator_count:5;
    	unsigned char counter_size:1;
   	unsigned char reserved:1;
    	unsigned char legacy_replacement:1;
    	unsigned short pci_vendor_id;
    	
    	// Address of Event Timer Block
    	ACPI_GENERIC_ADDRESS address;
    
    	unsigned char hpet_number;
    	unsigned short minimum_tick;
    	unsigned char page_protection;
    	   
}__attribute__ ((packed)) ACPI_TABLE_HPET;


typedef struct _ACPI_TABLE_MADT {
	// Header
    	char signature[4];
    	unsigned int length;
    	unsigned char rersion;
    	unsigned char checksum;
    	char oem_id[6];
    	char oem_table_id[8];
    	unsigned int oem_revision;
    	unsigned int creator_id;
    	unsigned int creator_revision;
    	
	//
	unsigned int lapic;
	unsigned int flag;
	unsigned int struc;
    	   
}__attribute__ ((packed)) ACPI_TABLE_MADT;

extern ACPI_TABLE_RSDP *rsdp;
extern ACPI_TABLE_RSDT *rsdt;
extern ACPI_TABLE_XSDT *xsdt;
extern ACPI_TABLE_FADT *fadt;
extern ACPI_TABLE_DSDT *dsdt;
extern ACPI_TABLE_HPET *hpet;

unsigned long acpi_probe(ACPI_TABLE_RSDT *_rsdt, ACPI_TABLE_XSDT *_xsdt, char *signature);
void setup_acpi();
void poweroff();
void reboot();

#endif
