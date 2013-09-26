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
static SS_RQSTATUS rqs;

/* Initial the ready queue (system ring). */
void SSInit() {
	SDSInitRing(&sys_r, SS_READYQUEUESIZE, (void *)pcbs);
	rqs.err = SS_READYQUEUEOK;
	rqs.run = SS_RUNSCHEDULING;
	return;
}

/* Admit a job into the ready queue. */
uint8_t SSAdmitJob(void *p, SS_CB cb) {
	SS_PCB pcb;
	pcb.cb = cb;
	pcb.p = p;
	rqs.err = SDSPushRing(&sys_r, &pcb, sizeof(SS_PCB));
	return rqs.err;
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
	rqs.run = run;
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
		pcb = SSScheduleJob(&(rqs.err));
		/* Dispatch the job if there is more then one job. */
		if(rqs.err == SS_READYQUEUEOK) {
			/* Dispatch and execute the job. */
			SSDispatchJob(pcb);
			/* Exit the job. */
			rqs.err = SSExitJob();
			/* Counts. */
			n += 1;
		}
	}while(rqs.err == SS_READYQUEUEOK);

	return;
}

/* Main loop for system scheduling. */
uint8_t SSMainLoop() {
	SS_PCB *pcb;

	while(rqs.run == SS_RUNSCHEDULING) {
		/* Schedule and pick a job. */
		pcb = SSScheduleJob(&(rqs.err));
		/* Dispatch the job if there is more then one job. */
		if(rqs.err == SS_READYQUEUEOK) {
			/* Dispatch and execute the job. */
			SSDispatchJob(pcb);
			/* Exit the job. */
			rqs.err = SSExitJob();
		}
	}

	/* Consume left jobs if it was break with that request. */
	if(rqs.run == SS_BREAKANDCONSUMELEFT)
		SSConsumeLeft(0);

	return 0;
}

/* Have the debug information of ready queue (system ring). */
SS_RQSTATUS * SSDebug() {
	rqs.len = sys_r.len;
	rqs.size = SDSSize(&sys_r);

	return &rqs;
}
