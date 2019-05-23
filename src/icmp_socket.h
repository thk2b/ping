#ifndef _ICMP_SOCKET_H_
#define _ICMP_SOCKET_H_
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef int icmpsock_t;

icmpsock_t icmp_socket__new(void);

int icmp_socket__send(
    icmpsock_t s,
    struct sockaddr_in *to,
    void *payload,
    size_t size
);

int icmp_socket__recv(
    icmpsock_t s,
    struct sockaddr_in *from,
    void *buf,
    size_t size
);

#endif
