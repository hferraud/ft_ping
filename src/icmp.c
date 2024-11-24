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
	header.un.echo.id = htons(ping_data->pid);
	header.un.echo.sequence = htons(ping_data->sequence);
	return header;
}

uint16_t icmp_checksum(void* packet, size_t len) {
	u_int16_t* buffer;
	u_int32_t sum;

	sum = 0;
	buffer = (uint16_t*)packet;
	while (len > 1) {
		sum += *buffer;
		if (sum > 0xFFFF) {
			sum = (sum >> 16) + (sum & 0xFFFF);
		}
		++buffer;
		len -= 2;
	}
	if (len == 1) {
		sum += *(u_int8_t*) buffer;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	return (u_int16_t) ~sum;
}