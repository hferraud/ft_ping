#include <unistd.h>
#include <string.h>

#include "icmp.h"

icmphdr_t create_icmp_header();
static uint32_t icmp_checksum(void* packet, size_t len);

void create_icmp_packet(uint8_t* packet, size_t len) {
	icmphdr_t header = create_icmp_header();

	memcpy(packet, &header, sizeof(header));
	header.checksum = icmp_checksum(packet, len);
	memcpy(packet, &header, sizeof(header));
}

icmphdr_t create_icmp_header() {
	icmphdr_t header;

	header.type = ICMP_ECHO;
	header.un.echo.id = getpid();
	header.un.echo.sequence = 0;
	header.code = 0;
}

uint32_t icmp_checksum(void* packet, size_t len) {
	uint32_t sum;

	for (sum = 0; len > 1; len -= 2) {
		sum += *(uint16_t*)packet++;
	}
	if (len > 0) {
		sum += *(uint16_t*)packet;
	}
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return ~sum;
}