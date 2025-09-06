# Curso de docker
## Imágenes

Por lo general cuando se construye una imagen, la que sea se parte de una imagen base, esto en la capa FROM. para poder escoger entre unas u otras podemos acudir a distintos repositorios de imagenes ya construidas como 
- [Docker hub](https://hub.docker.com/)
- [Github Containter Registry](https://docs.github.com/es/packages/working-with-a-github-packages-registry/working-with-the-container-registry)

Para hacer uso de ellos en el FROM se debe indicar el origen de la imagen, por ejemplo si lo que queremos es desplegar un mongodb de docker hub
```bash
docker pull mongo:8.0.13-noble # De esta forma lo descargamos

docker run -p 27017:27017 -d mongo:8.0.13-noble # De esta forma lo ejecutamos
````

El nombre de los contenedores se formatea en tags, que tienen la siguiente estructura
```bash
<repositorio de origen>/<autor>/nombre_imagen:version
```
Al descargarlo podemos ver que harña varios pulls. Si descargamos verisones similares de la mimsa imagen solo se hará pull de las layers que sean diferentes, el resto se notificarán como alredy exist, o is up to date si no cambia nada. por ejemplo
```bash
docker pull mongo:latest
```

```bash 
docker pull mongo:3.0.18.noble
```
Aunque es la misma imagen tiene distinta etiqueta por lo que se conserva la informacion de todas las capas

Siempre es buena practica consultar las imagenes disponibles en el repositorio original (docer hub, maven,...) para poder hacer un uso correcto de ellas

#### Imagen de ejemplo
Vamos a construir rapidamente una imagen de apache y php en ubuntu 22 como ejemplo.

```bash
# version inicial de la imagen de apache y php [V0]
FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y apache2
RUN echo "ServerName localhost" >> /etc/apache2/apache2.conf

RUN rm -rf /var/lib/apt/lists/*

CMD ["apache2ctl", "-D", "FOREGROUND"]
````

En este caso l aimagen corre el proceso en foreground que es es como debe hacerse pero aun no tiene php en ella. 

Primero construimos la imagen:

```bash
docker build -t php_and_apache:v0 ./
```
y la ejecutamos:

```bash
docker run --name phpApache -d php_and_apache:v0
```

## Comandos de interes:

```bash
docker ps # Para ver los contenedored que hay vivos
docker ps -a # Para ver todos los contenedores
docker stop <nombre del contenedor> # Detiene el contenedor, borrando todo lo que no esté en almacenamiento persistente
docker rm <nombre del contenedor> # Borra el contenedor pero debe haber sido detendido previamente
docker rm <nomre del contenedor> --force # Borra el contenedor aunque esté corriendo
```
<span style="color:red">Siempre que se modifique algo en el Dokerfile se debe recostuir la imagen</span>

### Comandos que admite un Dockerfile

- FROM:
 Imagen base de la que parte el dockerfile
- RUN 
Ejecuciones de comandos vinculados a dependencias que se deben instalar
- COPY
Copia de ficheros del dokerhost al contenedor
- ADD
- ENV
- EXPOSE
- LABEL
- USER
- VOLUME
- CMD


#### FROM

Clona una imagene de base sobre la que vamos a construir nuestro contenedor. Esta imagen puede ser bien un S.O el cual se virtualizará aprovechando el kernel del docker host (Virtualizacion ligera) o un contenedor que ya funcione sobre el cual queramos añadir una determinada funcionalidad

```bash
FROM ubuntu:22.04

FROM nginx:latest

...
```

#### RUN

Como ya sabemos permite instalar dependencias en las imagenes, son comandos que deben de tener un final y se deben usar sin interaccion del usuario, como:

```bash
sudo apt-get <> -y # donde el -y permite que no se pida confirmacion al usuario de como se desea proceder
```

#### COPY 

Traslada el directorio del host que se le especifique como primer argumento al destino que se le especifique como segundo argumento en el contenedor

```bash
COPY <origen> <destino>
```

De esta forma añadimos a la imagen que construimos antes una web de ejemplo

```bash
# Conteendor en el que corren los servicios de apache y php

FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y apache2
RUN echo "ServerName localhost" >> /etc/apache2/apache2.conf

COPY beryllium /var/www/html # Esta linea hace que se copie el html nuevo en el root de apache, no es la mejor forma de proceder pero funciona por que sustituye el dominio por defecto

RUN rm -rf /var/lib/apt/lists/*

CMD ["apache2ctl", "-D", "FOREGROUND"]
```

#### ADD

Funciona igual que COPY pero interpreta el fichero no solo como un directorio, si por ejemplo es un .tar lo descomprime, si es una URL lo descarga como haría wget

#### ENV

Es una de las opciones más utiles por que permite establecer parámetros programables en el contenedor aunque la imagen sea inmutable, en el momento de construirse estos se interpretan y permiten fijar paramrtros como el hostname, secretos y claves, paramentros de configracion como URLs,...

Un ejemplo simple:

```bash
# Ejemplo de uso de env
FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

ENV USER_NAME=Juan

RUN rm -rf /var/lib/apt/lists/*

CMD ["/bin/sh", "-c", "echo $USER_NAME"]
```
Si construimos y ejecutamos la imagen
```bash
>> docker build -t env_test:v0 ./
>> docker run env_test:v0
Juan
```
### Workdir

Esta instruccion fija el directorio desde el que se ejecutarán los comandos que se indiquen en el dockerfile. de forma que si lo modificamos y no tenemos en cuenta eso podemos descolocar directorios o perder el control de donde los ponemos, por defecto los contenedores trabajan en eldiretorio raiz del usuario root

ejemplo de uso de workdir:

``` bash
FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN apt-get install python3 python3-pip -y

COPY ejemplo_python /ejemplo_python

COPY ejemplo_python/ejemplo_python.py /ejemplo_python.py

# WORKDIR /ejemplo_python

CMD ["python3","ejemplo_python.py"]
```

En esta imagen, si no usamos workdir el directorio no se cambia y se ejecutará el codigo de python del segundo copy y no se leera el fichero por lo cual nos avisará de que no existe.

```bash
docker build -t ejemplo ./
docker run ejemplo        
>> El archivo './texto.txt' no existe.
```

En cambio si descomentamos esa línea nos saldrá lo que se haya puesto en el txt. 

```bash
docker build -t ejemplo ./
docker run ejemplo        
>> Has usado bien workdir :)
```
### Label

Se trata de etiquetas que estan pensadas para dar metadara a la imagen. 

```bash
FROM ubuntu:22.04

LABEL vendor="<creador de la imagen>"
LABEL version="10.0"
LABEL description="Imagen de ejemplo"

CMD ["echo","Labels usados"]
```

### User

User define quien ejecuta la tarea que indican los comandos inferiores, por ejemplo si creeamos un servicio que debe configurado por un usuario concreto. Adicionalmente el usuario debe exitir para que el comando funcione correctamente. Por defecto si no se pone nada el usuario es root.

Un ejemplo usando el comando whoami que devuelve el usuario con el que se ejecuta:

```bash
FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y sudo &&
RUN rm -rf /var/lib/apt/lists/*
RUN useradd -m -s /bin/bash example_user
USER example_user
CMD ["whoami"]
```
Ahora ejecutamos:
```bash
docker build -t ejemplo ./
docker run ejemplo
>> example_user
```

Esto puede provocar problemas de permisos si lo ejecutamos incorrectamente pero si se gestiona bien puede evitar ataques de desplazamiento lateral en nuestro sistema por lo que es una buena práctcica no usar siempre el usuario por defecto.

Un ejemplo de contenedor que da problemas

```bash
FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash example_user

USER example_user

CMD ["bin/bash","-c","echo $USER >> usuario.txt"]
````

Esta imagen intenta crear un archivo pero no le hemos dado permisos al nuevo usuario para crearlo por lo que devueve:

```bash
prueba_user % docker run ejemplo        
>> bin/bash: line 1: usuario.txt: Permission denied
```
### Entrypoint
Cuando el comando que se quiere ejecutar en docker requiere de lanzar varios procesos por ejemplo o de descargar cosas que varian con el tiempo como un repositorio con un conjunto de datos, como por ejemplo un modelo de AI de hugginface podemos ejecutarlos con un entrypoint o un punto de entrada a traves del cual podemos ejecutar una lista de comandos completa cada vez que se instancie un contenedor desde a la imagnen.

Esto sigue la filosifía DevOps en la que el contenido/modelo/datos del contenedor se actualizan sin necesidad de reconstruir la imagen, haciendo que con un reinicio del contenedor sea suficiente.

Otra opcion util de usar entrypoints es que muchas veces las aplicaciones que ejecutemos no son capaces de interpretar variables de entorno (ENV`s) por si mismos por lo que se puede hacer uso de templates que tomen la variable de entorno y la sustutuyan en el template. Esto podrá no parecer util pero en orquestacion es crucial para que los sistemas funcionen bien.

Por lo general el comando ENTRYPOINT lo que indica a docker es como debe ejecutar el comando CMD, por defecto 
este entrypoint es 

```bash
ENTRYPOINT ["/bin/bash","-c"] # Entrypoint por defecto
```
Que implica que se ejecutará en bash la linea de comandos y argumentos que se reciba en CMD.

Sin embargo, podemos hacer modificaciones haciendo uso por ejemplo de ficheros de entrypoint.sh, por ejemplo:

```bash
#!/bin/sh
set -eu # Si algun comando falla o hay variables sin defir el proceso falla
# Genera el index.html final a partir de la plantilla
envsubst < /var/www/html/index.html.template > /var/www/html/index.html
# Toma el comando del CMD
exec "$@"
```

De esta forma primero se ejecutará la sustitucion de variables de entorno que hace envsubst y luego exec $@ ejecutará lo que se haya recibido por el CMD que en este caso sería la instruccion

```bash
CMD ["apachectl","-D","FOREGROUND"]
```

Por utlimo definimos la plantilla que se va a usar. En este caso una [plantilla HTML](/imagenes/prueba_entrypoint/index.html.template) en la que esta indicado con:

```bash
${HOST_USER}
```

Donde y que variable de entorno debe sustiuirse para generar el fichero final.

Un ejemplo de entrypoint que tambien es util es para añadir logs al contenedor que estamos ejecutando, por ejemplo

```bash
#!/bin/bash
set -eu
echo "Ejecutando comando en el CMD"
echo "Ejecutado por: ${USER:-$(whoami)}"
# Toma el comando del CMD
exec "$@"
```

Para ver el log:

```bash
docker build -t imagen_ejemplo ./
docker run -p 8080:80 --name contenedor_ejemplo -d imagen_ejemplo
docker logs contenedor_ejemplo
```

Un ejemplo en el que se hace uso de todo esto es el siguiente:

```bash
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y --no-install-recommends nginx libcap2-bin ca-certificates
RUN rm -rf /var/lib/apt/lists/*

# Crear grupo y usuario
RUN groupadd -r webgroup
RUN useradd -m -s /usr/sbin/nologin -g webgroup webuser

# Permitir que nginx pueda abrir puertos <1024
RUN setcap 'cap_net_bind_service=+ep' /usr/sbin/nginx

# Crear directorios necesarios
RUN mkdir -p /var/lib/nginx/body
RUN mkdir -p /var/lib/nginx/fastcgi
RUN mkdir -p /var/lib/nginx/proxy
RUN mkdir -p /var/lib/nginx/scgi
RUN mkdir -p /var/lib/nginx/uwsgi
RUN mkdir -p /var/log/nginx
RUN mkdir -p /run/nginx
RUN mkdir -p /run
RUN touch /run/nginx.pid

# Asignar grupo webgroup a los directorios y dar permisos 770
RUN chgrp -R webgroup /var/lib/nginx /var/log/nginx /run
RUN chmod -R 770 /var/lib/nginx /var/log/nginx /run

COPY nginx.conf /etc/nginx/nginx.conf
COPY site/ /var/www/site/
RUN chown -R webuser:webgroup /var/www/site

USER webuser
EXPOSE 80
CMD ["nginx", "-g", "daemon off"]
```

Finalmente vamos a desplegar en contenedores una arquitectura simple de apache + php reforzado con TLS 









