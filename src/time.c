#include <sys/time.h>
#include <math.h>

#include "time.h"

#include <stdio.h>

struct timeval elapsed_time(struct timeval start, struct timeval end) {
	struct timeval elapsed;

	elapsed.tv_sec = end.tv_sec - start.tv_sec;
	elapsed.tv_usec = end.tv_usec - start.tv_usec;
	normalize_timeval(&elapsed);
	return elapsed;
}

float tv_to_ms(struct timeval tv) {
	return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
}

struct timeval sec_to_tv(const double seconds) {
	struct timeval tv;

	tv.tv_sec = (long)seconds;
	tv.tv_usec = (long)((seconds - tv.tv_sec) * 1000000);
	return tv;
}

void normalize_timeval(struct timeval *tv) {
	while (tv->tv_usec < 0) {
	    tv->tv_usec += 1000000;
	    tv->tv_sec--;
	}
	while (tv->tv_usec >= 1000000) {
	    tv->tv_usec -= 1000000;
	    tv->tv_sec++;
	}
	if (tv->tv_sec < 0) {
		tv->tv_sec = 0;
		tv->tv_usec = 0;
	}
}