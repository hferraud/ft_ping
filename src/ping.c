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
static int32_t process_response(ping_data_t *ping_data, ping_response_t *ping_response);

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
	rtt_g.destination = args->destination;
	rtt_g.socket_fd = ping_data->socket_fd;
	return 0;
}

/**
 * @return On success 0 is returned. On error, -1 is returned.
 */
int32_t ping(command_args_t *args, ping_data_t *ping_data) {
	struct timeval	send_timestamp;
	struct timeval	recv_timestamp;
	ping_response_t ping_response;

	print_ping_info(args, ping_data);
	while (1) {
		if (echo_request(ping_data, &send_timestamp) == -1) {
			return -1;
		}
		do {
			if (echo_response(ping_data, &recv_timestamp) == -1) {
				return -1;
			}
			if (process_response(ping_data, &ping_response) == -1) {
				return -1;
			}
		} while (ping_response.id != ping_data->pid);
		ping_response.trip_time = elapsed_time(send_timestamp, recv_timestamp);
		update_rtt(tv_to_ms(ping_response.trip_time));
		if (print_ping_status(ping_data, &ping_response) != 0)
			return -1;
		sleep_ping_delay(ping_response.trip_time);
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

#include <errno.h>

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
	do {
		errno = 0;
		printf("trying to get IP address...\n");
		status = recvfrom(
			ping_data->socket_fd,
			ping_data->packet,
			ping_data->packet_size + sizeof(struct iphdr),
			0,
			(struct sockaddr *)&ping_data->address,
			&address_len
		);
	} while (errno == EAGAIN);
	gettimeofday(recv_timestamp, NULL);
	if (errno != EAGAIN && status != (ssize_t)(ping_data->packet_size + sizeof(struct iphdr))) {
		free(ping_data->packet);
		dprintf(STDERR_FILENO, "status: %zu\n", status);
		dprintf(STDERR_FILENO, "errno: %d\n", errno);
		perror("echo_response: recvfrom");
		return -1;
	}
	return 0;
}

/**
 * @return On success 0 is returned. If the checksum is invalid -1 is returned.
 */
static int32_t process_response(ping_data_t *ping_data, ping_response_t *ping_response) {
	struct iphdr	*ip_header;
	struct icmphdr	*icmp_header;
	uint16_t		checksum;

	ip_header = (struct iphdr *)ping_data->packet;
	icmp_header = (struct icmphdr *)(ping_data->packet + sizeof(struct iphdr));
	checksum = icmp_header->checksum;
	icmp_header->checksum = 0;
	ping_response->id = icmp_header->un.echo.id;
	if (icmp_header->un.echo.id != ping_data->pid) {
		free(ping_data->packet);
		return 0;
	}
	ping_response->ttl = ip_header->ttl;
	ping_response->code = icmp_header->code;
	ping_response->type = icmp_header->type;
	if (checksum != icmp_checksum(icmp_header, sizeof(icmp_header))) {
		free(ping_data->packet);
		dprintf(STDERR_FILENO, "Invalid checksum in response.\n");
		return -1;
	}
	free(ping_data->packet);
	return 0;
}
