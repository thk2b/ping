#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

int raw_socket__new(void) {
    return socket(PF_INET, SOCK_RAW, AF_INET);
}

int main() {
    int sock = raw_socket__new();
    if (sock == -1) {
        perror(strerror(errno));
    }
}
