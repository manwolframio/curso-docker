# Redes en Docker

Partiendo del punto más básico, cuando instalamos Docker, este mismo proceso nos crea tres redes.

- Un network bridge y una interfaz llamada ```docker0```. A este bridge será al que se conecten tanto el host como los distintos contenedores que despleguemos, siempre que no estemos definiendo una red propia.

- Una referencia a la pila de red del host ```host```.

- Una red para contenedores que no hagan uso de esta pila ```none```.


Para consultar estas redes se puede usar el comando:

```bash
docker network ls

NETWORK ID     NAME      DRIVER    SCOPE
473284a86458   bridge    bridge    local
d2f775e0f794   host      host      local
44b012ff7d08   none      null      local
```

Para comprobarlo, vamos a desplegar un contenedor de los anteriores, por ejemplo:

```bash
docker run -d --name "prueba" ubuntu:22.04 tail -f /dev/null
```

Con este contenedor en ejecución podemos consultar su estado de red con:

```bash
docker inspect prueba
```

Con el que obtendremos el siguiente resultado:

```
"Networks": {
"bridge": {
    "IPAMConfig": null,
    "Links": null,
    "Aliases": null,
```

Donde se observa que la red asignada es ```bridge```, que es la red por defecto, la cual podemos consultar con:

```bash
docker network inspect bridge
``` 
Donde podremos ver lo siguiente:

- El direccionamiento de la red. 
```bash
"Subnet": "172.17.0.0/16",
"Gateway": "172.17.0.1"
```

- Los contenedores asociados a ella:
```bash
"Containers": {
    "caa62506219f7b7388d9c46644d9b7cb12d905c67c0937162cb601dc36d82bcb": {
        "Name": "prueba",
        "EndpointID": "935088ca476da2414df65a817a215062d98d50ad495cd328cee80a69615f58a5",
        "MacAddress": "be:71:28:ab:90:72",
        "IPv4Address": "172.17.0.2/16",
        "IPv6Address": ""
    }
},
```

- La interfaz con la que el host se conecta a este bridge:

```bash
"com.docker.network.bridge.name": "docker0",
```

Para comprobar esto ahora vamos a construir dos contenedores con la función ping instalada:

```bash
FROM ubuntu:22.04

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y iputils-ping

CMD ["tail", "-f", "/dev/null"]
```

Ahora construimos la imagen y lanzamos los dos contenedores:

```bash
# Construimos la imagen
docker build -t ubuntu-ping .

# Lanzamos los dos contenedores
docker run --name cont_1 -d ubuntu-ping

docker run --name cont_2 -d ubuntu-ping
```

Con ambas instancias ya creadas, vamos a ver cuál es la IP que se le ha asignado a uno de ellos, por ejemplo a ```cont_1```:

```bash
docker inspect cont_1

"Gateway": "172.17.0.1",
"IPAddress": "172.17.0.2",
```

Si lo que hemos aprendido es correcto, cuando el otro contenedor haga ping a esta IP debe ser capaz de alcanzar al contenedor, ya que están en la misma subred ```bridge```, que es la red por defecto.
Para ello:

```bash
docker exec cont_2 ping -c 4 172.17.0.2

PING 172.17.0.2 (172.17.0.2) 56(84) bytes of data.
64 bytes from 172.17.0.2: icmp_seq=1 ttl=64 time=0.098 ms
64 bytes from 172.17.0.2: icmp_seq=2 ttl=64 time=0.128 ms
64 bytes from 172.17.0.2: icmp_seq=3 ttl=64 time=0.157 ms
64 bytes from 172.17.0.2: icmp_seq=4 ttl=64 time=0.140 ms

--- 172.17.0.2 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3079ms
rtt min/avg/max/mdev = 0.098/0.130/0.157/0.021 ms
```

Como vemos ha funcionado perfectamente.

# Cómo crear una red

- Crear una red con direccionamiento automático:

```bash
docker network create <nombre de la red>
```

- Crear una subred con direccionamiento manual:

Cuando Docker crea una red bridge, también crea en el host una interfaz puente, por ejemplo ```docker0```. Esa interfaz recibe una IP dentro de la subred de la red Docker, y esa IP actúa como gateway para los contenedores conectados a esa red.

```bash
docker networ create --subnet <dir de red / mascara> --driver bridge --gateway <direccion del red del gw> <nombre de la red>
```

Vamos ahora a realizar una prueba, creando dos redes, ```red-1``` y ```red-2```, y en cada una de ellas vamos a conectar un contenedor partiendo de la imagen que generamos antes para hacer ping.

```bash
# Red 1: 10.0.0.0/24
docker network create  --subnet 10.0.0.0/24 --gateway 10.0.0.1 red_1
# Red 2: 20.0.0.0/24
docker network create  --subnet 20.0.0.0/24 --gateway 20.0.0.1 red_2

docker network ls 
NETWORK ID     NAME      DRIVER    
28779a71c41d   red_1     bridge    local
7e5883f9faa2   red_2     bridge    local
```

Ahora creamos los contenedores:

```bash
ocker run --name "contenedor_red_1" -d --network "red_1" ubuntu-ping 

docker run --name "contenedor_red_2" -d --network "red_2" ubuntu-ping 
```

Y vemos el direccionamiento que tiene cada uno:

```bash
# Contenedor en red 1
docker network inspect red_1

"Name": "contenedor_red_1",
"EndpointID": "486c1d06f27a6312eef07c7120607e44433a897e508ee1c130fe25106311bc5d",
"MacAddress": "7a:e0:73:11:6b:9e",
"IPv4Address": "10.0.0.2/24",

# Contenedor en red 2
docker network inspect red_2

"Name": "contenedor_red_2",
"EndpointID": "4a839777a97949204059ea595351610cba9ec2a0e0424cf2d8cd1b26f4b9d9fd",
"MacAddress": "9a:43:4b:97:b4:e9",
"IPv4Address": "20.0.0.2/24",
```

Cuando el contenedor de la red 1 hace ping al gw de la red 1:

```bash
docker exec -it contenedor_red_1 ping 10.0.0.1 -c 4

PING 10.0.0.1 (10.0.0.1) 56(84) bytes of data.
64 bytes from 10.0.0.1: icmp_seq=1 ttl=64 time=0.148 ms
64 bytes from 10.0.0.1: icmp_seq=2 ttl=64 time=0.054 ms
64 bytes from 10.0.0.1: icmp_seq=3 ttl=64 time=0.124 ms
64 bytes from 10.0.0.1: icmp_seq=4 ttl=64 time=0.121 ms
```

Sin embargo, cuando hace ping al otro contenedor:

```bash
docker exec -it contenedor_red_1 ping 20.0.0.2 -c 4

PING 20.0.0.2 (20.0.0.2) 56(84) bytes of data.

--- 20.0.0.2 ping statistics ---
4 packets transmitted, 0 received, 100% packet loss, time 3074ms
```

Como se ve, el ping no es respondido porque está dentro de otra subnet a la que aún no tiene acceso.

## Cómo conectar contenedores a redes

Como hemos visto antes, cuando instanciamos un contenedor en base a una imagen puede asociarse además a una red; sin embargo, en el caso anterior hemos visto cómo, cuando no están dentro de la misma subnet, los contenedores no se comunican.

Otra opción dentro de la parte de networking de Docker es asociar el contenedor a la red cuando ya existe. Para ello se puede usar el siguiente comando:

```bash
docker network connect <red> <nombre del contenedor>
```

Partiendo del caso anterior, en el que el contenedor 2 no podía comunicarse con el contenedor 1 por estar en distinta subred, vamos ahora a asociar el contenedor 2 a la red 1 también, haciendo que tenga un puerto en ambas. Para ello, ejecutamos el siguiente comando:

```bash
network connect red_1 contenedor_red_2
```

Ahora consultamos la red 1 para ver si ya están los dos contenedores:

```bash
"Containers": {
"0d8480b3a47de9b4effb5ce63abd2cbf152b567c088f1aa35f434da047f768e8": {
    "Name": "contenedor_red_2",
    "EndpointID": "2054a2756ac5a1cfd63bae0cc4ee89907476b70e6a3c3b026a9930fc47722b40",
    "MacAddress": "96:3e:16:6f:d3:8b",
    "IPv4Address": "10.0.0.3/24",
    "IPv6Address": ""
},
"2f3eacdbe4282cc1f4022f96ca6ecb9d6b824003b232dfe5056e026a4a926fef": {
    "Name": "contenedor_red_1",
    "EndpointID": "486c1d06f27a6312eef07c7120607e44433a897e508ee1c130fe25106311bc5d",
    "MacAddress": "7a:e0:73:11:6b:9e",
    "IPv4Address": "10.0.0.2/24",
    "IPv6Address": ""
}
```

Ahora sí están los dos, de forma que entre ambos ya se pueden comunicar correctamente:

```bash
docker exec -it contenedor_red_1 ping 10.0.0.2 -c 4

PING 10.0.0.2 (10.0.0.2) 56(84) bytes of data.
64 bytes from 10.0.0.2: icmp_seq=1 ttl=64 time=0.036 ms
64 bytes from 10.0.0.2: icmp_seq=2 ttl=64 time=0.073 ms
64 bytes from 10.0.0.2: icmp_seq=3 ttl=64 time=0.104 ms
64 bytes from 10.0.0.2: icmp_seq=4 ttl=64 time=0.102 ms
```


