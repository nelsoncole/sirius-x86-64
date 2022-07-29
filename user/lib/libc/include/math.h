#ifndef __MATH_H
#define __MATH_H


#define FP_NAN         0
#define FP_INFINITE    1
#define FP_ZERO        2
#define FP_SUBNORMAL   3
#define FP_NORMAL      4

#ifndef FP_ILOGB0
# define FP_ILOGB0 (-INT_MAX)
#endif
#ifndef FP_ILOGBNAN
# define FP_ILOGBNAN INT_MAX
#endif

#ifndef MATH_ERRNO
# define MATH_ERRNO 1
#endif
#ifndef MATH_ERREXCEPT
# define MATH_ERREXCEPT 2
#endif
#ifndef math_errhandling
# define math_errhandling MATH_ERRNO
#endif

#ifndef HUGE_VAL
#define HUGE_VAL (1.0e999999999)
#endif

#ifndef HUGE_VALF
#define HUGE_VALF (1.0e999999999F)
#endif

#include <tgmath.h>


extern double pow(double x, double y);
extern double sqrt(double x);
extern double atan(double x);
extern double acos(double x);
extern double asin(double x);
extern double sin(double x);
extern double cos(double x);
extern double tan(double x);

extern double floor(double x);
extern double ceil(double x);

extern double exp(double x);
extern double log10(double x);
extern double log(double y);
extern double log2(double x);
extern double ldexp(double x, int exp);
extern double frexp(double value, int *exp);
extern double atan2(double y, double x);

extern double fmax(double x, double y);
extern float fmaxf(float x, float y);
extern double fmin(double x, double y);
extern float fminf(float x, float y);

extern int min(int x, int y);
extern int max(int x, int y);

extern double fabs(double x);


extern double cosh(double x);
extern double tanh(double x);
extern double sinh(double x);

extern double modf(double x, double *iptr);

#endif
