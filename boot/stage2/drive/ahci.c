/*
 * File Name: ahci.c
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


#define true 	1

HBA_MEM_T *hba_mem_space;

unsigned int HBA_BASE;
unsigned char *dmaphys;

uint16_t sata_idtfy[256];

const char *ahci_type[] = {
    0,0,0, //reserved
    "SATA",
    "SATAPI",
    "SEMB",
    "PM",
};


static int sata_port_initialize(HBA_MEM_T *mmio);

void ahci_initialize (unsigned int mmio /*ABAR*/) {

	HBA_BASE = (unsigned int) malloc(0x10000); // 64 KiB
	// FIXME É importante que este endereço seja o físico na memória
	dmaphys =(unsigned char*) malloc(0x10000); // 64 KiB;


    	hba_mem_space = (HBA_MEM_T*) mmio;

    	sata_port_initialize(hba_mem_space);

}

// Port x Command and Status
// stop and start
static void stop_cmd(HBA_PORT_T *port)
{
	// PxCMD.ST, PxCMD.CR, PxCMD.FRE and PxCMD.FR, all clearing

	// Clear FIS Receive Enable (FRE - bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
	// espera o FR (bit14)
	while(port->cmd & HBA_PxCMD_FR); 

	// Apagar o ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
	// espera o CR (bit15) terminar
	while(port->cmd & HBA_PxCMD_CR);

}


static void start_cmd(HBA_PORT_T *port)
{
    	// Aguarde ate que o CR (Command List Running (bit15)) ser apagado
    	while((port->cmd &HBA_PxCMD_CR));

    	// Define FIS Receive Enable (bit4) and Sart (bit0)
    	port->cmd |= HBA_PxCMD_FRE;
    	port->cmd |= HBA_PxCMD_ST;

} 

/*
    
 *TODO: Configuração do espaço de memória da porta:
        Antes de reiniciar o espaço de memória da porta,
        o software deve aguardar que os comandos pendentes atuais terminem,
        e diga ao HBA que pare de receber o FIS da porta, stop_cmd(),
        no final da configuração do espaço de memória da porta, o software pode iniciar um comando start_cmd()

*/

static void sata_port_confg(HBA_PORT_T *port, HBA_MEM_T *abar)
{
    	// PxCLB, used 1KB
    	// PxFB, used 256Bytes

    	stop_cmd(port);

    	//port->is = (uint32_t) -1; // Clear pending interrupt bits

    	port->clb   = HBA_BASE;
    	port->clbu  =0;
    	memset((char *)(port->clb), 0, 1024);
    

    	port->fb    =  port->clb + 1024;
    	port->fbu   =0;
    	memset((char *)(port->fb), 0, 256);


    	HBA_BASE += 1024*2;
    
    	start_cmd(port);
}


// AHCI set command header
// Command List structure
// cfis (Host to Device)
// acmd (atapi command)
// Physical Region Descriptor Table (PRDT)
// Command Table

/* FIXME
static void sata_set_acmd(HBA_CMD_TBL_T *cmdtable)
{
    	HBA_ACMD_T *acmd = (HBA_ACMD_T*)(&cmdtable->acmd);

}*/

static void sata_set_cfis(int satanum,HBA_CMD_TBL_T *cmdtable,uint8_t c,\
uint8_t command,uint16_t feature,uint64_t lba,uint16_t count,uint8_t ctrl)
{
    	uint32_t data =0;

    	H2D_REGISTER_FIS_T *cfis = (H2D_REGISTER_FIS_T*)(&cmdtable->cfis);
    	HBA_ACMD_T *acmd = (HBA_ACMD_T*)(&cmdtable->acmd);

    	cfis->fis_type = FIS_TYPE_REG_H2D;
    	cfis->pmport = 0;
    	cfis->rsv0 = 0;
    	cfis->c = c;       
    	cfis->icc = 0;

    	cfis->command = command;
    	cfis->control = ctrl;
 
    	switch(ata[satanum].dev_type) {

    		case SATA_DEVICE_TYPE:
    			cfis->featurel =feature &0xff;
    			cfis->lba0 = lba &0xff;
    			cfis->lba1 = (lba >>8) &0xff;
    			cfis->lba2 = (lba >>16) &0xff;
    			cfis->device  = 1<<6;	// LBA mode;
    			cfis->lba3 = (lba >>24) &0xff;
    			cfis->lba4 = (lba >>32) &0xff;
    			cfis->lba5 = (lba >>40) &0xff;
    			cfis->featureh = (feature >> 8) &0xff;
    			cfis->countl = count &0xff;
    			cfis->counth = (count >>8) &0xff;
    		break;

    		case SATAPI_DEVICE_TYPE:
    			// Aqui configura SATAPI
    			// Modo PIO = 0, Modo DMA = 1
    			cfis->featurel = feature &0xff;
    			cfis->featureh = (feature >> 8) &0xff;

    			data = ata[satanum].bps;
    			// Size bytes per sector
    			cfis->lba0 = data &0xff;
    			cfis->lba1 = (data >>8) &0xff;

    			cfis->device  = 1<<6;	// LBA mode;

    			acmd->packet[0] = ATAPI_CMD_READ;
    			acmd->packet[1] = 0;
    			acmd->packet[2] = (lba >>24) &0xff;
    			acmd->packet[3] = (lba >>16) &0xff;
    			acmd->packet[4] = (lba >>8) &0xff;
    			acmd->packet[5] = lba &0xff;
    			acmd->packet[6] = 0;
    			acmd->packet[7] = 0;
    			acmd->packet[8] = 0;
    			acmd->packet[9] = count &0xff;
    			acmd->packet[10]= 0;
    			acmd->packet[11]= 0;

    		break;

    	}

     
}

static void sata_set_prdt(HBA_CMD_TBL_T *cmdtblbase,void *dba,uint32_t dbc)
{

    	// Vamos trabalhar com apenas uma entrada 4MB no max

    	cmdtblbase->prdt_entry[0].dba = (uint32_t)dba;
    	cmdtblbase->prdt_entry[0].dbau = 0;
    	cmdtblbase->prdt_entry[0].rsv0 = 0;
    	cmdtblbase->prdt_entry[0].dbc = dbc;
    	cmdtblbase->prdt_entry[0].rsv1 = 0;
    	cmdtblbase->prdt_entry[0].i = 1;

}
static void sata_set_cmdHeader(HBA_PORT_T *port,int satanum,HBA_CMD_HEADER_T *cmdbase,char cfl,char w,char p,\
char r,char b,uint32_t prdtl,uint32_t prdbc)
{
    	cmdbase->cfl = cfl;
    	cmdbase->a = (ata[satanum].dev_type == SATAPI_DEVICE_TYPE)? 1 :0;
    	cmdbase->w = w;
    	cmdbase->p = p;
    	cmdbase->r = r;
    	cmdbase->b = b;
    	cmdbase->c = 0; //TODO Clear busy com o R_OK (HBA_PORT.tfd)
    	cmdbase->rsv0 = 0;
    	cmdbase->pmp = 0;
    	cmdbase->prdtl = prdtl;
    	cmdbase->prdbc = prdbc;
    	cmdbase->ctba  = port->fb + 256; // TODO
    	cmdbase->ctbau = 0;
   
}


// Usaremos um comando ATA_IDENTIFY
static int ahci_ata_indentify(HBA_PORT_T *port,char command,char pmp,void *buf)
{

    	// Spin lock timeout counter
    	int spin = 0;
    	int slot = 0;

    	// Redefinir registo de interrupção de porta 
    	port->is = (uint32_t) -1;


    	// Received FIS
    	// HBA_FIS_T *rfis = (HBA_FIS_T*)(port->fb);

    	// Defina o comando de cabeçalho ou slot e configura
    	HBA_CMD_HEADER_T *cmdheader0 = (HBA_CMD_HEADER_T*)(port->clb);
    	sata_set_cmdHeader(port,0,cmdheader0,(sizeof(H2D_REGISTER_FIS_T)/sizeof(uint32_t)),0,0,0,0,1,0);
    
    	// Defina Comando de tabela
    	HBA_CMD_TBL_T *cmdtable = (HBA_CMD_TBL_T*)(cmdheader0->ctba);
    	memset(cmdtable,0,sizeof(HBA_CMD_TBL_T) +(cmdheader0->prdtl-1)*sizeof(HBA_PRDT_ENTRY_T));
    
	// Configura a PRDT
    	sata_set_prdt(cmdtable,dmaphys,512 -1);

    	// Configure o Comando FIS
    	H2D_REGISTER_FIS_T *cfis = (H2D_REGISTER_FIS_T*)(&cmdtable->cfis);
    	// HBA_ACMD_T *acmd = (HBA_ACMD_T*)(&cmdtable->acmd);

    	cfis->fis_type = FIS_TYPE_REG_H2D;
    	cfis->pmport = pmp;
    	cfis->c = 1;
    	cfis->device = 0xE0;
    	cfis->command = command;

    	// command issue
    	port->ci = 1 << slot;
    
    	// Espera o comando completar
	while ((port->tfd & (ATA_SR_BSY | ATA_SR_DRQ)) && spin < 1000000)
	{
        	if(port->tfd &ATA_SR_ERR)
        	{
        		printf("sata(x) Read disk CMD ATA IDENTIFY error\n");
			return (int) -1;
        
        	}

		spin++;
	}

	if (spin == 1000000)
	{
		printf("port(x) is hung \n");
		return (int) -1;
	}


    	// Verificar bit de erro no port.is

    	while(true)
	{

        	if((port->ci & (1<<slot)) == 0)break;
        	if (port->is &HBA_PxIS_TFES)
		{
            		// Task file error
	        	printf("sata(x) read disk CMD ATA IDENTIFY error\n");
			return (int) -1;
        	}
    	}

    	if (cmdheader0->prdbc != 512){
	        printf("sata(x) read disk CMD ATA IDENTIFY error ---\n");
		return (int) -1;
    	}


	 __asm__ __volatile__("cld; rep; movsd;"::"D"(buf),\
    	"S"(dmaphys),"c"(cmdheader0->prdbc/4));

    	return (int) 0;
            
}

static int sata_identify(HBA_PORT_T *port, int satanum)
{
    	uint32_t ssts = port ->ssts;
    	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

    	int npm    = 0;
    	int dev_type   = 0;
    	int bps    = 0;
    	//int lba_ype    = 0;
    	//int mode    = 0;

    	// Check drive status
    	if (det != HBA_PORT_DET_PRESENT)
		return (-1);
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return (-1);

    	if(port->sig == SATA_SIG_ATA)
	{
        	dev_type = SATA_DEVICE_TYPE;
        	bps  = 512;

        	ahci_ata_indentify(port,ATA_CMD_IDENTIFY_DEVICE,0,sata_idtfy);


		ata[satanum].dev_type	= dev_type ;// (sata_idtfy[0]&0x8000)? 0xffff:SATA_DEVICE_TYPE;
        	ata[satanum].lba_type	= (sata_idtfy[83]&0x0400)? ATA_LBA48:ATA_LBA28;
        	ata[satanum].mode 	= (sata_idtfy[49]&0x0100)? ATA_DMA_MODO:ATA_PIO_MODO;
		ata[satanum].bps 	= bps; 
		// Total number of sectors = LBA28 60-61, LBA48 100-103
		ata[satanum].sectors	= sata_idtfy[60] &0xffff;
		ata[satanum].sectors	+= sata_idtfy[61] << 16 &0xffffffff;

    	} else if(port->sig == SATA_SIG_ATAPI) {
        	dev_type = SATAPI_DEVICE_TYPE;
        	bps  = 2048;

        	ahci_ata_indentify(port,ATA_CMD_IDENTIFY_PACKET_DEVICE,0,&sata_idtfy);

		ata[satanum].dev_type = dev_type;
    		ata[satanum].bps   = bps;
    		ata[satanum].mode  = ATA_DMA_MODO;    // PIO = 0, DMA = 1
    		ata[satanum].lba_type   = ATA_LBA48;    // LBA28 or LBA48

		/*ata[satanum].dev_type	= dev_type;//(sata_idtfy[0]&0x8000)? 0xffff:SATAPI_DEVICE_TYPE;
        	ata[satanum].lba_type	= (sata_idtfy[83]&0x0400)? ATA_LBA48:ATA_LBA28;
        	ata[satanum].mode 	= (sata_idtfy[49]&0x0100)? ATA_DMA_MODO:ATA_PIO_MODO;
		ata[satanum].bps 	= bps; */
		// Total number of sectors = LBA28 60-61, LBA48 100-103
		ata[satanum].sectors	= 0;

    
    	} else if(port->sig == SATA_SIG_SEMB) {
        	dev_type = SEMB_DEVICE_TYPE;
        	bps  = 0;
 

    	} else if(port->sig == SATA_SIG_PM){
        	dev_type = PM_DEVICE_TYPE;
        	bps  = 0;
           
    	}else {

		dev_type = ATA_DEVICE_NULL;
        	bps  = 0;

        	printf("sata%d device not found\n",satanum);
        	return (-1);
    	}

    	printf("sata%d device type: %s\n",satanum,ahci_type[dev_type]);
       
    	ata[satanum].dev_num = ata[satanum].np   = satanum;
    	ata[satanum].npm  = npm;
	
	ata[satanum].flag |= 0x80000000; // active

    	return (0);

}


// Serial ATA, detected devices
static int sata_port_initialize(HBA_MEM_T *mmio)
{

	int  i;

	// HBA Reset (HR) (mmio->ghc&0x1)

	int total_np = (mmio->cap &0x1f) + 1;
	
	// salve
	ata->ptotal = total_np;	
	
    	for(i =0;i < total_np;i++)
    	{
       		sata_port_confg(&mmio->ports[i],mmio);
       
       		sata_identify(&mmio->ports[i],i);

    	}  

    	return (0);
}


// Em nossa implementação, consideramos em apenas o uso de um único slot
// e uma PRDT (4MB) a cada pedido de acesso de transferência
static int ahci_read(int satanum,HBA_PORT_T *port, uint64_t lba, uint32_t count,void *buf)
{
	if(!(ata[satanum].flag&0x80000000)) return (-1);

    	// Spin lock timeout counter
    	int spin = 0;
    	int slot = 0;

    	// Redefinir registo de interrupção de porta 
    	port->is = (uint32_t) -1;


    	// Received FIS
    	// HBA_FIS_T *rfis = (HBA_FIS_T*)(port->fb);

    	// Defina o comando de cabeçalho ou slot e configura
    	HBA_CMD_HEADER_T *cmdheader0 = (HBA_CMD_HEADER_T*)(port->clb);

    	sata_set_cmdHeader(port,satanum,cmdheader0,(sizeof(H2D_REGISTER_FIS_T)/sizeof(uint32_t)),0,0,0,0,1,0);
    
    	// Defina Comando de tabela
    	HBA_CMD_TBL_T *cmdtable = (HBA_CMD_TBL_T*)(cmdheader0->ctba);
	memset(cmdtable,0,sizeof(HBA_CMD_TBL_T) +(cmdheader0->prdtl -1)*sizeof(HBA_PRDT_ENTRY_T));
    
    	// Configura a PRDT
    	sata_set_prdt(cmdtable,dmaphys/*buf*/,(count *ata[satanum].bps) -1);
    

    	// Configure o Comando FIS

    	switch(ata[satanum].dev_type)
    	{
        	case SATA_DEVICE_TYPE:
            		if(ata[satanum].mode == ATA_DMA_MODO) {
            			sata_set_cfis(satanum,cmdtable,1,ATA_CMD_READ_DMA_EXT,0,lba,count,0);
			} else {

				return (-1);
			}
    		break;
        	case SATAPI_DEVICE_TYPE:
        	    	if(ata[satanum].mode == ATA_DMA_MODO) {
				return (-1);
            			//sata_set_cfis(satanum,cmdtable,1,ATA_CMD_PACKET,1 /*feature*/,lba,count,0);
			} else {

				return (-1);
			}     
    		break;
    }

    	// Command issue
    	port->ci = 1 << slot;
    
    	// Espera o comando completar
	while ((port->tfd & (ATA_SR_BSY | ATA_SR_DRQ)) && spin < 1000000)
	{
        	if(port->tfd &ATA_SR_ERR)
        	{
        		//print("sata%d Read disk error\n",satanum);
		    	return (int) -1;
        
        	}
		spin++;
	}

	if (spin == 1000000)
	{
		//print("port%d is hung %d\n",satanum,cmdheader0->prdbc);

		return (int) -1;
	}


    	// Verificar bit de erro no port.is
    	while(true) {

        	if((port->ci & (1<<slot)) == 0)break;
        	if (port->is &HBA_PxIS_TFES)
		{
            		// Task file error
	        	//print("sata%d read disk error\n",satanum);
		    	return (int) -1;
        	}
    	}

    	// Verifique se todos bytes foram transferidos    
    	if (cmdheader0->prdbc != (count*ata[satanum].bps))
	{
	      	//print("sata%d read disk error---\n",satanum);
		return (int) 0; //-1;
    	}


	__asm__ __volatile__("cld; rep; movsd;"::"D"(buf),\
            "S"(dmaphys),"c"(cmdheader0->prdbc/4));

    	return (int) 0;
            

}
static int ahci_write(int satanum,HBA_PORT_T *port, uint64_t lba, uint32_t count,void *buf)
{
	
	if(!(ata[satanum].flag&0x80000000)) return (-1);

	
			

    	// Spin lock timeout counter
   	int spin = 0;
    	int slot = 0;

    	// Redefinir registo de interrupção de porta 
    	port->is = (uint32_t) -1;
	// Received FIS
    	// HBA_FIS_T *rfis = (HBA_FIS_T*)(port->fb);

    	// Defina o comando de cabeçalho ou slot e configura
    	HBA_CMD_HEADER_T *cmdheader0 = (HBA_CMD_HEADER_T*)(port->clb);

    	sata_set_cmdHeader(port,satanum,cmdheader0,(sizeof(H2D_REGISTER_FIS_T)/sizeof(uint32_t)),1/*write*/,0,0,0,1,0);
    
    	// Defina Comando de tabela
    	HBA_CMD_TBL_T *cmdtable = (HBA_CMD_TBL_T*)(cmdheader0->ctba);
	memset(cmdtable,0,sizeof(HBA_CMD_TBL_T) +(cmdheader0->prdtl -1)*sizeof(HBA_PRDT_ENTRY_T));


	__asm__ __volatile__("cld; rep; movsd;"::"D"(dmaphys),\
            "S"(buf),"c"((count * ata[satanum].bps)/4));

    
    	// Configura a PRDT
    	sata_set_prdt(cmdtable,dmaphys/*buf*/,(count * ata[satanum].bps) -1);
    

    	// Configure o Comando FIS

    	switch(ata[satanum].dev_type)
    	{
        	case SATA_DEVICE_TYPE:
            		if(ata[satanum].mode == ATA_DMA_MODO) {
            			sata_set_cfis(satanum,cmdtable,1,ATA_CMD_WRITE_DMA_EXT,0,lba,count,0);
			} else {
			
				return (-1);

			}
    		break;
        	case SATAPI_DEVICE_TYPE:

        	    	if(ata[satanum].mode == ATA_DMA_MODO) {
            				return (-1);
				//sata_set_cfis(satanum,cmdtable,1,ATA_CMD_PACKET,1 /*feature*/,lba,count,0);
			} else {
				return (-1);

			}   
    		break;
    	}

    	// Command issue
    	port->ci = 1 << slot;
    
    	// Espera o comando completar
	while ((port->tfd & (ATA_SR_BSY | ATA_SR_DRQ)) && spin < 1000000)
	{
        	if(port->tfd &ATA_SR_ERR)
        	{
        		//print("sata%d write disk error\n",satanum);
		    	return (int) -1;
        
        	}
		spin++;
	}

	if (spin == 1000000)
	{
		//print("port%d is hung %d\n",satanum,cmdheader0->prdbc);

		return (int) -1;
	}


    	// Verificar bit de erro no port.is
    	while(true) {

        	if((port->ci & (1<<slot)) == 0)break;
        	if (port->is &HBA_PxIS_TFES)
		{
            		// Task file error
	        	//print("sata%d write  disk error\n",satanum);
		    	return (int) -1;
        	}
    	}

    	// Verifique se todos bytes foram transferidos    
    	if (cmdheader0->prdbc != (count*ata[satanum].bps))
	{
	      	//print("sata%d write disk error--- %d\n",satanum,cmdheader0->prdbc);

		return (int) 0; //-1;
    	}

    	return (int) 0;
}


int sata_read_sector(int satanum,uint64_t lba, uint32_t count,void *buf)
{
	int i,rc;

	unsigned int offset = lba;
	unsigned int new_addr = 0;
	unsigned int new_addr2 = 0;

	unsigned int times = count/8;

	unsigned char *ubuf = (unsigned char *) buf;

	for(i=0;i<times;i++) {

		new_addr = (i*ata[satanum].bps*8);

		rc = ahci_read(satanum,&hba_mem_space->ports[satanum],offset,8,ubuf+new_addr);

		offset += 8;

		new_addr2 = new_addr + (ata[satanum].bps*8);

		if(rc) return rc;

	}

	times = count%8;
	if(times) {

		rc = ahci_read(satanum,&hba_mem_space->ports[satanum],offset,times,ubuf+new_addr2);
	}


	return (rc);
    
}
int sata_write_sector(int satanum,uint64_t lba, uint32_t count,void *buf)
{
	int i,rc;

	unsigned int offset = lba;
	unsigned int new_addr = 0;
	unsigned int new_addr2 = 0;

	unsigned int times = count/8;

	unsigned char *ubuf = (unsigned char *) buf;

	for(i=0;i<times;i++) {

		new_addr = (i*ata[satanum].bps*8);

		rc = ahci_write(satanum,&hba_mem_space->ports[satanum],offset,8,ubuf+new_addr);

		offset += 8;

		new_addr2 = new_addr + (ata[satanum].bps*8);

		if(rc) return rc;

	}

	times = count%8;
	if(times) {

		rc = ahci_write(satanum,&hba_mem_space->ports[satanum],offset,times,ubuf+new_addr2);
	}


	return (rc);
    
}

