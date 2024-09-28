#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

#define SOCKET_TIMEOUT_SEC 1
#define SOCKET_TIMEOUT_USEC 0

int32_t set_socket_timeout(int32_t socket_fd, uint32_t sec, uint32_t usec);

/**
 * @return -1 on error, 0 otherwise.
 */
int32_t init_icmp_socket() {
	int32_t socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	set_socket_timeout(socket_fd, SOCKET_TIMEOUT_SEC, SOCKET_TIMEOUT_USEC);
}

/**
 * @return -1 on error, 0 otherwise.
 */
int32_t set_socket_timeout(int32_t socket_fd, uint32_t sec, uint32_t usec) {
	struct timeval timeout = {sec, usec};
	if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
		return -1;
	}
	return setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

/**
 * @return -1 on error, 0 otherwise.
 */
int32_t dns_lookup(char* hostname, struct sockaddr_in *address) {
	struct hostent* host = gethostbyname(hostname);
	if (host == NULL) {
		return -1;
	}
	address->sin_addr = *(struct in_addr *)host->h_addr_list[0];
	return 0;
}
