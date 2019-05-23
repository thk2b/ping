#ifndef _ECHO_H_
#define _ECHO_H_

#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define ECHO_REQ_SIZE (sizeof (echo_request_t))
#define ECHO_RES_SIZE (20 + sizeof (echo_request_t)) // includes ip packet

typedef struct {
    struct icmp hdr;
    /* payload here */
} echo_request_t;

int echo__new_request(char *buf, size_t bufsize);
int echo__process_response(char *buf, size_t bufsize);

#endif