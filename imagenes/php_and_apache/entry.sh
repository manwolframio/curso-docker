#!/usr/bin/env bash
set -eu

BOOTSTRAP_VERSION="v5.3.3"
BOOTSTRAP_DIR="/opt/bootstrap"
WEB_BOOTSTRAP_DIR="/var/www/html/miweb/assets/bootstrap"

echo "ServerName DockerContainer" >/etc/apache2/conf-available/servername.conf || true
a2enconf servername >/dev/null 2>&1 || true

# Clonar Bootstrap solo si no está ya presente
if [ ! -d "${BOOTSTRAP_DIR}" ]; then
  echo "Clonando Bootstrap ${BOOTSTRAP_VERSION}..."
  git config --global advice.detachedHead false
  git clone -q --depth 1 --branch "${BOOTSTRAP_VERSION}" https://github.com/twbs/bootstrap.git "${BOOTSTRAP_DIR}"
fi

# Copiar solo los archivos dist (CSS, JS, etc.) a la web
mkdir -p "${WEB_BOOTSTRAP_DIR}"
cp -a "${BOOTSTRAP_DIR}/dist/"* "${WEB_BOOTSTRAP_DIR}/" || true

# Iniciar Apache en foreground
echo "Arrancando servidor apache TLS"
exec "$@"
