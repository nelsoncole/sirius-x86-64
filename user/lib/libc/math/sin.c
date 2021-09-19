#include <math.h>


static double pot(double x, int y)
{   double acc = 1.0;
    int n=0;
    while(n<y){
        acc *=x;
        n++;
    }
    return acc;
}

static int fat(int n)
{
    int i=1;
    int acc = 1;
    while(i<=n) {
        acc = acc*i;
        i ++;
    }
    return acc;
}
double sin(double x)
{
    int n =0;
    double acc = 0.0;
    while(n<10) {
        acc += pot(-1.0, n)*pot(x, 2*n+1)/fat(2*n+1);
        //printf("%d sin(%lf)=%lf\n",n,x,acc);
 
        n ++;
    }

	return acc;
}
