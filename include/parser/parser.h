#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdbool.h>

typedef struct {
	bool verbose;
} config_t;

config_t parse(int argc, char** argv);

#endif
