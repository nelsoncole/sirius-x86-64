#include <math.h>



double sin(double x)
{
	double ret = 0;
	/*__asm__ __volatile__ (
		"finit;"
		"fldl %1;"
		"fsin;"
		: "=t"(ret) : "m"(x) ); */


	return (ret);
}
