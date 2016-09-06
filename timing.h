#ifndef __TIMING_H__
#define __TIMING_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>



#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
 
 
void myclock_gettime(struct timespec *ts) {
 
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
clock_serv_t cclock;
mach_timespec_t mts;
host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
clock_get_time(cclock, &mts);
mach_port_deallocate(mach_task_self(), cclock);
ts->tv_sec = mts.tv_sec;
ts->tv_nsec = mts.tv_nsec;
#else
clock_gettime(CLOCK_MONOTONIC, ts);
#endif
 
}
 





double ticks;
double secs;
struct tms t1,t2;
struct timeval tv1, tv2;

struct timespec ts1, ts2;

void startClockTime() {
	myclock_gettime(&ts1);
}

long endClockTime() {
	myclock_gettime(&ts2);
	return (ts2.tv_sec-ts1.tv_sec)*1000000000+(ts2.tv_nsec-ts1.tv_nsec);
}


void startTimer() {
	ticks= (double)sysconf(_SC_CLK_TCK);
	gettimeofday(&tv1,NULL);
	times (&t1);
}


double timeFromBegin() {
	times (&t2);
	return ((t2.tms_utime+t2.tms_stime)-(t1.tms_utime+t1.tms_stime))/ticks;
}


void startRealTimer() {
	gettimeofday(&tv1,NULL);
}

long realTimeFromBegin() {
	gettimeofday(&tv2,NULL);
	return (tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
}

#endif
