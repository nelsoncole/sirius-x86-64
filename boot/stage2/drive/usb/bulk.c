#include <ehci.h>
#include <stdio.h>

int ehci_wait_for_completion(EHCI_TD *td)
{
	unsigned ticks = 10000;
	
	while(1)
	{
		volatile unsigned int status = (volatile unsigned int)td->token;
	
		if(status & (1 << 4)){
			// not anymore active and failed miserably
            		printf("[EHCI] Transmission failed due babble error\n");
            		return 1;
		}
		
		if(status & (1 << 3)){
			// not anymore active and failed miserably
            		printf("[EHCI] Transmission failed due transaction error\n");
            		return 2;
		}
		
		if(status & (1 << 6)){
		
			// not anymore active and failed miserably
            		printf("[EHCI] Transmission failed due serious error\n");
            		return 3;
		
		}
		if(status & (1 << 5)){
            		// not anymore active and failed miserably
            		printf("[EHCI] Transmission failed due data buffer error\n");
		    	return 4;
        	}
       		if(!ticks--){
            		printf("EHCI FAIL Timeout\n");
			return 0;
        	}
        	if(!(status & (1 << 7))){
            		// not anymore active and succesfull ended
            		return 0;
        	}

	}
   
    	return -1;
}

void ehci_int_td( EHCI_TD *td, EHCI_TD *nextlink, unsigned int toggle, unsigned int packettype,
unsigned int len, const void *data) {

    	td->nextlink = (unsigned long)nextlink;
    	td->altlink = 1;

	if(packettype == 0) 
	{
		//printf("type is out token\n");
		td->token = (toggle << 31) | (len << 16) | (3 << 10) | (0 << 8) | (1 << 7);
	}
	else if(packettype == 1)
	{
		//printf("type is in token\n");
		td->token = (toggle << 31) | (len << 16) | (3 << 10) | (1 << 8) | (1 << 7);
	}
	
	else if(packettype == 2)
	{
		// printf("type is setup token\n");
		td->token = (toggle << 31) | (len << 16) | (3 << 10) | (2 << 8) | (1 << 7);
	}


    	td->buffer[0] = (unsigned int) data;
    	td->extbuffer[0] = (unsigned int)0;//(data >> 32);
    	
     
}


void ehci_init_qh(EHCI_QH *sqh1, EHCI_QH *sqh2,EHCI_QH *curlink, EHCI_TD *std, int interrupt, 
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
	sqh2->qhlp = (unsigned int)sqh1 |2;
    	sqh2->ch = ch;	
    	sqh2->caps = caps;
    	sqh2->curlink = (unsigned int)curlink;
    	
    	sqh2->nextlink = (unsigned int)std;
    	sqh2->altlink = 1;

    	sqh2->token = 0;
    	
    	 // Eerste commando
    	
   	sqh1->qhlp  = ((unsigned long)sqh2) | 2;
   	sqh1->ch = ch;	
   	sqh1->ch |= 1 << 15; // T
    	sqh1->caps = caps;
    	sqh1->curlink = 0;
   	
   	sqh1->altlink = 1;
    	sqh1->nextlink = 1;
    	sqh1->token = 0x40; // IOC
    	
}

