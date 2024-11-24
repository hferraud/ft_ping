#ifndef PING_PARSER_H
#define PING_PARSER_H

typedef struct {
	size_t			count;
	size_t			size;
	struct timeval	interval;
	int32_t			ttl;
	int8_t			verbose;
	int8_t			numeric;
	char			*destination;
} command_args_t;

typedef void(*parse_fct_t)(char const*, command_args_t *);

void parse(int argc, char** argv, command_args_t *cmd_args);

#endif
