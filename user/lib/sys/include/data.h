#ifndef __DATA_H
#define __DATA_H

typedef struct _user{
	unsigned long mouse;
	unsigned long clock;

}__attribute__((packed)) user_t; 

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

#endif
