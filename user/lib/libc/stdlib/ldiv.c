#include <stdlib.h>

#undef        ldiv

ldiv_t ldiv(long numer, long denom) {
	
	ldiv_t d;

	d.quot = numer/denom;
	d.rem = numer%denom;
	
	if (numer >= 0 && d.rem < 0) {
		d.quot++;
		d.rem -= denom;

	} else if (numer < 0 && d.rem > 0) {
		d.quot--;
		d.rem += denom;
	}
	
	
	return (d);
}
