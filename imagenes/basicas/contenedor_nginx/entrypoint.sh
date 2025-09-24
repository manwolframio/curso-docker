#!/bin/bash
echo "Arrancando el servidor web - $(date '+%Y-%m-%d %H:%M:%S')"
echo "Servicio arrancado por: ${USER:-$(whoami)}"
exec "$@"