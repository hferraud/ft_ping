#ifndef PING_PRINT_H
#define PING_PRINT_H

#include <sys/time.h>

#include "ping.h"

void print_ping_info(command_args_t *args, ping_data_t *ping_data);
void print_ping_status(ping_data_t *ping_data, uint8_t ttl, struct timeval elapsed);
void print_rtt();

#endif
