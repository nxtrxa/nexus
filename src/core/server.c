#include "core/server.h"
#include "core/connection.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

server_instance server_init(int epfd, const char* __p) {
    if (epfd < 0) {
        return NULL;
    }

    server_instance __s = calloc(1, sizeof *__s);
    if (!__s) {
        return NULL;
    }

    __s->port = atoi(__p);
    __s->epfd = epfd;
    __s->addr = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_port = htons(__s->port),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    if ((__s->fd = socket(__s->addr.sin_family, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        perror("socket");
        free(__s);
        return NULL;
    }

    int opt = 1;
    if (setsockopt(__s->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0) {
        perror("setsockopt");
        close(__s->fd);
        free(__s);
        return NULL;
    }

    if (bind(__s->fd, (struct sockaddr*)&__s->addr, sizeof __s->addr) < 0) {
        perror("bind");
        close(__s->fd);
        free(__s);
        return NULL;
    }

    if (listen(__s->fd, SERVER_BACKLOG) < 0) {
        perror("listen");
        close(__s->fd);
        free(__s);
        return NULL;
    }
    return __s;
}

void server_destroy(server_instance __s) {
    if (!__s) {
        return;
    }

    if (__s->fd >= 0) {
        close(__s->fd);
    }

    if (__s->epfd >= 0) {
        close(__s->epfd);
    }

    free(__s);
}

void server_handler(struct server *restrict __s) {
    for (;;) {
        struct sockaddr_in cli_addr;
        socklen_t addrlen = sizeof cli_addr;

        fd_t cli_fd = accept4(__s->fd, (struct sockaddr*)&cli_addr, &addrlen, SOCK_NONBLOCK);
        if (cli_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            perror("accept4");
            return;
        }

        struct connection* conn = conn_init(cli_fd);
        if (!conn) {
            close(cli_fd);
            continue;
        }

        struct epoll_event ev = {
            .events = EPOLLIN | EPOLLRDHUP,
            .data.ptr = conn,
        };

        if (epoll_ctl(__s->epfd, EPOLL_CTL_ADD, cli_fd, &ev) < 0) {
            perror("epoll_ctl");
            close(cli_fd);
            free(conn);
            continue;
        }

        char cli_ip[INET6_ADDRSTRLEN] = {0};
        inet_ntop(cli_addr.sin_family, &cli_addr.sin_addr, cli_ip, sizeof cli_ip);
        printf("new connection from %s:%hu\n", cli_ip, ntohs(cli_addr.sin_port));
    }
}

