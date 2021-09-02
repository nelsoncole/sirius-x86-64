#include <signal.h>
#include <stdio.h>

//POSIX.1-2001, POSIX.1-2008, C89, C99.
sighandler_t signal (int signum, sighandler_t handler)
{
	printf("panic: signal( %x, %lx ) \n", signum, handler);
	for(;;);
    	return 0;
}

int sigaction ( int signum, const struct sigaction *act,
                struct sigaction *oldact )
{	printf("panic: sigaction( %x, ..., ... )\n", signum);
	for(;;);
    	return -1;
}

