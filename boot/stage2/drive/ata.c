/*
 * File Name: ata.c
 *
 *
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, nelsoncole
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <storage.h>


PCI_COFIG_SPACE *ata_pci = NULL;
unsigned int ata_record_dev,ata_record_channel;

extern void ahci_initialize (unsigned int mmio /*ABAR*/);

void ata_wait(int p)
{
	if(p > 3) return;

	inportb(ata[p].cmd_block_base_addr + ATA_REG_STATUS);
	inportb(ata[p].cmd_block_base_addr + ATA_REG_STATUS);
	inportb(ata[p].cmd_block_base_addr + ATA_REG_STATUS);
	inportb(ata[p].cmd_block_base_addr + ATA_REG_STATUS);

	/*int i = 4;

	while(i--) __asm__ __volatile__("out %%al,$0x80"::);*/
}


int ata_status_read(int p)
{

   	return (inportb(ata[p].cmd_block_base_addr + ATA_REG_STATUS) &0xff);
    

}


int ata_wait_not_busy(int p)
{
    	while(ata_status_read(p) &ATA_SR_BSY)
    	if(ata_status_read(p) &ATA_SR_ERR)
    	return 1;

    	return 0;

}

int ata_wait_busy(int p)
{
    	while(!(ata_status_read(p) &ATA_SR_BSY))
    	if(ata_status_read(p) &ATA_SR_ERR)
    	return 1;

    	return 0;

}


int ata_wait_no_drq(int p)
{
    	while(ata_status_read(p) &ATA_SR_DRQ)
    	if(ata_status_read(p) &ATA_SR_ERR)
    	return 1;

    	return 0;
}

int ata_wait_drq(int p)
{
    
    	while(!(ata_status_read(p) &ATA_SR_DRQ))
    	if(ata_status_read(p) &ATA_SR_ERR)
    	return 1;

    	return 0;
}

void ata_cmd_write(int p,unsigned int cmd)
{
           
    	// no_busy      	
	ata_wait_not_busy(p);
	outportb(ata[p].cmd_block_base_addr + ATA_REG_CMD,cmd);
	ata_wait(p);  // Esperamos 400ns

}

void ata_soft_reset(int p)
{
    	unsigned char data =  inportb(ata[p].ctrl_block_base_addr);
    	outportb(ata[p].ctrl_block_base_addr, data | 0x4);
    	outportb(ata[p].ctrl_block_base_addr, data &0xfb);    
}

void ata_pio_read(int p,void *buffer,unsigned size)
{
	__asm__ __volatile__("\
	cld;\
        rep; insw"::"D"(buffer),
       	"d"(ata[p].cmd_block_base_addr + ATA_REG_DATA),"c"(size/2));


}


void ata_pio_write(int p,void *buffer,unsigned size)
{
	__asm__ __volatile__("\
	cld;\
      	rep; outsw"::"S"(buffer),
	"d"(ata[p].cmd_block_base_addr + ATA_REG_DATA),"c"(size/2));
}


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
static void ata_bus_install(int p,int irqn,int dev_num,int channel,\
int cmd_block_base_addr,int ctrl_block_base_addr,int bus_master_base_addr)
{

    	ata[p].irqn = irqn;
    	ata[p].dev_num = dev_num;
    	ata[p].channel = channel;
    	ata[p].cmd_block_base_addr = cmd_block_base_addr;
    	ata[p].ctrl_block_base_addr = ctrl_block_base_addr;
    	ata[p].bus_master_base_addr = bus_master_base_addr;


}

static int detect_devtype (int p)
{
	int spin,st;
	//ata_soft_reset(ctrl->dev_ctl);
	ata_soft_reset(p);

	ata_wait_not_busy(p);

	outportb(ata[p].cmd_block_base_addr + ATA_REG_SECCOUNT,0);	// Sector Count 7:0
	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA0,0);        	// LBA 7-0   
	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA1,0);        	// LBA 15-8
	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA2,0);        	// LBA 23-16

    
    	// Select device,
    	outportb(ata[p].cmd_block_base_addr + ATA_REG_DEVSEL,0xA0| ata[p].dev_num << 4);
	ata_wait(p);

	ata_wait_not_busy(p);

	ata_cmd_write(p,ATA_CMD_IDENTIFY_DEVICE);
	ata_wait(p);
	

	spin = 1000000;
	while(spin--) { 
		// Ignora bit de erro
		if(!(ata_status_read(p)&ATA_SR_BSY))break;
	}



	// ponto flutuante
	st = ata_status_read(p);
	if(!st) {

		return ATADEV_UNKNOWN;
	}
	

	unsigned char lba1 = inportb(ata[p].cmd_block_base_addr + ATA_REG_LBA1);
	unsigned char lba2 = inportb(ata[p].cmd_block_base_addr + ATA_REG_LBA2);

 
	/* differentiate ATA, ATAPI, SATA and SATAPI */
	if (lba1==0x14 && lba2==0xEB) return ATADEV_PATAPI;
	if (lba1==0x69 && lba2==0x96) return ATADEV_SATAPI;
	if (lba1==0x00 && lba2==0x00) return ATADEV_PATA;
	if (lba1==0x3c && lba2==0xc3) return ATADEV_SATA;

	return ATADEV_UNKNOWN;
}

static int ata_identify_device(int p,unsigned short *buffer)
{


	switch(detect_devtype(p)) {

		case ATADEV_PATA:
		printf("Uinidade%d PATA\n",p);
        	ata_pio_read(p,buffer,512);
        	ata_wait_not_busy(p);
        	ata_wait_no_drq(p);


		// config ata[n]
		ata[p].dev_type	= (buffer[0]&0x8000)? 0xffff:ATADEV_PATA;
        	ata[p].lba_type	= (buffer[83]&0x0400)? ATA_LBA48:ATA_LBA28;
        	ata[p].mode 	= ATA_PIO_MODO;//FIXME(buffer[49]&0x0100)? ATA_DMA_MODO:ATA_PIO_MODO;
		ata[p].bps 	= 512; 

			break;

		case ATADEV_PATAPI:
		printf("Uinidade%d PATAPI\n",p);
		ata_cmd_write(p,ATA_CMD_IDENTIFY_PACKET_DEVICE);
        	ata_wait(p);
        	ata_wait_drq(p); 
        	ata_pio_read(p,buffer,512);
        	ata_wait_not_busy(p);
        	ata_wait_no_drq(p);


		// config ata[n]
		ata[p].dev_type	=(buffer[0]&0x8000)? ATADEV_PATAPI : 0xffff;
        	ata[p].lba_type	= ATA_LBA28;
        	ata[p].mode 	= ATA_PIO_MODO;//FIXME(buffer[49]&0x0100)? ATA_DMA_MODO:ATA_PIO_MODO;
        	ata[p].bps 	= 2048; 

			break;

		case ATADEV_SATA:

			printf("Uinidade%d SATA\n",p);
			for(;;);

			break;

		case ATADEV_SATAPI:

			printf("Uinidade%d SATAPI\n",p);
			for(;;);

			break;
		case ATADEV_UNKNOWN:
		ata[p].dev_type	= ATADEV_UNKNOWN;
		printf("Uinidade%d Not found\n",p);

			break;

	}

	return 0;   
}


static void set_ata_device_and_sector(int p,int count,unsigned long long/*UINT64*/ addr)
{
    	switch(ata[p].lba_type)
    	{
    		case 28:
        	//Mode LBA28
		outportb(ata[p].cmd_block_base_addr + ATA_REG_SECCOUNT,(unsigned char)count);	// Sector Count 7:0
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA0,(unsigned char)(addr &0xff));	// LBA 7-0   
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA1,(unsigned char)(addr >> 8 &0xff)); 	 // LBA 15-8
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA2,(unsigned char)(addr >> 16 &0xff));	 // LBA 23-16
         	// Modo LBA active, Select device, and LBA 27-24
        	outportb(ata[p].cmd_block_base_addr + ATA_REG_DEVSEL,0x40 |(ata[p].dev_num<<4) | ((unsigned char)(addr>>24 &0x0f)));
        	// verifique se e a mesma unidade para nao esperar pelos 400ns
        	if(ata_record_dev != ata[p].dev_num && ata_record_channel != ata[p].channel) {
            	ata_wait(p);
            	//verifique erro
            	ata_record_dev      = ata[p].dev_num;
            	ata_record_channel  = ata[p].channel;}
        		break;
    		case 48:
        	//Mode LBA48
        	outportb(ata[p].cmd_block_base_addr + ATA_REG_SECCOUNT,0);		// Sector Count 15:8
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA0,(unsigned char)(addr >> 24 &0xff)); 	// LBA 31-24   
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA1,(unsigned char)(addr >> 32 &0xff));	        // LBA 39-32
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA2,(unsigned char)(addr >> 40 &0xff));	        // LBA 47-40
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_SECCOUNT,count);	        // Sector Count 7:0
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA0,(unsigned char)(addr &0xff));		// LBA 7-0   
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA1,(unsigned char)(addr >> 8 &0xff));          // LBA 15-8
	    	outportb(ata[p].cmd_block_base_addr + ATA_REG_LBA2,(unsigned char)(addr >> 16 &0xff));	        // LBA 23-16
		// Modo LBA active, Select device,        
        	outportb(ata[p].cmd_block_base_addr + ATA_REG_DEVSEL,0x40 | ata[p].dev_num<<4);   
        	// verifique se e a mesma unidade para nao esperar pelos 400ns
        	if(ata_record_dev != ata[p].dev_num && ata_record_channel != ata[p].channel) {
            	ata_wait(p);
            	//verifique erro
            	ata_record_dev      = ata[p].dev_num;
            	ata_record_channel  = ata[p].channel;}
        		break;
    		case 0:
        	printf("PANIC,ATA Modo CHS not suport Unidade%d\n",p);
		for(;;){
		
		}
		
        		break;

       }
       
}

int ata_read_sector(int p, int count, unsigned long long/*UINT64*/ addr, void *buffer)
{ 

	switch(ata[p].dev_type) {

		case ATADEV_UNKNOWN:
		return -1;
			break;

		case ATADEV_PATA:

		//select device, lba, count
        	set_ata_device_and_sector(p,count,addr);
		
     		switch(ata[p].mode) 
		{

			case ATA_PIO_MODO:
			switch(ata[p].lba_type) 
			{
				case ATA_LBA28:
				ata_cmd_write(p,ATA_CMD_READ_SECTORS);
					break;
				case ATA_LBA48:
				ata_cmd_write(p,ATA_CMD_READ_SECTORS_EXT);
					break;

			}
			ata_wait_not_busy(p);
            		if(ata_wait_drq(p) != 0)return -1;
            		ata_pio_read(p,buffer,ata[p].bps);
            		ata_wait_not_busy(p);
            		if(ata_wait_no_drq(p) != 0)return -1;

				break;

			case ATA_DMA_MODO:
			return -1;
				break;

		}
			break;

		case ATADEV_PATAPI:
		return -1;
			break;

	}
        	
        return 0;

}

int ata_write_sector(int p, int count, unsigned long long/*UINT64*/ addr, void *buffer)
{   
   

	switch(ata[p].dev_type) {

		case ATADEV_UNKNOWN:
		return -1;
			break;

		case ATADEV_PATA:

		//select device, lba, count
        	set_ata_device_and_sector(p,count,addr);
		
     		switch(ata[p].mode) 
		{

			case ATA_PIO_MODO:
			switch(ata[p].lba_type) 
			{
				case ATA_LBA28:
				ata_cmd_write(p,ATA_CMD_WRITE_SECTORS);
					break;
				case ATA_LBA48:
				ata_cmd_write(p,ATA_CMD_WRITE_SECTORS_EXT);
					break;

			}

			//ata_wait_irq(ata[p].irq); //FIXME IRQs
            		ata_wait_not_busy(p);
            		if(ata_wait_drq(p) != 0)return -1;
            		ata_pio_write(p,buffer,ata[p].bps);

            		//Flush Cache
			switch(ata[p].lba_type) 
			{
				case ATA_LBA28:
				ata_cmd_write(p,ATA_CMD_FLUSH_CACHE);
					break;
				case ATA_LBA48:
				ata_cmd_write(p,ATA_CMD_FLUSH_CACHE_EXT);
					break;

			}

            		ata_wait_not_busy(p);
            		if(ata_wait_no_drq(p) != 0)return -1;

				break;

			case ATA_DMA_MODO:
			return -1;
				break;

		}
			break;

		case ATADEV_PATAPI:
		return -1;
			break;

	}
        	
        return 0;


}


int ata_initialize()
{

	printf("Initializing the ATA Controller:\n");

	memset(ata,0,sizeof(ata_t)*32);

	int p;
	unsigned short *buffer = (unsigned short*) malloc(0x1000);
	ata_pci = (PCI_COFIG_SPACE *) malloc(0x1000);
	
    	unsigned int data = pci_scan_bcc(PCI_CALSS_MASS_STORAGE);

    	if(data  == -1) {
    		printf("PIC: Massa Storage Controller not found!\n");
    		for(;;);
    	}


    	data = ata_pci_configuration_space(data  >>  8 &0xff,data  >> 3  &31,data &7);

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
    			ahci_initialize(ATA_BAR5);

		break;

		default:
			printf("IDE or AHCI controller not found");
			for(;;);
		break;
	}

    	

    	return (0);
                                                                                                 
}
