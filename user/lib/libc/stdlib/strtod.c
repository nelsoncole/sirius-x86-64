#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>


#undef        strtod

//================================================

//credits.
//Copyright (C) 2002-2011 Michael Ringgaard.
//All rights reserved. 


//em assembly
//; Floating point infinity
//_infinity:
//__infinity:     db      000h, 000h, 000h, 000h, 000h, 000h, 0f0h, 07fh  

//char _infinity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f };
//unsigned long __infinity = &_infinity[0];

double _infinity = 0x7ff0000000000000;
//math.h
//#define HUGE_VAL _infinity

double strtod(const char *str, char **endptr){
	
  double number;
  int exponent;
  int negative;
  char *p = (char *) str;
  double p10;
  int n;
  int num_digits;
  int num_decimals;

  // Skip leading whitespace
  while (isspace(*p)) p++;

  // Handle optional sign
  negative = 0;
  switch (*p) {
    case '-': negative = 1; // Fall through to increment position
    case '+': p++;
  }

  number = 0.;
  exponent = 0;
  num_digits = 0;
  num_decimals = 0;

  // Process string of digits
  while (isdigit(*p)) {
    number = number * 10. + (*p - '0');
    p++;
    num_digits++;
  }

  // Process decimal part
  if (*p == '.') {
    p++;

    while (isdigit(*p)) {
      number = number * 10. + (*p - '0');
      p++;
      num_digits++;
      num_decimals++;
    }

    exponent -= num_decimals;
  }

  if (num_digits == 0) 
  {
	 //#bugbug 
    //errno = ERANGE;
    return 0.0;
  }

  // Correct for sign
  if (negative) number = -number;

  // Process an exponent string
  if (*p == 'e' || *p == 'E') {
    // Handle optional sign
    negative = 0;
    switch (*++p) {
      case '-': negative = 1;   // Fall through to increment pos
      case '+': p++;
    }

    // Process string of digits
    n = 0;
    while (isdigit(*p)) {
      n = n * 10 + (*p - '0');
      p++;
    }

    if (negative) {
      exponent -= n;
    } else {
      exponent += n;
    }
  }

  if (exponent < DBL_MIN_EXP  || exponent > DBL_MAX_EXP) 
  {
	 //#bugbug 
    //errno = ERANGE;
    //return (double) HUGE_VAL;
	  
	  return (double) _infinity;
  }

  // Scale the result
  p10 = 10.;
  n = exponent;
  if (n < 0) n = -n;
  while (n) {
    if (n & 1) {
      if (exponent < 0) {
        number /= p10;
      } else {
        number *= p10;
      }
    }
    n >>= 1;
    p10 *= p10;
  }

	
  //if (number == HUGE_VAL){
   if (number == _infinity){		
	  //#bugbug
	  //errno = ERANGE; 
  }
	
  if (endptr) *endptr = p;

  return number;
}
