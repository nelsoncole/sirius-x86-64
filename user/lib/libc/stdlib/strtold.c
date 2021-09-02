#include <stdlib.h>
#include <limits.h>


#undef        strtold


//Convert a string to an float.

long double strtold(const char *str, char **endptr) {

  return strtod(str, endptr);
  
}
