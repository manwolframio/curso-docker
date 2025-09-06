#!/bin/bash
set -eu

# Para poder usar TLS tenemos que crear un certificado autofirmado en el directorio en el que estamos trabajando
exec "$@"