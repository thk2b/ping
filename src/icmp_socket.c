#include <icmp_socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

icmpsock_t   icmp_socket__new(void) {
    icmpsock_t sock = (icmpsock_t)socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int ttl = 64;
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))) {
        return -1;
    }
    return sock;
}

int         icmp_socket_send(
    icmpsock_t s,
    struct sockaddr_in *dst,
    void *payload,
    size_t size
) {
    return 1;
}

void        *icmp_socket_recv(
    icmpsock_t s,
    void *buf,
    size_t size
) {
    return NULL;
}
