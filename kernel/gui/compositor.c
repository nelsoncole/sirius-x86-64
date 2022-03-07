#include <mm.h>
#include <gui.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#include <data.h>

#include <i965.h>

#define TASK_BARRA 36

#define SSE_MMREG_SIZE 16

extern unsigned long timer_ticks;
int rf;
extern char cursor18x18[18*20];
extern void copymem(void *s1, const void *s2, unsigned long n);

extern void *sse_memcpy(void * s1, const void * s2, size_t len);

PAINT *paint, *paint_ready_queue, *paint_foco;

extern mouse_t *mouse;
unsigned long gid;

unsigned long window_count;
extern int launcher;
int screan_spin_lock;
extern int w_remove_spinlock;
static void *trans_memcpy(void *s1, const void *s2, unsigned n)
{
    unsigned p = n / 4;
    unsigned int *dst = (unsigned int *)s1;
    unsigned int *src = (unsigned int *)s2;
    while( p --)
    {
        if( (*src&0x80000000) ){
            
            unsigned char *rgb = (unsigned char *)dst;
            unsigned char k = 0;

            // tom de cinza
            // Luminosidade = R*0.3 + G*0.59 + B*0.11
            /*k = ( ((rgb[2] * 30 ) / 100) + ((rgb[1] * 59 ) / 100) + ((rgb[0] * 11 ) / 100) );
            rgb[0] = k;
            rgb[1] = k;
            rgb[2] = k; */

            // Luminosidade de qualquer cor é dada pela multiplicação por uma constante
            // k (R, G, B) = (kR, kG, kB)
            // aqui podemos dividir para baixar a luminosidade, obtendo a transparência  
            k = 3;
            rgb[0] = rgb[0]/k;
            rgb[1] = rgb[1]/k;
            rgb[2] = rgb[2]/k;

            dst ++;
            src ++;
        } else {
            *dst++ = *src++;
        }
    }
    return s1;
}

void window_add(unsigned long addr) {

	PAINT *new = (PAINT*) malloc(sizeof(PAINT));//(addr + 0x1000 - 32);
	WINDOW *w = (WINDOW*) addr;
	
	while(paint_ready_queue->spinlock);
	paint_ready_queue->spinlock++;


	// tmp aponta para inicio da lista
    PAINT *tmp = paint_ready_queue;
    // verificar se ja esta registrada
    while (tmp) {
    	
        if(tmp->w == addr)
        {
        	paint_ready_queue->spinlock = 0;
        	return;
        }
    	
        tmp = tmp->next;
    }
    	
    memset(new, 0, 32);
	new->w = addr;
	new->next = 0;
	
	w->gid = gid ++; 
    	
    	
    tmp = paint_ready_queue;
    while (tmp->next) {
    	
    	tmp = tmp->next;
    }
    	
    tmp->next = new;
    paint_foco = new;
    window_count ++;
    paint_ready_queue->spinlock = 0;
}

void window_remove(unsigned long addr) {

    if(window_count < 1) return; 

	//WINDOW *w = (WINDOW*) addr;
	
	while(paint_ready_queue->spinlock != 0){}
	paint_ready_queue->spinlock++;

    while(w_remove_spinlock != 0){}
    w_remove_spinlock ++;

	// tmp aponta para inicio da lista
    PAINT *tmp = paint_ready_queue;
    // procura janela
    while (tmp) {
    	
    	if(tmp->next->w == addr)
    	{
                window_count --;
                PAINT *save =  tmp->next;
                tmp->next = tmp->next->next;
                if(paint_foco == save) paint_foco = 0;     
                free(save);
                break;
    	}
    	
    		tmp = tmp->next;
    }
    	    	
    w_remove_spinlock = 0;
    paint_ready_queue->spinlock = 0;
}

void window_foco(unsigned long addr) {
	
	while(paint_ready_queue->spinlock);
	paint_ready_queue->spinlock++;

	// tmp aponta para inicio da lista
    PAINT *tmp = paint_ready_queue;
    PAINT *pre = tmp;
    while (tmp) {

    	if(tmp->w == addr) {
    		paint_foco = tmp;
    		if(tmp->next != 0){
    			pre->next = tmp->next;
    			tmp->next = 0;
    			pre = paint_ready_queue;
    			while (pre->next) {
    				pre = pre->next;
    			}
    	
    			pre->next = tmp;
    		}
    		
    		break;
    	}
    	pre = tmp;
    	tmp = tmp->next;
    }
    	
    paint_ready_queue->spinlock = 0;
}

void paint_desktop(void *bankbuffer) {
    int count = 0;
	while(paint_ready_queue->spinlock != 0){}
	paint_ready_queue->spinlock = 1;

	paint = paint_ready_queue->next;
	while(paint){
	    if(paint->w != 0) {
            count++;
		    WINDOW *w = (WINDOW*) paint->w;
		
		    while(*(unsigned char *)&w->spinlock != 0){};
		    *(unsigned char *)&w->spinlock = 1;
      
		    unsigned long start  = (unsigned long) &w->start;
		    unsigned long zbuf  = ( unsigned long) bankbuffer;
		
		    long y =  w->pos_y;
		    long x =  w->pos_x;
		
		    long pos = ((w->pos_y * gui->pixels_per_scan_line) + w->pos_x) << 2;//*4;
		    long width = w->width;
		    long height = w->height;
            unsigned int style = w->style;		
    
		    long len = width;
		
		    if( (x + width) > gui->pixels_per_scan_line ) {
			    len = width - ((x + width) - gui->pixels_per_scan_line );			
		
		    }
				
		    if( x > gui->pixels_per_scan_line ) {
			    paint = paint->next;
                *(unsigned char *)&w->spinlock = 0;
			    continue;
		    }
  				
		    for(int i=0; i < height; i++) {
		        unsigned char *src = (unsigned char *) (start + (i*(width << 2)));
				unsigned char *dst = (unsigned char *) (zbuf + (i*(gui->pixels_per_scan_line << 2)) + pos);
				// alinhar 
				int n = (((unsigned long)dst)%SSE_MMREG_SIZE);
                dst -= n;
         		
         		
         		if (w->gid != 0) {
         			if( (i + y) < (gui->vertical_resolution - TASK_BARRA)){
                        if(style & 0x80000000) // transparencia
                            trans_memcpy( dst, src, len << 2);        			    
                        else
         				    sse_memcpy( dst, src, len << 2);
                            //copymem(dst, src, len << 2);
         			}
         		} else { // launcher
         			if(i < gui->vertical_resolution) {
         				sse_memcpy( dst, src, len << 2);
                        //copymem(dst, src, len << 2);
         			}
         		}
         		
         	}
           
            *(unsigned char *)&w->spinlock = 0;
       }
       	paint = paint->next;
    }


	paint_ready_queue->spinlock = 0;

    rf = count;
}


extern void DrawMouse(unsigned int color, unsigned int *buffer,void *MouseBuffer);
void paint_cursor(unsigned char *zbuf, WINDOW *addr) {

	if(gtt->INTEL) {
        // update_graphic_cursor(mouse->x, mouse->y);
        return;
    }
		
		WINDOW *w = (WINDOW*) addr;
		
		//unsigned long start  = (unsigned long) &w->start;
		
		w->pos_y = mouse->y;
		w->pos_x = mouse->x;

		const char *font_data = (const char*) cursor18x18;
		unsigned int mask;

		for(int t=0;t < w->height;t++) {
		
		
			for(int i= 0;i < w->width ;i ++)
			{
			
				mask = font_data[i+ (t*w->width)];
			
               	      	if(mask == 1) {
               	        	put_pixel_buff(w->pos_x + i, w->pos_y + t, -1, zbuf);
               	        
               	      	}else if( mask == 2){
               	      		put_pixel_buff(w->pos_x + i, w->pos_y + t, 0x101020, zbuf);
               	      	}
                       
	
			}
			
		} 

}

void compose()
{

	screan_spin_lock = 0;
    window_count = 0;
    paint_foco = 0;
	WINDOW *mouseaddr;

	cli();
	alloc_pages(0, 1024*4, (unsigned long *)&gui->bank_buffer);
	alloc_pages(1, 1, (unsigned long *)&paint);
	alloc_pages(0, 4, (unsigned long *)&mouseaddr);
	sti();
	
	
	memset(paint, 0,sizeof(PAINT));
	paint_ready_queue = paint;

	unsigned char *vram = ( unsigned char*)((long)gui->virtual_buffer);
	unsigned char *zbuf  = ( unsigned char*)((long)gui->bank_buffer);

    // TODO
    //gui->virtual_buffer = zbuf;
	
	unsigned int len = gui->pitch * gui->height;
	
	memset(zbuf, 0,len);
	
	
	mouseaddr->width = 18;
	mouseaddr->height = 20;
    
    launcher = 0;
    for (;;){
        
        paint_desktop(zbuf);
        paint_cursor(zbuf,mouseaddr);
        

		//#if WAIT_FOR_VERTICAL_RETRACE
        //while ((inportb(0x3DA) & 0x08));
        //while (!(inportb(0x3DA) & 0x08));
		//#endif
		
  		//copymem(vram, zbuf, len);
  		sse_memcpy(vram, zbuf, len);
  		
  	}
  	
} 	
