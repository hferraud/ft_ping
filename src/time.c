#include <sys/time.h>

#include "time.h"

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

void normalize_timeval(struct timeval *tv) {
	while (tv->tv_usec < 0) {
	    tv->tv_usec += 1000000;
	    tv->tv_sec--;
	}
	while (tv->tv_usec >= 1000000) {
	    tv->tv_usec -= 1000000;
	    tv->tv_sec++;
	}
}