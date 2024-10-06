#include "parser.h"
#include "socket.h"
#include "errno.h"

#include <stdio.h>

int main(int argc, char** argv) {
	command_args_t args;
	struct sockaddr_in address;

	args = parse(argc, argv);
	if (dns_lookup(args.destination, &address) == -1) {
		return errno;
	}
	int socket_fd = init_icmp_socket();
	if (socket_fd == -1) {
		return errno;
	}
	ping(socket_fd, address);
	return 0;
}
