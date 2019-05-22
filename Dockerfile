FROM debian

RUN apt-get update
RUN apt-get install build-essential -y

ENTRYPOINT [ "/bin/bash" ]