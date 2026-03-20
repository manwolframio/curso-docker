# Docker compose

Es una herramienta útil para desplegar aplicaciones multicontenedor.
Permite definir, mediante ficheros ```.yaml``` con los que se definen todos los aspectos de despliegue de los que hemos hablado anteriormente como la construcción de los mismos, redes o volumenes. Para no tener que hacerlo por comandos.

Estan compuestos por cuatro partes:

- services (Donde definiremos los contenedores) (obligatorio)
- networks (redes)
- volumes (donde definimos los volumenes)


Los ficheros tienen el siguiente aspecto:
```bash
services:

volumes:

networks:
```
# Uso básico

Primero un pequeño ejemplo con un contenedor nginx:

- Con docker solo:
```bash
docker run -d --name "nginx-container" -p 80:80 nginx
```

- Con compose:
```bash
services:
  web-server:
    container_name: nginx-server
    image: nginx
    ports:
    - "8080:80"
```

Lo arrancamos:
```bash
docker compose build # construimos
docker compose up # levantamos
```

Lo detenemos:
```bash
docker compose down -v # detenemos
```

Si queremos ahora probar con variables de entorno. Antes usabamos:
```bash
docker run -it --rm mysql_cli:v0 mysql -h 172.17.0.1 -u root -p
```
Ahora podemos usar:

```docker-compose.yaml```

```bash
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

```bash
echo "MYSQL_ROOT_PASSWORD=root
MYSQL_DATABASE=mi_base
MYSQL_USER=usuario
MYSQL_PASSWORD=clave" > mysql.env
```

y ahora:

```bash
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

## Crear volumenes en compose

De la misma forma que en Docker se podía asociar un volumen a un contenedor con la opción ```-v```.

En Compose esto se declara dentro del propio fichero YAML. La ventaja es que el volumen queda descrito como parte del despliegue, por lo que no hace falta recordar el comando completo cada vez que se quiera levantar el servicio.

En este caso seguimos partiendo del contenedor de MySQL que ya teníamos definido antes. La diferencia es que ahora, dentro del servicio ```database```, se añade el bloque ```volumes``` para montar el volumen ```mysql-vol``` sobre la ruta ```/var/lib/mysql```, que es donde MySQL almacena internamente sus datos. Además, al final del fichero se declara ese volumen dentro del bloque raíz ```volumes```, haciendo que Compose lo cree y lo gestione automáticamente.

```bash
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

```bash
docker compose volumes  
DRIVER    VOLUME NAME
local     mysql-volumes_mysql-vol
```

Al consultar los volúmenes puede verse que Compose no deja el nombre exactamente igual, sino que le añade un prefijo relacionado con el directorio o proyecto. En este caso, aparece declarado como ```mysql-vol``` aparece finalmente como ```mysql-volumes_mysql-vol```.

## Crear redes en compose 

La definición de redes en Compose sigue la misma idea que la de volúmenes: en lugar de crear la red manualmente por comandos y luego conectar los contenedores, todo queda descrito dentro del propio fichero de despliegue.

Para mostrarlo se muestra el ejemplo de dos servicios web nginx.

```bash
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