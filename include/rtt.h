#ifndef PING_RTT_H
#define PING_RTT_H

#include <unistd.h>
#include <sys/time.h>

#include "ping.h"

typedef struct rtt_s {
	int32_t			socket_fd;
	size_t			transmitted;
	size_t			received;
	float			sum;
	float			squared_sum;
	float			min;
	float			max;
} rtt_t;

void update_rtt(float travel_time);
float get_rtt_avg();
float get_rtt_stddev();

#endif
