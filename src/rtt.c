#include <stdio.h>
#include <math.h>

#include "rtt.h"
#include "time.h"

extern rtt_t rtt_g;

void update_rtt(float travel_time) {
	rtt_g.received += 1;
	if (rtt_g.min == 0 || travel_time < rtt_g.min) {
		rtt_g.min = travel_time;
	}
	if (rtt_g.max == 0 || travel_time > rtt_g.max) {
		rtt_g.max = travel_time;
	}
	rtt_g.sum += travel_time;
	rtt_g.squared_sum += travel_time * travel_time;
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
