#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "client_handler.h"

#define BUFFER_SIZE 1024

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    printf("[INFO] Cliente conectado. FD=%d\n", client_fd);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("[INFO] Cliente desconectado. FD=%d\n", client_fd);
            } else {
                perror("[ERROR] recv");
            }
            break;
        }

        printf("[RECV FD=%d] %s\n", client_fd, buffer);

        if (send(client_fd, buffer, bytes_received, 0) < 0) {
            perror("[ERROR] send");
            break;
        }
    }

    close(client_fd);
}
