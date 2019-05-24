FROM debian

RUN apt-get update
RUN apt-get install build-essential -y

ADD . /ping
WORKDIR /ping
RUN make

ENTRYPOINT [ "/bin/bash" ]