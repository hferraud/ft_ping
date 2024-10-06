#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <netinet/in.h>

int32_t ping(int32_t socket_fd, struct sockaddr_in address);

#endif
