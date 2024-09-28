#include <string.h>

#include "parser.h"

static void	demux(char const *, config_t*);
static void	parse_option(char const *, config_t*);
void parse_verbose(char const*, config_t*);
static void parse_destination(char const *, config_t*);

config_t parse(int argc, char** argv) {
	config_t config;
	for (int i = 1; i < argc; ++i) {
		demux(argv[i], &config);
	}
}

void demux(char const * arg, config_t* config) {
	if (*arg == '-') {
		parse_option(arg, config);
	} else {
		parse_destination(arg, config);
	}
}

void parse_option(char const *arg, config_t* config) {
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

void parse_verbose(char const* arg, config_t* config) {
	config->verbose = true;
}

void parse_destination(char const *arg, config_t* config) {
	config->destination = strdup(arg);
}
