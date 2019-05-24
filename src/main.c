#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <icmp_socket.h>
#include <echo.h>
#include <ping.h>
#include <host.h>

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

int main(int ac, char **av) {
    errno = 0;
    if (ac == 1) {
        return usage(1, av[0]);
    }
    icmpsock_t sock = icmp_socket__new();
    if (sock == -1) {
        return error("icmp_socket__new");
    }
    struct sockaddr_in target = {0};
    host_t host = {0};
    if (host__new(av[1], &target, &host)) {
        return error("host__new");
    }
    return ping(sock, &target, &host);
}
