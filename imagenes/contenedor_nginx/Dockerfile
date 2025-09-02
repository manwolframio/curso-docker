FROM ubuntu:22.04

LABEL vendor="Nicolas Manso"
LABEL version="1.0.stable"

RUN apt-get update
RUN apt-get install -y --no-install-recommends nginx libcap2-bin ca-certificates
RUN rm -rf /var/lib/apt/lists/*

RUN groupadd -r webgroup
RUN useradd -m -s /usr/sbin/nologin -g webgroup webuser

RUN setcap 'cap_net_bind_service=+ep' /usr/sbin/nginx

RUN mkdir -p /var/lib/nginx/body
RUN mkdir -p /var/lib/nginx/fastcgi
RUN mkdir -p /var/lib/nginx/proxy
RUN mkdir -p /var/lib/nginx/scgi
RUN mkdir -p /var/lib/nginx/uwsgi
RUN mkdir -p /var/log/nginx
RUN mkdir -p /run/nginx
RUN mkdir -p /run
RUN touch /run/nginx.pid

RUN chgrp -R webgroup /var/lib/nginx /var/log/nginx /run
RUN chmod -R 770 /var/lib/nginx /var/log/nginx /run

COPY nginx.conf /etc/nginx/nginx.conf
COPY site/ /var/www/site/

COPY entrypoint.sh /entrypoint.sh
RUN chgrp -R webgroup /entrypoint.sh
RUN chmod +x /entrypoint.sh

RUN chown -R webuser:webgroup /var/www/site

USER webuser
ENTRYPOINT ["/entrypoint.sh"]
CMD ["nginx", "-g", "daemon off;"]