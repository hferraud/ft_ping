#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

#include "ping.h"
#include "time.h"
#include "socket.h"

void print_ping_info(ping_data_t *ping_data) {
	printf("PING  (%s) ", inet_ntoa(ping_data->address.sin_addr));
	printf("%zu(%zu) bytes of data.\n",
		   ping_data->packet_size - sizeof(struct icmphdr),
		   ping_data->packet_size + sizeof(struct iphdr));
}

/**
 * @return On success 0 is returned. On error, -1 is returned.
 */
int32_t print_ping_status(ping_data_t *ping_data, uint8_t ttl, struct timeval elapsed) {
	char host[NI_MAXHOST];
	if (reverse_dns_lookup(&ping_data->address, host, sizeof(host)) == -1) {
		return -1;
	}
	printf("%zu bytes ", ping_data->packet_size);
	printf("from %s (%s): ", host, inet_ntoa(ping_data->address.sin_addr));
	printf("icmp_seq=%hu ", ping_data->sequence);
	printf("ttl=%d ", ttl);
	printf("time=%.1f ms\n", tv_to_ms(elapsed));
	return 0;
}