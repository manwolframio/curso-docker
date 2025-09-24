# Ejemplo de uso de env
FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

ENV USER_NAME=Juan

RUN rm -rf /var/lib/apt/lists/*

CMD ["/bin/sh", "-c", "echo $USER_NAME"]
