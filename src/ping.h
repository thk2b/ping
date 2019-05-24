#ifndef _PING_H_
#define _PING_H_
#include <sys/time.h>
#include <icmp_socket.h>

typedef struct {
    struct timeval min;
    struct timeval max;
    struct timeval avg;
    struct timeval mdev;
    uint64_t count;
} _ping_t;

int ping(
    icmpsock_t sock,
    struct sockaddr_in *target
);

#endif