#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <netinet/in.h>

#include "ping.h"

int32_t	init_icmp_socket(command_args_t *cmd_args);
void dns_lookup(char* hostname, struct sockaddr_in *address);

#endif
