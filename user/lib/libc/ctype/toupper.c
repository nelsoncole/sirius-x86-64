#include <ctype.h>


// converte o caracter minúsculo em maiúsculo
int toupper(int c)
{

	if (islower(c)) c -= 'a'-'A';

	return (c);

}
