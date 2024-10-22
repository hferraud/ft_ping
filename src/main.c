#include <signal.h>
#include <stdlib.h>

#include "parser.h"
#include "print.h"
#include "rtt.h"

void sigint_handler(int signal);

rtt_t rtt_g = {0};

int main(int argc, char** argv) {
	command_args_t args;
	ping_data_t ping_data = {0};

	args = parse(argc, argv);
	if (init_ping(&args, &ping_data) == -1) {
		return EXIT_FAILURE;
	}
	signal(SIGINT, sigint_handler);
	if (ping(&args, &ping_data) == -1) {
		close(ping_data.socket_fd);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void sigint_handler(int signal) {
	(void)signal;
	print_rtt();
	close(rtt_g.socket_fd);
	exit(0);
}
