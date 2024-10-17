#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

#include "ping.h"
#include "time.h"
#include "socket.h"
#include "rtt.h"

extern rtt_t rtt_g;

void print_ping_info(command_args_t *args, ping_data_t *ping_data) {
	printf("PING %s (%s): ", args->destination, inet_ntoa(ping_data->address.sin_addr));
	printf("%zu data bytes\n", ping_data->packet_size - sizeof(struct icmphdr));
}

void print_ping_status(ping_data_t *ping_data, uint8_t ttl, struct timeval elapsed) {
	printf("%zu bytes ", ping_data->packet_size);
	printf("from %s: ", inet_ntoa(ping_data->address.sin_addr));
	printf("icmp_seq=%hu ", ping_data->sequence);
	printf("ttl=%d ", ttl);
	printf("time=%.3f ms\n", tv_to_ms(elapsed));
}

void print_rtt() {
	printf("--- %s ping statistics ---\n", rtt_g.destination);
	printf("%zu packets transmitted, ", rtt_g.transmitted);
	printf("%zu packets received, ", rtt_g.received);
	printf("%zu%% packet loss\n", (1 - rtt_g.received / rtt_g.transmitted) * 100);
	if (rtt_g.received == 0) {
		return;
	}
	printf("round-trip min/avg/max/stddev = ");
	printf("%.3f/", rtt_g.min);
	printf("%.3f/", get_rtt_avg());
	printf("%.3f/", rtt_g.max);
	printf("%.3f ms\n", get_rtt_stddev());
}
