#ifndef __MSR_H
#define __MSR_H

#define A32_EFER_MSR 0xC0000080

int getmsr(unsigned int msr, unsigned int *low, unsigned int *hight);
int setmsr(unsigned int msr, unsigned int low, unsigned int hight);

#endif
