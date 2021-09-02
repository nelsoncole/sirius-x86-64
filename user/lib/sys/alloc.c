#include <alloc.h>
#include <string.h>
#include <stdio.h>


unsigned long __m_c;
unsigned long __m_i;

//static __alloc_t __m_i[ALLOC_COUNT]__attribute__((aligned(4096)));
static int alloc_spin_lock;

extern unsigned long __window;
unsigned long addr;
unsigned long __block_r(unsigned nb)
{
	unsigned _nb = nb;
	
	if(!_nb)_nb +=1;

	__asm__ __volatile__("int $0x72":"=a"(addr):"d"(4),"c"(_nb));

	return (addr);
}


void __free_block_r(unsigned long addr)
{
	__asm__ __volatile__("int $0x72"::"d"(5),"D"(addr));

}

void __heap_r()
{
	__alloc_t *__m = (__alloc_t*) __m_i;
	
	__m_c = 0;

	alloc_spin_lock = 0;
	
	memset(__m, 0, sizeof(__alloc_t)*ALLOC_COUNT);
}

void *__malloc_r(unsigned s, int type)
{
	if(!s) return (void*)0;
	
	if(__m_c >= ALLOC_COUNT ) printf("Panic: __malloc, sem espaco na tabela de alocacao\n");
	
	while(alloc_spin_lock);
	alloc_spin_lock++;
	
	int nb = s/0x1000;
	if(s%0x1000) nb ++;

	int no = 0;
	int x = 0;
	int re = 0;
	
	if(type != 0){ 
	
		re = 0x80;
		nb = REALLOC_FIRST_SIZE/0x1000;
		if(REALLOC_FIRST_SIZE%0x1000) nb ++;
		goto single;
	}
	
	if(s >= 0x1000) goto single;

	__alloc_t *m;
	__alloc_t *__m = (__alloc_t*) __m_i;
	unsigned long b;
	unsigned long a = 0;
	for(int i= 0; i < ALLOC_COUNT; i ++){
		if(__m->flag == 2 && __m->pool >= s){
			a = __m->addr + __m->size;
			__m->pool -= s;
			__m->size += s;
			b = (unsigned long)__m;
			__m =(__alloc_t*)(unsigned long) __m_i;
			for(int i=0; i< ALLOC_COUNT;i ++){
				if(__m->flag){
					__m ++;
					continue;
				}
				m = __m;
				__m = (__alloc_t*)b;
				__m->sub += 1;
				m->flag = 4;
				m->addr = a;
				m->size = s;
				m->sub  = b;
				no ++;
				break;
			}
			break;
		}
		__m ++;
	}

	x = 1;
single:
	if(!no) { 
	__m = (__alloc_t*)(unsigned long) __m_i;
	for(int i=0; i<ALLOC_COUNT;i ++){
		if(__m->flag){
			__m ++;
			continue;
		}
		m = __m;
		a = (unsigned long) __block_r(nb);
		m->addr = a;
		m->flag = (1 + x) | re;
		m->size = s;
		m->pool = (0x1000*nb) - s;
		m->sub  = 1;
		break;
	}}


	alloc_spin_lock = 0;
	__m_c ++;
	return (void*)a;
}

void __free_r(void *p)
{
	if(!p)return;

	while(alloc_spin_lock);
	alloc_spin_lock++;
	
	__alloc_t *__m, *m;
	unsigned long a = (unsigned long)p;
	__m = (__alloc_t*)(unsigned long) __m_i;
        for(int i=0; i< ALLOC_COUNT;i ++){
                if(__m->addr != a){
                        __m ++;
                        continue;
                }
                m = __m;
                __m_c --;
                
		if(m->flag == 1) __free_block_r((unsigned long)p);
		else {
			if(m->flag == 4)
				__m = (__alloc_t*) m->sub;

			 __m->sub -= 1;
			if(__m->sub > 0 && m->flag == 2)
				break;

			if(__m->sub <= 0){ 
				__free_block_r((unsigned long)__m->addr);
				__m->addr = 0;
                               __m->flag = 0;
                               __m->size = 0;
                               __m->pool = 0;
                               __m->sub  = 0;
			}
		}
              	m->addr = 0;
             	m->flag = 0;
             	m->size = 0;
             	m->pool = 0;
		m->sub  = 0;
		
		
		break;
        }
        
        alloc_spin_lock = 0;
}


void *__realloc_r(void *ptr, unsigned s)
{

	//printf("panic: realloc %lx size = %d %d\n",ptr, s, __m_c);
	
	unsigned long r = 0;
	int yes = 0;	
	 	
	if(!ptr) return (void*) __malloc_r(s, 1);
	
	
	while(alloc_spin_lock);
	alloc_spin_lock++;
	
	__alloc_t *m;
	__alloc_t *__m = (__alloc_t*) __m_i;
	
	unsigned long a = (unsigned long)ptr;

	 for(int i=0; i< ALLOC_COUNT;i ++){
	 
            	if(__m->addr != a){
                        __m ++;
                        continue;
                }
                
               
             	m = __m;
             	
 		yes = 1;
                
              	if((m->size + s ) > REALLOC_FIRST_SIZE) {
              		printf("panic: realoc sem espaco, %lx size = %d %d\n",ptr, s, __m_c);
              		for(;;);
              	}
              	else {
              		r = m->addr;
              		
              		m->size += s;
              	}
                
                
   	} 

	alloc_spin_lock = 0;
	
	if(yes == 0) return (void*) __malloc_r(s, 1);

	return (void*)r;
}
