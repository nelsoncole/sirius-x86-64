#include <math.h>


double ceil(double x)
{
	int n = (int) x;

    if (n >= x) {
        return n;
    }

    return n + 1;
}
