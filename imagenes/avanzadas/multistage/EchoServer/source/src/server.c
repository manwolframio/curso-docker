#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "server.h"
#include "client_handler.h"

static client_queue_t client_queue;

/* Función que ejecuta cada thread del pool */
static void *worker_thread(void *arg) {
    (void)arg;
    while (1) {
        int client_fd = queue_pop(&client_queue);
        handle_client(client_fd);
    }
    return NULL;
}

void start_server(int port) {
    int server_fd;
    struct sockaddr_in server_addr;

    /* Inicializar cola */
    queue_init(&client_queue);

    /* Crear pool de threads */
    pthread_t pool[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (pthread_create(&pool[i], NULL, worker_thread, NULL) != 0) {
            perror("[ERROR] pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Crear socket TCP */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] socket");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* Configurar dirección */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    /* Enlazar */
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Escuchar */
    if (listen(server_fd, BACKLOG) < 0) {
        perror("[ERROR] listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Servidor TCP escuchando en el puerto %d...\n", port);

    /* Aceptar clientes y ponerlos en la cola */
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("[ERROR] accept");
            continue;
        }

        printf("[INFO] Nueva conexión desde %s:%d -> FD=%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);

        queue_push(&client_queue, client_fd);
    }

    close(server_fd);
}
