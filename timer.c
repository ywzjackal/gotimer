#include "timer.h"
#define USE_SCHED_FIFO 1

 void jktimer_start(jktimer_t* p_timer, long long int n_begin_sec, int n_begin_nsec,
		long long int n_interval_sec, int n_interval_nsec, int n_max_count) {
	p_timer->n_maxCount = n_max_count;
	//
	p_timer->timerspec.it_value.tv_sec = n_begin_sec;
	p_timer->timerspec.it_value.tv_nsec = n_begin_nsec;
	p_timer->timerspec.it_interval.tv_sec = n_interval_sec;
	p_timer->timerspec.it_interval.tv_nsec = n_interval_nsec;
	//
	struct itimerspec out;
	int rt = timer_settime(p_timer->timerId, TIMER_FLAGS, &p_timer->timerspec, &out);
	if (rt != 0) {
		printf("`timer_settime` Fail! bs:%lld, bns:%09d, is:%lld, ins:%09d, c:%d \n"
			   "errno:%d, msg:%s\n",
			n_begin_sec,n_begin_nsec,n_interval_sec,n_interval_nsec,n_max_count,
			errno,strerror(errno));
		exit(-5);
	}
	printf("`timer_settime` Success! bs:%lld, bns:%09d, is:%lld, ins:%09d, c:%d \n",
		n_begin_sec,n_begin_nsec,n_interval_sec,n_interval_nsec,n_max_count);

	return;
 }

 void jktimer_stop(jktimer_t* p_timer) {
	struct itimerspec timerspec;
	memset(&timerspec, 0, sizeof(timerspec));
	int rt = timer_settime(p_timer->timerId, TIMER_FLAGS, &timerspec, NULL);
	return;
 }

 void MuxTimerCallbackWarp(union sigval val) {
	jktimer_t* p_timer = (jktimer_t*) val.sival_ptr;
    int n_overrun = timer_getoverrun(p_timer->timerId);

	p_timer->counter++;
	val.sival_int = p_timer->counter;

	GoCallback(p_timer->callbackParam);

	if (p_timer->n_maxCount) {
		if (p_timer->n_maxCount <= p_timer->counter) {
			printf("timer max count arrived!\n");
			jktimer_stop(p_timer);
		}
	}
 }

 void jktimer_init(jktimer_t* p_timer, clockid_t type, int n_sched_priority,
					void* p_callback_params_ptr, const char* str_name){
	int rt = 0;
	bzero(p_timer->str_name, MUX_TIMER_NAME_SIZE);
	strcpy(p_timer->str_name, str_name);
	p_timer->callbackParam = p_callback_params_ptr;
	p_timer->schedParam.sched_priority = n_sched_priority;
 #if USE_SCHED_FIFO
	rt = sched_setscheduler(0, SCHED_FIFO, &p_timer->schedParam);
	if(rt){
		printf("Fail to `sched_setscheduler` errno:%d, msg:%s\n",errno,strerror(errno));
	}
 #endif
	//
	p_timer->sig.sigev_notify = SIGEV_THREAD;
	p_timer->sig.sigev_notify_function = MuxTimerCallbackWarp;
	p_timer->sig.sigev_value.sival_ptr = p_timer;
	p_timer->sig.sigev_notify_attributes = &p_timer->pthreadAttr;
	pthread_attr_init(&p_timer->pthreadAttr);
	pthread_attr_setschedparam(&p_timer->pthreadAttr, &p_timer->schedParam);

	rt = timer_create(type, &p_timer->sig, &p_timer->timerId);
	if(rt != 0){
		printf("`timer_create` Fail \n errno:%d, msg:%s\n",errno,strerror(errno));
		exit(-10);
	}
	printf("`timer_create` Success, timer id: %p \n", (void*)p_timer->timerId);

	return;
 }

 void jktimer_delete(jktimer_t* p_timer){
	int rt = 0;
	rt = timer_delete(p_timer->timerId);
	if(rt == 0){
		printf("`timer_delete` Success! %s\n", p_timer->str_name);
	}else{
		printf("`timer_delete` Fail! timer:'%s', errno:%d,msg:%s\n",
		       p_timer->str_name,errno,strerror(errno));
	}
 }