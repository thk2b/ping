#include <icmp_socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>

icmpsock_t icmp_socket__new(void) {
    icmpsock_t sock = (icmpsock_t)socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) {
        return -1;
    }
    int ttl = 64;
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))) {
        return -1;
    }
    return sock;
}

#include <stdio.h>
int icmp_socket__send(
    icmpsock_t s,
    struct sockaddr_in *dst,
    void *payload,
    size_t size
) {
    ssize_t sent = sendto(s, payload, size, 0, (struct sockaddr*)dst, sizeof(struct sockaddr_in));
    if (sent <= 0) {
        return 1;
    }
    printf("sent %zu bytes\n", sent);
    return 0;
}

int icmp_socket__recv(
    icmpsock_t s,
    void *buf,
    size_t size
) {
    struct iovec vec = {
        .iov_base = buf,
        .iov_len = size
    };
    struct msghdr hdr = {0};
    char ctrlbuf[1000] = {0};
    hdr.msg_name = NULL;
    hdr.msg_namelen = 0;
    hdr.msg_iov = &vec;
    hdr.msg_iovlen = 1;
    hdr.msg_control = ctrlbuf;
    hdr.msg_controllen = 1000;
    ssize_t recieved = recvmsg(s, &hdr, 0);
    if (recieved < 0) {
        return 1;
    }
    return 0;
}
/*
int icmp_socket__recv(
    icmpsock_t s,
    void *buf,
    size_t size
) {
    socklen_t addrlen;
    struct sockaddr_in from;
    ssize_t recieved = recvfrom(s, buf, size, 0, (struct sockaddr*)&from, &addrlen);
    if (recieved < 0) {
        return 1;
    }
    return 0;
}
*/
