
#include <stdlib.h>
#include <limits.h>

#undef        atof


//Convert a string to an float.

double atof(const char *nptr)
{
  return (double) strtod (nptr, (char **) NULL);
}
