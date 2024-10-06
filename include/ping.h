#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <netinet/in.h>

#include "parser.h"

typedef struct ping_data_s {
	struct sockaddr_in	address;
	uint8_t				*packet;
	size_t				packet_size;
	int32_t				socket_fd;
	uint16_t			sequence;
	uint16_t			pid;
	uint8_t				type;
} ping_data_t;

int32_t ping(ping_data_t *ping_data);
int32_t init_ping(command_args_t *args, ping_data_t *ping_data);

#endif
