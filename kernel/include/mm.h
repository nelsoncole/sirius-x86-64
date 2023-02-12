#ifndef __MM_H
#define __MM_H

#define DEVICE_PAGE_DIR 	0x180000
#define DEVICE_PAGE_TABLE 	0x181000
#define DEVICE_PDPTE_ENTRY 	3
#define DEVICE_MEMORY_MAP 	0xC0000000

#define ALLOC_PAGE_MEMORY 	0x100000000
#define ALLOC_PDPTE_ENTRY 	4

//C0000000h - FFFFFFFFh = 1024MiB
// align 2 MiB

void initialize_mm_mp();
int mm_mp( unsigned long phy_addr, unsigned long *virt_addr,unsigned size, int flag);

unsigned long ram_setup(unsigned long entry_pointer_info);
unsigned long alloc_frame();
int free_frame(unsigned long addr);
unsigned long alloc_pages_setup(unsigned long entry_pointer_info);
unsigned long alloc_pages(int type, unsigned len, unsigned long *addr);
void free_pages(void *addr);

unsigned long get_phy_addr (unsigned long v);


#define POOL_COUNT 512 
typedef struct {
    unsigned long ptr;
    unsigned int size;
    unsigned int flag;
}__attribute__((packed)) POOL_SYSCALL;


#endif
