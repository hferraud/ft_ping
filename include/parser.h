#ifndef PING_PARSER_H
#define PING_PARSER_H

typedef struct {
	int	verbose;
	size_t count;
	char *destination;
} command_args_t;

typedef void(*parse_fct_t)(char const*, command_args_t*);

int parse(int argc, char** argv, command_args_t *cmd_args);

#endif
