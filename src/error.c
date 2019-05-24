#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int error(char *msg) {
    char *e = NULL;
    if (errno) {
        e = strerror(errno);
    }
    dprintf(2, "ping: error: %s%s %s\n",
        msg ? msg : "",
        msg ? e ? ":" : "" : "",
        e ? e : ""
    );
    return 1;
}
