#include <math.h>


double sqrt(double x)
{

	double ret;
	__asm__ __volatile__ (
		"finit;"
		"fldl %1;" // st(0) => st(1), st(0) = x. FLDL means load double float
		"fsqrt;"  // st(0) = square root st(0)
		: "=t"(ret) : "m"(x) );


	return (ret);
}
