CC = gcc
CFLAGS = -Wall -pthread -Iinclude
SRC = src/main.c src/server.c src/queue.c src/client_handler.c
OBJ = $(SRC:.c=.o)

BIN ?= echo_server

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)
