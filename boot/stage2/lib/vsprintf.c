#include <stdio.h>
#include <stdlib.h>

static char *_vsputs_r(char *dest, char *src)
{
	unsigned char *usrc = (unsigned char *) src;
	unsigned char *udest = (unsigned char *) dest;
	
	while(*usrc) *udest++ = *usrc++;

	return (char * ) udest;
	
}

int vsprintf(char * str,const char * fmt, va_list ap)
{

	char *str_tmp = str;

	char _c_r[] = "\0\0";

	int index = 0;
	unsigned char u;	
	int d;
	char c, *s;
	char buffer[256];

	while (fmt[index])
	{
		switch (fmt[index])
		{
		case '%':
			++index;
			switch (fmt[index])
			{
			
			case 'c':
				*_c_r = c = (char) va_arg (ap, int);
				str_tmp  = _vsputs_r(str_tmp,_c_r);
				break;

			case 's':
				s = va_arg (ap, char*);
				str_tmp  = _vsputs_r(str_tmp,s);
				break;

			case 'd':
			case 'i':
				d = va_arg (ap, int);
				itoa (d,buffer);
				str_tmp  = _vsputs_r(str_tmp,buffer);
				break;

			case 'u':
				u = va_arg (ap, unsigned int);
				itoa  (u,buffer);
				str_tmp  = _vsputs_r(str_tmp,buffer);
				break;

			case 'X':
			case 'x':
				d = va_arg (ap, int);
				i2hex(d, buffer,8);
				str_tmp  = _vsputs_r(str_tmp,buffer);
				break;
			
			default:
				str_tmp  = _vsputs_r(str_tmp,"%%");
				break;
				
				
			}
			break;

		default:
			*_c_r = fmt[index]; //
			str_tmp  = _vsputs_r(str_tmp,_c_r);
			break;
		}
		++index;
	}

    	return ((int)str_tmp - (int)str);

}

