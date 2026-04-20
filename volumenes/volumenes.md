# Volúmenes

## Índice
- [Importancia de los volúmenes](#importancia-de-los-volúmenes): por qué el almacenamiento persistente es necesario y qué pasa si no se usa.
- [Definir volúmenes de host](#definir-volúmenes-de-host): cómo montar una ruta del host dentro de un contenedor.
- [Volúmenes anónimos](#volúmenes-anónimos): cómo funcionan los volúmenes sin nombre y cuándo usarlos con cautela.
- [Instrucción de volúmenes en el Dockerfile](#instrucción-de-volúmenes-en-el-dockerfile): cómo declarar un volumen desde la imagen.
- [Volúmenes con nombre](#volúmenes-con-nombre): cómo crear, reutilizar y compartir volúmenes entre contenedores.

En este apartado se aprenderá a manejar los distintos tipos de objetos que permiten mantener almacenamiento persistente.

Existen tres tipos de volúmenes en Docker:
- Host
- Anonymous
- Named volumes

Esta parte está más asignada a contenedores que no corren standalone, es decir, con docker run, sino que se ejecutan empleando orquestadores como Docker Compose o Kubernetes, en los que estos volúmenes se pueden gestionar más fácilmente.

## Importancia de los volúmenes
Vamos a lanzar un primer contenedor con un volumen, en este caso un mysql al que le asociaremos un volumen:

```python
docker run -d  --name "mysql_server" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3306:3306 mysql:9.0
```

Este comando generará un contenedor con una BBDD MySQL que tendrá una base de datos llamada ```docker-db``` y una contraseña de root.
Ahora vamos a acceder a la BBDD de dicho contenedor:

```python
docker exec -it  mysql_server mysql -h localhost -p12345678
``` 

Cuando ya seamos capaces de acceder a ella vamos a ejecutar el siguiente comando:
```python
docker exec -it  mysql_server mysqldump -h localhost -p12345678 sys > dump.sql
```

Que lo que hace es generar una copia de la BBDD sys que es una de las que aparecen por defecto y la almacena en el host, en el fichero ```dump.sql```

Ahora, con este dump, vamos a generar un nuevo contenedor con MySQL.

```python
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3307:3306 mysql:9.0
```

Y vamos a cargar en su BBDD el dump anterior con el comando:
```python
docker exec -i mysql_server_replica mysql -h localhost -p12345678 docker-db < dump.sql
```
Saltará un error ya que estamos cargando tablas con distintas funciones, lo ignoramos, ya que solo queremos ver el funcionamiento del almacenamiento no persistente.

Para ello, ahora consultamos esta BBDD de la replica:

```python
docker exec -it mysql_server_replica mysql -h localhost -p12345678 docker-db          

mysql> SHOW tables

+-----------------------------------------------+
| Tables_in_docker-db                           |
+-----------------------------------------------+
| host_summary                                  |
| host_summary_by_file_io                       |
| host_summary_by_file_io_type                  |
| host_summary_by_stages                        |
| host_summary_by_statement_latency             |
```

Si ahora eliminamos la réplica, este contenido se borrará, lo cual podemos observar de la siguiente forma:

```python
# Eliminamos el contenedor
docker rm -f mysql_server_replica

# Creamos uno nuevo
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3307:3306 mysql:9.0

# Volvemos al ver sus tablas
docker exec -it mysql_server_replica mysql -h localhost -p12345678 docker-db

mysql> SHOW tables;
Empty set (0.00 sec) # Se muestran vacias para dicha BBDD
```

¿Qué hubiese pasado si esta información fuese valiosa? Aquí es donde entra el concepto de almacenamiento persistente, es decir, de volúmenes que, aunque se elimine el contenedor, permanecen activos manteniendo esta información.

Como nota, en este caso hemos eliminado manualmente los contenedores, lo cual hace que parezca que esta idea no tenga sentido, pero los contenedores se pueden reiniciar, pueden fallar y pueden colapsar, por lo que es necesario mantener la información a buen recaudo.

# Definir volúmenes de host

Los **volúmenes de host Docker** permiten persistir datos fuera del ciclo de vida del contenedor, mapeando directorios de la máquina física al contenedor. Son esenciales para compartir archivos entre el host y el contenedor, o entre contenedores, almacenándose generalmente en ```/var/lib/docker/volumes/```. Se gestionan mediante docker volume.

Este mapa que se establece entre contenedor y host es similar al concepto de montar volúmenes que se maneja en sistemas operativos.

Para ver su funcionamiento vamos a ver una pequeña prueba con la imagen anterior, pero asociándole un volumen. Para ello, se añade al comando ```docker run``` la opción ```-v <direccion del host>:<direccion del contenedor>```

```python
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3306:3306 -v /var/lib/docker/volumes/mysql_server_replica:/var/lib/mysql mysql:9.0
```

Esto lo que hace es montar el directorio ```/var/lib/docker/volumes/mysql_server_replica``` en la ruta ```/var/lib/mysql``` en el contenedor ```mysql_server_replica```. 

Este directorio ```/var/lib/mysql``` es en el que mysql opera para almacenar las bases de datos que es el motivo por el que se le está dando esta persistencia.

Si queremos comprobar si se ha montado correctamente podemos ejecutar los siguientes comandos:

```python
docker inspect mysql_server_replica
```

que nos devuelve en formato JSON toda la información del contenedor o, si queremos algo más concreto:

```python
docker inspect mysql_server_replica --format '{{json .Mounts}}'

[{"Type":"bind","Source":"/var/lib/docker/volumes/mysql_server_replica","Destination":"/var/lib/mysql","Mode":"","RW":true,"Propagation":"rslave"}]
```

que nos devuelve la parte del JSON en la que se especifican los volúmenes.

Ahora, si eliminamos el contenedor pero no el volumen, podemos restituirlo fácilmente o recuperar la información.

# Volúmenes anónimos

En este tipo de volúmenes su funcionamiento es análogo al de los volúmenes de host. Se definen al ejecutar el contenedor, salvo que esta vez no tienen un punto de montaje en el host, sino que funcionan como espacios persistentes no accesibles desde el host.

Para crearlos el proceso es análogo al anterior, añadiendo ```-v <directorio del contenedor>```. Por ejemplo:

```python
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3306:3306 -v /var/lib/mysql mysql:9.0
```

Para ver el resultado se puede ejecutar el mismo comando de antes:

```python
docker inspect mysql_server_replica --format '{{json .Mounts}}'

[{"Type":"volume","Name":"b465952e4a64a39e2dafa6b6fc92e99dd6b4edae38ce5a257e46c19bae1e9698","Source":"/var/lib/docker/volumes/b465952e4a64a39e2dafa6b6fc92e99dd6b4edae38ce5a257e46c19bae1e9698/_data","Destination":"/var/lib/mysql","Driver":"local","Mode":"","RW":true,"Propagation":""}]
```

Este es el tipo que menos debe usarse, ya que es complicado acceder y rescatar la información en dichos contenedores.

# Instrucción de volúmenes en el Dockerfile

Como hemos visto antes, cuando se instancia un contenedor a partir de una imagen se pueden sobreescribir algunas de sus capas, cosa que será igual en Docker Compose y los distintos orquestadores. Ya hemos visto que se puede sobreescribir el ```CMD```. También hemos visto que se pueden sobreescribir las variables de entorno ```ENV```. Pues con volúmenes también se puede. Para ello, cuando definimos una imagen, es posible también definir volúmenes en esta, usando la instrucción ```VOLUME``` que ya vimos.

Un ejemplo simple: 

```python
FROM ubuntu:22.04
# Se crea un volumen anonimo en el contenedor
VOLUME ["/opt"]

CMD ["tail", "-f", "/dev/null"]
```

```python
# Creamos el contenedor
docker build -t imagen_prueba_vol:0 .

# Lo ejecutamos
docker run -d --name "test_vol" imagen_prueba_vol:0

# Metemos cosas en el directorio al que se ha asociado el volumen
docker exec test_vol touch /opt/sample_1.txt

docker exec test_vol touch /opt/sample_2.txt
docker exec test_vol ls /opt/       

sample_1.txt
sample_2.txt
```

Ahora que ya tenemos ficheros en el contenedor, es momento de ver donde están. Para ello consultamos el docker root

```python
docker info | grep -i root Docker Root Dir: /var/lib/docker
```
Pues en dicha carpeta está el volumen montado.

Un pequeño comentario: si estamos llenando todo de volúmenes y repos y queremos dejarlo limpio, se propone el siguiente comando:

```python
docker rm -f $(docker ps -aq)
docker system prune -a --volumes -f
```

# Volúmenes con nombre

Esta es la mejor versión de volúmenes de Docker, ya que permite generar volúmenes, compartirlos entre varios contenedores y gestionarlos bien.

Cómo se crea un volumen con nombre:

```python
docker volume create <nombre del volumen>
```

Por ejemplo
```python
docker volume create mysql_vol
```

Ahora podemos comprobar si está bien creado:

```python
docker volume ls

DRIVER    VOLUME NAME
local     mysql_vol
```

Y podemos asignárselo a un contenedor. Para ello retomamos la imagen MySQL que habíamos ejecutado antes:

```python
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3307:3306 -v mysql_vol:/var/lib/mysql mysql:9.0
```

Para comprobar ahora que hay cosas, vamos a ejecutar un contenedor con Ubuntu con el mismo volumen montado, pero en un directorio distinto, por ejemplo:

```python
docker run -it -v mysql_vol:/home/mysql --rm ubuntu:22.04 bash

# Se nos abre una consola

> cd home/
> ls

mysql

> cd mysql/
> ls

binlog.000001   client-cert.pem   ibtmp1                  performance_schema   sys ...
```

Efectivamente, hemos podido montar el mismo volumen en este nuevo contenedor y hemos visto que los archivos del contenedor con MySQL están en él.







