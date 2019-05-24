#include <stdio.h>
#include <errno.h>
#include <string.h>

int error(char *msg) {
    char *e;
    if (errno) {
        e = strerror(errno);
    } else {
        e = "Unknown error";
    }
    dprintf(2, "ping: error: %s: %s\n", msg ? msg : "", e);
    return 1;
}
