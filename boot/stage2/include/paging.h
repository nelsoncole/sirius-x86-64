#ifndef __PAGING_H
#define __PAGING_H

//32-bit Paging
typedef struct _page_table {
	unsigned int p :1;
	unsigned int rw :1;
	unsigned int us :1;
	unsigned int pwt :1;
	unsigned int pcd :1;
	unsigned int a :1;
	unsigned int d :1;
	unsigned int pat :1;
	unsigned int g :1;
	unsigned int ign :3;
	unsigned int frames :20;

}__attribute__((packed)) page_table_t;

typedef struct _page_directory {
	unsigned int p :1;
	unsigned int rw :1;
	unsigned int us :1;
	unsigned int pwt :1;
	unsigned int pcd :1;
	unsigned int a :1;
	unsigned int ign :1;
	unsigned int ps :1;
	unsigned int ignored :4;
	unsigned int addrpt :20;

}__attribute__((packed)) page_directory_t;

// 4-LEVEL PAGING
typedef struct _pae_page_table {
	unsigned long long  p :1;
	unsigned long long rw :1;
	unsigned long long us :1;
	unsigned long long pwt :1;
	unsigned long long pcd :1;
	unsigned long long a :1;
	unsigned long long d :1;
	unsigned long long pat :1;
	unsigned long long g :1;
	unsigned long long ign2 :3;
	unsigned long long frames :40;
	unsigned long long rs2 :12;

}__attribute__((packed)) pae_page_table_t;

typedef struct _pae_page_directory {
	unsigned long long p :1;
	unsigned long long rw :1;
	unsigned long long us :1;
	unsigned long long pwt :1;
	unsigned long long pcd :1;
	unsigned long long a :1;
	unsigned long long ign1 :1;
	unsigned long long ps :1;
	unsigned long long ign2 :4;
	unsigned long long phy_addr_pt :40;
	unsigned long long rs2 :12;
}__attribute__((packed)) pae_page_directory_t;
	
typedef struct _pae_page_directory_pointer_table{
	unsigned long long p :1;
	unsigned long long rw :1;
	unsigned long long us :1;
	unsigned long long pwt :1;
	unsigned long long pcd :1;
	unsigned long long rs1 :4;
	unsigned long long ign :3;
	unsigned long long phy_addr_pd :40;
	unsigned long long rs2 :12;

}__attribute__((packed)) pae_page_directory_pointer_table_t;

typedef struct _pml4_table{
	unsigned long long p :1;
	unsigned long long rw :1;
	unsigned long long us :1;
	unsigned long long pwt :1;
	unsigned long long pcd :1;
	unsigned long long rs1 :4;
	unsigned long long ign :3;
	unsigned long long phy_addr_pdpt :40;
	unsigned long long rs2 :12;

}__attribute__((packed)) pml4_table_t;



void page_install(void);
	
	
#endif
