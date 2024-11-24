#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <netinet/in.h>

#include "parser.h"

typedef struct ping_data_s {
	struct sockaddr_in	address;
	struct timeval		send_timestamp;
	command_args_t		cmd_args;
	uint8_t				*packet;
	size_t				packet_size;
	int32_t				socket_fd;
	uint16_t			sequence;
	uint16_t			pid;
	uint8_t				type;
} ping_data_t;

typedef struct ping_response_s {
	struct timeval		trip_time;
	struct sockaddr_in	address;
	struct timeval		recv_timestamp;
	size_t				packet_size;
	int32_t				id;
	uint8_t				ttl;
	uint8_t				code;
	uint8_t				type;
} ping_response_t;

void ping(ping_data_t *ping_data);
void init_ping(ping_data_t *ping_data);

#endif
