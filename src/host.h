#ifndef _HOST_H_
#define _HOST_H_
#include <arpa/inet.h>
#include <netdb.h>

/*
** stores all the host related information
*/
typedef struct {
    char *cannonname;
    char ipstr[20];
    char fullname[NI_MAXHOST];
} host_t;

/*
** find the information pertaining to the given name, either an IP address
** or a hostname
** addr points to the host's address and host contains all relevant information
*/
int host__new(char *name, struct sockaddr_in *addr, host_t *host);

#endif