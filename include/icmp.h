#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <netinet/ip_icmp.h>

#define DEFAULT_PACKET_SIZE 64

typedef struct icmphdr icmphdr_t;

void create_icmp_packet(uint8_t* packet, size_t len);

#endif
