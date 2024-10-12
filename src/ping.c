#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include "icmp.h"
#include "ping.h"
#include "parser.h"
#include "socket.h"
#include "print.h"
#include "time.h"
#include "rtt.h"

extern rtt_t rtt_g;

static int32_t echo_request(ping_data_t *ping_data, struct timeval *send_timestamp);
static int32_t echo_response(ping_data_t *ping_data, struct timeval *recv_timestamp);
static int32_t process_response(ping_data_t *ping_data, struct iphdr *ip_header);

/**
 * @return On success 0 is returned. On error, -1 is returned.
 */
int32_t init_ping(command_args_t *args, ping_data_t *ping_data) {
	ping_data->packet_size = DEFAULT_PACKET_SIZE + sizeof(struct icmphdr);
	ping_data->sequence = 0;
	ping_data->pid = getpid();
	ping_data->type = ICMP_ECHO;
	if (dns_lookup(args->destination, &ping_data->address) != 0) {
		return -1;
	}
	ping_data->socket_fd = init_icmp_socket();
	if (ping_data->socket_fd == -1) {
		return -1;
	}
	rtt_g.socket_fd = ping_data->socket_fd;
	return 0;
}

/**
 * @return On success 0 is returned. On error, -1 is returned.
 */
int32_t ping(command_args_t *args, ping_data_t *ping_data) {
	struct timeval	send_timestamp;
	struct timeval	recv_timestamp;
	struct timeval	travel_time;
	struct iphdr	response_ip_header;

	print_ping_info(args, ping_data);
	while (1) {
		if (echo_request(ping_data, &send_timestamp) == -1) {
			return -1;
		}
		if (echo_response(ping_data, &recv_timestamp) == -1) {
			return -1;
		}
		if (process_response(ping_data, &response_ip_header) == -1) {
			return -1;
		}
		travel_time = elapsed_time(send_timestamp, recv_timestamp);
		update_rtt(tv_to_ms(travel_time));
		print_ping_status(ping_data, response_ip_header.ttl, travel_time);
		sleep_ping_delay(travel_time);
		ping_data->sequence++;
	}
}

/**
 * @return On success 0 is returned. On error, -1 is returned and errno is set.
 */
static int32_t echo_request(ping_data_t *ping_data, struct timeval *send_timestamp) {
	ssize_t status;

	ping_data->packet = calloc(1, ping_data->packet_size);
	if (ping_data->packet == NULL) {
		perror("echo_request: calloc");
		return -1;
	}
	create_icmp_packet(ping_data);
	gettimeofday(send_timestamp, NULL);
	status = sendto(
		ping_data->socket_fd,
		ping_data->packet,
		ping_data->packet_size,
		0,
		(struct sockaddr *)&ping_data->address,
		sizeof(ping_data->address)
	);
	if (status != (ssize_t)ping_data->packet_size) {
		perror("echo_request: sendto");
		return -1;
	}
	rtt_g.transmitted += 1;
	return 0;
}

/**
 * @return On success 0 is returned. On error, -1 is returned and errno is set.
 */
static int32_t echo_response(ping_data_t *ping_data, struct timeval *recv_timestamp) {
	ssize_t status;
	socklen_t address_len = sizeof(ping_data->address);

	ping_data->packet = malloc(ping_data->packet_size + sizeof(struct iphdr));
	if (ping_data->packet == NULL) {
		perror("echo_response: malloc");
		return -1;
	}
	status = recvfrom(
		ping_data->socket_fd,
		ping_data->packet,
		ping_data->packet_size + sizeof(struct iphdr),
		0,
		(struct sockaddr *)&ping_data->address,
		&address_len
	);
	gettimeofday(recv_timestamp, NULL);
	if (status != (ssize_t)(ping_data->packet_size + sizeof(struct iphdr))) {
		free(ping_data->packet);
		perror("echo_response: recvfrom");
		return -1;
	}
	return 0;
}

/**
 * @return On success 0 is returned. If the checksum is invalid -1 is returned.
 */
static int32_t process_response(ping_data_t *ping_data, struct iphdr *ip_header) {
	struct icmphdr	*icmp_header;
	uint16_t		checksum;

	*ip_header = *(struct iphdr *)ping_data->packet;
	icmp_header = (struct icmphdr *)(ping_data->packet + sizeof(struct iphdr));
	checksum = icmp_header->checksum;
	icmp_header->checksum = 0;
	if (checksum != icmp_checksum(ping_data->packet + sizeof(struct iphdr), ping_data->packet_size)) {
		dprintf(STDERR_FILENO, "Invalid checksum in response.\n");
		return -1;
	}
	free(ping_data->packet);
	return 0;
}
