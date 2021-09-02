
#include <stdlib.h>


#undef        atol


//Convert a string to an int.

long atol(const char *nptr)
{
  return (long) strtol (nptr, (char **) NULL, 10);
}
