#ifndef PING_RTT_H
#define PING_RTT_H

#include <unistd.h>
#include <sys/time.h>

#include "ping.h"

typedef struct rtt_s {
	struct timeval	start;
	struct timeval	last;
	ping_data_t		*ping_data;
	size_t			transmitted;
	size_t			received;
	float			sum;
	float			squared_sum;
	float			min;
	float			max;
} rtt_t;

void update_rtt(struct timeval recv_time, float travel_time);
void print_rtt();

#endif
