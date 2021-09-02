
#ifndef __ASM_H__
#define __ASM_H__


#define inportb(p)({\
	unsigned char val;\
	__asm__ __volatile__("int $0x72;":"=a"(val):"d"(10),"c"(0),"S"(0),"D"(p)); val&0xff; })
#define inportw(p)({\
	unsigned short val;\
	__asm__ __volatile__("int $0x72;":"=a"(val):"d"(10),"c"(1),"S"(0),"D"(p));val&0xffff; })

#define inportl(p)({\
	unsigned int val;\
	__asm__ __volatile__("int $0x72;":"=a"(val):"d"(10),"c"(2),"S"(0),"D"(p));val&0xffffffff; })


#define outportb(p,val)__asm__ __volatile__("int $0x72;"::"d"(10),"c"(3),"S"(val),"D"(p))

#define outportw(p,val)__asm__ __volatile__("int $0x72;"::"d"(10),"c"(4),"S"(val),"D"(p))

#define outportl(p,val)__asm__ __volatile__("int $0x72;"::"d"(10),"c"(5),"S"(val),"D"(p))


#endif
