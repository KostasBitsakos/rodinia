#ifndef __TIMING_H__
#define __TIMING_H__

#include <sys/time.h>
#include <CL/cl.h>

void time_measure_start(struct timeval *tv);
void time_measure_end(struct timeval *tv);

/* tvsub: ret = x - y. */
static inline void tvsub(struct timeval *x,
		struct timeval *y,
		struct timeval *ret)
{
	ret->tv_sec = x->tv_sec - y->tv_sec;
	ret->tv_usec = x->tv_usec - y->tv_usec;
	if (ret->tv_usec < 0) {
		ret->tv_sec--;
		ret->tv_usec += 1000000;
	}
}

#if 0 // ckatsak
float probe_event_time(cl_event, cl_command_queue);
#endif

static inline float
probe_event_time(cl_event event, cl_command_queue command_queue) {
	clFinish(command_queue);

	cl_int error;// = 0;
	cl_ulong eventStart, eventEnd;

	error = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,
			sizeof(cl_ulong), &eventStart, NULL);
	if (error != CL_SUCCESS) {
		printf("ERROR (%d) in event start profiling.\n", error);
		return 0;
	}
	error = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,
			sizeof(cl_ulong), &eventEnd, NULL);
	if (error != CL_SUCCESS) {
		printf("ERROR (%d) in event end profiling.\n", error);
		return 0;
	}

	return (float)((eventEnd - eventStart) / 1000000.0);
}

#endif
