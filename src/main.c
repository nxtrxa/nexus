#include "core/connection.h"
#include "core/eventloop.h"

int main(int argc, char** argv) {
    if (argc != 3 || strcmp(argv[1], "serve") != 0) {
        fprintf(stderr, "Usage: %s serve <port>\n", argv[0]);
        return 1;
    }

    port_t port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port: %d\n", port);
        return 1;
    }

    int epfd = epoll_create1(0);
    Server* s = server_init(epfd, argv[2]);

    eventloop(s);

    close(s->fd);
    close(s->epfd);
    return 0;
}
