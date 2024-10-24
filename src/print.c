#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

#include "ping.h"
#include "time.h"
#include "socket.h"
#include "rtt.h"

static char *get_icmp_description(int type, int code);
static int32_t print_icmp_description(ping_data_t *ping_data, ping_response_t *ping_response);

extern rtt_t rtt_g;

#define NITEMS(p) sizeof(p)/sizeof((p)[0])

typedef struct icmp_code_description_s {
	int type;
	int code;
	char *diag;
} icmp_code_description_t;

icmp_code_description_t icmp_code_description[] = {
	{ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Net Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed and DF set"},
	{ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route Failed"},
	{ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Network Unknown"},
	{ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Host Unknown"},
	{ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host Isolated"},
	{ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Destination Network Unreachable At This TOS"},
	{ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Destination Host Unreachable At This TOS"},
	{ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Network"},
	{ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Host"},
	{ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Type of Service and Network"},
	{ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Type of Service and Host"},
	{ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
	{ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Frag reassembly time exceeded"}};

void print_ping_info(command_args_t *args, ping_data_t *ping_data) {
	printf("PING %s (%s): ", args->destination, inet_ntoa(ping_data->address.sin_addr));
	printf("%zu data bytes\n", ping_data->packet_size - sizeof(struct icmphdr));
}

int32_t print_ping_status(ping_data_t *ping_data, ping_response_t *ping_response) {
	if (ping_response->type != ICMP_ECHOREPLY) {
		return print_icmp_description(ping_data, ping_response);
	}
	printf("%zu bytes ", ping_response->packet_size);
	printf("from %s: ", inet_ntoa(ping_response->address.sin_addr));
	printf("icmp_seq=%hu ", ping_data->sequence);
	printf("ttl=%d ", ping_response->ttl);
	printf("time=%.3f ms\n", tv_to_ms(ping_response->trip_time));
	return 0;
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

static int32_t print_icmp_description(ping_data_t *ping_data, ping_response_t *ping_response) {
	char host[INET_ADDRSTRLEN];
	int32_t status;

	status = getnameinfo((struct sockaddr *)&ping_response->address, sizeof(ping_response->address), host, INET_ADDRSTRLEN, NULL, 0, 0);
	for (size_t i = ping_data->nb_pending + 1; i != 0; i--) {
		printf("%zu bytes ", ping_response->packet_size);
		if (status == 0) {
			printf("from %s (%s): ", host, inet_ntoa(ping_response->address.sin_addr));
		} else {
			printf("from %s: ", inet_ntoa(ping_response->address.sin_addr));
		}
		printf("%s\n", get_icmp_description(ping_response->type, ping_response->code));
	}
	return 0;
}

static char *get_icmp_description(int type, int code) {
	icmp_code_description_t *p;
	for (p = icmp_code_description; p < icmp_code_description + NITEMS(icmp_code_description); p++) {
		if (p->type == type && p->code == code) {
			return p->diag;
		}
	}
	return NULL;
}
