#ifndef PING_RTT_H
#define PING_RTT_H

#include <unistd.h>
#include <sys/time.h>

#include "ping.h"

typedef struct rtt_s {
	char			*destination;
	size_t			transmitted;
	size_t			received;
	int32_t			socket_fd;
	float			sum;
	float			squared_sum;
	float			min;
	float			max;
} rtt_t;

void update_rtt(ping_response_t *ping_response);
float get_rtt_avg();
float get_rtt_stddev();

#endif
