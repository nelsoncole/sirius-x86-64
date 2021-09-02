#ifndef __LOCALE_H
#define __LOCALE_H

#define LC_ALL	    0 /**< All categories.                                                                           */
#define LC_COLLATE  1 /**< Regular expressions and collation functions.                                              */
#define LC_CTYPE    2 /**< Regular expressions, character classification, character conversion, and wide-characters. */
#define LC_MONETARY 3 /**< Monetary values.                                                                          */
#define LC_NUMERIC  4 /**< Numeric values.                                                                           */
#define LC_TIME     5 /**< Time conversion.  										 */

extern char *setlocale(int, const char *);

#endif
