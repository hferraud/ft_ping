#ifndef PING_PRINT_H
#define PING_PRINT_H

#include <sys/time.h>

#include "ping.h"

void print_ping_info(ping_data_t *ping_data);
void print_ping_status(ping_data_t *ping_data, ping_response_t *ping_response);
void print_rtt();

#endif
