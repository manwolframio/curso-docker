FROM ubuntu:22.04

# Definimos las variables de entoro 
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Instalamos las dependencias
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
      ca-certificates tzdata \
      apache2 apache2-utils \
      php libapache2-mod-php php-common \
      openssl \
      git && \
    rm -rf /var/lib/apt/lists/*
    
# Ajustamos el server name para que no de warnings apache
RUN echo "ServerName DockerContainer" > /etc/apache2/conf-available/servername.conf && \
    a2enconf servername

# Generamos el directorio
RUN mkdir -p /var/www/html/miweb

# Copiamos la web
COPY ./web/ /var/www/html/miweb/

# Copiamos los ficheros de configuracion el servidor de http y https 
COPY ./apache/sites/000-default.conf /etc/apache2/sites-available/000-default.conf
COPY ./apache/sites/default-ssl.conf /etc/apache2/sites-available/default-ssl.conf

# Configuramos los ficheros con los certificados y los modulos que las utilizan
RUN mkdir -p /etc/ssl/apache && \
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
      -keyout /etc/ssl/apache/apache-container.key \
      -out /etc/ssl/apache/apache-container.crt \
      -subj "/C=ES/ST=Madrid/L=Madrid/O=Local/OU=Docker/CN=DockerContainer" && \
    chmod 600 /etc/ssl/apache/apache-container.key && \
    a2enmod ssl headers rewrite http2 && \
    a2ensite 000-default.conf default-ssl.conf

# Habilitamos los permisos para que se lea la web 
RUN chown -R www-data:www-data /var/www/html && \
    find /var/www/html -type d -exec chmod 755 {} \; && \
    find /var/www/html -type f -exec chmod 644 {} \;

# Copiamos el fichero de arranque
COPY ./entry.sh /entry.sh
RUN chmod +x /entry.sh

# Arranque del sistema
ENTRYPOINT ["/entry.sh"]
CMD ["apachectl", "-D", "FOREGROUND"]