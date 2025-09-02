#!/bin/sh
set -eu
# Genera el index final a partir de la plantilla
envsubst < /var/www/html/index.html.template > /var/www/html/index.html

echo "Ejecutando comando de arranque de apache en el CMD"
echo "Ejecutado por: ${USER:-$(whoami)}"

# Toma el comando del CMD
exec "$@"
