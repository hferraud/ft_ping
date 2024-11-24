#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include "ping.h"
#include "time.h"
#include "rtt.h"

static void print_icmp_description(const ping_data_t *ping_data, ping_response_t *ping_response);
static void print_ip_data(const ping_data_t *ping_data);
static void print_ip_header(struct ip *ip);
static char *get_icmp_description(int type, int code);

extern rtt_t rtt_g;

#define NITEMS(p) sizeof(p)/sizeof((p)[0])
#define HOST_BUF_LEN 256

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

void print_ping_info(const ping_data_t *ping_data) {
	printf("PING %s (%s): ", ping_data->cmd_args.destination, inet_ntoa(ping_data->address.sin_addr));
	printf("%zu data bytes", ping_data->packet_size - sizeof(struct icmphdr));
	if (ping_data->cmd_args.verbose) {
		printf (", id 0x%04x = %u", ping_data->pid, ping_data->pid);
	}
	printf("\n");
}

void print_ping_status(ping_data_t *ping_data, ping_response_t *ping_response) {
	if (ping_response->type != ICMP_ECHOREPLY) {
		return print_icmp_description(ping_data, ping_response);
	}
	printf("%zu bytes ", ping_response->packet_size);
	printf("from %s: ", inet_ntoa(ping_response->address.sin_addr));
	printf("icmp_seq=%hu ", ping_data->sequence);
	printf("ttl=%d ", ping_response->ttl);
	printf("time=%.3f ms\n", tv_to_ms(ping_response->trip_time));
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

static void print_icmp_description(const ping_data_t *ping_data, ping_response_t *ping_response) {
	char host[HOST_BUF_LEN];
	int32_t status;

	status = getnameinfo((struct sockaddr *)&ping_response->address, sizeof(ping_response->address), host, HOST_BUF_LEN, NULL, 0, 0);
	printf("%zu bytes ", ping_response->packet_size);
	if (!ping_data->cmd_args.numeric && status == 0) {
		printf("from %s (%s): ", host, inet_ntoa(ping_response->address.sin_addr));
	} else {
		printf("from %s: ", inet_ntoa(ping_response->address.sin_addr));
	}
	printf("%s\n", get_icmp_description(ping_response->type, ping_response->code));
	if (ping_data->cmd_args.verbose) {
		print_ip_data(ping_data);
	}
}

static void print_ip_data(const ping_data_t *ping_data) {
	int hlen;
	unsigned char *cp;
	struct ip *ip;

	ip = (struct ip *)(ping_data->packet + sizeof(struct iphdr) + sizeof(struct icmphdr));
	print_ip_header(ip);

	hlen = ip->ip_hl << 2;
	cp = (unsigned char *)ip + hlen;

	if (ip->ip_p == IPPROTO_TCP)
		printf("TCP: from port %u, to port %u (decimal)\n", (*cp * 256 + *(cp + 1)),
			   (*(cp + 2) * 256 + *(cp + 3)));
	else if (ip->ip_p == IPPROTO_UDP)
		printf("UDP: from port %u, to port %u (decimal)\n", (*cp * 256 + *(cp + 1)),
			   (*(cp + 2) * 256 + *(cp + 3)));
	else if (ip->ip_p == IPPROTO_ICMP) {
		int type = *cp;
		int code = *(cp + 1);

		printf("ICMP: type %u, code %u, size %u", type, code,
			   ntohs(ip->ip_len) - hlen);
		if (type == ICMP_ECHOREPLY || type == ICMP_ECHO)
			printf(", id 0x%04x, seq 0x%04x", *(cp + 4) * 256 + *(cp + 5),
				   *(cp + 6) * 256 + *(cp + 7));
		printf("\n");
	}
}

static void print_ip_header(struct ip *ip) {
	size_t hlen;
	unsigned char *cp;

	hlen = ip->ip_hl << 2;
	cp = (unsigned char *)ip + sizeof(*ip);

	size_t j;

	printf("IP Hdr Dump:\n ");
	for (j = 0; j < sizeof(*ip); ++j)
		printf("%02x%s", *((unsigned char *)ip + j),
			   (j % 2) ? " " : "");
	printf("\n");

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
	printf(" %1x  %1x  %02x", ip->ip_v, ip->ip_hl, ip->ip_tos);
	printf(" %04x %04x", (ip->ip_len > 0x2000) ? ntohs(ip->ip_len) : ip->ip_len,
		   ntohs(ip->ip_id));
	printf("   %1x %04x", (ntohs(ip->ip_off) & 0xe000) >> 13,
		   ntohs(ip->ip_off) & 0x1fff);
	printf("  %02x  %02x %04x", ip->ip_ttl, ip->ip_p, ntohs(ip->ip_sum));
	printf(" %s ", inet_ntoa(*((struct in_addr *)&ip->ip_src)));
	printf(" %s ", inet_ntoa(*((struct in_addr *)&ip->ip_dst)));
	while (hlen-- > sizeof(*ip))
		printf("%02x", *cp++);

	printf("\n");
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
