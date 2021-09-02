#include <pic.h>
#include <io.h>

void pic_install(void)
{

	// Envia ICW1 reset
	outportb(0x20,0x11);	// reset PIC 1
	outportb(0xA0,0x11);	// reset PIC 2

	// Envia ICW2 start novo PIC 1 e 2
	outportb(0x21,0x20);	// PIC 1 localiza no IDT 38-32 
	outportb(0xA1,0x28);	// PIC 2 localiza no IDT 47-40

	// Envia ICW3
	outportb(0x21,0x04);	// IRQ2 conexao em cascata com o PIC 2
	outportb(0xA1,0x02);
	
	// Envia ICW4
	outportb(0x21,0x01);
	outportb(0xA1,0x01);

	// Desabilita todas as interrupcoes
	// OCW1
	outportb(0x21,0xFB); // IRQ2
	outportb(0xA1,0xFF);

}
