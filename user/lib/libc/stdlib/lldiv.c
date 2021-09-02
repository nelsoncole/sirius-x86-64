#include <stdlib.h>

#undef        lldiv

lldiv_t lldiv(long long numer,long long denom) {
	
	lldiv_t d;

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
