
#include <stdlib.h>


#undef        atoi


//Convert a string to an int.

int atoi (const char *nptr)
{
  return (int) strtol (nptr, (char **) 0, 10);
}
