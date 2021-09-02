#include <window.h>

MSG_T msg;
 
int getmsg(int msg, WINDOW *w) {


	// esta funcao deve mudar de nome
	fmouse(w);
	
	
	int id = msg_read();
	
	return id;
}


void msg_write (int id) 
{
	if(msg.offset1 >= MSG_MAX) {
	
		// deslocar os dados para o inicio
		msg.offset2 = 0;
		msg.offset1 = 0;
		msg.size = 0;
		
	}
	
	msg.buf[msg.offset1++] = id;
	
	msg.size ++;

}

int msg_read()
{
	int id;
	
	if(msg.size < 1) return 0;
	
	if(msg.offset2 >= MSG_MAX) 
		msg.offset2 = 0;
	
	id = msg.buf[msg.offset2];
	
	msg.buf[msg.offset2++] = 0;
	
	return id;
}

void msg_init()
{
	msg.offset2 = 0;
	msg.offset1 = 0;
	msg.size = 0;

}
