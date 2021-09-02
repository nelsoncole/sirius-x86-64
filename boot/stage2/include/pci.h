#ifndef __PCI_H
#define __PCI_H

#include <stdio.h>

#define MAX_BUS 256
#define MAX_DEV 32 // 32
#define MAX_FUN 8  // 8
#define PCI_PORT_ADDR 0xCF8
#define PCI_PORT_DATA 0xCFC



#define CONFIG_ADDR(bus,device,fn,offset)\
                       (\
                       (((unsigned int)(bus) &0xff) << 16)|\
                       (((unsigned int)(device) &0x3f) << 11)|\
                       (((unsigned int)(fn) &0x07) << 8)|\
                       ((unsigned int)(offset) &0xfc)|0x80000000)
                       
#include <io.h>
                       
typedef struct _PCI_CLASS_NAME{
  unsigned int classcode;
  char *name;
}__attribute__ ((packed)) PCI_CLASS_NAME;

typedef struct _PCI_COFIG_SPACE{
    	unsigned short	vid;
    	unsigned short	did;
   	unsigned short	cmd;
    	unsigned short	sts;
    	unsigned char	pif;
    	unsigned char	rid;
    	unsigned char	clss;
    	unsigned char	subclss;
    	unsigned char	primary_master_latency_timer;
    	unsigned char	header_type;
    	unsigned char	BIST;
    	unsigned int	bar0;
    	unsigned int	bar1; 
    	unsigned int	bar2;
    	unsigned int	bar3;
    	unsigned int	bar4;
    	unsigned int	bar5;
    	unsigned short	subsystem_vendor_id;
    	unsigned short	subsystem_id;  
    	unsigned int	capabilities_pointer;
    	unsigned char	interrupt_line;
    	unsigned char	interrupt_pin;

}__attribute__((packed)) PCI_COFIG_SPACE;


int read_pci_config_addr(int bus,int dev,int fun, int offset);
void write_pci_config_addr(int bus,int dev,int fun,int offset,unsigned int data);
unsigned int pci_read_config_dword(int bus,int dev,int fun,int offset);
void pci_write_config_dword(int bus,int dev,int fun,int offset,unsigned int data);
int pci_get_info(void *buffer,int max_bus);
unsigned int pci_scan_bcc(unsigned int bcc);
unsigned int pci_scan_class(int class);
unsigned int pci_scan_bcc_scc(unsigned int bcc,unsigned int scc);
unsigned int pci_scan_bcc_scc_prog(unsigned int bcc,unsigned int scc,unsigned int prog);
unsigned int pci_scan_vendor(unsigned short vendor);
unsigned int pci_check_vendor(int bus,int dev,int fun,unsigned short vendor);
int pci_size(int base, int mask);


#endif
