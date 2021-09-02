#include <ehci.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>


// Port Status

#define PORT_CONNECTION                 (1 << 0)    // Current Connect Status
#define PORT_ENABLE                     (1 << 1)    // Port Enabled
#define PORT_SUSPEND                    (1 << 2)    // Suspend
#define PORT_OVER_CURRENT               (1 << 3)    // Over-current
#define PORT_RESET                      (1 << 4)    // Port Reset
#define PORT_POWER                      (1 << 8)    // Port Power
#define PORT_SPEED_MASK                 (3 << 9)    // Port Speed
#define PORT_SPEED_SHIFT                9
#define PORT_TEST                       (1 << 11)   // Port Test Control
#define PORT_INDICATOR                  (1 << 12)   // Port Indicator Control
#define PORT_CONNECTION_CHANGE          (1 << 16)   // Connect Status Change
#define PORT_ENABLE_CHANGE              (1 << 17)   // Port Enable Change
#define PORT_OVER_CURRENT_CHANGE        (1 << 19)   // Over-current Change


extern EHCI_MEM_T *ehci_driver;


extern EhciCMD *cmd1;
extern EHCI_TD *td1;
extern EHCI_TD *td2;
extern EHCI_TD *td3;
extern EHCI_QH *qh1;
extern EHCI_QH *qh2;
extern EHCI_QH *qh3;
extern void ehci_memset();


unsigned char* usb_stick_send_and_recieve_scsi_command(EHCI_MEM_T *device,unsigned char* out,unsigned long expectedIN,unsigned long expectedOut, int portnumber);
int usb_hub_init()
{

	printf("USB HUB install\n");
	
	struct usb_hub_desc hubdesc;
	memset(&hubdesc,0, sizeof (hubdesc));
	
	usb_control_msg (ehci_driver, 1, 0xa0, 0x6,  0x29 << 8, 0, sizeof(hubdesc), &hubdesc);
	printf("Hub descriptor: len:%d, typ:0x%x, cnt:%d, char:0x%x, pwg:%d, curr:%d\n",
                hubdesc.length, hubdesc.type, hubdesc.portcnt,
                hubdesc.characteristics, hubdesc.pwdgood,
                hubdesc.current);
         
      	printf("hubdesc.variable[] = %x,%x\n",hubdesc.variable[0],hubdesc.variable[1]);    
                
   	
   	// ehci_set_device_configuration hub;
   	usb_control_msg (ehci_driver, 1, (0 << 5), 0x9, 1, 0, 0,0);
   	
   	//Power on all Hub ports.
   	for(int i=1; i <= hubdesc.portcnt; i++ )
   	{
   		printf("Power on - port %d\n", i);
   		//Power on the port and wait for possible device connect
      		usb_control_msg (ehci_driver, 1, 0x23, 0x3,  8, i, 0,0);
   	
   	}
   	//wait();
   	
   	for(int i=1; i <= hubdesc.portcnt; i++ )
   	{
   		// Reset ports
   		if(usb_control_msg (ehci_driver, 1, 0x23, 0x3, 4, i, 0,0))
   		{
   			return 0;
   		}
   		
   		unsigned int status;
   		// Wait (2ms*hubdesc.pwdgood) = 100ms
   		wait(1000000);
   		usb_control_msg (ehci_driver, 1, 0xa3, 0,  0, i, 4, &status);
   		
   		if(status&1){
   		
   			printf("Hub Port%d A device is present on this port, status: %x\n",i,status);
   			
   			if(status&1){
   				printf("PORT_ENABLE\n");
   				
   				struct usb_device_desc desc;
   				memset(&desc,0, sizeof (desc));
   				
   				usb_control_msg (ehci_driver, 0, 
   				0x80, 0x6,  0x01 << 8 , 0, sizeof (desc), &desc);
   				
   				printf("length %x, type 0x%x, bcdusb 0x%x, class 0x%x\n",
   				desc.length,desc.type,desc.bcdusb,desc.class);
   				
   				printf("subclass 0x%x, protocol 0x%x, maxpacketsize 0x%x\n",
   				desc.subclass,desc.protocol,desc.maxpacketsize);
   				
   				printf("vendorid 0x%x, productid 0x%x, devicever 0x%x\n",
   				desc.vendorid,desc.productid,desc.devicever);
   				
   				printf("vendorstr 0x%x, productstr 0x%x, serialstr 0x%x,\
   				confcount 0x%x\n",
   				desc.vendorstr,desc.productstr,desc.serialstr,desc.confcount);
   			
   			
   				
   			}else {
   			
   				printf("PORT_DISABLE\n");
   			}
   		
   		}else{
   			printf("Hub Port%d No device is present on this port, status: %x\n",i,status);
   		}
   		
   		
   		
   	
   	} 	
   	
 	return 0;  	
}


int usb_control_msg (EHCI_MEM_T *controller, int port, 
			int bRequestType,
                     	int bRequest,
			int wValue,
    			int wIndex, int wLength, void *data){
	ehci_memset();

    	// first make the command
    	EhciCMD *setup = (EhciCMD*) cmd1;
    	setup->bRequest = bRequest;
    	setup->bRequestType = bRequestType;
    	setup->wIndex = wIndex;
    	setup->wLength = wLength;
    	setup->wValue = wValue;

    	EHCI_TD *command = (EHCI_TD*) td1;
    	EHCI_TD *status = (EHCI_TD*) td2;
    	EHCI_QH *head1 = (EHCI_QH*) qh1;
    	EHCI_QH *head2 = (EHCI_QH*) qh2;

    	
    	ehci_int_td( command, status, 0, 2/*type is setup*/, 8, setup);
    	
    	ehci_int_td( status, (EHCI_TD *)1, 1, 1/*PID instellen*/, wLength, data);
    	
    	ehci_init_qh(head1, head2, 0, command, 0, 2, port, 0, 64);
    	
    	controller->asynclistaddr = ((unsigned long)head1);
    	ehci_asynchronous_schedule_enable (controller);
    	
    	int r = ehci_wait_for_completion(status);
    
    	ehci_asynchronous_schedule_disable (controller);
    	controller->asynclistaddr = 1;
   
	
	return r;
    			
}




int ehci_recieve_bulk(EHCI_MEM_T *mmio,unsigned long expectedIN,void *buffer, int portnumber)
{
	
	EHCI_QH* head1 = qh1; 
    	EHCI_QH* head2 = qh2;
    	EHCI_TD* trans = td1;
    
    	ehci_memset();    
    
        trans->nextlink = 1;
    	trans->altlink = 1;
        trans->token |= (expectedIN << 16);
        trans->token |= (1 << 31); // toggle
        trans->token |= (1 << 7); // actief
        trans->token |= (1 << 8); // IN token
        trans->token |= (0x3 << 10); // maxerror
        trans->buffer[0] = (unsigned long)buffer;
    
        
         // Tweede commando
    	head2->altlink = 1;
    	head2->nextlink = (unsigned long)trans; // qdts2
    	head2->qhlp = ((unsigned long)head1) | 2;
    	head2->curlink = 1; // qdts1
    	head2->ch |= 1 << 14; // dtc
    	head2->ch |= 512 << 16; // mplen
    	head2->ch |= 2 << 12; // eps
    	head2->ch |= 1 << 8; // endpoint 1
    	head2->ch |= portnumber; // device
    	head2->caps = 0x40000000;

    	//
    	// Eerste commando
    	head1->altlink = 1;
    	head1->nextlink = 1;
    	head1->qhlp = ((unsigned long)head2) | 2;
    	head1->curlink = 0;
    	head1->ch = 1 << 15; // T
    	head1->token = 0x40; 
        
        
      	mmio->asynclistaddr = ((unsigned long)head1) ;
    	mmio->cmd |= 0b100000;

    	int r = ehci_wait_for_completion(trans);
    
    	mmio->cmd &= ~0b100000;
    	mmio->asynclistaddr = 1;

	return r;
}


int ehci_send_bulk(EHCI_MEM_T *mmio,unsigned char* out,unsigned long expectedOut, int portnumber){
    	//
    	// Send bulk
    	EHCI_TD *command = td1;
    	EHCI_QH *head1 = qh1;
    	EHCI_QH *head2 = qh2;
    
    	ehci_memset();
    
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
    	head2->qhlp = ((unsigned long)head1) | 2;
    	head2->curlink = 0; // qdts1
    	head2->ch |= 512 << 16; // mplen
    	head2->ch |= 2 << 12; // eps
    	head2->ch |= portnumber;
    	head2->ch |= 2 << 8;// device->endpointBulkOUT << 8; // endpoint 2
    	head2->caps = 0x40000000;
    	
    //	printf("[EHCI] BULK: Sending %x \n",head2->characteristics); //  heeft = 0x2006201 moet = 0x2002201

    	//
    	// Eerste commando
    	head1->altlink = 1;
    	head1->nextlink = 1;
    	head1->qhlp = ((unsigned long)head2) | 2;
    	head1->curlink = 0;
    	head1->ch = 1 << 15; // T
    	head1->token = 0x40;

    
    	mmio->asynclistaddr = ((unsigned long)head1);
    	mmio->cmd |= 0b100000;
    	
    	int r = ehci_wait_for_completion(command);
    
    	mmio->cmd &= ~0b100000;
    	mmio->asynclistaddr = 1;


    
    	return r;
}


unsigned char* usb_stick_send_and_recieve_scsi_command(EHCI_MEM_T *device,unsigned char* out,unsigned long expectedIN,unsigned long expectedOut, int portnumber)
{

	int r;
	
	r = usb_control_msg (device, portnumber, 0x30, 0, 0, 1, expectedOut, out);
	//r =  ehci_send_bulk(device, out, expectedOut, portnumber);
	
	if(r) {
	
		printf("[SMSD] Sending bulk error\n");
		return 0;
	
	}
	
	unsigned char *buffer = malloc(expectedIN);
	memset(buffer,0,expectedIN);
	r = ehci_recieve_bulk(device,expectedIN,buffer, portnumber);
	if(r) {
	
		printf("[SMSD] Recieving bulk error\n");
		return 0;
	
	}


	CommandStatusWrapper *csw = (CommandStatusWrapper*)buffer;
	
	printf("csw->signature %x, csw->status %x\n",csw->signature,csw->status);
	
	
	if(csw->signature!=USB_STORAGE_CSW_SIGN){
		unsigned char* cuv = malloc(0x1000);
		r = ehci_recieve_bulk(device,13,cuv, portnumber);
		
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
		
		r = ehci_recieve_bulk(device,csw->dataResidue,buffer,portnumber);
		
		if((unsigned long)buffer== 0){printf("D");
			return (unsigned char *) 0;
		}
	}
	return buffer;
}
