#include <stdlib.h>
#include <argp.h>

#include "parser.h"

static error_t parse_opt(int key, char* arg, struct argp_state *state);

int parse(int argc, char** argv, command_args_t* cmd_args) {
	const struct argp_option argp_options[] = {
			{"count", 'c', "NUMBER", 0, "stop after sending NUMBER packets", 0},
			{"verbose", 'v', NULL, 0, "verbose output", 0},
			{0},
	};
	const char args_doc[] = "HOST ...";
	const char doc[] = "Send ICMP ECHO_REQUEST packets to network hosts."
					   "\vOptions marked with (root only) are available only to "
					   "superuser.";
	struct argp argp = {
			argp_options,
			parse_opt,
			args_doc,
			doc,
			NULL,
			NULL,
			NULL,
	};
	argp_parse(&argp, argc, argv, 0, NULL, cmd_args);
	return 0;
}

static error_t parse_opt(int key, char* arg, struct argp_state *state) {
	command_args_t *cmd_args = (command_args_t *)state->input;
	switch (key) {
		case 'c':
			cmd_args->count = atoll(arg);
			break;
		case 'v':
			cmd_args->verbose = 1;
			break;
		case ARGP_KEY_ARG:
			cmd_args->destination = arg;
			break;
		case ARGP_KEY_NO_ARGS:
			argp_error(state, "missing host operand");
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
