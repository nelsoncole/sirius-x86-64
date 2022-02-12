#include <mm.h>
#include <paging.h>

#include <stdio.h>
#include <string.h>
#include <io.h>

//Estas função é para mapear dispositvos, mm_mp(...)
unsigned long g_mm_mp_index;
pae_page_directory_t *device_pde;

void initialize_mm_mp()
{

	device_pde = (pae_page_directory_t *) (DEVICE_PAGE_DIR);
	memset(device_pde,0,512*sizeof(pae_page_directory_t));
	
	unsigned long addr = (unsigned long) device_pde;
	
	// 0xC0000000  3GiB
	pdpte[DEVICE_PDPTE_ENTRY].p = 1;
	pdpte[DEVICE_PDPTE_ENTRY].rw = 1;
	pdpte[DEVICE_PDPTE_ENTRY].pcd= 1;
	pdpte[DEVICE_PDPTE_ENTRY].phy_addr_pd = (addr >> 12) &0xffffffffffff;
	
	
	g_mm_mp_index = 0;
	
}

int mm_mp( unsigned long phy_addr, unsigned long *virt_addr,unsigned size, int flag) 
{

	pae_page_directory_t *pde = device_pde;
	
	pae_page_table_t *pte = (pae_page_table_t *) (DEVICE_PAGE_TABLE + (0x1000*g_mm_mp_index));
	pae_page_table_t *__pte = pte;
	
	unsigned long addr = phy_addr;
	int index = g_mm_mp_index;
	
	if(size > 0x40000000 )
	{
		printf("Memory Map error, size > 1GiB\n");
		*(unsigned long*)(virt_addr) = 0;
		return 1;
	
	}else if(size > (0x40000000-(index*0x200000)))
	{
		printf("Memory Map error, size > %d bytes, (0x%d bytes)\n",(0x40000000-(index*0x200000)),size);
		*(unsigned long*)(virt_addr) = 0;
		return 2;
	} 
	
	int count = (size/PAGE_SIZE);
	
	if(size%PAGE_SIZE)
	{
		count++;
	}
	
	int count2 = (count/512);
	
	if(count%512)
	{
		count2++;
	}
	
	
	memset(pte,0,512*sizeof(pae_page_table_t)*count2);
	
	//2 MiB
	for(int i=0;i < count; i++) {
		
		__pte->p = 1;
		__pte->rw = 1;
		__pte->pcd= 1;
		
		if(flag&1){
			__pte->us = 1;
		}
		__pte->frames = (addr >>12) &0xffffffffffff;
		__pte++;
		
		addr += 0x1000;
	}

	addr = (unsigned long)pte;
	
	for(int i=0;i < count2; i++) {
		pde[index + i].p = 1;
		pde[index + i].rw = 1;
		if(flag&1){
			pde[index + i].us = 1;
		}
		pde[index + i].pcd= 1;
		pde[index + i].phy_addr_pt = (addr >>12) &0xffffffffffff;
		
		addr += 0x1000;
		
		g_mm_mp_index++;
	}
	

	flush_tlb(); // Actualiza TLB
	//__asm__ __volatile__("invlpg (%0)" ::"r" (pml4e) : "memory");
	
	*(unsigned long*)(virt_addr) = (DEVICE_MEMORY_MAP + (index * 0x200000));
	
	return 0;
}

// TODO
// 0x2000000 -- 0x3000000 -- 32 MiB RAM_BITMAP
// 0x3000000 -- 0x5000000 -- 48 MiB PAGE_BITMAP
// 0x5000000 -- 0x5001000 -- 80 MiB ALLOC_PAGE_DIR
// 0x5001000 -- 0x5201000 -- 80.04 MiB ALLOC_PAGE_TABLE
// 0x5201000 -- 0x7800000 -- 120 MIB (TODO Reservado para page table)

unsigned char *RAM_BITMAP;
unsigned char *PAGE_BITMAP;
pae_page_directory_t *alloc_pde;
pae_page_table_t *alloc_pte;
unsigned long ram_setup(unsigned long entry_pointer_info)
{

	RAM_BITMAP = (unsigned char*) (0x2000000);
	
	// Reserved Kernel 128MiB
	int len = ((128*1024)/4)/8;
	
	memset(RAM_BITMAP, -1, len);
	
	// Free 128 MiB to 512 MiB
	int offset = len;
	len = (((512 - 128)*1024)/4)/8;
	
	memset(RAM_BITMAP + offset, 0, len);

	return ((unsigned long) RAM_BITMAP);
}

unsigned long alloc_frame()
{
	unsigned char *bmp = RAM_BITMAP + 4096;
	int limit = ((512*1024)/4);
	for(unsigned int i = 4096; i <  limit; i++ ){
	
		for(int t = 0; t < 8; t++) {
		
			if( !((*bmp >> t) & 0x1) ) {

				*bmp |= (1 << t);
				return (i*8+t)*0x1000;
			}
		}
		
		bmp++;
	}
	
	return 0;
}

int free_frame(unsigned long addr)
{
    if (!addr || addr < 0x8000000) return 0;
    
	unsigned int block = addr/0x1000;
	unsigned int byte = block/8;
	unsigned int bit = block%8;
	
	unsigned char *bmp = (unsigned char*)(RAM_BITMAP + byte);

	*bmp &=~(1 << bit);
	
	return byte | (bit << 24);
}



int alloc_spin_lock;
unsigned long alloc_pages_setup(unsigned long entry_pointer_info)
{
	alloc_spin_lock = 0;

    PAGE_BITMAP = (unsigned char*) (0x3000000);
    memset(PAGE_BITMAP, 0, 0x2000000);

	alloc_pde = (pae_page_directory_t *) (0x5000000);
	pae_page_directory_t *pde = alloc_pde;
	memset( pde,0, 512*sizeof(pae_page_directory_t));
	
	
	alloc_pte = (pae_page_table_t *) (0x5001000);
	pae_page_table_t *pte = alloc_pte;
	memset(pte, 0, 512*sizeof(pae_page_table_t) *512);
	
	unsigned long addr = 0;
	// 1 GiB
	for(int i=0;i < 512*512; i++) {
		
		pte->p = 0;
		pte->rw = 1;
		pte->frames = (addr >>12) &0xffffffffffff;
		pte++;
		
	}
	
	addr = (unsigned long) alloc_pte;
	for(int i=0;i < 512; i++) {
		
		pde->p = 1;
		pde->rw = 1;
		pde->us = 1;
		pde->phy_addr_pt = (addr >>12) &0xffffffffffff;
		
		addr +=0x1000;
		pde++;
	}
	
	// 0x100000000  4GiB
	addr = (unsigned long) alloc_pde;
	pdpte[ALLOC_PDPTE_ENTRY].p = 1;
	pdpte[ALLOC_PDPTE_ENTRY].rw = 1;
	pdpte[ALLOC_PDPTE_ENTRY].us= 1;
	pdpte[ALLOC_PDPTE_ENTRY].phy_addr_pd = (addr >> 12) &0xffffffffffff;
	
	
	//configurar para usuario
	pml4e->us = 1;
	
	
	return 0;
}

/**
 * Aloca bloco de 4KiB
 *
 **/
int v_pool(pae_page_table_t *pte, unsigned len) 
{	
	pae_page_table_t *_pte = pte;
	
	if(_pte->p != 0 ) return 1;
	
	for(int i=0; i < len; i++) {
	
		if(_pte->p != 0 ) return -1;
		
		_pte ++;
	}
	
	return 0;
}
// 00 00 00 00
static int linear(unsigned char *p, int len, int of){
    unsigned char *bmp = p;
    int a = 0;
    for(int i=0; i < len; ){
       
        for(int x= of; x < 8; x +=2){
            if(i >= len) break;
            if(!(*bmp & (1<<x))) {
              a ++;  
            }else{
                return 0;
            }
            i++; 
        }
        of = 0;
        bmp ++;
    }

    return a;
}

static void masked(unsigned char *p, int len, int of){

    int a = 0;

    unsigned char *bmp =p;

    for(int i=0; i < len;){
        for(int x= of; x < 8; x +=2){
            if(i >= len) break;
            if(a == 0)
                *bmp |= (1<<x);
            else
                *bmp |= (3<<x);

            a = 1;
            i++;
        }
        of = 0;
        bmp ++;
    }
}
static void umasked(unsigned char *p, int len, int of){
    unsigned char *bmp = p;
    for(int i=0; i < len;){
        for(int x= of; x < 8; x +=2){
            if(i >= len) break;
            *bmp &=~(3<<x);
            i++;
        }
        of = 0;
        bmp ++;
    }
}
int get_free_page(unsigned len){
    // TODO 512GiB
    unsigned char *bmp = PAGE_BITMAP;
    int a = 0;
    int z = 0;
    for(int i =0; i < (268435456/8); i++){

        for(int x=0; x < 8; x +=2){
            if(!(*bmp & 1<<x)) {
                z = linear(bmp, len, x);
                if (len == z ){
                    masked(bmp, len, x);
                    return a;
                }
                
            } 
            a ++;
        }

        bmp++;
    }

    // panic
    while(1){};
}

int get_busy_page_size(int index){
    int a = 0;
    unsigned char *bmp = PAGE_BITMAP;
    int x = index/4;
    bmp += x;
    x = index%4;
    x = x*2;
    int sim = 0;
    while(1){
        for(int i=x; i < 8; i +=2){
          if((*bmp & 3<<i) == (1 <<i) || (*bmp & 3<<i) == 0 ){
            if(sim == 1)
                return a;

            sim = 1;
          }
          a++;  
        }
        bmp++;
        x = 0;
    }

    return 0;
}

unsigned long alloc_pages(int type, unsigned len, unsigned long *addr)
{
	if(!len) return 0;

	while(alloc_spin_lock){}

	alloc_spin_lock++;

	pae_page_table_t *pte = alloc_pte;

    int index = get_free_page(len);
    pte += index;

    for(int i=0; i < len; i++) {
        unsigned long frame = alloc_frame();
		if(!frame){ printf("FIXME frame"); while(1){}}
        pte->p = 1;
		if(type) pte->us = 1;
        pte->frames = (frame >>12) &0xffffffffffff;
		pte++;
    
    }

    flush_tlb();

    *(unsigned long*)(addr) = (ALLOC_PAGE_MEMORY + (index * 0x1000));

    alloc_spin_lock = 0;
    return len;

}

void free_pages(void *addr)
{
	if(!addr || 0)return;

    while(alloc_spin_lock){}

	alloc_spin_lock++;

	pae_page_table_t *pte = alloc_pte;
    
    unsigned char *bmp = PAGE_BITMAP;
    unsigned long a = (unsigned long)addr;
    a -= ALLOC_PAGE_MEMORY;
    a /= 0x1000;
    int index = (int)a;

    pte += index;

    int len = get_busy_page_size(index);
   
    bmp += index/4;
    a = (index%4)*2;

    umasked(bmp, len, a);

    for(int i=0; i < len; i++){

        unsigned long phy = pte->frames << 12;// &0xffffffffffff;
		free_frame(phy);
		pte->p = 0;		
		pte->us = 0;
		pte->frames = 0;

        pte++;
    }

    alloc_spin_lock = 0;
}


unsigned long get_phy_addr (unsigned long v) {

    int table = (int) (v >> 12 &0x1FF);
    int directory = (int) (v >> 21 &0x1FF);
    int directory_ptr = (int) (v >> 30 &0x1FF);
    //int pml4 = (int) (v >> 39 &0x1FF);
    	
    	
    pae_page_directory_t *pde = (pae_page_directory_t *)( pdpte[directory_ptr].phy_addr_pd << 12 &0xffffffffffff);
    pae_page_table_t *pte  = (pae_page_table_t *) (pde[directory].phy_addr_pt << 12 &0xffffffffffff);
	
	unsigned long phy = pte[table].frames << 12 &0xffffffffffff;

	return phy;
}

