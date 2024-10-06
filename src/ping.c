#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <stdio.h>

#include "icmp.h"

int32_t echo_request(int32_t socket_fd, struct sockaddr_in address, struct timeval *send_timestamp);
int32_t echo_response(int32_t socket_fd, struct sockaddr_in address, struct timeval *recv_timestamp);

int32_t ping(int32_t socket_fd, struct sockaddr_in address) {
	struct timeval send_timestamp;
	struct timeval recv_timestamp;

	if (echo_request(socket_fd, address, &send_timestamp) == -1) {
		return -1;
	}
	if (echo_response(socket_fd, address, &recv_timestamp) == -1) {
		return -1;
	}
	return 0;
}


/**
 * @return -1 on error, 0 otherwise.
 */
int32_t echo_request(int32_t socket_fd, struct sockaddr_in address, struct timeval *send_timestamp) {
	uint8_t packet[ECHO_REQUEST_PACKET_SIZE];
	ssize_t status;

	create_icmp_packet(packet, ECHO_REQUEST_PACKET_SIZE);
	gettimeofday(send_timestamp, NULL);
	status = sendto(socket_fd, packet, ECHO_REQUEST_PACKET_SIZE, 0, (struct sockaddr *)&address, sizeof(address));
	if (status != ECHO_REQUEST_PACKET_SIZE) {
		perror("echo_request: sendto");
		return -1;
	}
	return 0;
}

int32_t echo_response(int32_t socket_fd, struct sockaddr_in address, struct timeval *recv_timestamp) {
	uint8_t packet[ECHO_RESPONSE_PACKET_SIZE];
	ssize_t status;
	socklen_t address_len = sizeof(address);

	status = recvfrom(socket_fd, packet, ECHO_RESPONSE_PACKET_SIZE, 0, (struct sockaddr *)&address, &address_len);
	gettimeofday(recv_timestamp, NULL);
	if (status != ECHO_RESPONSE_PACKET_SIZE) {
		//TODO: perror
		return -1;
	}
	return 0;
}