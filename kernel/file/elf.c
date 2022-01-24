#include <stdio.h>
#include <header.h>

typedef unsigned int Elf32_Word;
typedef unsigned int Elf32_Addr;
typedef unsigned int Elf32_Off;
typedef unsigned short Elf32_Half;
typedef int Elf32_Sword;
typedef unsigned long long Elf32_Lword;


typedef unsigned int Elf64_Word;
typedef unsigned long long Elf64_Addr;
typedef unsigned long long Elf64_Off;
typedef unsigned short Elf64_Half;
typedef unsigned long long Elf64_Xword;
typedef int	Elf64_Sword;
typedef unsigned long long Elf64_Sxword;
typedef unsigned long long Elf64_Lword;

#define ELFMAG0   0x7f
#define ELFMAG1   'E'
#define ELFMAG2   'L'
#define ELFMAG3   'F'

#define EI_NIDENT 16

typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
} Elf32_Ehdr;

typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf64_Half      e_type;
        Elf64_Half      e_machine;
        Elf64_Word      e_version;
        Elf64_Addr      e_entry;
        Elf64_Off       e_phoff;
        Elf64_Off       e_shoff;
        Elf64_Word      e_flags;
        Elf64_Half      e_ehsize;
        Elf64_Half      e_phentsize;
        Elf64_Half      e_phnum;
        Elf64_Half      e_shentsize;
        Elf64_Half      e_shnum;
        Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

//Section Header
typedef struct {
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} Elf32_Shdr;

typedef struct {
	Elf64_Word	sh_name;
	Elf64_Word	sh_type;
	Elf64_Xword	sh_flags;
	Elf64_Addr	sh_addr;
	Elf64_Off	sh_offset;
	Elf64_Xword	sh_size;
	Elf64_Word	sh_link;
	Elf64_Word	sh_info;
	Elf64_Xword	sh_addralign;
	Elf64_Xword	sh_entsize;
} Elf64_Shdr;

// Symbol Table Entry
typedef struct {
	Elf32_Word	st_name;
	Elf32_Addr	st_value;
	Elf32_Word	st_size;
	unsigned char	st_info;
	unsigned char	st_other;
	Elf32_Half	st_shndx;
} Elf32_Sym;

typedef struct {
	Elf64_Word	st_name;
	unsigned char	st_info;
	unsigned char	st_other;
	Elf64_Half	st_shndx;
	Elf64_Addr	st_value;
	Elf64_Xword	st_size;
} Elf64_Sym;

#define ELF32_ST_BIND(i)   ((i)>>4)
#define ELF32_ST_TYPE(i)   ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define ELF64_ST_BIND(i)   ((i)>>4)
#define ELF64_ST_TYPE(i)   ((i)&0xf)
#define ELF64_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define ELF32_ST_VISIBILITY(o) ((o)&0x3)
#define ELF64_ST_VISIBILITY(o) ((o)&0x3)

// Relocation Entries
typedef struct {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
} Elf32_Rel;

typedef struct {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
	Elf32_Sword	r_addend;
} Elf32_Rela;

typedef struct {
	Elf64_Addr	r_offset;
	Elf64_Xword	r_info;
} Elf64_Rel;

typedef struct {
	Elf64_Addr	r_offset;
	Elf64_Xword	r_info;
	Elf64_Sxword	r_addend;
} Elf64_Rela;

#define ELF32_R_SYM(i)	((i)>>8)
#define ELF32_R_TYPE(i)   ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

#define ELF64_R_SYM(i)    ((i)>>32)
#define ELF64_R_TYPE(i)   ((i)&0xffffffffL)
#define ELF64_R_INFO(s,t) (((s)<<32)+((t)&0xffffffffL))


// Program Header
typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct {
	Elf64_Word	p_type;
	Elf64_Word	p_flags;
	Elf64_Off	p_offset;
	Elf64_Addr	p_vaddr;
	Elf64_Addr	p_paddr;
	Elf64_Xword	p_filesz;
	Elf64_Xword	p_memsz;
	Elf64_Xword	p_align;
} Elf64_Phdr;

// Dynamic Structure
typedef struct {
	Elf32_Sword	d_tag;
   	union {
   		Elf32_Word	d_val;
   		Elf32_Addr	d_ptr;
	} d_un;
} Elf32_Dyn;

//extern Elf32_Dyn	_DYNAMIC[];

typedef struct {
	Elf64_Sxword	d_tag;
   	union {
   		Elf64_Xword	d_val;
   		Elf64_Addr	d_ptr;
	} d_un;
} Elf64_Dyn;

//extern Elf64_Dyn	_DYNAMIC[];

int load_elf64_header(void *bf, program_header_t *phdr){
    Elf64_Ehdr *header = (Elf64_Ehdr*)bf;

    if(header->e_ident[4] != 2){
        printf("ELF: Invalid file class, target is not 62-bit\n");
        return 1;
    }

    if(header->e_type != 2) {
        printf("ELF: Invalid type, is not executable file [e_type=%d]\n",header->e_type);
        return 2;
    }

    if(header->e_machine != 62){
        printf("ELF: Invalid machine, is not AMD x86-64 architecture [e_machine=%d]\n",header->e_machine);
        return 3;
    }

    Elf64_Shdr *sections = (Elf64_Shdr *)((char*)bf + header->e_shoff);
    phdr->end = 0;
    for(int i=0; i < header->e_shnum; i++ ){
        Elf64_Shdr *section = sections++;
        if(!section->sh_addr){
            continue;
        }

        if(section->sh_type == 1){
            for(unsigned int e = 0 ; e < section->sh_size ; e++){
			   phdr->end = (unsigned long) section->sh_addr + section->sh_size;
			}

        }
    } 

    phdr->start = 0x8000000000;
    phdr->entry = header->e_entry;

    return 0;
}

int load_elf64_segment(void *bf){
    Elf64_Ehdr *header = (Elf64_Ehdr*)bf;

    Elf64_Shdr *sections = (Elf64_Shdr *)((char*)bf + header->e_shoff);

    for(int i=0; i < header->e_shnum; i++ ){
        Elf64_Shdr *section = sections++;
        if(!section->sh_addr){
            continue;
        }

        if(section->sh_type == 1){
            for(unsigned int e = 0 ; e < section->sh_size ; e++){
			    ((unsigned char*)section->sh_addr)[e] = ((unsigned char*)bf + section->sh_offset)[e];
			}

        }
    } 

    return 0;
}
