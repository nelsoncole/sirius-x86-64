#include <time.h>

extern unsigned long *__tm;

struct tm *localtime(const time_t *timer)
{

	return (struct tm*) __tm;

}
