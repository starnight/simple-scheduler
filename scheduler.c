/*------------------------------------------------------------------------------
  Copyright (c) 2013 The Simple Scheduler Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include "scheduler.h"

/* Packaged callback function. */
typedef struct _SS_PACKAGED_CALLBACK {
	SS_CB cb;
	void *p;
} SS_PCB;

/* System ring for scheduler. */
static SDS_RING sys_r;
/* Ready queue (system ring) memory. */
static SS_PCB pcbs[SS_READYQUEUESIZE];
/* Ready queue (system ring) status. */
static SS_SSTATUS ss;

/* Initial the scheduler. */
void SSInit() {
	SDSInitRing(&sys_r, SS_READYQUEUESIZE, (void *)pcbs);
	ss.err = SS_READYQUEUEOK;
	ss.run = SS_RUNSCHEDULING;
	return;
}

/* Admit a job into the ready queue. */
uint8_t SSAdmitJob(void *p, SS_CB cb) {
	SS_PCB pcb;
	pcb.cb = cb;
	pcb.p = p;
	ss.err = SDSPushRing(&sys_r, &pcb, sizeof(SS_PCB));
	return ss.err;
}

/* Schedule and choose the job. */
inline SS_PCB * SSScheduleJob(uint8_t *err) {
	return (SS_PCB *)SDSFrontRing(&sys_r, sizeof(SS_PCB), err);
}

/* Dispatch and execute the job. */
inline void SSDispatchJob(SS_PCB *pcb) {
	pcb->cb(pcb->p);
	return;
}

/* Exit the job. */
inline uint8_t SSExitJob() {
	return SDSPopRing(&sys_r);
}

/* Break scheduling. */
void SSBreak(uint8_t run) {
	ss.run = run;
	return;
}

/* Consume left jobs in ready queue. */
void SSConsumeLeft(uint8_t m) {
	SS_PCB *pcb;
	uint8_t n = 0;

	do {
		/* Make sure the consumed left jobs are under limit. */
		if((m != 0) && (n >= m)) break;
		/* Schedule and pick a job. */
		pcb = SSScheduleJob(&(ss.err));
		/* Dispatch the job if there is more then one job. */
		if(ss.err == SS_READYQUEUEOK) {
			/* Dispatch and execute the job. */
			SSDispatchJob(pcb);
			/* Exit the job. */
			ss.err = SSExitJob();
			/* Counts. */
			n += 1;
		}
	}while(ss.err == SS_READYQUEUEOK);

	return;
}

/* Main loop for system scheduling. */
void SSMainLoop() {
	SS_PCB *pcb;

	while(ss.run == SS_RUNSCHEDULING) {
		/* Schedule and pick a job. */
		pcb = SSScheduleJob(&(ss.err));
		/* Dispatch the job if there is more then one job. */
		if(pcb != NULL) {
			/* Dispatch and execute the job. */
			SSDispatchJob(pcb);
			/* Exit the job. */
			ss.err = SSExitJob();
		}
	}

	/* Consume left jobs if it was break with that request. */
	if(ss.run == SS_BREAKANDCONSUMELEFT)
		SSConsumeLeft(0);

	return;
}

/* Have the debug information of the scheduler. */
SS_SSTATUS * SSDebug() {
	ss.len = sys_r.len;
	ss.size = SDSSize(&sys_r);

	return &ss;
}
