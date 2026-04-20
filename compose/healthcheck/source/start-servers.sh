#!/bin/sh
set -eu

: "${WEB1_DIR:?Falta definir WEB1_DIR}"
: "${WEB2_DIR:?Falta definir WEB2_DIR}"
: "${WEB1_PORT:?Falta definir WEB1_PORT}"
: "${WEB2_PORT:?Falta definir WEB2_PORT}"

export TZ="${TZ:-Europe/Madrid}"
export FLASK_ENV="${FLASK_ENV:-production}"

flask --app "${WEB2_DIR}/app.py" run --host=0.0.0.0 --port="${WEB2_PORT}" &
pid_web2=$!

echo "web2 arrancada desde ${WEB2_DIR} en el puerto ${WEB2_PORT} con PID: ${pid_web2}"

exec flask --app "${WEB1_DIR}/app.py" run --host=0.0.0.0 --port="${WEB1_PORT}"