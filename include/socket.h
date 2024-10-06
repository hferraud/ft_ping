#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <inttypes.h>
#include <netinet/in.h>
#include <netdb.h>

int32_t	init_icmp_socket();
int32_t dns_lookup(char* hostname, struct sockaddr_in *address);

#endif
