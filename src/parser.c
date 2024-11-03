#include <string.h>
#include <getopt.h>

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int parse(int argc, char** argv, command_args_t* cmd_args) {
	struct option long_options[] = {
		{"verbose", no_argument, &cmd_args->verbose, 1},
		{"count", required_argument, 0, 'c'},
		{0, 0, 0, 0,}
	};
	int option_index = 0;
	int option;

	do {
		option = getopt_long(argc, argv, "vc:", long_options, &option_index);
		if (option == -1) {
			break;
		}
		switch (option) {
			case 0:
				break;
			case 'v':
				printf("Option %s\n", long_options[option_index].name);
				cmd_args->verbose = 1;
				break;
			case 'c':
				printf("Option -c with value %s\n", optarg);
				cmd_args->count = atoi(optarg);
				break;
			default:
				dprintf(STDERR_FILENO, "Unrecognized option\n");
				return -1;
		}
	} while (option != -1);
	if (optind >= argc) {
		dprintf(STDERR_FILENO, "ping: missing host operand\n");
		dprintf(STDERR_FILENO, "Try 'ping --help' or 'ping --usage' for more information\n");
	}
	cmd_args->destination = argv[optind];
	return 0;
}
