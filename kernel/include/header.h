#ifndef __HEADER_H
#define __HEADER_H

typedef struct _HEADER {

	char		magic[8]; // "_ cole _"
	unsigned long	eflag;
	unsigned long	header;
	unsigned long	start;
	unsigned long	stack;
	unsigned long	end;
	unsigned long	argc;
	unsigned long	argv;

}__attribute__((packed)) HEADER;



#endif
