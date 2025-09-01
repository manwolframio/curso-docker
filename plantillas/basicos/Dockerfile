# Imagen base de un contenedor con ubuntu 22

## Capa 1: Imagen base
FROM ubuntu:22.04

## Capa de entorno
ENV LANG=en_US.utf8
## Capa 2: RUN de dependencias y directorios

# Actualizar el índice de paquetes
RUN apt-get update
# Instalar locales
RUN apt-get install -y locales
# Generar el locale en_US.UTF-8
RUN localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
# -- Nuestras cosas --




# Borramos la cache de paquetes para aligerar la imagen
RUN rm -rf /var/lib/apt/lists/*


