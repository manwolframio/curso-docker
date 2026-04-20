FROM ubuntu:22.04

ENV TZ=Europe/Madrid
ENV DEBIAN_FRONTEND=noninteractive

ENV WEB1_DIR=/apps/web-1
ENV WEB2_DIR=/apps/web-2

ENV WEB1_PORT=3000
ENV WEB2_PORT=3001

COPY web-1 /apps/web-1
COPY web-2 /apps/web-2
COPY requierments.txt /apps/requierments.txt
COPY start-servers.sh /apps/start-servers.sh

RUN apt-get update && apt-get upgrade -y && \
    apt-get install python3 python3-pip python3-dev curl -y && \
    cd /apps/ && pip3 install -r requierments.txt

WORKDIR /apps/

CMD ["/bin/bash", "start-servers.sh"]
