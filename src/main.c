#include "core/connection.h"
#include "core/eventloop.h"

#include <string.h>
#include <stdlib.h>

static int parse_port(int argc, char** argv, port_t* out_port) {
    if (argc < 3 || strcmp(argv[1], "serve") != 0) {
        return -1;
    }

    if (argc == 3) {
        *out_port = atoi(argv[2]);
        return 0;
    }

    if (argc == 4 && strcmp(argv[2], "-p") == 0) {
        *out_port = atoi(argv[3]);
        return 0;
    }

    return -1;
}

int main(int argc, char** argv) {
    port_t port = 0;
    if (parse_port(argc, argv, &port) < 0) {
        fprintf(stderr, "Usage: %s serve -p <port>\n", argv[0]);
        fprintf(stderr, "   or: %s serve <port>\n", argv[0]);
        return 1;
    }

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port: %d\n", port);
        return 1;
    }

    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return 1;
    }

    char port_str[8] = {0};
    snprintf(port_str, sizeof port_str, "%d", port);
    server_instance s = server_init(epfd, port_str);
    if (!s) {
        close(epfd);
        return 1;
    }

    event_loop(s);

    server_destroy(s);
    return 0;
}
