#include <pci.h>
#include <stdio.h>


static int lpc_pci_configuration_space(int bus,int dev,int fun)
{
  
    	return 0;
}


void setup_lpc()
{

	unsigned int data = pci_scan_bcc_scc(0x6,0x1);
    	if(data  == -1)	{
    		puts("PCI PANIC: LPC Controller not found!\n");
    		return; // Abortar
    	}

	printf("LPC found\n");
	
    	lpc_pci_configuration_space(data  >>  24 &0xff,data  >> 16  &0xff,data &0xffff);
    	



}
