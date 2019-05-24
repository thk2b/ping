#ifndef _PING_H_
#define _PING_H_
#include <sys/time.h>
#include <icmp_socket.h>
#include <host.h>

/*
** stores all the ping related statistical and presentational information
*/
typedef struct {
    float min;
    float max;
    float avg;
    float mdev;
    uint64_t received;
    uint64_t sent;
    host_t *host;
} _ping_t;

/*
** start pinging the target untill a SIGINT
** print information on every recieved packet and finish with a summary
*/
int ping(
    icmpsock_t sock,
    struct sockaddr_in *target,
    host_t *host
);

#endif