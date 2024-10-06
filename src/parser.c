#include <string.h>

#include "parser.h"

static void	demux(char const *, command_args_t*);
static void	parse_option(char const *, command_args_t*);
void parse_verbose(char const*, command_args_t*);
static void parse_destination(char const *, command_args_t*);

command_args_t parse(int argc, char** argv) {
	command_args_t config = {0};

	for (int i = 1; i < argc; ++i) {
		demux(argv[i], &config);
	}
	return config;
}

void demux(char const * arg, command_args_t* config) {
	if (*arg == '-') {
		parse_option(arg, config);
	} else {
		parse_destination(arg, config);
	}
}

void parse_option(char const *arg, command_args_t* config) {
	char const		name[] = {'v', 0};
	parse_fct_t f[] = {parse_verbose};

	size_t i = 0;
	while (name[i]) {
		if (name[i] == arg[1]) {
			f[i](arg, config);
		}
		++i;
	}
}

void parse_verbose(char const* arg, command_args_t* config) {
	(void)arg;
	config->verbose = true;
}

void parse_destination(char const *arg, command_args_t* config) {
	config->destination = strdup(arg);
}
