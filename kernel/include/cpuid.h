#ifndef __CPUID_H
#define __CPUID_H


int cpuid_vendor(char *string);
int cpuid_processor_brand(char *string);

void cpuid(unsigned int code, unsigned int*a, unsigned int *d);
int cpuid_string(unsigned int code, unsigned int where[4]);


#endif



// Volume 2: Instruction Set Reference, A-Z p471, Volume 3 p2701, Volume 4: Model-Specific Registers p4413
