#include <sys/time.h>

struct timeval elapsed_time(struct timeval start, struct timeval end) {
	struct timeval elapsed;

	elapsed.tv_sec = end.tv_sec - start.tv_sec;
	elapsed.tv_usec = end.tv_usec - start.tv_usec;
	if (elapsed.tv_usec < 0) {
		elapsed.tv_sec -= 1;
		elapsed.tv_usec += 1000000;
	}
	return elapsed;
}

float tv_to_ms(struct timeval tv) {
	return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
}
