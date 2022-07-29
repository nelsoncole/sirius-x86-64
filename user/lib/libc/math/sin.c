#include <math.h>

/*
Copyright 2021 Plan 9 Foundation

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#define p0      .1357884097877375669092680e8
#define p1     -.4942908100902844161158627e7
#define p2      .4401030535375266501944918e6
#define p3     -.1384727249982452873054457e5
#define p4      .1459688406665768722226959e3
#define q0      .8644558652922534429915149e7
#define q1      .4081792252343299749395779e6
#define q2      .9463096101538208180571257e4
#define q3      .1326534908786136358911494e3

#define PIO2    1.570796326794896619231e0
#define PI      (PIO2+PIO2)

double __sinus(double arg, int quad)
{
	double e, f, ysq, x, y, temp1, temp2;
	int k;

	x = arg;
	if(x < 0) {
		x = -x;
		quad += 2;
	}
	x *= 1/PIO2;	// underflow?
	if(x > 32764) {
		y = modf(x, &e);
		e += quad;
		modf(0.25*e, &f);
		quad = e - 4*f;
	} else {
		k = x;
		y = x - k;
		quad += k;
		quad &= 3;
	}
	if(quad & 1)
		y = 1-y;
	if(quad > 1)
		y = -y;

	ysq = y*y;
	temp1 = ((((p4*ysq+p3)*ysq+p2)*ysq+p1)*ysq+p0)*y;
	temp2 = ((((ysq+q3)*ysq+q2)*ysq+q1)*ysq+q0);
	return temp1/temp2;
}

double sin(double x)
{
    return __sinus(x, 0);
}
