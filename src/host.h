#ifndef _HOST_H_
#define _HOST_H_
#include <arpa/inet.h>
#include <netdb.h>

typedef struct {
    char *cannonname;
    char ipstr[20];
    char fullname[NI_MAXHOST];
} host_t;

int host__new(char *name, struct sockaddr_in *addr, host_t *host);

#endif