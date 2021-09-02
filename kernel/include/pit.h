#ifndef __PIT_H
#define __PIT_H

#define FREQ 100

void pit_enable(int control);
void pit_set_frequencia(unsigned int freq);
void timer_wait(unsigned long  ticks);


#endif
