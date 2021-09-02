
#include <stdlib.h>
#include <limits.h>


#undef        strtof


float strtof(const char *str, char **endptr) {

  return (float) strtod(str, endptr);

}
