FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y apache2 php libapache2-mod-php php-mysql

COPY phpinfo /var/www/phpinfo

COPY phpinfo.conf /etc/apache2/sites-available/phpinfo.conf

RUN a2ensite phpinfo && a2dissite 000-default && \
    echo "ServerName phpinfo.local" > /etc/apache2/conf-available/servername.conf && \
    a2enconf servername

CMD ["apachectl", "-D", "FOREGROUND"]
