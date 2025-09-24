#include "queue.h"

void queue_init(client_queue_t *q) {
    q->front = q->rear = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void queue_push(client_queue_t *q, int client_fd) {
    pthread_mutex_lock(&q->lock);

    while (q->count == QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    q->clients[q->rear] = client_fd;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

int queue_pop(client_queue_t *q) {
    pthread_mutex_lock(&q->lock);

    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    int client_fd = q->clients[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);

    return client_fd;
}
