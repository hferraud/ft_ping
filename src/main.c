#include "parser.h"
#include "socket.h"
#include "errno.h"

#include <stdio.h>

int main(int argc, char** argv) {
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
