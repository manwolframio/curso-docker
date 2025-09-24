# Primer stage: Constructor
FROM ubuntu:22.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive
RUN \
    apt-get update && apt-get upgrade -y \
    && apt-get install -y \
    build-essential make \
    && rm -rf /var/lib/apt/lists/*

COPY ./source /app
WORKDIR /app
RUN make BIN=echo_server

# Segundo stage: Ejecucion
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /app
COPY --from=builder /app/echo_server .
ENTRYPOINT ["./echo_server"]
CMD ["--port", "8000"]
