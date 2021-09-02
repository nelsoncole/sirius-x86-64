#include <tss.h>
#include <string.h>


extern unsigned long _rsp0;

TSS tss[1];

void load_ltr(int tr)
{
	__asm__ __volatile__ (\
	"movw %w0,%%ax;\
	ltrw %%ax;"\
	::"a"(tr)\
	);

}


void tss_install()
{
	memset(&tss,0, sizeof(TSS));
	//pilha do ring0
	tss->rsp0 = (unsigned long)_rsp0;
}

