#include <unistd.h>
#include <string.h>

#include "icmp.h"

struct icmphdr create_icmp_header();
static uint16_t icmp_checksum(void* packet, size_t len);

void create_icmp_packet(uint8_t* packet, size_t len) {
	struct icmphdr *header;

	header = (struct icmphdr *)packet;
	*header = create_icmp_header();
	header->checksum = icmp_checksum(packet, len);
}

struct icmphdr create_icmp_header() {
	struct icmphdr header = {0};

	header.type = ICMP_ECHO;
	header.un.echo.id = getpid();
	header.un.echo.sequence = 0;
	header.code = 0;
	return header;
}

uint16_t icmp_checksum(void* packet, size_t len) {
	uint32_t sum;
	uint16_t *buffer = packet;

	for (sum = 0; len > 1; len -= 2) {
		sum += *buffer++;
	}
	if (len == 1) {
		sum += *(uint8_t *)packet;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return ~sum;
}