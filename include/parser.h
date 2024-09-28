#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdbool.h>

#include "ping.h"

typedef struct {
	bool verbose;
	char *destination;
} config_t;

typedef void(*parse_fct_t)(char const*, config_t*);

config_t parse(int argc, char** argv);

#endif
