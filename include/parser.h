#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdbool.h>

#include "ping.h"

typedef struct {
	bool verbose;
	char *destination;
} command_args_t;

typedef void(*parse_fct_t)(char const*, command_args_t*);

command_args_t parse(int argc, char** argv);

#endif
