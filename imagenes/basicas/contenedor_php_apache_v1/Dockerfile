# Conteendor en el que corren los servicios de apache y php

FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y apache2
RUN echo "ServerName localhost" >> /etc/apache2/apache2.conf

COPY beryllium /var/www/html

RUN rm -rf /var/lib/apt/lists/*

CMD ["apache2ctl", "-D", "FOREGROUND"]