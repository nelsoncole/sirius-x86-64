#include <stdlib.h>

#undef        div

div_t div(int numer, int denom) {
	
	div_t d;

	d.quot = numer/denom;
	d.rem = numer%denom;

	if (numer >= 0 && d.rem < 0) {
		d.quot++;
		d.rem -= denom;
	}
	else if (numer < 0 && d.rem > 0) {
		d.quot--;
		d.rem += denom;
	}
	
	
	return (d);
}
