#ifndef _SIGNAL_H_
#define _SIGNAL_H_

	#define NR_SIGNALS 23
	#define NSIG       23

	/* Signals. */
	#define SIGNULL  0
	#define SIGKILL  1
	#define SIGSTOP  2
	#define SIGURG   3
	#define SIGABRT  4
	#define SIGBUS   5
	#define SIGCHLD  6
	#define SIGCONT  7
	#define SIGFPE   8
	#define SIGHUP   9
	#define SIGILL  10
	#define SIGINT  11
	#define SIGPIPE 12
	#define SIGQUIT 13
	#define SIGSEGV 14
	#define SIGTERM 15
	#define SIGTSTP 16
	#define SIGTTIN 17
	#define SIGTTOU 18
	#define SIGALRM 19
	#define SIGUSR1 20
	#define SIGUSR2 21
	#define SIGTRAP 22
	

	#define SIG_DFL ((void (*)(int))0)	/* Default action */
	#define SIG_IGN ((void (*)(int))1)	/* Ignore action */
	#define SIG_ERR ((void (*)(int))-1)	/* Error return */
	
	typedef int	sig_atomic_t;		/* Atomic entity type (ANSI) */
	typedef unsigned int sigset_t;		/* 32 bits */
	
	struct sigaction {
		void (*sa_handler)(int);
		sigset_t sa_mask;
		int sa_flags;
	};
	
	//void (*signal(int _sig, void (*_func)(int)))(int);
	
typedef unsigned int sigset_t;

/* Type of a signal handler.  */
typedef void (*sighandler_t)(int);
typedef void (*_sig_func_ptr)(int);

typedef sighandler_t __sighandler_t;
typedef sighandler_t sig_t;
	
sighandler_t signal (int signum, sighandler_t handler);


#endif
