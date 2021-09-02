#include <window.h>
#include <string.h>
#include <stdio.h>

#define OBJ_MAX 256

extern long	obj_list;
unsigned long	ptr_obj[OBJ_MAX];


static MENU_T *objm;
static HANDLE_T *obj;


static int foc;

int init_process(HANDLE_T *o)
{
	objm = (MENU_T *) 0;
	obj = (HANDLE_T *) 0;
	
	msg_init();
	
	foc = 0;
	if(o != 0) {
	
		foc =  o->type;
		obj = (HANDLE_T *) o;
	}
	
	return 0;
}

int obj_focprocess()
{
	switch(foc){
		case TYPE_EDITBOX:
			if(obj == 0) break;
			m_edit(obj);
		break;
		case TYPE_LISTBOX:
			if(obj == 0) break;
			printf("none\n");
		break;
		case TYPE_FILE_LISTBOX:
			if(obj == 0) break;
			m_file_list(obj);
		break;
	}

	return 0;
}

int obj_process(int x, int y)
{
	if(obj_list == 0 || x < 0 || y < 0) { 
		while(mouse2->b&0x1); // espera soltar
		return -1;
	}
	
	
	unsigned int *type = 0;
	WINDOW *w = 0;
	int idx = 0;
	int ret = 0;
	int x1, x2, y1, y2;
	
	int i = 0;
	
	for(i = 0; i < obj_list; i ++) {
	
		type = (unsigned int*) ptr_obj[i];
		
		x1 = type[1];
		y1 = type[2];
		x2 = type[3];
		y2 = type[4];
		
		idx = i;
		
		if(x > x1 && y > y1 &&  x < (x1+x2) && y < (y1+y2) ) { 
			break;
		}
		
	}
	
	if(i == obj_list) {
	
		while(mouse2->b&0x1); // espera soltar
		return -1;
	}
	
	switch(*type) {
		case TYPE_MENUBOX:
			objm = (MENU_T *) ptr_obj[idx];
			w = (WINDOW*) objm->w;
			
			msg_write (objm->id); // enviar mensagem
			
			ret = menumotor(objm);
		break;
		case TYPE_EDITBOX:
			obj = (HANDLE_T *) ptr_obj[idx];
			w = (WINDOW*) obj->w;
			
			msg_write (obj->id); // enviar mensagem
			
			ret = 0;
			
			foc =  TYPE_EDITBOX;
			
			while(mouse2->b&0x1);
		break;
		case TYPE_FILE_LISTBOX:
			obj = (HANDLE_T *) ptr_obj[idx];
			w = (WINDOW*) obj->w;
			
			msg_write (obj->id); // enviar mensagem
			
			ret = 0;
			
			foc =  TYPE_FILE_LISTBOX;

			while(mouse2->b&0x1);
		break;
		case TYPE_LISTBOX:
			obj = (HANDLE_T *) ptr_obj[idx];
			w = (WINDOW*) obj->w;
			
			msg_write (obj->id); // enviar mensagem
			
			ret = 0;
			
			foc =  TYPE_LISTBOX;
			
			while(mouse2->b&0x1);
		break;
		default:
			while(mouse2->b&0x1); // espera soltar
			ret = 0;
		return -1;
	
	}

	if(ret != 0) {
		while(w->spinlock)__asm__ __volatile__("pause;");
		w->spinlock++;
		
		update_objs(w);
		
		w->spinlock = 0;
	}
	

	return 0;
}


int register_obj(void *obj) {

	if(obj_list == 0) {
		// firts
		memset(ptr_obj, 0, sizeof(unsigned long)*OBJ_MAX);
	}
	
	if(obj_list >= OBJ_MAX ) return 0;
	
	ptr_obj[obj_list++] = (unsigned long) obj;
	
	
	return obj_list;	
}


void update_objs(WINDOW *w)
{
	unsigned int *type = 0;
	//limpar a area
	drawline(w->area_x , w->area_y, w->area_width , w->area_height, w->bg, w);
	
	for(int i = 0; i < obj_list; i ++) {
	
		type = (unsigned int*) ptr_obj[i];
		
		if(*type == TYPE_MENUBOX ) continue;
		
		switch(*type) {
			case TYPE_EDITBOX:
				obj = (HANDLE_T *) ptr_obj[i];
				update_editbox(obj);
			
			break;
			default:
			break;
	
		}
		
	}
}


