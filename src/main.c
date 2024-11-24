#include <signal.h>
#include <stdlib.h>
#include <error.h>

#include "parser.h"
#include "print.h"
#include "rtt.h"

void sigint_handler(int signal);
void at_exit(void);

rtt_t rtt_g = {0};

int main(int argc, char** argv) {
	ping_data_t ping_data = {0};

	parse(argc, argv, &ping_data.cmd_args);
	init_ping(&ping_data);
	if (atexit(at_exit) == -1) {
		close(ping_data.socket_fd);
		error(EXIT_FAILURE, 0, "atexit()");
	}
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		error(EXIT_FAILURE, 0, "signal()");
	}
	ping(&ping_data);
	return EXIT_SUCCESS;
}

void sigint_handler(int signal) {
	(void)signal;
}

void at_exit(void) {
	print_rtt();
	close(rtt_g.socket_fd);
}
