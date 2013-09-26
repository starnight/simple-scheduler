/*------------------------------------------------------------------------------
  Copyright (c) 2013 The Simple Scheduler Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef _SIMPLE_SCHEDULER_H__
#define _SIMPLE_SCHEDULER_H__

#include <stdio.h>
#include <stdint.h>
#include "datastructure/datastructure.h"

/* Callback function prototype. */
typedef void (*SS_CB)(void *);

/* Ready queue status. */
typedef struct _SS_ready_queue_status {
	uint8_t len;
	uint8_t size;
	uint8_t err;
	uint8_t run;
} SS_RQSTATUS;

/* Define the ready queue length. */
#ifndef SS_READYQUEUESIZE
#define SS_READYQUEUESIZE	8
#endif

/* Define the ready queue running status. */
#define SS_RUNSCHEDULING		0
#define SS_BREAKSCHEDULING		1
#define SS_BREAKANDCONSUMELEFT	2

/* Define the ready queue error status. */
#define SS_READYQUEUEOK		SDS_OK
#define SS_READYQUEUEEMPTY	SDS_BUFFEREMPTY
#define SS_READYQUEUEFULL	SDS_BUFFEROVERFLOW

/* Initial the ready queue (system ring). */
void SSInit();
/* Admit a job into the ready queue. */
uint8_t SSAdmitJob(void *, SS_CB);
/* Main loop for system scheduling. */
uint8_t SSMainLoop();
/* Break scheduling. */
void SSBreak(uint8_t);
/* Consume left jobs in ready queue. */
void SSConsumeLeft(uint8_t);
/* Have the debug information of ready queue (system ring). */
SS_RQSTATUS * SSDebug();

#endif
