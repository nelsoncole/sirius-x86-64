
void ehci_int_td( EHCI_TD *td, unsigned int toggle, unsigned int packettype,
unsigned int len, const void *data) {

    	td->nextlink = 1;
    	td->altlink = 1;

	if(packettype == 0) 
	{
		printf("type is out token\n");
		td->token = (1 << 31) | (len << 16) | (3 << 10) | (0 << 8) | (1 << 7);
	}
	else if(packettype == 1)
	{
		printf("type is in token\n");
		td->token = (1 << 31) | (len << 16) | (3 << 10) | (1 << 8) | (1 << 7);
	}
	
	else if(packettype == 2)
	{
		printf("type is setup token\n");
		td->token = (1 << 31) | (len << 16) | (3 << 10) | (2 << 8) | (1 << 7);
	}


    	td->buffer[0] = (unsigned int) data;
    	td->extbuffer[0] = (unsigned int)0;//(data >> 32);
    	
   
    	
   
}

void ehci_init_qh(EHCI_QH *qh1, EHCI_QH *qh2, EHCI_TD *td, int interrupt, 
unsigned int speed,unsigned int addr, unsigned int endp, unsigned int maxSize)
{
    	unsigned int ch = (maxSize << 16) | 0x4000 | (speed << 12) | (endp << 8) | addr;
  
  	unsigned int caps = (1 << 30);

    	if (!interrupt) {
    	
        	ch |= (5 << 28);
    	}

   	 if (interrupt) {
        	// interrupt schedule mask - start on frame 0
        	caps |= 0x01;
    	}


	// Tweede commando
	qh2->qhlp = (unsigned int)qh1 |2;
    	qh2->ch = ch;	
    	qh2->caps = caps;
    	qh2->curlink = 0;
    	
    	qh2->nextlink = (unsigned int)td;
    	qh2->altlink = 1;

    	qh2->token = 0;
    	
    	 // Eerste commando
    	
   	qh1->qhlp  = ((unsigned long)qh2) | 2;
   	qh1->ch = ch;	
   	qh1->ch |= 1 << 15; // T
    	qh1->caps = caps;
    	qh1->curlink = 0;
   	
   	qh1->altlink = 1;
    	qh1->nextlink = 1;
    	qh1->token = 0x40; // IOC
    	
}

int ehci_process_qh(EHCI_QH *qh) 
{
    	if (qh->token & (1 << 6))
    	{
    	   return 0;
    	}
    	else if (qh->nextlink & 1)
    	{
        	if (~qh->token & (1 << 7) )
        	{
        	 	if (qh->token & (1 << 5) )
            		{
                		printf(" Data Buffer Error\n");
                		return 1;
            		}
            		if (qh->token & (1 << 4) )
            		{
                		printf(" Babble Detected\n");
                		return 1;
            		}
            		if (qh->token & (1 << 3) )
            		{
                		printf(" Transaction Error\n");
                		return 1;
            		}
            		if (qh->token & (1 << 2) )
            		{
                		printf(" Missed Micro-Frame\n");
                		return 1;
            		}

        	}
    	}

	return 0;
}

int _ehci_wait_for_completion(EHCI_TD *td)
{
     
   	unsigned ticks = 10000000;
   	
    	while(1)
    	{
    	
        	if(td->token & (1 << 4))
        	{
            		printf("[EHCI] Transmission failed due babble error\n");
           		return 1;
        	}
        	if(td->token & (1 << 3))
        	{
            		printf("[EHCI] Transmission failed due transaction error\n");
			return 1;
        	}
        	if(td->token & (1 << 6))
        	{
            		printf("[EHCI] Transmission failed due serious error\n");
		    	return 1;
        	}
        	if(td->token & (1 << 5))
        	{
            		printf("[EHCI] Transmission failed due data buffer error\n");
		    	return 1;
        	}
        	if(!(td->token & (1 << 7)))
        	{	// not anymore active and succesfull ended
            		return 0;
        	}
       	if(!ticks--)
       	{
            		printf("EHCI FAIL Timeout\n");
            		return 1;
        	}
    	}
   
    	return 0;
}

int _ehci_send_qh(EHCI_QH *qh, EHCI_MEM_T *mmio,const void *data, unsigned int len, int addr)
{
    	
    	ehci_int_td( td, 0, 0, len, data);
	ehci_init_qh( qh, qh2, td, 1, 1, addr, 2, 512);
    	
    	ehci_driver->frindex = 0;
    	
    	while(ehci_driver->frindex < 1);
    	
    	for(int i=0;i < 10000000;i++);
    	
    	int r = ehci_process_qh(qh2);
    	//int r = _ehci_wait_for_completion(td);
    	
    	
    	EhciPrintQH(qh);
    	EhciPrintQH(qh2);
    	EhciPrintTD(td);

	return r;
}
int _ehci_recieve_qh(EHCI_QH *qh,EHCI_MEM_T *mmio,const void *data, unsigned int len, int addr)
{
    	
    	ehci_int_td( _td, 0, 1, len, data);
	ehci_init_qh( _qh, _qh2, _td, 1, 1, addr, 1, len/*512*/);
    	
    	ehci_driver->frindex = 0;
    	
    	while(ehci_driver->frindex < 2);
    	
    	
    	int r = ehci_process_qh(_qh2);
    	
    	
    	//EhciPrintQH(_qh);
    	//EhciPrintQH(_qh2);
    	//EhciPrintTD(_td);

	return r;
}


int _ehci_send_bulk(EHCI_TD *td, EHCI_QH *qh1, EHCI_QH *qh2,EHCI_MEM_T *mmio, const void *data, unsigned int len, int portnumber)
{

	printf("send\n");
	

	ehci_int_td( td, 0, 0, len, data);
	
	ehci_init_qh( qh1, qh2, td, 1, 1, portnumber, 2, 512);
	
	
	//mmio->asynclistaddr = ((unsigned int)qh1);
    	mmio->cmd |= 0b100000;
    	
    	int r = _ehci_wait_for_completion(td);
    
    	mmio->cmd &= ~0b100000;
    //	mmio->asynclistaddr = 1;
    	
    	
    	/*EhciPrintTD(td);
    	EhciPrintQH(qh2);
    	EhciPrintQH(qh1);*/
    	
    	printf("pass %x %x\n",qh2->token,qh1->token);
    	
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

unsigned char ehci_wait_for_completion(volatile EhciTD *status){
    unsigned char lstatus = 1;
   
   unsigned ticks = 100000000;
    while(1){
        volatile unsigned long tstatus = (volatile unsigned long)status->token;
        
       // printf("tstatus %x\n",tstatus);
        if(tstatus & (1 << 4)){
            // not anymore active and failed miserably
            printf("[EHCI] Transmission failed due babble error\n");
            lstatus = 0;
            break;
        }
        if(tstatus & (1 << 3)){
            // not anymore active and failed miserably
            printf("[EHCI] Transmission failed due transaction error\n");
            lstatus = 0;
            break;
        }
        if(tstatus & (1 << 6)){
            // not anymore active and failed miserably
            printf("[EHCI] Transmission failed due serious error\n");
            lstatus = 0;
            break;
        }
        if(tstatus & (1 << 5)){
            // not anymore active and failed miserably
            printf("[EHCI] Transmission failed due data buffer error\n");
            lstatus = 0;
            break;
        }
        if(!(tstatus & (1 << 7))){
            // not anymore active and succesfull ended
            lstatus = 1;
            break;
        }
       if(!ticks--){
            printf("EHCI FAIL Timeout\n");
            lstatus = 0;
            break;
        }
    }
    if(lstatus==0){
        //
        // Something is wrong... can we discover what is wrong?
        // unsigned char errorcount = (status->token >> 10) & 0b111; // maxerror
        // According to the specification of ehci, for CRC, Timeout,Bad PID the result is 0
        // For Babble and buffererror , result is not 0
    }
    return lstatus;
}

int ehci_recieve_bulk(EHCI_MEM_T *mmio,unsigned long expectedIN,void *buffer, int portnumber)
{
	
	EhciQH* qh = (EhciQH*) 0x200000; 
    	EhciQH* qh2 = (EhciQH*) 0x201000;
    	EhciTD* trans = (EhciTD*) 0x202000;
    	
      	
      	memset(qh,0,0x1000);
      	memset(qh2,0,0x1000);
      	memset(trans,0,0x1000);
        
    
        trans->nextlink = 1;
    	trans->altlink = 1;
        trans->token |= (expectedIN << 16);
        trans->token |= (1 << 31); // toggle
        trans->token |= (1 << 7); // actief
        trans->token |= (1 << 8); // IN token
        trans->token |= (0x3 << 10); // maxerror
        trans->buffer[0] = (unsigned long)buffer;
    
        
         // Tweede commando
    	qh2->altlink = 1;
    	qh2->nextlink = (unsigned long)trans; // qdts2
    	qh2->horizontal_link_pointer = ((unsigned long)qh) | 2;
    	qh2->curlink = 1; // qdts1
    	qh2->characteristics |= 1 << 14; // dtc
    	qh2->characteristics |= 512 << 16; // mplen
    	qh2->characteristics |= 2 << 12; // eps
    	qh2->characteristics |= 1 << 8; // endpoint 1
    	qh2->characteristics |= portnumber; // device
    	qh2->capabilities = 0x40000000;

    	//
    	// Eerste commando
    	qh->altlink = 1;
    	qh->nextlink = 1;
    	qh->horizontal_link_pointer = ((unsigned long)qh2) | 2;
    	qh->curlink = 0;
    	qh->characteristics = 1 << 15; // T
    	qh->token = 0x40; 
        
        
      	mmio->asynclistaddr = ((unsigned long)qh) ;
    	mmio->cmd |= 0b100000;

    	unsigned char result = ehci_wait_for_completion(trans);
    
    	mmio->cmd &= ~0b100000;
    	mmio->asynclistaddr = 1;

	if(result==0){
        	return 1;
    	}


	return 0;
}

int ehci_send_bulk(EHCI_MEM_T *mmio,unsigned char* out,unsigned long expectedOut, int portnumber){
    	//
    	// Send bulk
    	EhciTD *command = (EhciTD*) (0x100000);
    	EhciQH *head1 = (EhciQH*) (0x102000);
    	EhciQH *head2 = (EhciQH*) (0x103000);
    	
    	memset(command,0,0x1000);
    	memset(head1,0,0x1000);
    	memset(head2,0,0x1000);
    	
    
    	command->nextlink = 1;
    	command->altlink = 1;
    	command->token |= (expectedOut << 16); // setup size
    	command->token |= (1 << 7); // actief
    	command->token |= (0 << 8); // type is out
    	command->token |= (0x3 << 10); // maxerror
    	command->buffer[0] = (unsigned long)out;


    	//
    	// Tweede commando


    	head2->altlink = 1;
    	head2->nextlink = (unsigned long)command; // qdts2
    	head2->horizontal_link_pointer = ((unsigned long)head1) | 2;
    	head2->curlink = 0; // qdts1
    	head2->characteristics |= 512 << 16; // mplen
    	head2->characteristics |= 2 << 12; // eps
    	head2->characteristics |= portnumber;
    	head2->characteristics |= 2 << 8;// device->endpointBulkOUT << 8; // endpoint 2
    	head2->capabilities = 0x40000000;
    	
    //	printf("[EHCI] BULK: Sending %x \n",head2->characteristics); //  heeft = 0x2006201 moet = 0x2002201

    	//
    	// Eerste commando
    	head1->altlink = 1;
    	head1->nextlink = 1;
    	head1->horizontal_link_pointer = ((unsigned long)head2) | 2;
    	head1->curlink = 0;
    	head1->characteristics = 1 << 15; // T
    	head1->token = 0x40;

    
    	mmio->asynclistaddr = ((unsigned long)head1);
    	mmio->cmd |= 0b100000;
    	
    	unsigned char lstatus = ehci_wait_for_completion(command);
    
    	mmio->cmd &= ~0b100000;
    	mmio->asynclistaddr = 1;


    	if(lstatus==0){
        	return 1;
    	}
    	return 0;
}

unsigned char* usb_stick_send_and_recieve_scsi_command(EHCI_MEM_T *device,unsigned char* out,unsigned long expectedIN,unsigned long expectedOut, int portnumber);

int ehci_set_device_address(EHCI_MEM_T *controller,int addr)
{
	// 1) QH: a queuehead to connect to us
    	// 2) QTD: a command descriptor
    	// 3) QTD: a status descriptor
    	// 4) CMD: a command
    	
    	EhciCMD *cmd = (EhciCMD*) ehc_malloc(0x400);
    	memset(cmd,0,sizeof(EhciCMD));
    	cmd->bRequest = 0x05; // USB-REQUEST type SET-ADDR is 0x05
    	cmd->bRequestType |= 0; // direction is out
    	cmd->bRequestType |= 0 << 5; // type is standard
    	cmd->bRequestType |= 0; // recieve
    	cmd->wIndex = 0;
    	cmd->wLength = 0;
    	cmd->wValue = addr;
    	
    	EhciTD *command = (EhciTD*) ehc_malloc(0x400);
    	volatile EhciTD *status = (volatile EhciTD*) ehc_malloc(0x400);
    	EhciQH *head1 = (EhciQH*) ehc_malloc(0x400);
    	EhciQH *head2 = (EhciQH*) ehc_malloc(0x400);

    	command->nextlink = (unsigned long)status;
    	command->altlink = 1;
    	command->token |= (8 << 16); // setup size
    	command->token |= (1 << 7); // actief
    	command->token |= (0x2 << 8); // type is setup
    	command->token |= (0x3 << 10); // maxerror
    	command->buffer[0] = (unsigned long)cmd;

    	status->nextlink = 1;
    	status->altlink = 1;
    	status->token |= (1 << 8); // PID instellen
    	status->token |= (1 << 31); // toggle
    	status->token |= (1 << 7); // actief
    	status->token |= (0x3 << 10); // maxerror

    	//
    	// Tweede commando
	head2->altlink = 1;
    	head2->nextlink = (unsigned long)command; // qdts2
    	head2->horizontal_link_pointer = ((unsigned long)head1) | 2;
    	head2->curlink = 0; // qdts1
    	head2->characteristics |= 1 << 14; // dtc
    	head2->characteristics |= 64 << 16; // mplen
    	head2->characteristics |= 2 << 12; // eps
    	head2->capabilities = 0x40000000;

    	//
    	// Eerste commando
    	head1->altlink = 1;
    	head1->nextlink = 1;
    	head1->horizontal_link_pointer = ((unsigned long)head2) | 2;
    	head1->curlink = 0;
    	head1->characteristics = 1 << 15; // T
    	head1->token = 0x40;
    	
    	controller->asynclistaddr = ((unsigned long)head1);
    	controller->cmd |= 0b100000;
    	wait(2000000);
    	
    	int r = ehci_wait_for_completion(status);
    
    	controller->cmd &= ~0b100000;
    	controller->asynclistaddr = 1;
    	
    	return r;
}

int ehci_get_device_descriptor(EHCI_MEM_T *controller,int addr,int size, const char *buf)
{

	EhciQH* qhead = (EhciQH*) ehc_malloc(0x400);
    	EhciQH* qhead2 = (EhciQH*) ehc_malloc(0x400);
    	EhciQH* qhead3 = (EhciQH*) ehc_malloc(0x400);
    	EhciTD* td = (EhciTD*) ehc_malloc(0x400);
    	EhciTD* trans = (EhciTD*) ehc_malloc(0x400);
    	volatile EhciTD* status = (volatile EhciTD*) ehc_malloc(0x400);
    	EhciCMD* commando = (EhciCMD*) ehc_malloc(0x400);

    	commando->bRequest = 0x06; // get_descriptor
    	commando->bRequestType |= 0x80; // dir=IN
    	commando->wIndex = 0; // windex=0
    	commando->wLength = size; // getlength=8
    	commando->wValue = 1 << 8; // get device info

    	//
    	// Derde commando
    	td->token |= 2 << 8; // PID=2
    	td->token |= 3 << 10; // CERR=3
    	td->token |= size << 16; // TBYTE=8
    	td->token |= 1 << 7; // ACTIVE=1
    	td->nextlink = (unsigned long)trans;
    	td->altlink = 1;
    	td->buffer[0] = (unsigned long)commando;

    	trans->nextlink = (unsigned long)status;
    	trans->altlink = 1;
    	trans->token |= (size << 16); // verwachte lengte
    	trans->token |= (1 << 31); // toggle
    	trans->token |= (1 << 7); // actief
    	trans->token |= (1 << 8); // IN token
    	trans->token |= (0x3 << 10); // maxerror
    	trans->buffer[0] = (unsigned long)buf;

    	status->nextlink = 1;
    	status->altlink = 1;
    	status->token |= (0 << 8); // PID instellen
    	status->token |= (1 << 31); // toggle
    	status->token |= (1 << 7); // actief
    	status->token |= (0x3 << 10); // maxerror


    	//
    	// Tweede commando
    	qhead2->altlink = 1;
    	qhead2->nextlink = (unsigned long)td; // qdts2
    	qhead2->horizontal_link_pointer = ((unsigned long)qhead) | 2;
    	qhead2->curlink = (unsigned long)qhead3; // qdts1
    	qhead2->characteristics |= 1 << 14; // dtc
    	qhead2->characteristics |= 64 << 16; // mplen
    	qhead2->characteristics |= 2 << 12; // eps
    	qhead2->characteristics |= addr; // device
    	qhead2->capabilities = 0x40000000;

    	//
    	// Eerste commando
    	qhead->altlink = 1;
    	qhead->nextlink = 1;
    	qhead->horizontal_link_pointer = ((unsigned long)qhead2) | 2;
    	qhead->curlink = 0;
    	qhead->characteristics = 1 << 15; // T
    	qhead->token = 0x40;
    	
    	controller->asynclistaddr = ((unsigned long)qhead);
    	controller->cmd |= 0b100000;
    	wait(2000000);
    	
    	int r = ehci_wait_for_completion(status);
    
    	controller->cmd &= ~0b100000;
    	controller->asynclistaddr = 1;

    	return r;
    	
}

int ehci_set_device_configuration(EHCI_MEM_T *controller,int addr,unsigned char config)
{

    	// 1) QH: a queuehead to connect to us
    	// 2) QTD: a command descriptor
    	// 3) QTD: a status descriptor
    	// 4) CMD: a command

    	// first make the command
    	EhciCMD *cmd = (EhciCMD*) ehc_malloc(0x400);
    	cmd->bRequest = 0x09; // USB-REQUEST type SET-CONFIGURATION is 0x09
    	cmd->bRequestType |= 0; // direction is out
    	cmd->bRequestType |= 0 << 5; // type is standard
    	cmd->bRequestType |= 0; // recieve
    	cmd->wIndex = 0; // ok
    	cmd->wLength = 0; // ok
    	cmd->wValue = config;

    	EhciTD *command = (EhciTD*) ehc_malloc(0x400);
    	EhciTD *status = (EhciTD*) ehc_malloc(0x400);
    	EhciQH *head1 = (EhciQH*) ehc_malloc(0x400);
    	EhciQH *head2 = (EhciQH*) ehc_malloc(0x400);

    	command->nextlink = (unsigned long)status;
    	command->altlink = 1;
    	command->token |= (8 << 16); // setup size
    	command->token |= (1 << 7); // actief
    	command->token |= (0x2 << 8); // type is setup
    	command->token |= (0x3 << 10); // maxerror
    	command->buffer[0] = (unsigned long)cmd;

    	status->nextlink = 1;
    	status->altlink = 1;
    	status->token |= (1 << 8); // PID instellen
    	status->token |= (1 << 31); // toggle
    	status->token |= (1 << 7); // actief
    	status->token |= (0x3 << 10); // maxerror

    	//
    	// Tweede commando


    	head2->altlink = 1;
    	head2->nextlink = (unsigned long)command; // qdts2
    	head2->horizontal_link_pointer = ((unsigned long)head1) | 2;
    	head2->curlink = 0; // qdts1
    	head2->characteristics |= 1 << 14; // dtc
    	head2->characteristics |= 64 << 16; // mplen
    	head2->characteristics |= 2 << 12; // eps
    	head2->characteristics |= addr; // device
    	head2->capabilities = 0x40000000;

    	//
    	// Eerste commando
    	head1->altlink = 1;
    	head1->nextlink = 1;
    	head1->horizontal_link_pointer = ((unsigned long)head2) | 2;
    	head1->curlink = 0;
    	head1->characteristics = 1 << 15; // T
    	head1->token = 0x40;
    
    	controller->asynclistaddr = ((unsigned long)head1);
    	controller->cmd |= 0b100000;
    	wait(2000000);
    	
    	int r = ehci_wait_for_completion(status);
    
    	controller->cmd &= ~0b100000;
    	controller->asynclistaddr = 1;
    
    	/*
    	EhciPrintQH(head1);
    	EhciPrintQH(head2);
	EhciPrintTD(status);
	EhciPrintTD(command); */
    
    	return r;
}


void ehci_port_init(EHCI_MEM_T *controller,int address)
{

	unsigned int *port  = (unsigned int*)&controller->port[address];
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
	int r = ehci_set_device_address(controller,address+1);
	
	if(!r)
	{
		printf("[EHCI] Port %x : Unable to set device address...\n",address);
        	return;
	}

	// get device descriptor
	
	char desc[8];
	memset(desc,0,8);
	r = ehci_get_device_descriptor(controller,address+1,8,desc);
	
	if(!r)
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
        	//return;
	 }
	 
	 r = ehci_set_device_configuration(controller,address+1,1);
	 
	 if(deviceclass == 0x9){ // HUB
	 
	 	usb_hub_init();
	 
	 }

}
void ehci_probe(EHCI_MEM_T *controller)
{

	int ports = (controller->hcsparams & 0xf);

	for(int i=0; i < ports;i++) {
		unsigned int *port  = (unsigned int*)&controller->port[i];
		
		unsigned int portinfo = *port;
		
		if(portinfo&0x3){
		
			printf("[EHCI] Port %d : connection detected! Port info %x\n",i,portinfo);
            		ehci_port_init(controller,i);
            		printf("Port info %x\n",portinfo);
        	}else {
        	
        		printf("[EHCI] Port %d : no connection! Port info %x\n",i,portinfo);
        		
        	}

	}
}


int _ehci_init(void)
{
	ehci_pool_start = 0;

	printf("EHCI initialize\n");
    	unsigned int data = pci_scan_bcc_scc_prog(0xC,0x3,0x20);
    	if(data  == -1)	{
    		puts("PCI PANIC: EHCI (USB2.0) Controller not found!\n");
    		return (-1); // Abortar
    	}

    	ehci_pci_configuration_space(data  >>  24 &0xff,data  >> 16  &0xff,data &0xffff);
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
    	
    	memset(ehci_pool,0,8192*sizeof(unsigned int));
    	
    	qh1 = (EHCI_QH*)ehc_malloc(0x400);
    	qh2 = (EHCI_QH*)ehc_malloc(0x400);
    	_qh = (EHCI_QH*)ehc_malloc(0x400);
	qh = (EHCI_QH*)ehc_malloc(0x400);
	td = (EHCI_TD*)ehc_malloc(0x400);
	_qh2 = (EHCI_QH*) ehc_malloc(0x400);
    	_td = (EHCI_TD*) ehc_malloc(0x400);
	
	
	
	// Periodic list queue setup
    	qh->qhlp = 1;
    	qh->ch = 0;
    	qh->caps = 0;
    	qh->curlink = 0;
    	
    	qh->nextlink = 1;
    	qh->altlink = 0;
    	qh->token = 0;
    	
    	
    	for (int i = 0; i < 5; ++i)
    	{
        	qh->buffer[i] = 0;
        	qh->extbuffer[i] = 0;
    	}
    	
    	// Periodic list queue setup
    	_qh->qhlp = 1;
    	_qh->ch = 0;
    	_qh->caps = 0;
    	_qh->curlink = 0;
    	
    	_qh->nextlink = 1;
    	_qh->altlink = 0;
    	_qh->token = 0;
    	
    	for (int i = 0; i < 5; ++i)
    	{
        	_qh->buffer[i] = 0;
        	_qh->extbuffer[i] = 0;
    	}
    	
    	for (int i = 0; i < 1024; ++i)
    	{
    	
        	periodic_list[i] = (unsigned int)qh | 2;
    	}
    	
    	
       periodic_list[1] = (unsigned int)_qh | 2;
   

	// Control Data Structure Segment  height 63:32
    	ehci_driver->ctrldssegment = 0;
    	ehci_driver->frindex = 0;
	// set periodic list base
    	ehci_driver->perodiclistbase = (unsigned int)&periodic_list;
    	controller->asynclistaddr = (unsigned int)qh1;

    	// set interrupt treshold (usbcmd) : default is 8mf with value 08
    	ehci_driver->cmd |= (0x40<<16);
	
    	// set frame list size : default is 1024 with value 0
    	ehci_driver->cmd |= (0<<2);

    	
   
    	// Clear status
    	ehci_driver->sts = 0x3f;
    	
    	// Enable controller
    	ehci_driver->cmd = (8 << 16) | (1 << 4) | (1 << 5) | 1;
    	while (ehci_driver->sts & (1 << 12)); // TODO - remove after dynamic port detection
    	
    
    	// set routing
    	ehci_driver->configflag |= 1;
    	for(int i=1000000;i > 1; i--);
    	
    	
    	irq_enable(11);
    	
    	ehci_probe(ehci_driver);
    	
    	for(;;);
    
    	
    	unsigned int lba = 1;
    	int mode = 0 ; // 0=6 1=10
   	unsigned long bufoutsize = sizeof(struct cbw_t);
	unsigned long bufinsize = USB_STORAGE_SECTOR_SIZE;
	unsigned char opcode = mode==1?0x28:8;//0x28; 
    	struct cbw_t *bufout = (struct cbw_t*)malloc(0x8000);
    	memset(bufout,0,bufoutsize);
    	bufout->lun = 0;
	bufout->tag = 1;
	bufout->sig = 0x43425355;
	bufout->wcb_len = mode==1?10:6;//10;
	bufout->flags = 0x80;
	bufout->xfer_len = bufinsize;
	if(mode==1){
		bufout->cmd[0] = opcode;
		bufout->cmd[1] = 0;
		bufout->cmd[2] = 0;
		bufout->cmd[3] = (lba >> 16) & 0xFF;
		bufout->cmd[4] = (lba >> 8) & 0xFF;
		bufout->cmd[5] = (lba) & 0xFF;
		bufout->cmd[6] = 0;
		bufout->cmd[7] = 0;
		bufout->cmd[8] = 1;
	}else{
		bufout->cmd[0] = opcode;
		bufout->cmd[1] = (lba >> 16) & 0xFF;
		bufout->cmd[2] = (lba >> 8) & 0xFF;
		bufout->cmd[3] = (lba) & 0xFF;
		bufout->cmd[4] = 1;
		bufout->cmd[5] = 0;
		bufout->cmd[6] = 0;
	}
    	
    
    	unsigned char* bufin = usb_stick_send_and_recieve_scsi_command(controller,(unsigned char*)bufout,bufinsize,bufoutsize, 1);
    	
    	printf("r = %s",bufin);
    	
    	
    	//for(int i = 0;i < 1000;i++) printf("%d ",ehci_driver->frindex);
    	
	return 0;
}



//TODO


unsigned char* usb_stick_send_and_recieve_scsi_command(EHCI_MEM_T *device,unsigned char* out,unsigned long expectedIN,unsigned long expectedOut, int portnumber)
{

	int r;
	//r =  ehci_send_bulk(device, out, expectedOut, portnumber);
	//r = _ehci_send_bulk(td,qh1,qh2,device, out, expectedOut, portnumber);
	r = _ehci_send_qh(qh,device, out, expectedOut, portnumber);
	if(r) {
	
		printf("[SMSD] Sending bulk error\n");
		return 0;
	
	}
	
	unsigned char* buffer = malloc(expectedIN);
	memset(buffer,0,expectedIN);
	//r = ehci_recieve_bulk(device,expectedIN,buffer, portnumber);
	r = _ehci_recieve_qh(qh,device,buffer, expectedIN,portnumber);
	if(r) {
	
		printf("[SMSD] Recieving bulk error\n");
		return 0;
	
	}


	CommandStatusWrapper *csw = (CommandStatusWrapper*)buffer;
	
	printf("csw->signature %x, csw->status %x\n",csw->signature,csw->status);
	
	
	if(csw->signature!=USB_STORAGE_CSW_SIGN){
		unsigned char* cuv = malloc(0x1000);
		//r = ehci_recieve_bulk(device,13,cuv, portnumber);
		r = _ehci_recieve_qh(qh,device,cuv,13,portnumber);
		if(r){
			printf("[SMSD] Recieving command status wrapper error\n");
			return (unsigned char *) 0;
		}
		csw = (CommandStatusWrapper*) cuv;
		printf("[SMSD] Status at end\n");
		if(csw->signature!=USB_STORAGE_CSW_SIGN){
			printf("[SMSD] Command Status Wrapper has a invalid signature\n");
			return (unsigned char *) 0;
		}
	}
	printf("[SMSD] Status=%x \n",csw->status);
	if(csw->status){
		printf("[SMSD] Status is not 0 \n");
		return (unsigned char *) 0;
	}
	if(csw->dataResidue){
		printf("[SMSD] Data residu %x \n",csw->dataResidue);
		for(int i = 0 ; i < 512 ; i++){printf("%x ",buffer[i]);}for(;;);
		printf("[SMSD] Asking for a re-read\n");
		
		//r = ehci_recieve_bulk(device,csw->dataResidue,buffer,portnumber);
		r = _ehci_recieve_qh(qh,device,buffer, csw->dataResidue,portnumber);
		if((unsigned long)buffer== 0){printf("D");
			return (unsigned char *) 0;
		}
	}
	return buffer;
}



//

int usb_control_msg (EHCI_MEM_T *controller, int dev, 
			int bRequestType,
                     	int bRequest,
			int wValue,
    			int wIndex,
    			int wLength,
			void *data){
			
	ehci_memset();

	EhciQH* qhead = qh1;
    	EhciQH* qhead2 = qh2;
    	EhciQH* qhead3 = qh3;
    	EhciTD* td = td1;
    	EhciTD* trans = td2;
    	EhciTD* status = td3;
    	
    	EhciCMD* setup = (EhciCMD*) cmd1;
    	setup->bRequest = bRequest;
    	setup->bRequestType = bRequestType;
    	setup->wIndex = wIndex;
    	setup->wLength = wLength;
    	setup->wValue = wValue;


	ehci_int_td( td, trans, 0, 2, 8, setup);
	ehci_int_td( trans, status, 1, 1, wLength, data);
	ehci_int_td( status, 1, 1, 0, 0, 0);
    	
    	ehci_init_qh(qhead, qhead2, qhead3, td, 0, 2, dev, 0, 64);
    	
    	
    	controller->asynclistaddr = ((unsigned long)qhead);
    	ehci_asynchronous_schedule_enable (controller);
    	
    	
    	int r = ehci_wait_for_completion(status);
   
    	ehci_asynchronous_schedule_disable (controller);
    	controller->asynclistaddr = 1;
    	

    	return r;
					
}

int usb_control_set (EHCI_MEM_T *controller, int dev, 
			int bRequestType,
                     	int bRequest,
			int wValue,
    			int wIndex){
	ehci_memset();

    	// first make the command
    	EhciCMD *setup = (EhciCMD*) cmd1;
    	setup->bRequest = bRequest;
    	setup->bRequestType = bRequestType;
    	setup->wIndex = wIndex;
    	setup->wLength = 0;
    	setup->wValue = wValue;

    	EhciTD *command = (EhciTD*) td1;
    	EhciTD *status = (EhciTD*) td2;
    	EhciQH *head1 = (EhciQH*) qh1;
    	EhciQH *head2 = (EhciQH*) qh2;

    	
    	ehci_int_td( command, status, 0, 2/*type is setup*/, 8, setup);
    	
    	ehci_int_td( status, 1, 1, 1/*PID instellen*/, 0, 0);
    	
    	ehci_init_qh(head1, head2, 0, command, 0, 2, dev, 0, 64);
    	
    	controller->asynclistaddr = ((unsigned long)head1);
    	ehci_asynchronous_schedule_enable (controller);
    	
    	int r = ehci_wait_for_completion(status);
    
    	ehci_asynchronous_schedule_disable (controller);
    	controller->asynclistaddr = 1;
   
	
	return r;
    			
}
