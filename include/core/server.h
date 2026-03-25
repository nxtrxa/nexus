#ifndef SERVER_H
#define SERVER_H

#include "utils/types.h"
#include <netinet/in.h>

constexpr int EPOLL_SIZE = 1024;

typedef struct {
    struct sockaddr_in addr;
    socklen_t addr_len;
    fd_t epfd;
    port_t port;
    fd_t fd;
} Server;

Server* server_init(int, const char*);
void server_cli_handler(Server* restrict);

#endif // SERVER_H
