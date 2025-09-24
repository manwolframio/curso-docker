#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

#define QUEUE_SIZE 32

typedef struct {
    int clients[QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} client_queue_t;

void queue_init(client_queue_t *q);
void queue_push(client_queue_t *q, int client_fd);
int queue_pop(client_queue_t *q);

#endif
