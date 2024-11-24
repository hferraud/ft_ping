#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>

#include "ping.h"

#define SOCKET_TIMEOUT_SEC 1
#define SOCKET_TIMEOUT_USEC 0

static void set_sockopt_broadcast(int32_t socket_fd);
static void set_sockopt_ttl(int32_t socket_fd, int32_t ttl);

int32_t init_icmp_socket(command_args_t *cmd_args) {
	int32_t socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_fd < 0) {
		error(EXIT_FAILURE, errno, "socket()");
	}
	set_sockopt_broadcast(socket_fd);
	set_sockopt_ttl(socket_fd, cmd_args->ttl);
	return socket_fd;
}

static void set_sockopt_broadcast(int32_t socket_fd) {
	uint32_t optval = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
}

static void set_sockopt_ttl(int32_t socket_fd, int32_t ttl) {
	if (ttl == 0) {
		return;
	}
	if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
            error(0, errno, "setsockopt(IP_TTL)");
}

void dns_lookup(char* hostname, struct sockaddr_in *address) {
	int32_t status;
	struct addrinfo hints = {0};
	struct addrinfo *res;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(hostname, NULL, &hints, &res);
	if (status != 0 || res == NULL) {
		error(EXIT_FAILURE, 0, "unknown host");
	}
	*address = *(struct sockaddr_in *)res->ai_addr;
	freeaddrinfo(res);
}

