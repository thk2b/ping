#include <icmp_socket.h>
#include <echo.h>
#include <ping.h>
#include <stdio.h>
#include <error.h>
#include <signal.h>

static _ping_t ping_g = {0};

static double ping__get_time(struct timeval *tv) {
    return (float)tv->tv_sec * 1000 + (float)tv->tv_usec / 1000;
}

static void ping__summary() {
    printf("rtt min/avg/max/mdev = %f/%f/%f/??? ms\n",
        ping__get_time(&ping_g.min),
        ping__get_time(&ping_g.avg),
        ping__get_time(&ping_g.max)
        // ping__get_time(ping_g.min),
    );
}

static void ping_g__update_min(_ping_t *p, struct timeval *val) {
    if (p->count == 0
    || (p->min.tv_sec >= val->tv_sec
    && p->min.tv_usec >= val->tv_usec)
    ) {
        p->min = *val;
    }
}

static void ping_g__update_max(_ping_t *p, struct timeval *val) {
    if (p->max.tv_sec <= val->tv_sec
    && p->max.tv_usec <= val->tv_usec) {
        p->max = *val;
    }
}

static void ping_g__update_avg(_ping_t *p, struct timeval *val) {
    p->avg.tv_sec = (long int)p->avg.tv_sec + ((long int)val->tv_sec - (long int)p->avg.tv_sec) / ++p->count;
    p->avg.tv_usec = (long int)p->avg.tv_usec + ((long int)val->tv_usec - (long int)p->avg.tv_usec) / p->count;
}

static int ping__process_response(
    char *buf,
    size_t bufsize,
    char *hostname,
    char *ipstr
) {
    struct timeval *sent_at = echo__res_get_payload(buf);
    struct timeval now;
    if (gettimeofday(&now, NULL)) {
        return 1;
    }
    struct timeval diff = {
        .tv_sec = now.tv_sec - sent_at->tv_sec,
        .tv_usec = now.tv_usec - sent_at->tv_usec,
    };
    printf("%zu bytes from %s (%s): icmp_seq=%u ttl=%u time=%f ms\n",
        bufsize, hostname, ipstr, echo__res_get_seq(buf), echo__res_get_ttl(buf), ping__get_time(&diff)
    );
    ping_g__update_min(&ping_g, &diff);
    ping_g__update_max(&ping_g, &diff);
    ping_g__update_avg(&ping_g, &diff);
    return 0;
}

void sigint(int s) {
    (void)s;
    ping__summary();
    exit(0);
}

#define PINGBUFSIZE 1024

#include <unistd.h>
int ping(
    icmpsock_t sock,
    struct sockaddr_in *target
) {
    char req_buf[PINGBUFSIZE] = {0};
    char *res_buf;
    struct msghdr *reciever = msg_reciever__new(&res_buf, target);
    char ipstr[10] = {0};
    char *hostname = "???";

    inet_ntop(AF_INET, (struct sockaddr*)target, ipstr, sizeof(struct sockaddr_in));
    signal(SIGINT, sigint);
    // setup signal
    if (echo__req_new(req_buf, PINGBUFSIZE)) {
        return error("echo_request__new");
    }
    u_short seq = 1;
    while (1) {
        echo__req_set_seq(req_buf, seq++);
        if (echo__req_set_payload(req_buf)) {
            return error("echo__set_payload");
        }
        if (icmp_socket__send(sock, target, req_buf, ECHO_REQ_SIZE)) {
            return error("icmp_socket__send");
        }
        if (icmp_socket__recv(sock, reciever) == -1) {//(long)ECHO_RES_SIZE) {
            return error("icmp_socket__recv");
        }
        if (ping__process_response(res_buf, ECHO_RES_SIZE, hostname, ipstr)) {
            return error("ping__process_response");
        }
    }
    return 0;
}
