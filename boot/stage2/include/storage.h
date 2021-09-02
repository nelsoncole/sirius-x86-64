#ifndef __STORAGE_H
#define __STORAGE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <pci.h>
#include <io.h>
#include <ata.h>
#include <ahci.h>



typedef struct _ata
{
	// device type IDE or AHCI
	unsigned int 	device;

	// numero total de portas
	unsigned int 	ptotal;

	// flag
	unsigned int 	flag;

	unsigned int 	sectors;

    	// bytes per sector
   	unsigned int	bps;

	// mode of transfere (0 = DMA or 1 = PIO)
    	unsigned int	mode;

	// LBA28 or LBA48
    	unsigned int	lba_type;

	// dev_num 0 = Primary master
	// dev_num 1 = Primary slava
	// dev_num 2 = Secondary master
	// dev_num 3 = Secondary slave
    	unsigned int	dev_num;

	// dev_type 0 = ATA, 1 = ATAPI
	// dev_type 2 = SATA 3 = SATAPI
    	unsigned int	dev_type;

	// channel 0 = Primary
	// channel 0 = Secondary
    	unsigned int	channel;
 
    	unsigned int	cmd_block_base_addr;
    	unsigned int	ctrl_block_base_addr;
    	unsigned int	bus_master_base_addr;

	// IRQn 14 or 15 
	unsigned int	irqn;

	// SATA 
	// Number of port
	unsigned int 	np;
	// Number of port multiplier
    	unsigned int 	npm;   

}__attribute__ ((packed)) ata_t;


extern unsigned char dv_num;
extern unsigned int dv_uid;

extern ata_t ata[32];

int read_sector(int dev,unsigned count,unsigned long long addr,void *buffer);
int write_sector(int dev,unsigned count,unsigned long long addr,void *buffer);

#endif
