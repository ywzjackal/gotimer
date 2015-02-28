#ifndef __JK_TIMER_HEADER_FILE
#define __JK_TIMER_HEADER_FILE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define TIMER_FLAGS TIMER_ABSTIME
#define MUX_TIMER_NAME_SIZE 128

typedef struct sigevent Sigevent;

typedef struct jktimer_t{
   pthread_attr_t pthreadAttr;
   struct sched_param schedParam;
   struct sigevent sig;
   void* callbackParam;
   struct itimerspec timerspec;
   timer_t timerId;
   int counter;
   int n_maxCount;
   char str_name[MUX_TIMER_NAME_SIZE];
}jktimer_t;

void MuxTimerCallbackWarp(union sigval val);

void jktimer_init(jktimer_t* p_timer, clockid_t type, int n_sched_priority,
					void* p_callback_params_ptr,	const char* str_name);	
					
void jktimer_stop(jktimer_t* p_timer);

void jktimer_start(jktimer_t* p_timer, long long int n_begin_sec, int n_begin_nsec,
		long long int n_interval_sec, int n_interval_nsec, int n_max_count);	

void jktimer_delete(jktimer_t* p_timer);

#endif