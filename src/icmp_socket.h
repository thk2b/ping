#ifndef _ICMP_SOCKET_H_
#define _ICMP_SOCKET_H_
#include <stdlib.h>

typedef int icmpsock_t;

icmpsock_t   icmp_socket__new(void);

int         icmp_socket__send(
    icmpsock_t s,
    struct sockaddr_in *dst,
    void *payload,
    size_t size
);

void        *icmp_socket__recv(
    icmpsock_t s,
    void *buf,
    size_t size
);

#endif
