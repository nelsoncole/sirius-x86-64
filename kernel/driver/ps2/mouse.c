#include <io.h>
#include <ps2.h>
#include <stdlib.h>
#include <gui.h>
#include <irq.h>
#include <stdio.h>
#include <string.h>

#include <data.h>
#include <i965.h>

#define SINAL_N -
#define SINAL_P +



const char *id_mouse_strings[]={
    "Default",
    "Unknown",
    "Unknown",    
    "IntelliMouse"
};


mouse_t *mouse;
mouse_t *mouse2;

unsigned char 	buttons;
int  		mouse_x;
int  		mouse_y;
int 		count_mouse;
int 		first_time;


// Algumas variáveis de controle
int speed = 1;
int mouse_position = 0;

static void mouse_refresh();


// Esta função será usada para escrever dados do mouse na porta 0x60, fora do IRQ12
void mouse_write(unsigned char write){

	kbdc_wait(1);
	outportb(0x64,0xD4);
	kbdc_wait(1);
	outportb(0x60,write);

}


// Esta função será usada para ler dados do mouse na porta 0x60, fora do IRQ12
unsigned char mouse_read(){

    
	kbdc_wait(0);
	return inportb(0x60);
}

// Esta rotina faz o Auto-teste 0xaa êxito, 0xfc erro
int MOUSE_BAT_TEST(){
    
    	unsigned char val;

    	while(1){

        	val = mouse_read();

        	if(val == KBDC_OK)return 0;
        	else if(val == KBDC_ERROR) {
       			return -1; 
       		}
    
        	// Reenviar o comando. 
        	// OBS: este comando não é colocado em buffer
        	mouse_write(KBDC_RESEND);       
        

        }

}


// Este é o manipulador do IRQ12
void mouse_handler(void) {

	switch(count_mouse++) 
	{
		case 0: 
			buttons = inportb(0x60);
			break;
		case 1:
			if(buttons & 0x10) // negative
				mouse_x = (int) inportb(0x60) | 0xFFFFFF00;
			else
				mouse_x = (int) inportb(0x60);
			break;
		case 2:
			if(buttons & 0x20) // negative
				mouse_y = (int)  inportb(0x60) | 0xFFFFFF00;
			else
				mouse_y = (int) inportb(0x60);
			
			if( (buttons & 0x80) || (buttons & 0x40) == 0 ) { 
				// x/y overflow ?
				mouse_refresh();	         
			}

			count_mouse = 0;

			break;
		default:
			inportb(0x60);
			count_mouse = 0;
			break;
	}	

}
// Actualizador do ponto de acomulação do mouse
static void mouse_refresh(){

/*                                      Mouse default (0xF6)
 *
 *   _____________________________________________________________________________________________
 *  { overflow y | overflow x | signal y | signal x| always 1 | middle btn | right btn | left btn }
 *  {____________|____________|__________|_________|__________|____________|___________|__________}
 *  {                                       Delta X                                               }
 *  {_____________________________________________________________________________________________}
 *  {                                       Delta Y                                               }
 *  {_____________________________________________________________________________________________}
 *
 *
 */    

	mouse->x += mouse_x;// coordenada X
	mouse->y -= mouse_y; // Coordenada Y
	mouse->z += 0;
	mouse->b = buttons;
	
	if(mouse->x < 0) mouse->x =0;
	else if(mouse->x > gui->horizontal_resolution) mouse->x = gui->horizontal_resolution;
	
	if(mouse->y < 0) mouse->y =0;
	else if(mouse->y > gui->vertical_resolution ) mouse->y = gui->vertical_resolution;
	
    //clears_creen();	
    //printf("(%d,%x,%d) ",mouse_x, mouse->b ,mouse->y );
     	
    update_graphic_cursor(mouse->x, mouse->y);
  
     	
    // Enviar dados do mouse para o aplicativo
    memcpy(mouse2, mouse, sizeof(mouse_t));
     	
}

// Instalação do mouse
void mouse_install()
{
	unsigned char tmp;
	

	mouse_x = 0;
	mouse_y = 0;
	buttons = 0;
	count_mouse= 4; // default, isto vai ajustar o mouse
	first_time = 1;
	
	mouse = (mouse_t *) malloc(sizeof(mouse_t));
	memset(mouse,0,sizeof(mouse_t));
	
	kernelmouse = (unsigned long) malloc(sizeof(mouse_t));
	mouse2 = (mouse_t *) kernelmouse;
	memset(mouse2,0,sizeof(mouse_t));
	
	mouse->x = gui->horizontal_resolution/2;
	mouse->y = gui->vertical_resolution/2;
	mouse->wx = gui->horizontal_resolution/2;
	mouse->wy = gui->vertical_resolution;
	
	// IRQ set handler
	fnvetors_handler[12] = &mouse_handler;
	// Enable IRQ Interrupt
	
	
	//Defina a leitura do byte actual de configuração do controlador PS/2
	kbdc_wait(1);
  	outportb(0x64, 0x20);
 
	// Enable the interrupts
	// Second PS/2
	kbdc_wait(0);
	tmp = inportb(0x60);

	tmp |= 2;

	// defina, a escrita  de byte de configuração do controlador PS/2
	kbdc_wait(1);
	outportb(0x64, 0x60);

	kbdc_wait(1);
  	outportb(0x60, tmp);

	// activar a segunda porta PS/2
	kbdc_wait(1);
	outportb(0x64,0xA8);
	kbdc_wait(0);

	
	mouse_write(KBDC_RESET);
	kbdc_wait_ack(); // ACK 

	// Tell the mouse to use default settings
	mouse_write(MOUSE_DEFAULT);
	kbdc_wait_ack(); // ACK 

	/*
	// Enable Z axis
	mouse_write(0xF3);
	mouse_wait_ack(); // ACK
	
	
	mouse_write(200);
	mouse_wait_ack(); // ACK
	
	
	mouse_write(0xF3);
	mouse_wait_ack(); // ACK
	
	
	mouse_write(100);
	mouse_wait_ack(); // ACK
	
	
	mouse_write(0xF3);
	mouse_wait_ack(); // ACK
	
	
	mouse_write(0x80);
	mouse_wait_ack(); // ACK


	mouse_write(0xF2);
	mouse_wait_ack(); // ACK
	*/

	// set sample rate
	mouse_write(0xF3);
	kbdc_wait_ack(); // ACK
	
	mouse_write(200);
	kbdc_wait_ack(); // ACK

	// set resolution
	mouse_write(MOUSE_SET_RESOLUTION);
	kbdc_wait_ack(); // ACK
	
	mouse_write(3);
	kbdc_wait_ack(); // ACK

	
	//set scaling 1:1
	mouse_write(0xE6);
	kbdc_wait_ack(); // ACK
	
	//set scaling 2:1
	//mouse_write(0xE7);
	//kbdc_wait_ack(); // ACK

	// Enable the mouse
	mouse_write(ENABLE_MOUSE);
	kbdc_wait_ack(); // ACK
	

}

