#include <io.h>
#include <ps2.h>
#include <stdlib.h>
#include <gui.h>
#include <irq.h>
#include <stdio.h>
#include <string.h>

#define SINAL_N -
#define SINAL_P +



const char *id_mouse_strings[]={
    "Default",
    "Unknown",
    "Unknown",    
    "IntelliMouse"
};


mouse_t *mouse;

unsigned char 	buttons;
int  		mouse_x;
int  		mouse_y;
int 		count_mouse;
int 		first_time;


// Algumas variáveis de controle

static int largura_da_tela = 1024;
static int altura_da_tela  = 768;
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



// Instalação do mouse
void mouse_install()
{
	unsigned char tmp;

	mouse_x = 0;
	mouse_y = 0;
	buttons = 0;
	count_mouse= 0;
	first_time = 1;

	
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

	kbdc_wait(1); //espera


	mouse_write(KBDC_RESET);
	mouse_read();  // ACK

	// Tell the mouse to use default settings
	mouse_write(MOUSE_DEFAULT);
	mouse_read();  // ACK

	// Set resolution
	/*kbdc_wait(1);
	outportb(0x64,MOUSE_SET_RESOLUTION);
	kbdc_wait(0);
	tmp = inportb(0x60);
	tmp |= 0x2;
	kbdc_wait(1);
	outportb(0x60, tmp);	*/

	// Enable the mouse
	mouse_write(ENABLE_MOUSE);
	mouse_read();  // ACK


	mouse = (mouse_t *) malloc(sizeof(mouse_t));
	memset(mouse,0,sizeof(mouse_t));
	
	mouse->x = 0x200;
	mouse->y = 0x100;
	
	// IRQ set handler
	// Enable IRQ Interrupt

	irq_enable(12);

}


// Este é o manipulador do IRQ12
void mouse_handler(void) {
    	// DICA DO TIO FRED
    	// Ao que parce ignorar a leitura do primeiro byte
    	// coloca nossos dados na ordem status, x, y

	if (first_time != 0) {
        	first_time = 0;
        	inportb(0x60);
		return;
    	}

	static unsigned char status;
	status = inportb(0x64);
    	if ((status & 0x01) && (status & 0x20)) {

		switch(count_mouse++) {
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
					mouse_y = (int) mouse_read() | 0xFFFFFF00;
				else
					mouse_y = (int) inportb(0x60);
				
				if( (buttons & 0x80) || (buttons & 0x40) == 0 ) { 
					// x/y overflow ?

					mouse_refresh();	
         
        			}


				count_mouse = 0;

			break;
			default:
				count_mouse = 0;
			break;
		}

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
 
 	largura_da_tela = gui->width;
	altura_da_tela	= gui->height;

	mouse->x += mouse_x;// coordenada X
	mouse->y -= mouse_y; // Coordenada Y
	mouse->z += 0;
	mouse->b = buttons;

	
	if(mouse->x < 0) mouse->x = 0;
     	else if(mouse->x > largura_da_tela) mouse->x = largura_da_tela;
     	if(mouse->y < 0) mouse->y = 0;
     	else if(mouse->y >altura_da_tela ) mouse->y = altura_da_tela;

	
    	//	clears_creen();	
     	// printf("(%d,%x,%d) ",mouse_x, mouse->b ,mouse_y );
     	
     	
     	//*(unsigned int*)(gtt->mmio_base + 0x70088) = (mouse->x &0x7fff) | (mouse->y &0x7fff) << 16;
     	
}


