#include <stdlib.h>
#include <argp.h>
#include <error.h>

#include "parser.h"
#include "time.h"

#define DEFAULT_INTERVAL	1
#define DEFAULT_SIZE		56

#define PING_MAX_SIZE		65399

enum {
	ARG_TTL = 256,
};

static error_t parse_opt(int key, char* arg, struct argp_state *state);
static size_t ping_cvt_number(const char *optarg, size_t maxval, int allow_zero);

void parse(int argc, char** argv, command_args_t* cmd_args) {
	const struct argp_option argp_options[] = {
		{"count", 'c', "NUMBER", 0, "stop after sending NUMBER packets", 0},
        {"interval", 'i', "NUMBER", 0, "wait NUMBER seconds between sending each packet", 0},
		{"ttl", ARG_TTL, "N", 0, "specify N as time-to-live", 0},
		{"verbose", 'v', NULL, 0, "verbose output", 0},
		{"numeric", 'n', NULL, 0, "do not resolve host addresses", 0},
		{"size", 's', "NUMBER", 0, "send NUMBER data octets", 0},
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
	cmd_args->interval = sec_to_tv(DEFAULT_INTERVAL);
	cmd_args->size = DEFAULT_SIZE;
	argp_parse(&argp, argc, argv, 0, NULL, cmd_args);
}

static error_t parse_opt(int key, char* arg, struct argp_state *state) {
	command_args_t *cmd_args = state->input;
	char *endptr;
	double seconds;
	switch (key) {
		case 'c':
			cmd_args->count = ping_cvt_number(arg, 0, 1);
			break;
		case 'v':
			cmd_args->verbose = 1;
			break;
		case 'n':
			cmd_args->numeric = 1;
			break;
		case 's':
			cmd_args->size = ping_cvt_number(arg, PING_MAX_SIZE, 1);
			break;
		case 'i':
			seconds = strtod(arg, &endptr);
            if (*endptr)
                argp_error(state, "invalid value (`%s' near `%s')", arg, endptr);
            cmd_args->interval = sec_to_tv(seconds);
			break;
		case ARG_TTL:
			cmd_args->ttl = (int32_t)ping_cvt_number(arg, 255, 0);
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num >= 1) {
				argp_error(state, "too many host operands");
			}
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

static size_t ping_cvt_number(const char *optarg, size_t maxval, int allow_zero) {
    char *p;
    unsigned long int n;

    n = strtoul(optarg, &p, 0);
    if (*p)
        error(EXIT_FAILURE, 0, "invalid value (`%s' near `%s')", optarg, p);

    if (n == 0 && !allow_zero)
        error(EXIT_FAILURE, 0, "option value too small: %s", optarg);

    if (maxval && n > maxval)
        error(EXIT_FAILURE, 0, "option value too big: %s", optarg);

    return n;
}

