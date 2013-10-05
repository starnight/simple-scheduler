Simple Scheduler
================

It is a simple scheduler library which does "First In, First Out" (FIFO) scheduling works with no priority
and non-preemptive multitasking.
It is the "Functions", which could also be called callback functions or jobs,
that Simple Scheduler schedules, not the "Processes".

Find more information from the [Wiki](https://github.com/starnight/simple-scheduler/wiki "Simple Scheduler Wiki").

Have the code
-------------

You can have it from the GitHub: [https://github.com/starnight/simple-scheduler](https://github.com/starnight/simple-scheduler "Simple Scheduler")

Or, clone the source code from GitHub.

```
git clone git://github.com/starnight/simple-scheduler.git
```

Demo usage
----------

Before compile this library, you must have the compiler, for example GCC, of
course.  There is a demo code which is '[demo.c](demo.c)'.  You can just use the
'make' utility which will download the dependency with git and compile the codes
according to the 'Makefile'.  Then execute the program 'demo.bin' for demo.  I
also use the program to test the APIs I wrote working right or wrong.

```
$ make
$ ./demo.bin
```

The program will admit jobs and do scheduling demo.  
It will also break scheduling at the time.  
Besides, also demoes consuming left jobs.

Dependency
----------

Before compiling, you must have the dependent library
[Simple Data Structure (SDS)](https://github.com/starnight/simple-data-structure "Simple Data Structure")
and put it into the subdirectory 'datastructure'.  
You could put it manually or just use the command "make",
"[Demo usage](#demo-usage)" talked above, which will check the dependency and
download automatically if it is needed.

Quick Start
-----------

This example demos the simple scheduler behavior.  

1. Have the unit simulating the interrupt. It will admit jobs (functions) into
   the ready queue while interrupt is triggered.
2. Initial the simple scheduler with SSInit().
3. Start to simulate triggering the interrupts talked above.
4. Call the simple scheduler main loop (SSMainLoop()) to do the scheduling,
   which also dispatches and executes jobs if it has more than one job.

Create a C file where the main function located.

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include "scheduler.h"

/* Define the alarm timer interval. */
#define TIMER_INTERVAL	2

void (*cbs[2])(void *);

/* The demo job foo#1. */
void foo1(void *p) {
	if(p != NULL) {
		printf("Executes foo#1 with an argument: %d.\n", *((int *)p));
		free(p);
	}
}

/* The demo job foo#2. */
void foo2(void *p) {
	if(p != NULL) {
		printf("Executes foo#2 with an argument: %d.\n", *((int *)p));
		free(p);
	}
}

/* The callback for time out. */
void alarm_handler() {
	int *r = NULL;
	int n;
	r = (int *) malloc(sizeof(int));
	*r = rand();
	n = *r % 2;
	/* Admit the random job with an argument. */
	SSAdmitJob(r, cbs[n]);
	printf("Admitted a job foo#%d with an argument: %d.\n", n, *r);
	/* Start next alarm. */
	alarm(TIMER_INTERVAL);
	return;
}

int main(void) {
	/* Bind the functions of demo jobs. */
	cbs[0] = foo1;
	cbs[1] = foo2;

	/* Initial the simple scheduler (System scheduler). */
	SSInit();
	
	/* Use the timer alarm to simulate the triggered event. */
	signal(SIGALRM, alarm_handler);
	/* Start timer alarm. */
	alarm(TIMER_INTERVAL);

	/* Going to the simple scheduler main loop. */
	SSMainLoop();

	return 0;
}
```

License
-------

Simple Scheduler's code uses the BSD license, see our '[LICENSE.md](https://github.com/starnight/simple-scheduler/blob/master/LICENSE.md "LICENSE.md")' file.
