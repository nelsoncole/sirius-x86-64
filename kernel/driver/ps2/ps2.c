#include <io.h>
#include <ps2.h>

#include <sleep.h>


// Esta função é usada para a calibragem do IBF e OBF
// Se a entra é 0 = IBF, se entrada é 1 = OBF

void kbdc_wait(int type)
{
	int spin = 10000;
      	if(type == 0) {
		   
           	while(!(inportb(0x64)&1) && spin--) {
             		outanyb(0xed);
             		
             		if(spin < 0) break;
             	}

        } else if (type == 1) {
			
             	while(inportb(0x64)&2 && spin--) {	
             		outanyb(0xed);
             		
             		if(spin < 0) break;
            	}
            	

      	}
      	
}

void kbdc_set_cmd(int val)
{
	kbdc_wait(1);
  	outportb(0x64, val);

}

static unsigned char kbdc_read(){

    
	kbdc_wait(0);
	return inportb(0x60);
}

void kbdc_wait_ack()
{
	int spin = 100;
	unsigned char ack = kbdc_read();
	while(ack != 0xFA && spin --) {
		ack = kbdc_read();
		
		if(spin < 0) break;
	}
		
	//printf("%x ",ack);

}
