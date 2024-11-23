#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <arpa/inet.h>

#include "icmp.h"
#include "ping.h"
#include "parser.h"
#include "socket.h"
#include "print.h"
#include "time.h"
#include "rtt.h"


#define DEFAULT_PACKET_SIZE 56
#define RESPONSE_OFFSET (sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct iphdr))

extern rtt_t rtt_g;

static void ping_send(ping_data_t *ping_data);
static void ping_recv(ping_data_t *ping_data, ping_response_t *ping_response);
static int32_t ping_select(ping_data_t *ping_data);
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
	ping_response_t ping_response = {0};

	print_ping_info(args, ping_data);
	ping_send(ping_data);
	while (1) {
		if (ping_select(ping_data) == -1) {
			ping_send(ping_data);
			continue;
		}
		ping_recv(ping_data, &ping_response);
		if (process_response(ping_data, &ping_response) == 0) {
			print_ping_status(ping_data, &ping_response, args);
		}
		free(ping_data->packet);
	}
}

static void ping_send(ping_data_t *ping_data) {
	ssize_t status;

	printf("Sending...\n");
	ping_data->packet = malloc(ping_data->packet_size);
	if (ping_data->packet == NULL) {
		error(EXIT_FAILURE, errno, "malloc failed");
	}
	create_icmp_packet(ping_data);
	gettimeofday(&ping_data->send_timestamp, NULL);
	status = sendto(
			ping_data->socket_fd,
			ping_data->packet,
			ping_data->packet_size,
			0,
			(struct sockaddr *)&ping_data->address,
			sizeof(ping_data->address)
	);
	free(ping_data->packet);
	if (status < 0) {
		error(EXIT_FAILURE, errno, "sending packet");
	}
	ping_data->sequence++;
	rtt_g.transmitted++;
}

static void ping_recv(ping_data_t *ping_data, ping_response_t *ping_response) {
	ssize_t status;
	socklen_t address_len = sizeof(ping_response->address);

	ping_data->packet = malloc(ping_data->packet_size + RESPONSE_OFFSET);
	if (ping_data->packet == NULL) {
		error(EXIT_FAILURE, errno, "malloc failed");
	}
	errno = 0;
	status = recvfrom(
			ping_data->socket_fd,
			ping_data->packet,
			ping_data->packet_size + RESPONSE_OFFSET,
			0,
			(struct sockaddr *)&ping_response->address,
			&address_len
	);
	printf("status: %lu\n", status);
	gettimeofday(&ping_response->recv_timestamp, NULL);
	if (status < 0) {
		free(ping_data->packet);
		error(EXIT_FAILURE, errno, "recvfrom failed");
	}
	rtt_g.received++;
	ping_response->packet_size = status - sizeof(struct iphdr);
}

/**
 *
 * @return 0 when the fd can be read, -1 when the select timed out
 */
static int32_t ping_select(ping_data_t *ping_data) {
	fd_set			fdset;
	int32_t			fd_max;
	int32_t			status;
	struct timeval	timeout;
	struct timeval	now;

	FD_ZERO(&fdset);
	FD_SET(ping_data->socket_fd, &fdset);
	fd_max = ping_data->socket_fd + 1;
	gettimeofday(&now, NULL);
	timeout.tv_sec = ping_data->send_timestamp.tv_sec
		+ 1 - now.tv_sec;
	timeout.tv_usec = ping_data->send_timestamp.tv_usec
		+ 0 - now.tv_usec;
	normalize_timeval(&timeout);
	status = select(fd_max, &fdset, NULL, NULL, &timeout);
	printf("status: %d\n", status);
	if (status < 0) {
		error(EXIT_FAILURE, errno, "select failed");
	} else if (status == 1) {
		printf("here2\n");
		return 0;
	}
	printf("here\n");
	return -1;
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
	if (checksum != icmp_checksum(icmp_header, sizeof(icmp_header))) {
		error(0, 0, "checksum mismatch from %s",
			inet_ntoa(ping_response->address.sin_addr));
		return -1;
	}
	ping_response->code = icmp_header->code;
	ping_response->type = icmp_header->type;
	if (icmp_header->type != ICMP_ECHOREPLY) {
		icmp_header = (struct icmphdr *)(ping_data->packet + sizeof(struct iphdr)
				+ sizeof(struct icmphdr) + sizeof(struct iphdr));
	}
	ping_response->id = ntohs(icmp_header->un.echo.id);
	if (ping_response->id != ping_data->pid) {
		return -1;
	}
	ping_response->ttl = ip_header->ttl;
	ping_response->trip_time = elapsed_time(ping_data->send_timestamp, ping_response->recv_timestamp);
	update_rtt(ping_response);
	return 0;
}
