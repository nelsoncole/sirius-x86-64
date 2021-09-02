#ifndef __TSS_H
#define __TSS_H

typedef struct _TSS {
	unsigned int reserved;
	unsigned long long rsp0, rsp1, rsp2;
	unsigned int reserved2[2];
	unsigned long long ist1, ist2, ist3, ist4, ist5, ist6, ist7;
	unsigned int reserved3[2];
	unsigned short reserved4;
	unsigned short io_map_base_addr;
	
}__attribute__((packed)) TSS;


extern TSS tss[1];
void load_ltr(int tr);
void tss_install();


#endif
