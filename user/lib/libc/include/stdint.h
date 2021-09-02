#ifndef _STDINT_H
#define _STDINT_H


typedef char int8_t ;
typedef unsigned char uint8_t ;

typedef short int16_t ;
typedef unsigned short uint16_t ;

typedef int int32_t ;
typedef unsigned int uint32_t ;

typedef long long int64_t;
typedef unsigned long long uint64_t;

/*

intN_t
uintN_t
int_leastN_t
uint_leastN_t
int_fastN_t
uint_fastN_t
intptr_t
uintptr_t
intmax_t
uintmax_t
INTN_MIN
INTN_MAX
UINTN_MAX
INT_LEASTN_MIN
INT_LEASTN_MAX
UINT_LEASTN_MAX
INT_FASTN_MIN
INT_FASTN_MAX
UINT_FASTN_MAX
INTPTR_MIN
INTPTR_MAX
UINTPTR_MAX
INTMAX_MIN
INTMAX_MAX
UINTMAX_MAX
PTRDIFF_MIN
PTRDIFF_MAX
SIG_ATOMIC_MIN
SIG_ATOMIC_MAX
SIZE_MAX
WCHAR_MIN
WCHAR_MAX
WINT_MIN
WINT_MAX
INTN_C(value)
UINTN_C(value)
INTMAX_C(value)
UINTMA

*/

typedef unsigned int intmax_t;
typedef struct {
	intmax_t quot;		/* quotient */
	intmax_t rem;		/* remainder */
} imaxdiv_t;



#endif
