#include <string.h>

#include <echo.h>
#include <unistd.h>
#include <stdio.h>

/*
** IP checksum as per https://tools.ietf.org/html/rfc1071
*/
static u_short cksum(void *vbuf, size_t bufsize) {
    uint16_t *data = (uint16_t*)vbuf;
    uint32_t sum = 0;
    size_t limit = bufsize / 2;
    char *buf = (char*)vbuf;
    for (size_t i = 0; i < limit; i++) {
        sum += ntohs(data[i]);
        if (sum > UINT16_MAX) {
            sum -= UINT16_MAX;
        }
    }
    if (bufsize & 1) {
        sum += ntohs((uint8_t)(buf[bufsize-1]));
        if (sum > UINT16_MAX) {
            sum -= UINT16_MAX;
        }
    }
    return htons(~sum);
}

static void recompute_checksum(echo_request_t *req) {
    req->hdr.icmp_cksum = 0;
    req->hdr.icmp_cksum = cksum(req, ECHO_REQ_SIZE);
}

/*
** return 1 if the checksum matches the buffer
*/
static int verify_chksum(u_short candidate, void *vbuf, size_t bufsize) {
    return candidate == cksum(vbuf, bufsize);
}

int echo__req_new(char *buf, size_t bufsize) {
    if (bufsize < ECHO_REQ_SIZE) {
        return 1;
    }
    bzero(buf, bufsize);
    echo_request_t *req = (echo_request_t*) buf;
    *req = (echo_request_t) {
        .hdr = {
            .icmp_type = ICMP_ECHO,
            .icmp_code = 0,
            .icmp_cksum = 0,
            .icmp_seq = 0,
            .icmp_id = htons(getpid()),
        },
        .data = {0}
    };
    return 0;
}

void echo__req_set_seq(char *buf, int seq) {
    echo_request_t *req = (echo_request_t*) buf;
    req->hdr.icmp_seq = htons(seq);
}

int echo__req_set_payload(char *buf) {
    echo_request_t *req = (echo_request_t*) buf;
    if (gettimeofday(&req->data, NULL)) {
        return 1;
    }
    recompute_checksum(req);
    return 0;
}

int echo__res_validate(char *buf) {
    echo_response_t *res = (echo_response_t*) buf;
    return verify_chksum(res->icmp.hdr.icmp_cksum, buf, ECHO_RES_SIZE)
        && res->icmp.hdr.icmp_id == htons(getpid());
}

struct timeval *echo__res_get_payload(char *buf) {
    echo_response_t *res = (echo_response_t*) buf;
    return &res->icmp.data;
}

u_short echo__res_get_seq(char *buf) {
    echo_response_t *res = (echo_response_t*) buf;
    return ntohs(res->icmp.hdr.icmp_seq);
}

u_short echo__res_get_ttl(char *buf) {
    echo_response_t *res = (echo_response_t*) buf;
    return res->ip_hdr.ip_ttl;
}
