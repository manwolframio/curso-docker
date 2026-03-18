FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y mysql-client && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*