#ifndef __SSE_H
#define __SSE_H

extern void salvefloat(char *region);
extern void rstorfloat(char *region);

void fpu_save(char *region);
void fpu_restore(char *region);

int setup_sse();

#endif
