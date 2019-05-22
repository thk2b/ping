FROM debian

RUN apt-get update
RUN apt-get install build-essential

RUN make
ENTRYPOINT [ "/bin/sh" ]