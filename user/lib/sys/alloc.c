#include <alloc.h>
#include <string.h>
#include <stdio.h>


struct __realloc {
    unsigned long ptr;
    unsigned long pool;
    unsigned long size;
    struct __realloc *next;
};


struct __realloc *realloc_list;
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


void *__malloc_r(unsigned s, int type);
void __heap_r()
{
	__alloc_t *__m = (__alloc_t*) __m_i;
	
	__m_c = 0;

	alloc_spin_lock = 0;
	
	memset(__m, 0, sizeof(__alloc_t)*ALLOC_COUNT);

    // init realloc
    realloc_list = (struct __realloc *) __malloc_r(0x1000, 0);
    memset(realloc_list , 0, sizeof(struct __realloc));
}

void *__malloc_r(unsigned s, int type)
{
	if(!s) return (void*)0;
	
	if(__m_c >= ALLOC_COUNT ) printf("Panic: __malloc, sem espaco na tabela de alocacao\n");
	
	while(alloc_spin_lock){}
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

    // verificar se e realloc
    struct __realloc *tmp = realloc_list->next;
    struct __realloc *tmp2 = realloc_list;
    unsigned long bu = (unsigned long) p;
    while(tmp){ 
        if(tmp->ptr == bu){ break;}
        tmp2 = tmp->next;
        tmp = tmp->next;
    }
    if(tmp != 0){
        tmp2->next = tmp->next;
        __free_r(tmp);
    }

    while(alloc_spin_lock){}
	alloc_spin_lock ++;
	
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

    //printf("realloc: %lx %d\n", ptr, s);

	unsigned long bu = 0;
    struct __realloc *blk = 0;	
	 	
	if(!ptr){

        struct __realloc *tmp = realloc_list;
        while(tmp->next){ 
            tmp = tmp->next;
        }

        blk = (struct __realloc *) __malloc_r(0x1000, 0);
        memset(blk , 0, sizeof(struct __realloc));

        int res = 0;
        if(s%0x1000)res = 1;
        unsigned long len = 0x1000*((s/0x1000)+res);

        blk->ptr = (unsigned long)__malloc_r(len, 0);
        blk->pool = len;
        blk->size = s;        

        tmp->next = blk;

        bu = blk->ptr;

    }else{
        struct __realloc *tmp = realloc_list->next;
        bu = (unsigned long) ptr;
        while(tmp){ 
            if(tmp->ptr == bu){ break;}
            tmp = tmp->next;
        }

        blk = tmp;
        if(blk){
            if((blk->pool - blk->size) >= s){
                blk->size += s;
            }else {

                blk->size += s;
                int res = 0;
                if(blk->size%0x1000)res = 1;
                unsigned long len = 0x1000*((blk->size/0x1000)+res);
                blk->size -= s;
                bu = (unsigned long)__malloc_r(len, 0);
                memcpy((void*)bu, (void*)blk->ptr ,blk->size);
                void *tmp_ptr = (void*)blk->ptr;
                blk->ptr = bu;
                blk->pool = len;
                blk->size += s; 

                __free_r(tmp_ptr);
            }
        
        }else{
            printf("panic: realloc %lx size = %d\n",ptr, s);
            while(1){}
        }
    }
	printf("%lx\n", bu);
	return (void*)bu;
}
