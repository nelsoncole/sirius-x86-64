
#ifndef __IO_H__
#define __IO_H__


#define outanyb(p) \
__asm__ __volatile__(\
"outb %%al,%0"::"dN"((p)) :"eax"\
)  /* Valeu Fred */

//IO R/W BYTE
#define inportb(p)({\
	unsigned char val;\
	__asm__ __volatile__("in %w1,%b0":"=a"(val):"d"(p));val&0xff; })

#define outportb(p,val)__asm__ __volatile__("out %b0,%w1"::"a"(val),"d"(p))

#define inpb(p)({\
	unsigned char val;\
	__asm__ __volatile__("in %w1,%b0":"=a"(val):"d"(p));val&0xff; })

#define outpb(p,val)__asm__ __volatile__("out %b0,%w1"::"a"(val),"d"(p))


//IO R/W WORD
#define inportw(p)({\
	unsigned short val;\
	__asm__ __volatile__("inw %w1,%w0":"=a"(val):"d"(p));val; })

#define outportw(p,val) __asm__ __volatile__ ("outw %w0,%w1"::"a"(val),"d"(p))

#define inpw(p)({\
	unsigned short val;\
	__asm__ __volatile__("inw %w1,%w0":"=a"(val):"d"(p));val; })

#define outpw(p,val) __asm__ __volatile__ ("outw %w0,%w1"::"a"(val),"d"(p))


//IO R/W DWORD
#define inportl(p)({\
	unsigned int val;\
	__asm__ __volatile__("inl %w1,%k0":"=a"(val):"d"(p));val; })

#define outportl(p,val) __asm__ __volatile__ ("outl %k0,%w1"::"a"(val),"d"(p))

//IO R/W DWORD
#define inportd(p)({\
	unsigned int val;\
	__asm__ __volatile__("inl %w1,%k0":"=a"(val):"d"(p));val; })

#define outportd(p,val) __asm__ __volatile__ ("outl %k0,%w1"::"a"(val),"d"(p))


#define inpl(p)({\
	unsigned int val;\
	__asm__ __volatile__("inl %w1,%k0":"=a"(val):"d"(p));val; })

#define outpl(p,val) __asm__ __volatile__ ("outl %k0,%w1"::"a"(val),"d"(p))

#define cli() __asm__ __volatile__ ("cli")
#define sti() __asm__ __volatile__ ("sti;out %b0,$0x80"::"a"(0))
#define hlt() __asm__ __volatile__ ("hlt")
#define nop() __asm__ __volatile__ ("nop")


void wait(int n);


#endif
