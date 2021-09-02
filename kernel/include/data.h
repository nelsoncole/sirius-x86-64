#ifndef __DATA_H
#define __DATA_H

#include <stdio.h>

extern unsigned char dv_num;
extern unsigned int dv_uid;

extern unsigned long kernelmouse;

typedef struct _user{
	unsigned long mouse;
	unsigned long clock;
}user_t; 

extern user_t *user;

//MOUSE


typedef struct _mouse {

	int	x;
	int	y;
	int	z;
	unsigned int	b;
	
	unsigned int wx, wy;

}__attribute__((packed)) mouse_t;
extern mouse_t *mouse;
extern mouse_t *mouse2;


extern FILE *pipe_launcher;

extern unsigned int interrupt_status;

#endif
