#include <ehci.h>
#include <pci.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <irq.h>
#include <io.h>

EHCI_MEM_T *ehci_driver;
ehci_t ehci;


EhciCMD *cmd1;
EHCI_TD *td1;
EHCI_TD *td2;
EHCI_TD *td3;
EHCI_QH *qh1;
EHCI_QH *qh2;
EHCI_QH *qh3;

EHCI_QH *qh;


#define  EHCI_ALLOC_SIZE 0x100

unsigned int periodic_list[1024] __attribute__ ((aligned (0x1000)));
unsigned int ehci_pool[8192]__attribute__ ((aligned (0x1000))); //32 KiB
unsigned int ehci_pool_start;

void ehci_memset()
{
	memset((char*)cmd1,0,EHCI_ALLOC_SIZE);
	memset((char*)td1,0,EHCI_ALLOC_SIZE);
	memset((char*)td2,0,EHCI_ALLOC_SIZE);
	memset((char*)td3,0,EHCI_ALLOC_SIZE);
	memset((char*)qh1,0,EHCI_ALLOC_SIZE);
	memset((char*)qh2,0,EHCI_ALLOC_SIZE);
	memset((char*)qh3,0,EHCI_ALLOC_SIZE);
}


void *ehc_malloc(int size)
{
	ehci_pool_start += size;
	unsigned int pool = (unsigned int)&ehci_pool;
	
	return (void*)(pool + (ehci_pool_start -size));

}


void ehci_start (EHCI_MEM_T *mmio)
{	mmio->cmd |= 1;

	while(1){
            if( mmio->cmd&1 ){
                break;
            }
        }
}

void ehci_stop (EHCI_MEM_T *mmio)
{
	mmio->cmd &= ~1;
		
	while(1){
            if( !(mmio->cmd&1) ){
                break;
            }
        }
}

void ehci_periodic_schedule_enable (EHCI_MEM_T *mmio)
{
	mmio->cmd |= 0x10;
}

void ehci_periodic_schedule_disable(EHCI_MEM_T *mmio)
{
	mmio->cmd &= ~0x10;
}

void ehci_asynchronous_schedule_enable (EHCI_MEM_T *mmio)
{
	mmio->cmd |= 0x20;
	
	while(!(mmio->cmd &0x20));
	
}

void ehci_asynchronous_schedule_disable(EHCI_MEM_T *mmio)
{
	mmio->cmd &= ~0x20;
}

void ehci_reset (EHCI_MEM_T *mmio)
{
	ehci_stop(mmio);
	
	// Reset the controller
    	mmio->cmd |= 2;
    	wait(2000000);

    	while(mmio->cmd&2) {
    		wait(100);
    	}
    
    
    	printf("[EHCI] Reset of EHCI Host Controller is succeed.\n");
	//printf("ehci reset failed!\n");
}


// TODO:

static int ehci_pci_configuration_space(int bus,int dev,int fun)
{
    	unsigned int data;
    	// VENDOR and DEVICE ID offset 0
    	data  = read_pci_config_addr(bus,dev,fun,0);
    	ehci.vid = data &0xffff;
    	ehci.did = data >> 16 &0xffff;

    	// BAR
    	data  = read_pci_config_addr(bus,dev,fun,0x10);
    	ehci.mmio_base = data &~0x3ff;
    	
    	ehci_driver= (EHCI_MEM_T*)ehci.mmio_base;
    	
    	return 0;
}

void ehci_sondagem(EHCI_MEM_T *controller);
int ehci_init(void)
{
	printf("EHCI initialize\n");
	
	ehci_pool_start = 0;
	memset(ehci_pool,0,(8192)*sizeof(unsigned int));

	
    	unsigned int data = pci_scan_bcc_scc_prog(0xC,0x3,0x20);
    	if(data  == -1)	{
    		puts("PCI PANIC: EHCI (USB2.0) Controller not found!\n");
    		return (-1); // Abortar
    	}

    	ehci_pci_configuration_space(data  >>  24 &0xff,data  >> 16  &0xff,data &0xffff);
    	
    	// USB2.0 bus0:dev29:fun0
    	//ehci_pci_configuration_space(0, 29,0);
    	printf("vid 0x%x, did 0x%x, mmio 0x%x\n",ehci.vid,ehci.did,ehci.mmio_base);
    	
    	
    	EHCI_MEM_T *controller = (EHCI_MEM_T*)ehci.mmio_base;
    	
    	printf("Host Controller Initialization\n");
    	
    	ehci_reset (controller);
    	
    	/*
    	In order to initialize the host controller, software should perform the following steps
		• Program the CTRLDSSEGMENT register with 4-Gigabyte segment where all of the interface data
	structures are allocated.
		• Write the appropriate value to the USBINTR register to enable the appropriate interrupts.
		• Write the base address of the Periodic Frame List to the PERIODICLIST BASE register. If there are no
	work items in the periodic schedule, all elements of the Periodic Frame List should have their T-Bits set
	to a one.
		• Write the USBCMD register to set the desired interrupt threshold, frame list size (if applicable) and turn
	the host controller ON via setting the Run/Stop bit.
		• Write a 1 to CONFIGFLAG register to route all ports to the EHCI controller (see Section 4.2).

	*/
	
	
	ehci_stop (controller);
	ehci_periodic_schedule_disable(controller);
	
	controller->cmd &= ~0b100000;
    	controller->asynclistaddr = 1;
    	

    	// set correct ints
    	ehci_driver->intr |= 0b10111;
    	
	
	qh = (EHCI_QH*)ehc_malloc(EHCI_ALLOC_SIZE);
	cmd1 = (EhciCMD *) ehc_malloc(EHCI_ALLOC_SIZE);
	td1 = (EHCI_TD*)ehc_malloc(EHCI_ALLOC_SIZE);
	td2 = (EHCI_TD*)ehc_malloc(EHCI_ALLOC_SIZE);
	td3 = (EHCI_TD*)ehc_malloc(EHCI_ALLOC_SIZE);
	qh1 = (EHCI_QH*)ehc_malloc(EHCI_ALLOC_SIZE);
	qh2 = (EHCI_QH*)ehc_malloc(EHCI_ALLOC_SIZE);
	qh3 = (EHCI_QH*)ehc_malloc(EHCI_ALLOC_SIZE);
	
	
	
	
	// Periodic list queue setup
    	qh->qhlp = 1;
    	qh->ch = 0;
    	qh->caps = 0;
    	qh->curlink = 0;
    	
    	qh->nextlink = 1;
    	qh->altlink = 0;
    	qh->token = 0;
    	
    	
    	for (int i = 0; i < 1024; ++i)
    	{
        	periodic_list[i] = (unsigned int)qh | 2;
    	}

	// Control Data Structure Segment  height 63:32
    	ehci_driver->ctrldssegment = 0;
    
	// set periodic list base
    	ehci_driver->perodiclistbase = (unsigned int)&periodic_list;
    	ehci_driver->frindex = 0;
    	
    	// set Asynchronous List Address Register
    	controller->asynclistaddr = (unsigned int)0;

    	// set interrupt treshold (usbcmd) : default is 8mf with value 08
    	ehci_driver->cmd |= (0x40<<16);
	
    	// set frame list size : default is 1024 with value 0
    	ehci_driver->cmd |= (0<<2);

    	// Clear status
    	ehci_driver->sts = 0x3f;
    	
    	irq_enable(11);
    	
    	// Enable controller
    	//ehci_periodic_schedule_enable (ehci_driver);
    	//ehci_start (ehci_driver);
    	
    	// set routing
    	ehci_driver->configflag |= 1;
    	wait(2000000);
    	
    	ehci_sondagem(ehci_driver);
    	
	return 0;
}

void ehci_port_init(EHCI_MEM_T *controller,int address);
int ehci_set_device_address(EHCI_MEM_T *controller,int addr);
int ehci_get_device_descriptor(EHCI_MEM_T *controller,int addr,int size, const char *buf);
int ehci_set_device_configuration(EHCI_MEM_T *controller,int addr,unsigned char config);

void ehci_sondagem(EHCI_MEM_T *controller)
{

	int ports = (controller->hcsparams & 0xf);
	int pn = 1;

	for(int i=0; i < ports;i++) {
		unsigned int *port  = (unsigned int*)&controller->port[i];
			
		unsigned int portinfo = *port;
		
		if(portinfo&0x3){
		
			printf("[EHCI] Port %d : connection detected! Port info %x\n",pn,portinfo);
            		ehci_port_init(controller,pn);
            		printf("Port info %x\n",portinfo);
        	}else {
        	
        		printf("[EHCI] Port %d : no connection! Port info %x\n",pn,portinfo);
        		
        	}
        
        	pn++;	

	}
}

void ehci_port_init(EHCI_MEM_T *controller,int address)
{

	unsigned int *port  = (unsigned int*)&controller->port[address - 1];
	unsigned int portinfo;
	// reset port wait 2ms (1ms = 1000000ns)
	while(!(*port&0x100) ) 
	{ 
		*port |= 0x100;
		wait(2000000);
	}
	
	while((*port&0x100) )
	{ 
		*port &= ~0x100;
		wait(2000000);
	}
	
	portinfo = *port;
	if(!(portinfo&0x4) ) { 
	
		printf("[EHCI] Port %d : Port is not enabled but connected, portifo %x\n",address,portinfo);
		return;
	}
	
	
	// set device address to device
	int r = ehci_set_device_address(controller,address);
	
	if(r)
	{
		printf("[EHCI] Port %x : Unable to set device address...\n",address);
        	return;
	}

	// get device descriptor
	char desc[8];
	memset(desc,0,8);
	r = ehci_get_device_descriptor(controller,address,8,desc);
	
	if(r)
	{	
		printf("[EHCI] Port %x : Unable to get devicedescriptor...\n",address);
        	return;
	}

	 // check result
	 int deviceclass = desc[4] &0xff;
	 printf("Magic number at descriptor (0x%x 0x%x)\n",desc[0],desc[1]);
	 printf("Max Packet Size %x \n",desc[7]);
	 
	 printf("Device Class %x \n",deviceclass);
	 
	 if(deviceclass == 0x00)
	 {
        	printf("[ECHI] Port %x : No class found! Asking descriptors...\n",address);
        	printf("Nelson, tem que configurar o dispositivo\n");
        	return;
	 }
	 
	 if(deviceclass == 0x08)
	 {
	 	printf("USB Class Mass Storage\n");
	 }
	 
	 if(deviceclass == 0x9){ // HUB
	 
	 	
	 	usb_hub_init();
	 	
	 
	 }
	 
	//r = ehci_set_device_configuration(controller,address,1);

}

int ehci_set_device_address(EHCI_MEM_T *controller,int addr)
{
	// 1) QH: a queuehead to connect to us
    	// 2) QTD: a command descriptor
    	// 3) QTD: a status descriptor
    	// 4) CMD: a command
    	ehci_memset();
    	
    	EhciCMD *cmd = (EhciCMD*) cmd1;//ehc_malloc(0x400);
    	cmd->bRequest = 0x05; // USB-REQUEST type SET-ADDR is 0x05
    	cmd->bRequestType |= 0; // direction is out
    	cmd->bRequestType |= 0 << 5; // type is standard
    	cmd->bRequestType |= 0; // recieve
    	cmd->wIndex = 0;
    	cmd->wLength = 0;
    	cmd->wValue = addr;
    	
    	EHCI_QH *head1 = qh1;
    	EHCI_QH *head2 = qh2;
    	EHCI_TD *command = td1;
    	EHCI_TD *status = td2;
    	
	
    	ehci_int_td( command, status, 0, 2/*type is setup*/, 8, cmd);
    	
    	ehci_int_td( status, (EHCI_TD *)1, 1, 1/*PID instellen*/, 8, 0);
    	
    	ehci_init_qh(head1, head2, 0, command, 0, 2, 0, 0, 64);
    	
    	controller->asynclistaddr = ((unsigned long)head1);
    	ehci_asynchronous_schedule_enable (controller);
    	
    	int r = ehci_wait_for_completion(status);
    
    	ehci_asynchronous_schedule_disable (controller);
    	controller->asynclistaddr = 1;
    	
    	
    	return r;
}

int ehci_get_device_descriptor(EHCI_MEM_T *controller,int addr,int size, const char *buf)
{

	ehci_memset();

	EHCI_QH* qhead = qh1;
    	EHCI_QH* qhead2 = qh2;
    	EHCI_QH* qhead3 = qh3;
    	EHCI_TD* td = td1;
    	EHCI_TD* trans = td2;
    	EHCI_TD* status = td3;
    	
    	EhciCMD* commando = (EhciCMD*) cmd1;
    	commando->bRequest = 0x06; // get_descriptor
    	commando->bRequestType |= 0x80; // dir=IN
    	commando->wIndex = 0; // windex=0
    	commando->wLength = size; // getlength=8
    	commando->wValue = 1 << 8; // get device info

	ehci_int_td( td, trans, 0, 2, size, commando);
	ehci_int_td( trans, status, 1, 1, size, buf);
	ehci_int_td( status, (EHCI_TD *)1, 1, 0, 0, 0);
    	
    	ehci_init_qh(qhead, qhead2, qhead3, td, 0, 2, addr, 0, 64);
    	
    	
    	controller->asynclistaddr = ((unsigned long)qhead);
    	ehci_asynchronous_schedule_enable (controller);
    	
    	
    	int r = ehci_wait_for_completion(status);
   
    	ehci_asynchronous_schedule_disable (controller);
    	controller->asynclistaddr = 1;

    	return r;
    	
}

int ehci_set_device_configuration(EHCI_MEM_T *controller,int addr,unsigned char config)
{
	// 1) QH: a queuehead to connect to us
    	// 2) QTD: a command descriptor
    	// 3) QTD: a status descriptor
    	// 4) CMD: a command

	ehci_memset();

    	// first make the command
    	EhciCMD *cmd = (EhciCMD*) cmd1;
    	cmd->bRequest = 0x09; // USB-REQUEST type SET-CONFIGURATION is 0x09
    	cmd->bRequestType |= 0; // direction is out
    	cmd->bRequestType |= 0 << 5; // type is standard
    	cmd->bRequestType |= 0; // recieve
    	cmd->wIndex = 0; // ok
    	cmd->wLength = 0; // ok
    	cmd->wValue = config;

    	EHCI_TD *command = td1;
    	EHCI_TD *status = td2;
    	EHCI_QH *head1 = qh1;
    	EHCI_QH *head2 = qh2;

    	
    	ehci_int_td( command, status, 0, 2/*type is setup*/, 8, cmd);
    	
    	ehci_int_td( status, (EHCI_TD *)1, 1, 1/*PID instellen*/, 8, 0);
    	
    	ehci_init_qh(head1, head2, 0, command, 0, 2, addr, 0, 64);
    	
    	controller->asynclistaddr = ((unsigned long)head1);
    	ehci_asynchronous_schedule_enable (controller);
    	
    	int r = ehci_wait_for_completion(status);
    
    	ehci_asynchronous_schedule_disable (controller);
    	controller->asynclistaddr = 1;
   
	
	return r;
    
}

void ehci_handler()
{

	unsigned int status = ehci_driver->sts;

	if(status&0b000001){
        	printf("[EHCI] Interrupt: transaction completed\n");
    
    	}else if(status&0b000010){
        	printf("[EHCI] Interrupt: error interrupt\n");
        	ehci_driver->sts &= ~0b100000;
        	ehci_driver->sts = 1 ;
    	}else if(status&0b000100){
        	printf("[EHCI] Interrupt: portchange\n");
    	}else if(status&0b001000){
        	printf("[EHCI] Interrupt: frame list rollover\n");
    	}else if(status&0b010000){
        	printf("[EHCI] Interrupt: host system error\n");
    	}else if(status&0b100000){
        	printf("[EHCI] Interrupt: interrupt on advance\n");
    	}else{
        	printf("[EHCI] Interrupt: unknown\n");
    	}
	
	
	ehci_driver->sts = status;
}
