#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <icmp_socket.h>
#include <echo.h>

#define BUFSIZE 1024

static int error(char *msg) {
    char *e;
    if (errno) {
        e = strerror(errno);
    } else {
        e = "Unknown error";
    }
    dprintf(2, "ping: error: %s: %s\n", msg ? msg : "", e);
    return 1;
}

static int usage(int ret, char *prog) {
    printf("usage: %s: host\n", prog);
    return ret;
}

static int resolve_host(char *s, struct sockaddr_in *dst) {
    struct addrinfo hints = {0};
    struct addrinfo *info = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    if (getaddrinfo(s, NULL, &hints, &info)) {
        return 1;
    }
    *dst = *(struct sockaddr_in*)info->ai_addr;
    return 0;
}

int main(int ac, char **av) {
    errno = 0;
    if (ac == 1) {
        return usage(1, av[0]);
    }
    icmpsock_t sock = icmp_socket__new();
    if (sock == -1) {
        return error("icmp_socket__new");
    }
    // struct sockaddr_in dst = {0};
    struct sockaddr_in dst = {0};
    if (resolve_host(av[1], &dst)) {
        return error("resolve_host");
    }
    char req_buf[BUFSIZE] = {0};
    if (echo__new_request(req_buf, BUFSIZE)) {
        return error("echo_request__new");
    }
    if (icmp_socket__send(sock, &dst, req_buf, ECHO_REQ_SIZE)) {
        return error("icmp_socket__send");
    }
    while (1) {
        char res_buf[BUFSIZE] = {0};
        if (icmp_socket__recv(sock, &dst, res_buf, ECHO_RES_SIZE)) {
            return error("icmp_socket__recv");
        }
        if (echo__process_response(res_buf, ECHO_RES_SIZE)) {
            return error("echo__process_response");
        }
    }
}
