#ifndef _ECHO_H_
#define _ECHO_H_

#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define ECHO_REQ_SIZE (sizeof(echo_request_t))
#define ECHO_RES_SIZE (sizeof(echo_response_t))

typedef struct {
    struct icmp hdr;
    char data[44];
} echo_request_t;

typedef struct {
    struct ip ip_hdr;
    echo_request_t icmp;
} echo_response_t;

int echo__new_request(char *buf, size_t bufsize);
int echo__process_response(char *buf, size_t bufsize);

#endif
