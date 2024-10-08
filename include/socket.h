#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <inttypes.h>
#include <netinet/in.h>
#include <netdb.h>

#include "ping.h"

int32_t	init_icmp_socket();
int32_t dns_lookup(char* hostname, struct sockaddr_in *address);
int32_t reverse_dns_lookup(struct sockaddr_in *address, char *host, size_t len);

#endif
