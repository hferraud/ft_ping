#include <stdio.h>
#include <math.h>
#include <netinet/ip_icmp.h>

#include "rtt.h"
#include "time.h"

extern rtt_t rtt_g;

void update_rtt(ping_response_t *ping_response) {
	float trip_time;

	if (ping_response->type != ICMP_ECHOREPLY) {
		return;
	}
	trip_time = tv_to_ms(ping_response->trip_time);
	rtt_g.received += 1;
	if (rtt_g.min == 0 || trip_time < rtt_g.min) {
		rtt_g.min = trip_time;
	}
	if (rtt_g.max == 0 || trip_time > rtt_g.max) {
		rtt_g.max = trip_time;
	}
	rtt_g.sum += trip_time;
	rtt_g.squared_sum += trip_time * trip_time;
}

float get_rtt_avg() {
	if (rtt_g.received == 0) {
		return 0;
	}
	return rtt_g.sum / rtt_g.received;
}

float get_rtt_stddev() {
	if (rtt_g.received == 0) {
		return 0;
	}
	float mean = get_rtt_avg();
	float variance = rtt_g.squared_sum / rtt_g.received - mean * mean;
	return sqrtf(variance);
}
