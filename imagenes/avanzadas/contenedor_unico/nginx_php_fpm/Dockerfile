FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN \
    apt-get update && apt-get install -y --no-install-recommends \
      curl gnupg2 ca-certificates lsb-release ubuntu-keyring \
    && curl -fsSL https://nginx.org/keys/nginx_signing.key | gpg --dearmor \
      | tee /usr/share/keyrings/nginx-archive-keyring.gpg >/dev/null \
    && echo "deb [signed-by=/usr/share/keyrings/nginx-archive-keyring.gpg] \
      http://nginx.org/packages/ubuntu $(lsb_release -cs) nginx" \
      > /etc/apt/sources.list.d/nginx.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
      nginx \
      php-fpm php-cli php-mysql php-soap php-xml php-zip php-mbstring php-gd \
      git \
    && rm -rf /var/lib/apt/lists/*

VOLUME ["/var/www/html", "/var/log/nginx", "/var/log/php8.1-fpm", "/var/lib/php/sessions"]

# Usuarios y configuracion general
COPY ./conf/www.conf /etc/php/8.1/fpm/pool.d/www.conf
COPY ./conf/nginx.conf /etc/nginx/nginx.conf
RUN usermod -aG www-data nginx

# Web y contenido
COPY ./conf/default.conf /etc/nginx/conf.d/default.conf 
RUN mkdir -p /var/www/dockercontainer/public
COPY ./web/ /var/www/dockercontainer/public 
RUN chmod o+rx /var/www/dockercontainer/public

# Arranque
COPY ./bin/start.sh /start.sh
RUN chmod +x /start.sh
CMD ["/start.sh"]
