#include <host.h>

#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

static int host__get_addr(char *s, struct sockaddr_in *dst, host_t *host) {
    struct addrinfo hints = {0};
    struct addrinfo *info = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    if (getaddrinfo(s, NULL, &hints, &info)) {
        return 1;
    }
    *dst = *(struct sockaddr_in*)info->ai_addr;
    host->cannonname = s;
    inet_ntop(AF_INET, &((struct sockaddr_in*)info->ai_addr)->sin_addr, host->ipstr, sizeof(struct sockaddr_in));
    freeaddrinfo(info);
    return 0;
}

static int host__get_fullname(struct sockaddr_in *addr, host_t *host) {
    return getnameinfo(
        (struct sockaddr*)addr,
        sizeof(struct sockaddr_in),
        host->fullname, sizeof(host->fullname),
        NULL, 0,
        NI_NAMEREQD
    );
}

int host__new(char *name, struct sockaddr_in *addr, host_t *host) {
    return host__get_addr(name, addr, host)
        || host__get_fullname(addr, host);
}
