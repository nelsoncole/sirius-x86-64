#ifndef __STDDEF_H
#define __STDDEF_H

#define DEF_HEAP_START 0x01000000 // 16 MiB end 0x02000000
#define DEF_HEAP_LIMIT 0x01000000 // 16 MiB

typedef unsigned size_t;
#define NULL ((void *)0)

#endif
