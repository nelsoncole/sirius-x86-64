#ifndef __PS2_H
#define __PS2_H

#define KBDC_RESET 0xFF
#define KBDC_RESEND 0xFE
#define KBDC_ERROR 0xFC
#define KBDC_ACK 0xFA
#define KBDC_ID 0xF2
#define KBDC_OK 0xAA

#define KEYBOARD_BUFFER_SIZE 2

void kbdc_wait(int type);
void kbdc_set_cmd(int val);

//MOUSE


typedef struct _mouse {

	int	x;
	int	y;
	int	z;
	unsigned int	b;
	unsigned int	handle;

}__attribute__((packed)) mouse_t;


#define MOUSE_DEFAULT 0xF6
#define ENABLE_MOUSE 0xF4
#define MOUSE_SET_RESOLUTION 0xE8
void mouse_handler(void);
void mouse_install();

//KEYBOARD
#define KBDC_LED 0xED

void keyboard_install();
void keyboard_handler(void);






#endif
