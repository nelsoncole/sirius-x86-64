#include <math.h>

double log(double y)
{
	int yi = y;
    int log2 = 0;
    double x, r;

    while (yi >>= 1) {
        log2++;
    }

    x = (double) (1 << log2);
    x = y / x;
    r = -1.7417939 + (2.8212026 + (-1.4699568 + (0.44717955 - 0.056570851 * x) * x) * x) * x;
    r += 0.69314718 * log2;

    return r;
}
