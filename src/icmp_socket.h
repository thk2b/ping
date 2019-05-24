#ifndef _ICMP_SOCKET_H_
#define _ICMP_SOCKET_H_
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef int icmpsock_t;

/*
** represents a raw IMCP socket
*/
icmpsock_t icmp_socket__new(void);

/*
** send the payload of size to the destination
*/
int icmp_socket__send(
    icmpsock_t s,
    struct sockaddr_in *to,
    void *payload,
    size_t size
);

/*
** create a message header for recvmsg
** dst will point to the data buffer where the message will be stored
** note that recieving a new message will overwrite the previous
** because of the use of static memory, there can be only one instance of a reciever at once
*/
#define MSGBUFSIZE 1024
struct msghdr *msg_reciever__new(
    char **dst,
    struct sockaddr_in *from
);


/*
** recieve a message
*/
ssize_t icmp_socket__recv(
    icmpsock_t s,
    struct msghdr *hdr
);

#endif
