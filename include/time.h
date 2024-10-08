#ifndef PING_TIME_H
#define PING_TIME_H

struct timeval elapsed_time(struct timeval start, struct timeval end);
float tv_to_ms(struct timeval tv);

#endif
