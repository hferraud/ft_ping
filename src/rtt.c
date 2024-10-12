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

void print_rtt() {
	printf("--- ping statistics ---\n");
	printf("%zu packets transmitted, ", rtt_g.transmitted);
	printf("%zu received, ", rtt_g.received);
	printf("%zu%% packets lost, ", (1 - rtt_g.received / rtt_g.transmitted) * 100);
	printf("time %zu ms\n", (size_t)tv_to_ms(elapsed_time(rtt_g.start, rtt_g.last)));
	if (rtt_g.received == 0) {
		return;
	}
	printf("rtt min/avg/max/mdev = ");
	printf("%.3f/", rtt_g.min);
	printf("%.3f/", rtt_g.sum / (float)rtt_g.received);
	printf("%.3f/", rtt_g.max);
	printf("%.3f ms\n", sqrtf((rtt_g.squared_sum / (float)rtt_g.received) -
		(rtt_g.sum / (float)rtt_g.received) * (rtt_g.sum / (float)rtt_g.received)));
}