#ifndef __SETJMP_H
#define __SETJMP_H

#define _JBLEN 64

typedef	char jmp_buf[_JBLEN];

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int val);

#endif
