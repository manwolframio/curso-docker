#!/bin/bash
set -eu

openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout apache-container.key -out apache-container.crt
