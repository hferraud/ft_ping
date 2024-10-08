#include "parser.h"
#include "socket.h"
#include "rtt.h"

rtt_t rtt_g = {0};

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
