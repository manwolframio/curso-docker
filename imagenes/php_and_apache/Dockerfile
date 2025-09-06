FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
COPY ./entry.sh /entry.sh

RUN \
    apt-get update && apt-get upgrade -y \
    && apt-get install apache2 php libapache2-mod-php php-common -y \
    && chmod +x /entry.sh \
    && echo "<?php phpinfo(); ?>" > /var/www/html/hola.php

ENTRYPOINT [ "/entry.sh" ]
CMD ["apachectl","-D","FOREGROUND"]


