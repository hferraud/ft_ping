#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <inttypes.h>
#include <netinet/in.h>

int		init_icmp_socket();
int32_t	dns_lookup(char* hostname, struct sockaddr_in *address);

#endif
