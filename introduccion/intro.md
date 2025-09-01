# Curso de Docker

## Introduccion
Se trata de un sistema basado en imagenes para crear aplicaciones portables y cuyo ciclo de vida podemos controlar

Se basan el la virtualziacion ligera, es decir, el contenedor va a constituir una micro máquina virtual que en vez de requerir de todos los componentes clasicos de una VM pasa a compartirlos con el host donde se ejecute. 

**VM**:
- VDISK
- VCPU
- Sistema operativo completo (>1 GB)

**Contenedor**:
- Comaprte disco, cpu y kernel con el host
- Sistemas operativos de < 100 MB
- Portables

Ejemplo de lanzamiento de un contenedor de ejemplo:

```bash
docker run -p 8080:80 httpd
````
Este ejecuta en un contenedor una imagen del servidor web de apache (Como se ejecuta en primer plano debemos detenerlo con control + c)

Si se quisiera ejecutar en foregroudn o segundo plano:

``` bash
docker run -p <puerto externo>:<puerto del contenedor> -d [imagen]
```
Para borrarlo se debe usar
```bash
docker ps # Para ver el nombre del contenedor
docker stop [nombre del contenedor] 
````

### Descripcion de docker:

Es un sistema que permite ejecutar aplicaciones de "bolsillo", es decir, partiendo del sistema operativo del host se montan sobre el sistemas con unas librerías fijas, requisistos, proceso de insalacion, etc. es decir que se puede correr un mini sistema dentro del host que se genera siempre de igual forma y que ejecuta uno o varios servicios a partir de los cuales se constrye una aplicacion (Microservicios). Esta centrado en la portabilidad, el control del ciclo de vida y la eficiencia.

  ## Arquitectura de docker

  - Docker host: Es el equipo fisico o virtual donde se ejecuta docker. Este esta compuesto a su vez de:


  - Docker daemon Servicio del sistema que eejcuta los contenedores

  - Docker rest api: Comunica el docker CLI en el que ejecutaremos los comandos que lanzaran las instancias con el daemon a traves de un sistema rest.

  - Docker CLI: interfaz para ejecutar comandos docker

  Que puede generar docker:

  Contenedores
  Imagenes
  Volumenes
  Redes y namespaces

# Imagenes

Una imagen de Docker es una plantilla inmutable que contiene:
- Un sistema base
- Las bibliotecas necesarias
- Los archivos de la aplicación
- La instrucción de arranque.

No se ejecuta por sí sola, sino que sirve como punto de partida para crear contenedores. 

Cada vez que se construye una imagen, se hace por capas, de modo que una capa representa el sistema base, otra las dependencias instaladas, otra los archivos copiados y así sucesivamente. Estas capas se apilan unas sobre otras y permiten que, si solo se cambia una parte, el resto pueda reutilizarse sin volver a generarse desde cero.

El valor de una imagen de Docker está en que garantiza que la aplicación se ejecutará siempre en el mismo entorno, sin importar dónde se despliegue. Eso significa que la misma imagen funcionará igual en un ordenador personal, en un servidor remoto o en una nube pública. Una imagen asegura:
- portabilidad
- consistencia
- facilidad para distribuir software 

porque encapsula todo en un solo paquete que luego puede compartirse en registros y desplegarse en cualquier máquina que tenga Docker.

La relación entre imagen y contenedor es la misma que la de un plano arquitectónico y una casa construida a partir de ese plano. La imagen es estática y describe cómo debe ser el entorno, mientras que el contenedor es la instancia viva que se crea y se destruye a partir de esa descripción.

## Capas

### Capa de imagen base
Para construir una imagen basica se incia indicando el S.O o la imaen de base a partir de la cual se comienza a construir. Para ello se recurre a la instruccion FROM

por ejemplo ubuntu indica:

https://hub.docker.com/_/ubuntu 

``` bash
FROM ubuntu:22.04
```

La propia ubuntu recomienda que si se usa se empiece de la siguiente forma

``` bash
RUN apt-get update && apt-get install -y locales && rm -rf /var/lib/apt/lists/* \
	&& localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
ENV LANG en_US.utf8
```

### Capa 2 o capa RUN

En la que se indica los comandos con los que se instalan las dependencias y programas necesarios para correr la aplicación que se quiere montar. 

No esta pensado para que sean comandos de ejecucion permanetente si no de instalacion, gestión de permisos, cambio y generacion de directorios ... Los comandos que se ejecuten con RUN se ejecutarán una sola vez durante la construccion de la imagen de Docker.

En el caso de la imagen de antes añadimos:

```bash
RUN apt-get install apache2 -y
```
En este caso nuestra aplicacion solo tiene una dependencia que es apache propiamente por que solo estamos instalando ese serivicio. En caso de necesitar ficheros presentes en el docker host se puede hacer uso bien de git

``` bash
RUN apt-get install git -y && git clone <el nobre del repo>
```

o bien de copiar directamente del docker host

```bash
COPY <origen en el docher host> <Destino en el contenedor>
````

Adicionalmente hay mas comandos para esta capa como workdir o users que permitiran otras funcionalidades pero para crear una imagen básica estos son suficientes
### Capa 3 o capa CMD

```bash
CMD ["apachectl","-D","FOREFROUND"]
```
Este sera el programa ejecutado en primer plano. el PID de este proceso será el que defina la vida del contenedor. Si el proceso finaliza el contenedor se detiene.

### Capa 4 o capa de ejecucion

Como se ha comentado la imagen que se construye en docker es un inmutable no un sistema en ejecucion. tras construir el contenedor apartir del dockerfle este se debe 'arrancar' con el comando docker run.

Con el contenedor corriendo podemos ver los logs, el estado, el tiempo de ejecucion...

Esta última es la capa de escritura y es a priori temporal, es decir no modifica la imagen y cuando el contenedor se detiene todo lo modificado se elimina. se dice que es temporal pero se pueden construir volumnenes y configuraciones almacenables que sean prsistentes aunque nunca cambiarán la imagen.

```bash
    docker run --name <nombre del contendor> -p <puerto del host>:<puerto del contenedor> [-d] [nombre de la imagen]
```

Como se ha comentado con docker se pueden construir volumenes para generar datos persistentes o redes para administrar los puntos de entrada, generar sistemas mas complejos e introducir orquestadores.

[Ejemplo de imagen apache](./Dockerfile)

