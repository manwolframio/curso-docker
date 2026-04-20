# Docker compose

Es una herramienta útil para desplegar aplicaciones multicontenedor.
Permite definir, mediante ficheros ```.yaml``` con los que se definen todos los aspectos de despliegue de los que hemos hablado anteriormente como la construcción de los mismos, redes o volumenes. Para no tener que hacerlo por comandos.

Estan compuestos por cuatro partes:

- services (Donde definiremos los contenedores) (obligatorio)
- networks (redes)
- volumes (donde definimos los volumenes)


Los ficheros tienen el siguiente aspecto:
```python
services:

volumes:

networks:
```
# Uso básico

Primero un pequeño ejemplo con un contenedor nginx:

- Con docker solo:
```python
docker run -d --name "nginx-container" -p 80:80 nginx
```

- Con compose:
```python
services:
  web-server:
    container_name: nginx-server
    image: nginx
    ports:
    - "8080:80"
```

Lo arrancamos:
```python
docker compose build # construimos
docker compose up # levantamos
```

Lo detenemos:
```python
docker compose down -v # detenemos
```

Si queremos ahora probar con variables de entorno. Antes usabamos:
```python
docker run -it --rm mysql_cli:v0 mysql -h 172.17.0.1 -u root -p
```
Ahora podemos usar:

```python

```python
services:
    database:
        container_name: mysql-container
        image: mysql:9.0
        ports:
            - "3306:3306"
        environment:
            - MYSQL_ROOT_PASSWORD=root
            - MYSQL_DATABASE=mi_base
            - MYSQL_USER=usuario
            - MYSQL_PASSWORD=clave

```

No obstante, no suele ser buena práctica dejarlo así por que tenemos las claves expuestas, para solucionarlo, es mejor tener ficheros de enviroment y cargarlos:

```python
echo "MYSQL_ROOT_PASSWORD=root
MYSQL_DATABASE=mi_base
MYSQL_USER=usuario
MYSQL_PASSWORD=clave" > mysql.env
```

y ahora:

```python
services:
  database:
    container_name: mysql-container
    image: mysql:9.0
    ports:
      - "3306:3306"
    env_file:
      - "mysql.env"
    environment:
      - MYSQL_DATABASE=docker-db
```

## Otras opciones

### Modificar el CMD 

Otra opción que tiene compose, al igual que la tenia el comando ```docker run``` es la de sobreescribir el ```CMD```del contenedor que estamos ejecutando. Para ello, cuando definimos un service, tras indicar la parte de la imagen, podemos indicar, con ```command``` lo que queremos que se ejecute. 

Sin embargo, antes de proceder debemos recordar que no hablamos de la capa RUN de la imagen, es decir, no debemos instalar dependencias ni modificar en si la imagen base, para eso hay otras opciones que podemos ejecutar más adelante. Esto solo vale para sustituir la linea ```CMD``` de la imagen base, por ejemplo:

```python
services:
  web-1:
    image: ubuntu:22.04
    environment:
      - DEBIAN_FRONTEND=noninteractive
      - TZ=Europe/Madrid
    command:
      - getent
      - ahostsv4
      - google.es

networks: {}
# vacio
volumes: {}
# vacio
```

Esto lo que hace es hacer una consulta al nombre ```google.es```empleando DNS. De forma que, podremos ver el resultado si hacemos lo siguiente:

#### 1) Levantamos los contenedores
```bash
docker compose up -d
```

```bash
[+] up 2/2
 ✔ Network mod-comandos_default   Created                                                                                                                             0.0s
 ✔ Container mod-comandos-web-1-1 Started                                                                                                                             0.3s
```

#### 2) Analizamos el estado de los contenedores

Este paso se hace por que el comadno que hemos puesto finaliza rapidamente y como hemos aprendido, cuando el proceso con ```PID=0``` Finaliza, el contenedor se destruye, por lo que al acabar la peticion DNS el contenedor debería haberse detenido.

```bash
docker compose ps -a
```

```bash
NAME                   IMAGE          COMMAND                  SERVICE   CREATED          STATUS                      PORTS
mod-comandos-web-1-1   ubuntu:22.04   "getent ahostsv4 goo…"   web-1     14 seconds ago   Exited (0) 13 seconds ago   
```

Efectivamente, hemos creado el contenedor hace 14s y el contenedor se ha detenido a los 13s, por lo que en realidad ha estado vivo un segundo (o menos), que es lo que ha tardado en resolver la consulta DNS, cuya respuesta podemos ver en la salida de estandar del contenedor, que se puede consultar de la siguiente forma:

```bash
docker compose logs web-1
```

```bash
web-1-1  | 142.251.142.131 STREAM google.es
web-1-1  | 142.251.142.131 DGRAM  
web-1-1  | 142.251.142.131 RAW
```

#### 3) Apagamos el escenario:
```bash
docker compose down -v
```


## Crear volumenes en compose

De la misma forma que en Docker se podía asociar un volumen a un contenedor con la opción ```-v```.

En Compose esto se declara dentro del propio fichero YAML. La ventaja es que el volumen queda descrito como parte del despliegue, por lo que no hace falta recordar el comando completo cada vez que se quiera levantar el servicio.

En este caso seguimos partiendo del contenedor de MySQL que ya teníamos definido antes. La diferencia es que ahora, dentro del servicio ```database```, se añade el bloque ```volumes``` para montar el volumen ```mysql-vol``` sobre la ruta ```/var/lib/mysql```, que es donde MySQL almacena internamente sus datos. Además, al final del fichero se declara ese volumen dentro del bloque raíz ```volumes```, haciendo que Compose lo cree y lo gestione automáticamente.

```python
services:
  database:
    container_name: mysql-container
    image: mysql:9.0
    ports:
      - "3306:3306"
    env_file:
      - mysql.env
    environment:
      - MYSQL_DATABASE=docker-db
    volumes:
      - mysql-vol:/var/lib/mysql

volumes:
  mysql-vol:
```

Esto hace que, aunque el contenedor se elimine y se vuelva a crear, la información siga en el volumen y no se pierda con el ciclo de vida del contenedor.

```python
docker compose volumes  
DRIVER    VOLUME NAME
local     mysql-volumes_mysql-vol
```

Al consultar los volúmenes puede verse que Compose no deja el nombre exactamente igual, sino que le añade un prefijo relacionado con el directorio o proyecto. En este caso, aparece declarado como ```mysql-vol``` aparece finalmente como ```mysql-volumes_mysql-vol```.

## Crear redes en compose 

La definición de redes en Compose sigue la misma idea que la de volúmenes: en lugar de crear la red manualmente por comandos y luego conectar los contenedores, todo queda descrito dentro del propio fichero de despliegue.

Para mostrarlo se muestra el ejemplo de dos servicios web nginx.

```python
services:
  web-server-main:
    image: nginx
    container_name: main-webserver
    ports:
      - "80:80"
    networks:
      - internal

  web-server-back:
    image: nginx
    container_name: backup-webserver
    ports:
      - "81:80"
    networks:
      - internal

networks:
  internal:
    driver: bridge
    ipam:
      config:
        - subnet: 10.0.0.0/24
          gateway: 10.0.0.1
```
La red se declara al final del fichero, dentro del bloque raíz ```networks```. En este ejemplo se está usando el driver ```bridge```, que es el más habitual para redes locales entre contenedores en un mismo host.

Además, se define un bloque ```ipam``` para fijar manualmente el direccionamiento de la red, indicando tanto la subred ```10.0.0.0/24``` como la puerta de enlace ```10.0.0.1```.

## Construccion de imagenes empleando compose


## Políticas de trabajo

Compose no es solo una herramienta para levantar y constuir imágenes, es un orquestador sencillo pero un orquestador por lo que tiene control de los ciclos de vida de los contenedores. 

Que quiere decir esto, que podemos controlar, como ya vimos, el número de recursos, que ocupan, el tiempo, cuando se reinician y cuando se reactivan y cuando operan correctamente.

### Control de estado de un contenedor

Muchas veces, cuando operamos con contenedores, un solo contenedor lleva más de un proceso en la imágen. Esto implica que el contenedor puede estar operativo por que el proceso con ``PID=0```funciona pero un proceso secundario nos ha dejado el servicio de dicho contenedor caido. Para ello, existe una herramienta que son los *healthchecks*. Estos son endpoints que se exponen para consultar si el servicio está corriendo correctamente.

Dentro de un service instanciado en compose se puede definir un *healtcheck* que permita al orquestador saber si el contenedor funciona. 

Para ello, lo primero es definir un punto de control en alguna parte de la imagen. Por ejemplo, vamos a crear un contenedor con dos servidores flask, uno que nos dirá la hora y uno que nos dirá el día. Para ello debemos desarrollar una imagen que al arrancar levante ambos servidores con PIDs separados. Uno heredará el PID0 y el otro sera un proceso hijo que tendrá otro PID.


1) Solo comprobamos uno de los endpoints del contenedor

```bash
web:
  build:
    context: ./source
    dockerfile: Dockerfile
  image: doble-web-python:latest
  ports:
  - 3000:3000
  - 3001:3001
  healthcheck:
        test: ["CMD", "curl", "-f", "http://localhost:3000/status"]
        interval: 2s
        timeout: 60s
        retries: 20
```

En este priemer caso, si el proceso que no tiene el PID0 se cae, falla o no se levanta, el health check no será capaz de verlo. Para arreglaro, se puede integrar un segundo comando el el comando de test de la siguiente forma:

2) Comprobamos los dos endpoints
```bash
web:
  build:
    context: ./source
    dockerfile: Dockerfile
  image: doble-web-python:latest
  ports:
  - 3000:3000
  - 3001:3001
  healthcheck:
    test:
      [
        "CMD-SHELL",
        "curl -f http://localhost:3000/healthcheck && curl -f http://localhost:3001/healthcheck || exit 1"
      ]
    interval: 2s
    timeout: 60s
    retries: 20
```

Donde el comando
```bash
curl -f http://localhost:3000/healthcheck && curl -f http://localhost:3001/healthcheck || exit 1
```
Une las respuestasd de los procesos de ambos curls y en funcion de ello, si terminan con 0 (todo bien) las dos, la resptuesta será (0|0) & 1 = 0. si alguno es 1, la respuesta será (x|y) & 1 {dónde cualquiera x, y o ambos son 1} = 1. 
Recordar que cualquier valor distinto de uno al finalizar un proceso implica algún fallo o warning.

### Politicas de uso y capacidad

En compose también podemos limitar el uso, igual en docker sin orquestador, que hace cada uno de los contenedores de la máquina host. Para ello, existen instrucciones especificas como ```mem_limit```, ```cpus``` o ```cpuset```.

Por ejemplo el siguiente yaml:

```bash
services:
  ubuntu-1:
    container_name: ubuntu-1
    image: ubuntu:22.04
    mem_limit: 20m
    cpus: 2
    command: ["sleep", "infinity"]

  ubuntu-2:
    container_name: ubuntu-2
    image: ubuntu:22.04
    mem_limit: 40m
    cpus: 4
    command: ["sleep", "infinity"]
```

Para ejecutarlo:

```bash
docker compose up -d
```

Si ahora consultamos las estadisticas de uso de ambos contenedores veremos que los limites asignados y los limites que indica son consistentes:

```bash
docker stats

CONTAINER ID   NAME       CPU %     MEM USAGE / LIMIT   MEM %     NET I/O         BLOCK I/O   PIDS
3f804036bf5b   ubuntu-1   0.00%     412KiB / 20MiB      2.01%     8.84kB / 126B   0B / 0B     1
d52237abe197   ubuntu-2   0.00%     404KiB / 40MiB      0.99%     8.58kB / 126B   0B / 0B     1
```

### Políticas de reinicio



### Politicas de dependencias