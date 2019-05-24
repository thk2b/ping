#ifndef _ECHO_H_
#define _ECHO_H_

#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define ECHO_REQ_SIZE (sizeof(echo_request_t))
#define ECHO_RES_SIZE (sizeof(echo_response_t))

#pragma pack(2)

typedef struct {
    struct icmp hdr;
    struct timeval data;
} echo_request_t;

typedef struct {
    struct ip ip_hdr;
    echo_request_t icmp;
} echo_response_t;

int echo__req_new(char *buf, size_t bufsize);
void echo__req_set_seq(char *buf, int seq);
int echo__req_set_payload(char *buf);
struct timeval *echo__res_get_payload(char *buf);
u_short echo__res_get_seq(char *buf);
u_short echo__res_get_ttl(char *buf);

#endif
