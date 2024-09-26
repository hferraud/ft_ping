#include "parser/parser.h"

static void	demux(char const *, config_t*);
static void	parse_option(char const *, config_t*);
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

}

void parse_destination(char const *arg, config_t* config) {

}
