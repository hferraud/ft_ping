#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "ping.h"

#define SOCKET_TIMEOUT_SEC 1
#define SOCKET_TIMEOUT_USEC 0

int32_t set_sockopt_timeout(int32_t socket_fd, uint32_t sec, uint32_t usec);
int32_t set_sockopt_broadcast(int32_t socket_fd);

/**
 * @return On success socket fd is returned. On error, -1 is returned and errno is set.
 */
int32_t init_icmp_socket() {
	int32_t socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_fd == -1) {
		perror("init_icmp_socket: socket");
		return -1;
	}
	if (set_sockopt_timeout(socket_fd, SOCKET_TIMEOUT_SEC, SOCKET_TIMEOUT_USEC) == -1) {
		close(socket_fd);
		return -1;
	}
	if (set_sockopt_broadcast(socket_fd) == -1) {
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}

/**
 * @return On success 0 is returned. On error, -1 is returned.
 */
int32_t set_sockopt_timeout(int32_t socket_fd, uint32_t sec, uint32_t usec) {
	struct timeval timeout = {sec, usec};
	if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
		perror("set_sockopt_timeout: setsockopt");
		return -1;
	}
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
		perror("set_sockopt_timeout: setsockopt");
		return -1;
	}
	return 0;
}

int32_t set_sockopt_broadcast(int32_t socket_fd) {
	uint32_t optval = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		perror("set_sockopt_broadcast: setsockopt");
		return -1;
	}
	return 0;
}

/**
 * @return On success 0 is returned. On error -1 is returned if no addresses are found,
 * a status code is returned otherwise.
 */
int32_t dns_lookup(char* hostname, struct sockaddr_in *address) {
	int32_t status;
	struct addrinfo hints = {0};
	struct addrinfo *res;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(hostname, NULL, &hints, &res);
	if (status != 0 || res == NULL) {
		dprintf(STDERR_FILENO, "ping: unknown host\n");
		return -1;
	}
	*address = *(struct sockaddr_in *)res->ai_addr;
	freeaddrinfo(res);
	return 0;
}

