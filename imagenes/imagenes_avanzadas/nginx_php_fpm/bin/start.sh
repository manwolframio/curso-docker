#!/bin/bash
set -eu

BOOTSTRAP_VERSION="v5.3.3"
BOOTSTRAP_DIR="/opt/bootstrap"
WEB_BOOTSTRAP_DIR="/var/www/dockercontainer/public/assets/bootstrap"

if [ ! -d "${BOOTSTRAP_DIR}" ]; then
  echo "Clonando Bootstrap ${BOOTSTRAP_VERSION}..."
  git config --global advice.detachedHead false
  git clone -q --depth 1 --branch "${BOOTSTRAP_VERSION}" https://github.com/twbs/bootstrap.git "${BOOTSTRAP_DIR}"
fi

mkdir -p "${WEB_BOOTSTRAP_DIR}"
cp -a "${BOOTSTRAP_DIR}/dist/"* "${WEB_BOOTSTRAP_DIR}/" || true

mkdir -p /run/php
echo "Arrancando apache-fpm"
php-fpm8.1 -F &
echo "Arrancando NGINX" & 
exec nginx -g 'daemon off;'
