#ifndef SERVER_H
#define SERVER_H

#include "utils/types.h"
#include <netinet/in.h>

enum { SERVER_BACKLOG = SOMAXCONN };

typedef struct {
    struct sockaddr_in addr;
    socklen_t addr_len;
    fd_t epfd;
    port_t port;
    fd_t fd;
} Server;

Server* server_init(int, const char*);
void server_destroy(Server*);
void server_cli_handler(Server* restrict);

#endif // SERVER_H
