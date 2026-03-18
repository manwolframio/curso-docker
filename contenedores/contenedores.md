# Contenedores

Una vez conocemos cómo crear imágenes, cómo se pueden construir, lo que son las capas y demás, es momento de pasar a operar con contenedores.

## Definición
Por norma, un contenedor es una instancia de una imagen previamente definida y, aunque puede asociarse con otros recursos como redes y volúmenes que pueden sí ser permanentes, es de por sí un objeto de solo lectura, por lo que todas las modificaciones que se realicen sobre un contenedor desaparecerán una vez se detenga dicho contenedor, salvo aquellos cambios asociados a recursos como volúmenes.

Para saber qué contenedores existen en un determinado equipo se puede ejecutar el comando:
```bash
docker ps
```
que listará aquellos contenedores que se encuentren en ejecución, o en su lugar:

```bash
docker ps -a
```

Que listará todos los contenedores en nuestro equipo.

Para hacer pruebas se va a descargar la imagen oficial de jenkins, referenciada en: https://hub.docker.com/layers/jenkins/jenkins/latest/images/

```bash
docker pull jenkins/jenkins:latest
```

Para ejecutarlo: 
```bash
docker run jenkins/jenkins:latest
```

Para ejecutarlo en segundo plano añadimos ```-d``` : 
```bash
docker run -d jenkins/jenkins:latest 
```

Para ejecutarlo en primer plano y terminar el contenedor cuando se finalice (`Ctrl+C`)

```bash
docker run -rm -ti jenkins/jenkins:latest 
```


Una vez está en ejecución podemos consultar su estado de la siguiente forma:

```bash
docker ps
CONTAINER ID   IMAGE                    COMMAND                  CREATED          STATUS          PORTS                 NAMES
134b9e6e801d   jenkins/jenkins:latest   "/usr/bin/tini -- /u…"   11 seconds ago   Up 11 seconds   8080/tcp, 50000/tcp   confident_austin
``` 
Como podemos observar, docker le ha asignado un nombre aleatorio al contenedor, en este caso ```confident_austin```.
Si lo que queremos ahora es detenerlo debemos hacerlo mediante este nombre: 

```bash
docker stop confident_austin
docker rm confident_austin
docker ps

CONTAINER ID   IMAGE     COMMAND   CREATED   STATUS    PORTS     NAMES
```

Ahora, para controlar este tag, se puede definir de la siguiente forma:
```bash
docker run -d --name "jenkins_main" jenkins/jenkins:latest

docker ps

CONTAINER ID   IMAGE                    COMMAND                  CREATED         STATUS         PORTS                 NAMES
86b39bdc70a6   jenkins/jenkins:latest   "/usr/bin/tini -- /u…"   7 seconds ago   Up 7 seconds   8080/tcp, 50000/tcp   jenkins_main
```
Donde ya se le asigna correctamente el nombre al contenedor. 

No obstante, durante todo este tiempo no se ha podido acceder al Jenkins que estamos desplegando, ya que es necesario llevar a cabo el port mapping, es decir, debemos decirle al Docker daemon qué puerto de nuestro host va a estar mapeado a qué puerto de dicho contenedor.

En este caso, y en general en el de todos los servicios web, el puerto suele ser el 80, no el caso de jenkins que es 8080, por lo que redirigremos el puerto 8080 del host al puerto 8080 del contenedor, para ello se debe añadir al comando de ejecución el comando ```-p <puerto del host : puerto interno del contenedor>```

Para ello ejecutamos el siguiente comando: 
```bash
docker run -d --name "jenkins_main" -p 8080:8080 jenkins/jenkins:latest
```

En el caso de Jenkins, después de hacer esto, la interfaz web nos pide una contraseña y nos indica en qué parte del contenedor está. En este caso: ```/var/jenkins_home/secrets/initialAdminPassword```. Sin embargo, esta parte del contenedor, como se comentaba, no es accesible desde el propio equipo de momento, ya que aún ni siquiera hemos montado un volumen compartido, por lo que deberemos acceder al contenedor. Para ello, la forma más común es mediante el comando ```docker exec <contenedor> <comando>```, que permite ejecutar distintas instrucciones en el contenedor. En este caso, como lo que queremos es poder acceder a dicho directorio, podemos proceder de dos formas.

1) Ejecutando el comando directamente

```bash
# Primero debemos conocer el nombre del contenedor:
docker ps
# Con dicho nombre ejecutamos el comando
docker exec jenkins_main cat /var/jenkins_home/secrets/initialAdminPassword 
<password>
```

2) Explorando el contenedor de forma interactiva, para lo que ejecutaremos bash, pero debemos hacerlo en modo interactivo para que podamos usarlo, para lo que añadiremos las opciones ```-it```:

```bash
# Primero debemos conocer el nombre del contenedor:
docker ps
# Con dicho nombre ejecutamos el comando
docker exec -it jenkins_main /bin/bash
# Ya dentro del contenedor:
jenkins@71ef9193ff52:/$ cd /var/jenkins_home/secrets/
jenkins@71ef9193ff52:~/secrets$ cat initialAdminPassword
<password>
```

# Arranque, parada y reinicio

Una vez ya tenemos las instancias definidas y en ejecución, habiendo usado el comando: 

```bash
docker run 
```

También es posible manejar el estado de esos contenedores, pudiendo detenerlos, renombrarlos, arrancarlos o reiniciarlos.

- Detener un contenedor:

```bash
docker stop <nombre>
```

- Arrancarlo:

```bash
docker start <nombre>
```

- Renombrarlo:

```bash
docker rename <nombre> <nuevo nombre>
```

- Reiniciarlo:

```bash
docker restart <nombre>
```
Finalmente, si lo que se quiere es eliminar por completo el contenedor, aunque esté en ejecución, se puede usar el comando:

```bash
docker rm -f <nombre>
```

Si lo que queremos es eliminar todo: 

```bash
docker ps -q | xargs docker rm -f 
```

# Variables de entorno

Aunque eso ya ha sido explicado antes. Cuando se manejan variables de entorno es simple usarlas para definir paths y además se pueden sobreescribir usando docker compose y otros orquestadores, cosa que veremos más adelante.

Una pequeña prueba:

```bash
FROM ubuntu:22.04

ENV NOMBRE_IMAGEN="UBUNTU2204"

CMD ["sh", "-c", "echo $NOMBRE_IMAGEN"]
```

La compilamos y ejecutamos:

```bash
docker build -t imagen_prueba:v0 .

docker run -d --name "contenedor_prueba" imagen_prueba:v0 
``` 

Ahora vamos a analizar sus logs para ver si coincide con el valor de entorno que estamos definiendo

```bash
# Obtenemos el nombre del cotenedor
docker ps -a
CONTAINER ID   IMAGE              COMMAND       CREATED          STATUS                      PORTS     NAMES
640d60602e0c   imagen_prueba:v0   "/bin/bash"   34 seconds ago   Exited (0) 33 seconds ago             contenedor_prueba

# Vemos su salida
docker logs contenedor_prueba

UBUNTU2204
```

Una segunda opción para generar variables de entorno es emplear la opción ```-e```, con la que podemos definir y sobreescribir valores de variables de entorno.

Como ejemplo, si ahora ejecutamos la misma imagen sobreescribiendo este valor:

```bash
# Ejecutamos sobreescribiendo el valor de la variable de entorno
docker run -d -e NOMBRE_IMAGEN=UBUNTU --name "contenedor_prueba" imagen_prueba:v0

# Ejecutamos la imgaen
docker run -d -e NOMBRE_IMAGEN=UBUNTU --name "contenedor_prueba" imagen_prueba:v0 

# Ahora analizamos el resultado en los logs
docker logs contenedor_prueba
UBUNTU
```

## Ejemplo 1: Contenedor MySQL desde cero

En este primer ejemplo de práctica con imágenes vamos a crear un contenedor con MySQL, con la imagen oficial, y veremos qué requisitos se plantean para crear instancias y cómo crear imágenes definitivas con dicha configuración.

En el caso de mysql, en la documentación oficial https://hub.docker.com/_/mysql nos dicen que debemos usar el siguiente comando:
```bash
$ docker run --name some-mysql -e MYSQL_ROOT_PASSWORD=my-secret-pw -d mysql:tag
```

Si lo analizamos brevemente, veremos que lo que se está haciendo en realidad es definir una variable de entorno ```MYSQL_ROOT_PASSWORD=my-secret-pw```, un nombre de imagen ```some-mysql``` y la propia imagen ```mysql:tag```. Esto mismo en realidad se puede traducir en un Dockerfile que sea:

```bash
FROM mysql:9.0

ENV MYSQL_ROOT_PASSWORD=root
ENV MYSQL_DATABASE=mi_base
ENV MYSQL_USER=usuario
ENV MYSQL_PASSWORD=clave
```

El problema de hacer esto es que la contraseña se queda escrita en el fichero, lo cual no suele ser buena práctica

Un aspecto interesante de esto es que no lleva ```RUN```. Esto se debe a que ya dispone de un entrypoint interno que ejecuta la BBDDD en primer plano (recordando con esto que en un contenedor, cuando el PID 0 finaliza se da por finalizado el contenedor).

Partiendo de esa imagen, la construimos, ejecutamos y vemos su estado:
```bash
# Construccion:
docker build -t mysql_con_env:v0 .

# Ahora la ejecutamos:

docker run --name "mysql_instance" -p 3306:3306  -d mysql_con_env:v0

# Vemos su estado
docker ps
CONTAINER ID   IMAGE              COMMAND                  CREATED         STATUS         PORTS                                         NAMES
49ae06573901   mysql_con_env:v0   "docker-entrypoint.s…"   6 seconds ago   Up 6 seconds   0.0.0.0:3306->3306/tcp, [::]:3306->3306/tcp   mysql_instance
```

Antes de proseguir, vamos a crear un segundo contenedor para que la BBDD que estamos generando pueda ser probada, para lo cual necesitamos un cliente MySQL. Por lo que vamos a crear una CLI portátil, es decir, un contenedor que contenga este programa y que además no tenga CMD, es decir, que el PID 0 se debe proporcionar como comando al ejecutar la imagen.
```bash
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y mysql-client && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
```

Partiendo de esta imagen, que ejecutaremos de la siguiente forma: ```docker run -it --rm mi-mysql-client mysql -h <nombre de la imagen sql: puerto sql> -u <usuario de la bbdd> -p```

En este caso:

```bash
manso@clarion mysql_bbdd % docker run -it --rm mysql_cli:v0 mysql -h 172.17.0.1 -u root -p
```

Otro aspecto relevante de los contenedores es que podemos analizar el consumo de los contenedores en ejecución con ```docker stats```

```bash
879f2f818686   mysql_server   0.46%     473.6MiB / 7.654GiB   6.04%     13.8kB / 25kB   75.1MB / 283MB   36
```

## Trasladar ficheros desde o hacia el contenedor

El comando en sí es ```docker cp``` y funciona de forma similar a comandos como scp o ftp, indicando el origen y el destino de la siguiente forma:

- Si es un contenedor: <nombre contenedor>:<dir al fichero>
- Si es el host: /<dir al fichero>

Por ejemplo:
```bash
docker cp apache:/index.html /home/user/escritorio/
```
Caso en el que estaríamos copiando el index de la web de un servidor apache al escritorio del host.


## Asignación de recursos en los contenedores

Cuando ejecutamos contenedores es posible limitar el consumo de los contenedores al ejecutarlos.

En el caso de la memoria, podemos usar:

```bash
docker run -m "[Cantidad de memoria]" <imagen>
```

Por ejemplo, en el caso del contenedor de mysql:
```bash
docker run --name "mysql_instance" -p 3306:3306  -d -m "5 gb" mysql_con_env:v0
```

Cuando comprobamos ahora el consumo de dicho contenedor, ya no se calcula sobre la máxima capacidad del equipo sino sobre el máximo de memoria asignada:
```bash
docker stats
# Como se observa el limit es 5 GiB, asignados antes

f85abf033264   mysql_instance   14.00%    11.7MiB / 5GiB      0.23%     1.17kB / 126B   36.9MB / 264MB   1
```

El resto de posibles límites que podemos imponer sobre un contenedor son los siguientes:

```bash
      --cpuset-mems string               MEMs in which to allow execution (0-3, 0,1)
      --kernel-memory bytes              Kernel memory limit
  -m, --memory bytes                     Memory limit
      --memory-reservation bytes         Memory soft limit
      --memory-swap bytes                Swap limit equal to memory plus swap: '-1' to enable unlimited swap
      --memory-swappiness int            Tune container memory swappiness (0 to 100) (default -1)
```

Para el caso de la CPU el proceso es similar, existiendo las siguientes limitaciones:

```bash
      --cpu-period int                   Limit CPU CFS (Completely Fair Scheduler) period
      --cpu-quota int                    Limit CPU CFS (Completely Fair Scheduler) quota
      --cpu-rt-period int                Limit CPU real-time period in microseconds
      --cpu-rt-runtime int               Limit CPU real-time runtime in microseconds
  -c, --cpu-shares int                   CPU shares (relative weight)
      --cpus decimal                     Number of CPUs
      --cpuset-cpus string               CPUs in which to allow execution (0-3, 0,1)
      --cpuset-mems string               MEMs in which to allow execution (0-3, 0,1)
```

Para este caso, la limitacion más simple no es ```-c```, al contrario que ```-m```, si no que es ```--cpuset-cpus``` que determina el ID de las CPUS que se pueden permitir al contenedor.
