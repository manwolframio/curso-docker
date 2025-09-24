#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "server.h"

static void print_usage(const char *prog_name) {
    printf("Uso: %s [--port PORT] o [-p PORT]\n", prog_name);
    printf("Opciones:\n");
    printf("  -p, --port <PORT>   Especifica el puerto de escucha (por defecto 8000)\n");
    printf("  -h, --help          Muestra este mensaje y termina\n");
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;

    static struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "p:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                if (port <= 0 || port > 65535) {
                    fprintf(stderr, "[ERROR] Puerto inválido: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    start_server(port);
    return 0;
}
