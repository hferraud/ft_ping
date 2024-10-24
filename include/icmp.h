#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <netinet/ip_icmp.h>

#include "ping.h"

void create_icmp_packet(ping_data_t *ping_data);
uint16_t icmp_checksum(void* packet, size_t len);

#endif
