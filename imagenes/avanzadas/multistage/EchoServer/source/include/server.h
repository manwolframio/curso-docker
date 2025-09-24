#ifndef SERVER_H
#define SERVER_H

#include "queue.h"

#define DEFAULT_PORT 8000
#define BACKLOG 10
#define THREAD_POOL_SIZE 4

void start_server(int port);

#endif
