#include <string.h>
/*
char *strtok_r(char *str, const char *delim, char **saveptr)
{
	char *pos = (str ? str : *saveptr);
	
	if(!pos) return NULL;
	
	while( strchr(delim, *pos) ) pos ++;

	if( *pos == '\0' ) return NULL;

	char *ret = pos;
	while( !strchr(delim, *pos) ) {
	
		if(!*pos) break;
		
		pos ++;
	}
	
	if( *pos != '\0' ) 
	{
		*pos = '\0';
		pos ++;
		*saveptr = pos;
	}else { 
		*saveptr = NULL;
	}
	
	
	return ret;
}


#undef        strtok

char *strtok(char * restrict s1,const char * restrict s2)
{
	static char *saveptr;
    	return strtok_r ( s1, s2, &saveptr );

}*/


/*
 **************************************
 * strtok_r:
 *     Usada em strtok.
 *
 * Credits: 
 *     Apple. (Open Source)
 *
 *
 * usado em c99 Gramado by Fred Nora 
 */

char *strtok_r ( char *s, const char *delim, char **last ){
	
    	char *spanp;
    	int c, sc;
    	char *tok;

    	if ( s == NULL && (s = *last) == NULL )
	{
		
	   	 return NULL;
    	};

    	// Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	
cont:
    
	c = *s++;
    
	for ( spanp = (char *)delim; (sc = *spanp++) != 0; )
    	{
	    	if (c == sc)
		{
	        	goto cont;
	    	}
    	};

	// No non-delimiter characters. 
    	if ( c == 0 )		
    	{
	    	*last = NULL;
	    	return NULL;
    	};
	
    	tok = s -1;

    	// Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
    	// Note that delim must have one NUL; we stop if we see that, too.
	 
    	for (;;)
    	{
	    	c = *s++;
	    	spanp = (char *) delim;
	    
		do {
	        	if ( (sc = *spanp++) == c )
	        	{
		        	if ( c == 0 )
				{
		            		s = NULL;
					
		        	}else {
					
		            		char *w = s -1;
		            
					*w = '\0';
		        	};
				
		        	*last = s;
				
		        	return tok;
	        	};
			
	    	} while( sc != 0 );
        
		//Nothing.
		
	};
	
    // NOTREACHED 
};


#undef        strtok

char *strtok(char * restrict s1,const char * restrict s2)
{
	static char *last;
	
    	return strtok_r ( s1, s2, &last );

}
