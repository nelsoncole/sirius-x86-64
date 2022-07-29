#include <math.h>

extern double __sinus(double arg, int quad);

double cos(double x)
{
	if(x < 0)
		x = -x;
	return __sinus(x, 1);
}
