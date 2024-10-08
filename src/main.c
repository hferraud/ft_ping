#include <signal.h>
#include <stdlib.h>

#include "parser.h"
#include "socket.h"
#include "rtt.h"

void sigint_handler(int signal);

rtt_t rtt_g = {0};

int main(int argc, char** argv) {
	signal(SIGINT, sigint_handler);
	command_args_t args;
	ping_data_t ping_data;

	args = parse(argc, argv);
	if (init_ping(&args, &ping_data) == -1) {
		return -1;
	}
	if (ping(&ping_data) == -1) {
		return -1;
	}
	return 0;
}

void sigint_handler(int signal) {
	(void)signal;
	print_rtt();
	close(rtt_g.socket_fd);
	exit(0);
}
