#include <string.h>

#include "icmp.h"

static struct icmphdr create_icmp_header(ping_data_t *ping_data);

void create_icmp_packet(ping_data_t *ping_data) {
	struct icmphdr *header;

	header = (struct icmphdr *)ping_data->packet;
	*header = create_icmp_header(ping_data);
	header->checksum = icmp_checksum(ping_data->packet, ping_data->packet_size);
}

static struct icmphdr create_icmp_header(ping_data_t *ping_data) {
	struct icmphdr header = {0};

	header.type = ping_data->type;
	header.un.echo.id = ping_data->pid;
	header.un.echo.sequence = ping_data->sequence;
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