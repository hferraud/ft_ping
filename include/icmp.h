#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <netinet/ip_icmp.h>

#define ICMP_PACKET_SIZE 64
#define IPHDR_SIZE 20
#define ECHO_REQUEST_PACKET_SIZE ICMP_PACKET_SIZE
#define ECHO_RESPONSE_PACKET_SIZE (ICMP_PACKET_SIZE + IPHDR_SIZE)

void create_icmp_packet(uint8_t* packet, size_t len);

#endif
