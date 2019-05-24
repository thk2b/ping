#include <icmp_socket.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

icmpsock_t icmp_socket__new(void) {
    icmpsock_t sock = (icmpsock_t)socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) {
        return -1;
    }
    int ttl = 64;
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))) {
        return -1;
    }
    struct timeval timeo = {
        .tv_sec = 20, .tv_usec = 0,
    };
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(struct timeval))
    || setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(struct timeval))) {
        return -1;
    }
    return sock;
}

int icmp_socket__send(
    icmpsock_t s,
    struct sockaddr_in *to,
    void *payload,
    size_t size
) {
    ssize_t sent = sendto(s, payload, size, 0, (struct sockaddr*)to, sizeof(struct sockaddr_in));
    if (sent < 0 || sent != (unsigned)size) {
        return 1;
    }
    return 0;
}

ssize_t icmp_socket__recv(icmpsock_t s, struct msghdr *hdr) {
    ssize_t recieved = recvmsg(s, hdr, 0);
    if (recieved < 0) {
        return -1;
    }
    return recieved;
}

struct msghdr *msg_reciever__new(char **dst, struct sockaddr_in *from) {
    static char buf[MSGBUFSIZE] = {0};
    static struct iovec vec = {
        .iov_base = buf,
        .iov_len = MSGBUFSIZE
    };
    static struct msghdr hdr = {
        .msg_iov = &vec,
        .msg_iovlen = 1
    };
    hdr.msg_name = (struct sockaddr*)from;
    hdr.msg_namelen = sizeof(struct sockaddr_in);
    *dst = buf;
    return &hdr;
}