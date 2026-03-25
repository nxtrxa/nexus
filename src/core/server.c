#include "core/server.h"
#include "core/connection.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

Server* server_init(int epfd, const char* __p) {
    Server* __s = malloc(sizeof *__s);
    __s->port = atoi(__p);
    __s->epfd = epfd;
    __s->addr = (struct sockaddr_in) {
        .sin_family = AF_INET,
    };
    int opt   = 1;



    if ((__s->fd = socket(__s->addr.sin_family, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        perror("socket");
        return NULL;
    }

    setsockopt(__s->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    if (bind(__s->fd, (struct sockaddr*)&__s->addr, sizeof __s->addr) < 0) {
        perror("bind");
        close(__s->fd);
        return NULL;
    }

    if (listen(__s->fd, SOMAXCONN) < 0) {
        perror("listen");
        close(__s->fd);
        return NULL;
    }
    return __s;
}

void server_cli_handler(Server *restrict __s) {
    struct sockaddr_in cli_addr;
    socklen_t addrlen = sizeof cli_addr;

    fd_t cli_fd = accept(__s->fd, (struct sockaddr*)&cli_addr, &addrlen);
    if (cli_fd < 0) {
        perror("accept");
        return;
    }

    char cli_ip[INET6_ADDRSTRLEN];
    inet_ntop(cli_addr.sin_family, &cli_addr.sin_addr, cli_ip, sizeof cli_ip);


    printf("new connection from %s:%hu\n", cli_ip, ntohs(cli_addr.sin_port));

    Connection* conn = conn_init(cli_fd);

    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLRDHUP,
        .data.ptr = conn,
    };

    if (epoll_ctl(__s->epfd, EPOLL_CTL_ADD, cli_fd, &ev) < 0) {
        perror("epoll_ctl");
        close(cli_fd);
        free(conn);
        return;
    }
}


