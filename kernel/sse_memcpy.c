#include <stddef.h>

#define small_memcpy( dst, src,n)\
{\
register unsigned long int dummy;\
__asm__ __volatile__(\
  "rep; movsb"\
  :"=&D"(dst), "=&S"(src), "=&c"(dummy)\
  :"0" (dst), "1" (src),"2" (n)\
  : "memory");\
}

#define SSE_MMREG_SIZE 16

void *sse_memcpy(void * s1, const void * s2, size_t len)
{
	void *retval = s1;
  	size_t n = len;
  	unsigned char *dst = (unsigned char *)s1;
  	unsigned char *src = (unsigned char *)s2;
  	
  	if( (((unsigned long int)dst)%SSE_MMREG_SIZE) != 0 \
  	|| (((unsigned long int)src)%SSE_MMREG_SIZE) != 0 )  {
  		// memoria nao alinhada 
  		__asm__ __volatile__(
  			"cld; rep movsq\n"
  			: :"D"(dst),"S"(src),"c"(n/8): "flags", "memory");
  			
  			if(n%8) small_memcpy(dst, src, n%8);
  			
  			return retval;
  	
  	}
  	
  	/* PREFETCH has effect even for MOVSB instruction */
  	__asm__ __volatile__ (
		"   prefetchnta (%0)\n"
		"   prefetchnta 32(%0)\n"
    	"   prefetchnta 64(%0)\n"
    	"   prefetchnta 96(%0)\n"
    	"   prefetchnta 128(%0)\n"
    	"   prefetchnta 160(%0)\n"
    	"   prefetchnta 192(%0)\n"
    	"   prefetchnta 224(%0)\n"
    	"   prefetchnta 256(%0)\n"
   		"   prefetchnta 288(%0)\n"
   		: : "r" (src) );
    		
    size_t x = n/64;
  	if(x > 0) {
  	
  		for(int i = 0; i < x; i ++) {
  			__asm__ __volatile__ (
    		"prefetchnta 320(%0)\n"
       		"prefetchnta 352(%0)\n"
        	"movaps (%0), %%xmm0\n"
       		"movaps 16(%0), %%xmm1\n"
        	"movaps 32(%0), %%xmm2\n"
        	"movaps 48(%0), %%xmm3\n"
       		"movntps %%xmm0, (%1)\n"
        	"movntps %%xmm1, 16(%1)\n"
	  		"movntps %%xmm2, 32(%1)\n"
      		"movntps %%xmm3, 48(%1)\n"
    	    :: "r" (src), "r" (dst) : "memory");
	    	dst += 64;
    		src += 64;
    		
   		}

	   	/* since movntq is weakly-ordered, a "sfence"
	     * is needed to become ordered again. */
	    __asm__ __volatile__ ("sfence":::"memory");
	    
	    // enables to use FPU 
	    __asm__ __volatile__ ("emms":::"memory");
	}
	
	if( n%64 ) small_memcpy(dst, src, n%64); 
	
	return retval;
}

