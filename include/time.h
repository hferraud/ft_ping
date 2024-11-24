#ifndef PING_TIME_H
#define PING_TIME_H

struct	        timeval elapsed_time(struct timeval start, struct timeval end);
float	        tv_to_ms(struct timeval tv);
void            normalize_timeval(struct timeval *tv);
struct timeval  sec_to_tv(double seconds);

#endif
