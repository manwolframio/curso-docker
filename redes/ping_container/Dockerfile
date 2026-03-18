FROM ubuntu:22.04

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y iputils-ping

CMD ["tail", "-f", "/dev/null"]
