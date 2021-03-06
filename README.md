# Ping
A minimal implementation of the ping command

## Quick start
```sh

make

./ping google.com
64 bytes from sfo07s16-in-f78.1e100.net (216.58.195.78): icmp_seq=1 ttl=61 time=4.859 ms
64 bytes from sfo07s16-in-f78.1e100.net (216.58.195.78): icmp_seq=2 ttl=61 time=4.991 ms
64 bytes from sfo07s16-in-f78.1e100.net (216.58.195.78): icmp_seq=3 ttl=61 time=4.875 ms
64 bytes from sfo07s16-in-f78.1e100.net (216.58.195.78): icmp_seq=4 ttl=61 time=4.843 ms
^C
--- google.com ping statistics ---
4 packets transmitted, 4 packets received, 0.0% packet loss
rtt min/avg/max/mdev = 4.843/4.892/4.991/??? ms
```

You must be root to run the command. A dockerfile is provided to run inside a Debian VM:

```sh
docker-machine create --driver virtualbox ping
docker-machine start ping
eval $(docker-machine env ping)
docker build -t local/ping .
docker run -itv `pwd`:/ping local/ping
make
```

## Usage

`usage: ./ping host`

## Implementation

This program uses a raw ICMP socket to sent an ICMP ECHO request to the destination with the sending time as a payload.
It then waits for the matching ICMP response and calculates the time difference.
The process is repeated untill a SIGINT signal is captured and the program exits after displaying statistics.

## Internals

The core of the project is layered into 3 modules with clear and minimally-overlapping roles. More specific information can be found in the header files.

### icmp_socket

Responsible for initializing the socket, and sending/receiving data.
Doesn't care what is being sent or recieved.

### echo

Manages the ICMP ECHO packet and filtering of incoming packets.
Doesn't care how the request is sent or received nor what happens to the payload.

### ping

Manages the main loop, request/response lifecyle, statistics and user interface.
