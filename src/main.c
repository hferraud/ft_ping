#include "parser.h"
#include "socket.h"
#include "errno.h"

#include <stdio.h>

int main(int argc, char** argv) {
	command_args_t args;
	ping_data_t ping_data;

	args = parse(argc, argv);
	init_ping(&args, &ping_data);
	ping(&ping_data);
	return 0;
}
