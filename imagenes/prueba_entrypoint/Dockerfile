FROM ubuntu:22.04

ENV HOST_USER="Servidor Docker"

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y apache2 gettext-base
RUN rm -rf /var/lib/apt/lists/*

COPY index.html.template /var/www/html/index.html.template
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["apachectl", "-D", "FOREGROUND"]
