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

/*
** represents an echo IMCP message
*/
typedef struct {
    struct icmp hdr;
    struct timeval data;
} echo_request_t;

/*
** represents an echo IMCP response
** note that it includes the IP header
*/
typedef struct {
    struct ip ip_hdr;
    echo_request_t icmp;
} echo_response_t;

/*
** initialize a new echo request in the buffer
*/
int echo__req_new(char *buf, size_t bufsize);

/*
** set the sequence number on the IMCP header
*/
void echo__req_set_seq(char *buf, int seq);

/*
** set the IMCP payload data, in this case the current time
*/
int echo__req_set_payload(char *buf);

/*
** validate a response by making sure it was sent
** by this process and that the packet was not corrupted
*/
int echo__res_validate(char *buf);

/*
** getters to the IMCP packet
*/
struct timeval *echo__res_get_payload(char *buf);
u_short echo__res_get_seq(char *buf);
u_short echo__res_get_ttl(char *buf);

#endif
