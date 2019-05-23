#include <string.h>

#include <echo.h>
#include <unistd.h>
#include <stdio.h>

static u_short cksum(void *vbuf, size_t bufsize) {
    uint16_t *data = (uint16_t*)vbuf;
    uint32_t sum = 0;
    size_t limit = bufsize / 2;
    char *buf = (char*)vbuf;
    for (size_t i = 0; i < limit; i++) {
        sum += ntohs(data[i]);
        if (sum > UINT16_MAX) {
            sum -= UINT16_MAX;
        }
    }
    if (bufsize & 1) {
        sum += ntohs((uint8_t)(buf[bufsize-1]));
        if (sum > UINT16_MAX) {
            sum -= UINT16_MAX;
        }
    }
    return htons(~sum);
}

static int verify_chksum(u_short candidate, void *vbuf, size_t bufsize) {
    return candidate == cksum(vbuf, bufsize);
}

static void print_icmp_header(struct icmp *msg) {
    printf("type: %u, code: %u, cksum: %u\n",
        msg->icmp_type,
        msg->icmp_code,
        msg->icmp_cksum
    );
}

int echo__new_request(char *buf, size_t bufsize) {
    if (bufsize < ECHO_REQ_SIZE) {
        return 1;
    }
    bzero(buf, bufsize);
    echo_request_t *req = (echo_request_t*) buf;
    *req = (echo_request_t) {
        .hdr = {
            .icmp_type = ICMP_ECHO,
            .icmp_code = 0,
            .icmp_cksum = 0,
            .icmp_seq = htons(1),
            .icmp_id = htons(getpid()),
        }
    };
    memcpy(req->data, "hello, world\n", 14);
    req->hdr.icmp_cksum = cksum((void*)req, ECHO_REQ_SIZE);
    return 0;
}

int echo__process_response(char *buf, size_t bufsize) {
    if (bufsize < ECHO_REQ_SIZE) {
        return 1;
    }
    echo_response_t *res = (echo_response_t*) buf;
    if (verify_chksum(res->icmp.hdr.icmp_cksum, buf, bufsize)) {
        printf("invalid cksum\n");
        return 1;
    }
    print_icmp_header(&res->icmp.hdr);
    write(1, buf + sizeof(struct ip), bufsize - sizeof(struct ip));
    printf("\n");
    return 0;
}
