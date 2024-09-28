#include <stdlib.h>
#include <netinet/ip_icmp.h>

#include "icmp.h"

int32_t ping(int32_t socket_fd) {

}

int32_t echo_request(int32_t socket_fd, struct sockaddr_in address) {
	uint8_t packet[DEFAULT_PACKET_SIZE];

	create_icmp_packet(packet, DEFAULT_PACKET_SIZE);
}