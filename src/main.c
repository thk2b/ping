#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

int raw_socket__new(void) {
    return socket(PF_INET, SOCK_RAW, AF_INET);
}

u_short cksum(void *vbuf, size_t bufsize) {
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
    return ~sum;
}

#define ECHO_REQ_SIZE (sizeof (struct icmp))
int echo_request(char *buf, size_t bufsize) {
    if (bufsize < ECHO_REQ_SIZE) {
        return 1;
    }
    bzero(buf, ECHO_REQ_SIZE);
    struct icmp *icmp_hdr = (struct icmp*) buf;

    *icmp_hdr = (struct icmp) {
        .icmp_type = ICMP_ECHO,
        .icmp_code = 0,
        .icmp_cksum = 0,
    };
    icmp_hdr->icmp_cksum = cksum((void*)icmp_hdr, sizeof(struct icmp));
    return 0;
}

int main() {
    char buf[400];
    int sock = raw_socket__new();
    if (sock == -1) {
        perror(strerror(errno));
    }
    
    struct sockaddr_in dst = {0};
    dst.sin_family = AF_INET;
    dst.sin_port = htons(80);
    dst.sin_addr.s_addr = inet_addr("8.8.8.8");

    if (echo_request(buf, 400 )) {
        perror("error initializing echo request");
    }
    int addrlen = sizeof(dst);

    if (sendto(sock, buf, ECHO_REQ_SIZE, 0, (struct sockaddr*)&dst, addrlen) < 0) {
        perror(strerror(errno));
    }
}
