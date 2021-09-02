#ifndef __TIME_H
#define __TIME_H

#include "size_t.h"

#define NULL ((void *)0)

#define CLOCKS_PER_SEC 100 // número de tiques do sistema por segundo

typedef long 	time_t;
typedef long	clock_t;

struct tm {
	int tm_sec;         /* seconds,  range 0 to 59          */
   	int tm_min;         /* minutes, range 0 to 59           */
   	int tm_hour;        /* hours, range 0 to 23             */
   	int tm_mday;        /* day of the month, range 1 to 31  */
   	int tm_mon;         /* month, range 0 to 11             */
   	int tm_year;        /* The number of years since 1900   */
   	int tm_wday;        /* day of the week, range 0 to 6    */
   	int tm_yday;        /* day in the year, range 0 to 365  */
   	int tm_isdst;       /* daylight saving time             */
};

extern clock_t clock(void);
extern double difftime(time_t time1, time_t time0);
extern time_t mktime(struct tm *timeptr);
extern time_t time(time_t *timer);
extern char *asctime(const struct tm *timeptr);
extern char *ctime(const time_t *timer);
extern struct tm *gmtime(const time_t *timer);
extern struct tm *localtime(const time_t *timer);
extern size_t strftime(char *s, size_t maxsize,const char *format, const struct tm *timeptr);


#endif
