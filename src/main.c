#include <assert.h>
#include <unistd.h>
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
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int ttl = 64;
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))) {
        return -1;
    }
    return sock;
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
    return htons(~sum);
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
        .icmp_id = getpid(),
        // .icmp_data = 1
    };
    icmp_hdr->icmp_cksum = cksum((void*)icmp_hdr, sizeof(struct icmp));
    return 0;
}

int main() {
    char out_buf[400] = {0}, in_buf[400] = {0};
    int sock = raw_socket__new();
    if (sock == -1) {
        perror(strerror(errno));
    }
    
    struct sockaddr_in dst = {0};
    struct sockaddr_in src = {0};
    dst.sin_family = AF_INET;
    dst.sin_port = htons(0);
    dst.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (echo_request(out_buf, 400)) {
        perror("error initializing echo request\n");
    }
    socklen_t addrlen = sizeof(dst);
    out_buf[ECHO_REQ_SIZE] = 'A';

    if (sendto(sock, out_buf, ECHO_REQ_SIZE + 1, 0, (struct sockaddr*)&dst, addrlen) < 0) {
        perror(strerror(errno));
    }
    if (recvfrom(sock, in_buf, ECHO_REQ_SIZE + 1, 0, (struct sockaddr*)&src, &addrlen) < 0) {
        perror(strerror(errno));
    } else {
        struct icmp *res = (struct icmp*)in_buf;
        printf("%u, %c\n", res->icmp_code, in_buf[ECHO_REQ_SIZE]);
        write(1, in_buf, ECHO_REQ_SIZE + 1);
    }
}
