/*------------------------------------------------------------------------------
  Copyright (c) 2013 The Simple Scheduler Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include "scheduler.h"

#ifndef TIMER_INTERVAL
#define TIMER_INTERVAL		1
#endif

#ifndef MAX_TRIGGER_TIMES
#define MAX_TRIGGER_TIMES	20
#endif

void (*cbs[3])(void *);
unsigned int tc;

/* Show the scheduler debug information. */
inline void debug() {
	SS_RQSTATUS *rqs;
	rqs = SSDebug();
	printf("The ready queue size is %d.\n", (int)rqs->len);
	printf("Remain %d callback functions.\n", (int)rqs->size);
	printf("The ready queue error status is %d.\n", (int)rqs->err);
	printf("The scheduler status is %d.\n", (int)rqs->run);
	return;
}

/* The demo job foo#0. */
void foo0(void *p) {
	printf("\033[1;31mfoo#0\033[m:\n");
	debug();
#ifdef DELAYJOB
	sleep(rand() % DELAYJOB);
#endif
}

/* The demo job foo#1. */
void foo1(void *p) {
	printf("\033[1;32mfoo#1\033[m:\t");
	SSAdmitJob(NULL, foo0);
	if (p != NULL) {
		printf("Get parameter \033[1;36m%d\033[m.  ", *((int *) p));
		free(p);
	}
	printf("Admitted a job.\n");
	debug();
#ifdef DELAYJOB
	sleep(rand() % DELAYJOB);
#endif
}

/* The demo job foo#2. */
void foo2(void *p) {
	printf("\033[1;33mfoo#2\033[m:\t");
	SSAdmitJob(NULL, foo1);
	printf("Admitted a job.\n");
	debug();
#ifdef DELAYJOB
	sleep(rand() % DELAYJOB);
#endif
}

/* The callback for time out. */
void alarm_handler() {
	int *r = NULL;
	int f = rand() % 3;
	/* Admit the random job which is foo#0 to foo#2. */
	/* Check the foo number to pass parameter or not. */
	if (f == 1) {
		r = (int *) malloc(sizeof(int));
		*r = rand();
		printf("Pass parameter \033[1;34m%d\033[m.\n", *r);
	}
	/* Admit the job. */
	SSAdmitJob(r, cbs[f]);

#if MAX_TRIGGER_TIMES > 0
	/* Check the trigger time. */
	if(tc < MAX_TRIGGER_TIMES) {
		tc += 1;
		/* Start next alarm. */
		alarm(TIMER_INTERVAL);

	}
	else {
		SSBreak(SS_BREAKSCHEDULING);
		printf("\033[31mScheduling is breaking!!!\033[m\n");
	}
#else
	/* Start next alarm. */
	alarm(TIMER_INTERVAL);
#endif
}

int main(void) {
	/* Bind the functions of demo jobs. */
	cbs[0] = foo0;
	cbs[1] = foo1;
	cbs[2] = foo2;

#if MAX_TRIGGER_TIMES > 0
	/* Initial triger times counter. */
	tc = 0;
#endif

	/* Initial the simple scheduler (System scheduler). */
	SSInit();

	/* Admit the demo jobs first time.  Just for demo. */
	SSAdmitJob(NULL, cbs[0]);
	SSAdmitJob(NULL, cbs[1]);
	SSAdmitJob(NULL, cbs[2]);
	
	/* Use the timer alarm to simulate the triggered event. */
	signal(SIGALRM, alarm_handler);
	/* Start timer alarm. */
	alarm(TIMER_INTERVAL);

	/* Going to the simple scheduler main loop. */
	SSMainLoop();

#if MAX_TRIGGER_TIMES > 0
	/* Consume one left job. */
	printf("Consume 1 left job.\n");
	SSConsumeLeft(1);
	/* Consume all left jobs. */
	printf("Consume all left jobs.\n");
	SSConsumeLeft(0);
#endif

	/* Show the simple scheduler debug information at the end. */
	debug();
#if MAX_TRIGGER_TIMES > 0
	printf("Events have been triggered \033[31m%d\033[m times.\n", tc);
#endif
	return 0;
}
