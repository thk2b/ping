#include <icmp_socket.h>
#include <echo.h>
#include <ping.h>
#include <stdio.h>
#include <error.h>
#include <signal.h>

static _ping_t ping_g = {0};

static float ping__get_time(struct timeval *tv) {
    return (float)(tv->tv_sec * 1000 + tv->tv_usec) / 1000;
}

static void ping__summary() {
    float loss = ((float)ping_g.sent - (float)ping_g.received) / (float)ping_g.sent * 100 ;
    printf("\n--- ??? ping statistics ---\n");
    printf("%lu packets transmitted, %lu packets received, %.1f%% packet loss\n",
        ping_g.sent, ping_g.received, loss
    );
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/??? ms\n",
        ping_g.min, ping_g.avg, ping_g.max
    );
}

static int ping__process_response(
    char *buf,
    size_t bufsize,
    char *hostname,
    char *ipstr
) {
    struct timeval *tv_sent_at = echo__res_get_payload(buf);
    struct timeval tv_now;
    if (gettimeofday(&tv_now, NULL)) {
        return 1;
    }
    struct timeval tv_diff = {
        .tv_sec = tv_now.tv_sec - tv_sent_at->tv_sec,
        .tv_usec = tv_now.tv_usec - tv_sent_at->tv_usec
    };
    float diff = ping__get_time(&tv_diff);
    if (ping_g.received == 0 || ping_g.min > diff) ping_g.min = diff;
    ping_g.received++;
    if (ping_g.max < diff) ping_g.max = diff;
    ping_g.avg = ping_g.avg + (diff - ping_g.avg) / ping_g.received;
    printf("%zu bytes from %s (%s): icmp_seq=%u ttl=%u time=%.3f ms\n",
        bufsize, hostname, ipstr, echo__res_get_seq(buf), echo__res_get_ttl(buf), diff
    );
    return 0;
}

void sigint(int s) {
    (void)s;
    ping__summary();
    exit(0);
}

int ping(
    icmpsock_t sock,
    struct sockaddr_in *target
) {
    char req_buf[ECHO_REQ_SIZE] = {0};
    char *res_buf;
    struct msghdr *reciever = msg_reciever__new(&res_buf, target);
    char ipstr[10] = {0};
    char *hostname = "???";

    inet_ntop(AF_INET, (struct sockaddr*)target, ipstr, sizeof(struct sockaddr_in));
    signal(SIGINT, sigint);
    if (echo__req_new(req_buf, ECHO_REQ_SIZE)) {
        return error("echo_request__new");
    }
    while (1) {
        // wait
        echo__req_set_seq(req_buf, ++ping_g.sent);
        if (echo__req_set_payload(req_buf)) {
            return error("echo__set_payload");
        }
        if (icmp_socket__send(sock, target, req_buf, ECHO_REQ_SIZE)) {
            return error("icmp_socket__send");
        }
        if (icmp_socket__recv(sock, reciever) != (long)ECHO_RES_SIZE) {
            return error("icmp_socket__recv");
        }
        if (ping__process_response(res_buf, ECHO_RES_SIZE, hostname, ipstr)) {
            return error("ping__process_response");
        }
    }
    return 0;
}
