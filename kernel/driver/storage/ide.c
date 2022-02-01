/*
 * File Name: ide.c
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

unsigned int ata_record_dev,ata_record_channel;

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


void ata_bus_install(int p,int irqn,int dev_num,int channel,\
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

int ata_identify_device(int p,unsigned short *buffer)
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
        	/*ata_wait(p);
        	ata_wait_drq(p); 
        	ata_pio_read(p,buffer,512);
        	ata_wait_not_busy(p);
        	ata_wait_no_drq(p);*/


		// config ata[n]
		ata[p].dev_type	= 0xffff;//(buffer[0]&0x8000)? ATADEV_PATAPI : 0xffff;
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
