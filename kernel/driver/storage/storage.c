#include <storage.h>

ata_t ata[32];

PCI_COFIG_SPACE *ata_pci = NULL;
extern unsigned int ata_record_dev,ata_record_channel;

static int ata_pci_configuration_space(int bus,int dev,int fun)
{
    	unsigned int data;
  
    	// Indentification Device
    	data  = read_pci_config_addr(bus,dev,fun,0);
    
    	ata_pci->vid = data &0xffff;
    	ata_pci->did = data >> 16 &0xffff;
    
    	// Classe code, programming interface, revision id
    	data  = read_pci_config_addr(bus,dev,fun,8);
    
    	ata_pci->clss      	= data  >> 24 &0xff;
    	ata_pci->subclss   	= data  >> 16 &0xff;
    	ata_pci->rid 		= data &0xff;

	if(ata_pci->clss == 1 && ata_pci->subclss == 1) {
		ata->device = ATA_IDE_CONTROLLER;

	} else if(ata_pci->clss == 1 && ata_pci->subclss == 4) {
		ata->device = ATA_RAID_CONTROLLER;
		printf("panic: RAID Controller\n");
		for(;;);

	} else if(ata_pci->clss == 1 && ata_pci->subclss == 6) {

		ata->device = ATA_AHCI_CONTROLLER;
	} else {
        	printf("IDE Controller not found!\n");
        	printf("RAID Controller not found!\n");
        	printf("AHCI Controller not found!\n");
        	for(;;);
	}



	    // Habilitar interrupcao (INTx#)
        data  = read_pci_config_addr(bus,dev,fun,4);
        write_pci_config_addr(bus,dev,fun,4,data & ~0x400);

	if(ata_pci->clss == 1 && ata_pci->subclss == 6){ 
		// AHCI

		data  = read_pci_config_addr(bus,dev,fun,8);
        	if(data &0x8000) {    
        		// Bus Master Enable
        		data  = read_pci_config_addr(bus,dev,fun,4);
        		write_pci_config_addr(bus,dev,fun,4,data | 0x4);

        	} 
        	
		/* MAP—Address Map Register (SATA)
        	7:6
        	00b = IDE mode
        	01b = AHCI mode
        	10b = RAID mode
        	11b = Reserved */

        	// AHCI mode
        	/*data  = read_pci_config_addr(bus,dev,fun,0x90);
        	data &= ~0x80;
        	data |=  0x40; 
        	write_pci_config_addr(bus,dev,fun,0x90,data);*/	
	}

	if(ata_pci->clss == 1 && ata_pci->subclss == 1){ 
		// IDE
       		// IDE Decode Enable
       		data  = read_pci_config_addr(bus,dev,fun,0x40);
       		write_pci_config_addr(bus,dev,fun,0x40,data | 0x80008000);

		data  = read_pci_config_addr(bus,dev,fun,8);
        	if(data &0x8000) {    
        		// Bus Master Enable
        		data  = read_pci_config_addr(bus,dev,fun,4);
        		write_pci_config_addr(bus,dev,fun,4,data | 0x4);

        	} 





		//Synchronous DMA Control Register
		// Enable UDMA
		data = read_pci_config_addr(bus,dev,fun,0x48);	
		write_pci_config_addr(bus,dev,fun,0x48,data | 0xf);


	}


	// Interrupt
    	data  = read_pci_config_addr(bus,dev,fun,0x3C);
    	ata_pci->interrupt_line  = data &0xff;
    	ata_pci->interrupt_pin   = data >> 8 &0xff;



	
    	ata_pci->bar0    =read_pci_config_addr(bus,dev,fun,0x10);
    	ata_pci->bar1    =read_pci_config_addr(bus,dev,fun,0x14);
    	ata_pci->bar2    =read_pci_config_addr(bus,dev,fun,0x18);
    	ata_pci->bar3    =read_pci_config_addr(bus,dev,fun,0x1C);
    	ata_pci->bar4    =read_pci_config_addr(bus,dev,fun,0x20);
    	ata_pci->bar5    =read_pci_config_addr(bus,dev,fun,0x24);

	
    	return (0);

}

int ata_initialize()
{

	printf("Initializing the ATA Controller:\n");

	memset(ata,0,sizeof(ata_t)*32);

	int p;
	unsigned short *buffer = (unsigned short*) malloc(0x1000);
	ata_pci = (PCI_COFIG_SPACE *) malloc(0x1000);
	
    unsigned int data = pci_scan_class(PCI_CALSS_MASS_STORAGE);

    if(data  == -1) {
    	printf("PIC: Massa Storage Controller not found!\n");
    	for(;;);
    }


    	data = ata_pci_configuration_space(data  >>  24 &0xff,data  >> 16  &0xff,data &0xffff);

    	if(data) {
        	printf("PANIC: IDE/AHCI PCI Configuration Space\n");
		for(;;);       
    	}



	// Initialize base address // IDE legacy
	ATA_BAR0 = (ata_pci->bar0 & ~7) + ATA_IDE_BAR0 * ( !ata_pci->bar0);
    	ATA_BAR1 = (ata_pci->bar1 & ~3) + ATA_IDE_BAR1 * ( !ata_pci->bar1);       
    	ATA_BAR2 = (ata_pci->bar2 & ~7) + ATA_IDE_BAR2 * ( !ata_pci->bar2);
    	ATA_BAR3 = (ata_pci->bar3 & ~3) + ATA_IDE_BAR3 * ( !ata_pci->bar3);
    	ATA_BAR4 = (ata_pci->bar4 & ~0x7) + ATA_IDE_BAR4 * ( !ata_pci->bar4);
    	ATA_BAR5 = (ata_pci->bar5 & ~0xf) + ATA_IDE_BAR5 * ( !ata_pci->bar5);



	switch(ata->device) {
    		
    		case ATA_IDE_CONTROLLER:
    		
    			printf("IDE Controller:\n");
			// Install  bus, ports IDE
    			ata_bus_install(0,14/*IDE_IRQ14*/,0,ATA_PRIMARY,ATA_BAR0,ATA_BAR1,ATA_BAR4 + 0);
    			ata_bus_install(1,14/*IDE_IRQ14*/,1,ATA_PRIMARY,ATA_BAR0,ATA_BAR1,ATA_BAR4 + 0);
    			ata_bus_install(2,15/*IDE_IRQ15*/,0,ATA_SECONDARY,ATA_BAR2,ATA_BAR3,ATA_BAR4 + 8);
    			ata_bus_install(3,15/*IDE_IRQ15*/,1,ATA_SECONDARY,ATA_BAR2,ATA_BAR3,ATA_BAR4 + 8);

			for(p = 0; p < 4; p++)
			ata_identify_device(p,buffer);
	
			ata_record_dev = ata_record_channel = -1;
		break;
		case ATA_AHCI_CONTROLLER:
		
			printf("AHCI Controller:\n");

    			// FIXME: se paginação é habilitada, aqui vamos mapear o BAR5
    			unsigned long virt;
    			mm_mp(ATA_BAR5, &virt, 0x200000/*2MiB*/,0);
    			ahci_initialize(virt);

		break;

		default:
			printf("IDE or AHCI controller not found");
			for(;;);
		break;
	}

    	

    	return (0);
                                                                                                 
}

int read_sector(int dev,unsigned count,unsigned long long addr,void *buffer)
{
	int i;
	unsigned char *buf = buffer;

	switch(ata->device) {

		case ATA_IDE_CONTROLLER:
			for(i = 0; i <count;i++) {
				if(ata_read_sector(dev,1,addr + i,buf + (ata[dev].bps * i)) )
					return (-1);
			}
		break;
		case ATA_AHCI_CONTROLLER:
			
			if( sata_read_sector(dev,addr, count, buf) ) return (-1);	
		break;

		default:
			return(-1);
		break;
	}


	return (0);
}

int write_sector(int dev,unsigned count,unsigned long long addr,void *buffer)
{
	int i;
	unsigned char *buf = buffer;

	switch(ata->device) {

		case ATA_IDE_CONTROLLER:
			for(i = 0; i <count;i++) {
				if(ata_write_sector(dev,1,addr + i,buf + (ata[dev].bps * i)) )
					return -1;
			}
		break;
		case ATA_AHCI_CONTROLLER:

			if( sata_write_sector(dev,addr, count, buf) ) return (-1);	
		break;

		default:
			return(-1);
		break;
	}


	return 0;
}




