#include <io.h>
#include <ps2.h>
#include <irq.h>
#include <stdio.h>

// Algumas macros define para o keyboard
#define ESC 0x1B
#define	TAB '\t'
#define ENTER '\n'
#define	SPACE ' '
#define BACKSPACE '\b'

#define KEY_F1 		(0x3B)
#define KEY_F2 		(KEY_F1 +1)
#define KEY_F3 		(KEY_F2 +1)
#define KEY_F4 		(KEY_F3 +1)
#define KEY_F5 		(KEY_F4 +1)
#define KEY_F6 		(KEY_F5 +1)
#define KEY_F7 		(KEY_F6 +1)
#define KEY_F8 		(KEY_F7 +1)
#define KEY_F9 		(KEY_F8 +1)
#define KEY_F10		(KEY_F9 +1)
#define KEY_F11		(0x57)
#define KEY_F12 		(KEY_F11 +1)

#define NUMELOCK 0
#define SCROLLOCK 0
/*
#define DEL 0x91
#define HOME 0x92
#define END 0x93
#define PGUP 0x94
#define PGDN 0x95
#define UP 0x96
#define LEFT 0x97
#define DOWN 0x98
#define RIGHT 0x99
*/

#define ALT	0
#define CTRL	0
#define LSHIFT	0
#define RSHIFT	0
#define PRTSCR	0

#define CAPSLOCK 0


unsigned char keyboard_charset = 0;

// Algumas variáveis estática, para o controle 
static int shift = 0;
static int caps_lock = 0;

unsigned char key_buffer[2];
int count = 0;


// Este é o nosso mapa de caracters minúsculas
const char ascii_minusculas[256] = {
	0/*NULL*/,ESC,'1','2','3','4','5','6','7','8','9','0','-','=',BACKSPACE,
	TAB,'q','w','e','r','t','y','u','i','o','p','[',']',ENTER,
	0,'a','s','d','f','g','h','j','k','l',';','\'','`',
	0,'\\','z','x','c','v','b','n','m',',','.','/',0,0,0,
	SPACE,
};


// Este é o nosso mapa de caracters maiúsculas
const char ascii_maiusculas[256] = {
	0/*NULL*/,ESC,'!','@','#','$','%','^','&','*','(',')','_','+',BACKSPACE,
	TAB,'Q','W','E','R','T','Y','U','I','O','P','{','}',ENTER,
	0,'A','S','D','F','G','H','J','K','L',':','"','~',
	0,'|','Z','X','C','V','B','N','M','<','>','?',0,0,0,
	SPACE,
};




// Esta função será usada para ler dados do teclado na porta 0x60, fora do IRQ1
unsigned char keyboard_read()
{
	kbdc_wait(0);
	return inportb(0x60);

}


// Esta função será usada para escrever dados do teclado na porta 0x60, fora do IRQ1
void keyboard_write(unsigned char write){

	kbdc_wait(1);
	outportb(0x60,write);
}


// Esta rotina faz o Auto-teste 0xaa êxito, 0xfc erro
int KEYBOARD_BAT_TEST(){
    
    	unsigned char val;

    	for(;;) {

        val = keyboard_read();

        if(val == KBDC_OK)return 0;
        else if(val == KBDC_ERROR) {
        
            return -1; 
        }
    
        // Reenviar o comando. 
        // OBS: este comando não é colocado em buffer
        keyboard_write(KBDC_RESEND);       
        

        }

}


void keyboard_install()
{

	// set current scan code set
	//keyboard_write(0xF0);
	// Set scan code set 2
	//keyboard_write(2);
	//keyboard_read();  // ACK

    	// IRQ set Handler
	// Enable IRQ Interrup
	irq_enable(1);
}


// Este é o nosso manipulador do mouse no vetor IDT
// TODO: 
// O Bit 7 (0x80) na scancode lido, nos informa se a tecla foi precionada ou solta  		
// Exemplo: O shift left quando precionado gera o scancode 0x2A quando solto 0xAA (0x2A + 0x80 = 0xAA)
void keyboard_handler(void)
{
	
	if(count >= 2)count = 0;

    	unsigned char scancode = inportb(0x60);

	key_buffer[count++] = scancode;
	
    	if(scancode & 0x80) {

        	if((scancode == 0xAA) || (scancode == 0xB6))
        	shift = 0;

    	} else {
            	if((scancode == 0x2A) || (scancode == 0x36)) {
            		shift = 1;
            		return;
            	}
    	}


	
    	if(scancode &0x80 || (scancode == 0x45)) {
    
        	

    	} else {

		
		
         	if( (shift == 1 ) || (caps_lock == 1)) {
                	keyboard_charset = ascii_maiusculas[scancode];


        	} else {

                	keyboard_charset = ascii_minusculas[scancode];
			
            	}

		//printf("%c",keyboard_charset);

        }	

 
}

