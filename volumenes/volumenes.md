# Volúmenes

En este apartado se aprenderá a manejar los distintos tipos de objetos que permiten mantener almacenamiento persistente.

Existen tres tipos de volumenes en docker:
- Host
- Anonymus
- Named volumes

Esta parte está mas asiganda a contenedores que no corren standalone, es decir, con docker run, si no que se ejecutan empleando orquestadores como docker compose o kubernettes, en la que estos volumenes se puedan gestionar más facilmente.

## Importancia de los volúmenes
Vamos a lanzar un primer contenedor con un volumen, en este caso un mysql al que le asociaremos un volumen:

```bash
docker run -d  --name "mysql_server" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3306:3306 mysql:9.0
```

Este comando generará un contenedor con una BBDD mysql que tendrá una base de datos llamada ```docker-db``` y una conraseña de root.
Ahora vamos a acceder a la bbdd de dicho contenedor:

```bash
docker exec -it  mysql_server mysql -h localhost -p12345678
``` 

Cuando ya seamos capaces de acceder a ella vamso a ejecutar el siguiente comando: 
```bash
docker exec -it  mysql_server mysqldump -h localhost -p12345678 sys > dump.sql
```

Que lo que hace es generar una copia de la BBDD sys que es una de las que aparecen por defecto y la almacena en el host, en el fichero ```dump.sql```

Ahora, con este dump, vamos a generar un nuevo contedor con mysql 

```bash
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3307:3306 mysql:9.0
```

Y vamos a cargar en sus bbdd el dump anterior con el comando:
```bash
docker exec -i mysql_server_replica mysql -h localhost -p12345678 docker-db < dump.sql
```
Saltará un error ya que estamos cargando tablas con distintas funciones, lo ignoramos, ya que solo queremos ver el funcionamiento del almacenamiento no persistente.

Para ello, ahora consultamos esta BBDD de la replica:

```bash
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

```bash
# Eliminamos el contenedor
docker rm -f mysql_server_replica

# Creamos uno nuevo
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3307:3306 mysql:9.0

# Volvemos al ver sus tablas
docker exec -it mysql_server_replica mysql -h localhost -p12345678 docker-db

mysql> SHOW tables;
Empty set (0.00 sec) # Se muestran vacias para dicha BBDD
```

Que hubiese pasado si esta informacion fuese valiosa?. Aquí es donde entra el concepto de almacenamiento persistente, es deir de volumenes que aunque se elimine el contedor permanecen activos manteniendo esta informaición.

Como nota, en este caso, hemos eliminado manualmente los contenedores, lo cual hace que parezca que esta idea no tenga sentido, pero los contenedores se pueden reiniciar, pueden fallar y pueden colapsar, por lo que es necesario mantener la informacion a buen recaudo.

# Definir volumenes de host

Los **volúmenes de host Docker** permiten persistir datos fuera del ciclo de vida del contenedor, mapeando directorios de la máquina física al contenedor. Son esenciales para compartir archivos entre el host y el contenedor, o entre contenedores, almacenándose generalmente en ```/var/lib/docker/volumes/```. Se gestionan mediante docker volume.

Este mapa que se establece entre contenedor y host es similar al concepto de montar volumenes que se maneja en ssoo. 

Para ver su funcionamiento vamos a ver una pequela prueba con la imagen anterior pero asociandola un volumen. Para ello, se añade al comando ```docker run`` la opcion ```-v <direccion del host>:<direccion del contenedor>```

```bash
docker run -d  --name "mysql_server_replica" -e "MYSQL_ROOT_PASSWORD=12345678" -e "MYSQL_DATABASE=docker-db" -p 3306:3306 -v /var/lib/docker/volumes/mysql_server_replica:/var/lib/mysql mysql:9.0
```

Esto lo que hace es montar el directorio ```/var/lib/docker/volumes/mysql_server_replica``` en la ruta ```/var/lib/mysql``` en el contenedor ```mysql_server_replica```. 

Este directorio ```/var/lib/mysql``` es en el que mysql opera para almacenar las bases de datos que es el motivo por el que se le está dando esta persistencia.

Si queremos comprobar si se ha montado correctamente podemos ejecutar los siguientes comandos:

```bash
docker inspect mysql_server_replica
```

que nos devuelve en formato json toda la informacion del contenedor o si queremos algo más concreto:

```bash
docker inspect mysql_server_replica --format '{{json .Mounts}}'

[{"Type":"bind","Source":"/var/lib/docker/volumes/mysql_server_replica","Destination":"/var/lib/mysql","Mode":"","RW":true,"Propagation":"rslave"}]
```

que nos devuelve la parte del json en la que se especifican los volumenes.

Ahora si eliminamos el contenedor pero no el volumen, podemos restituirlo facilmente o recuperar la informacion.

# Volumenes anonimos









