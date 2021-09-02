#include <ctype.h>


// converte o caracter maiúsculo em minúsculo
int tolower(int c)
{
	if (isupper(c)) c -= 'A'-'a';
	return (c);

}
