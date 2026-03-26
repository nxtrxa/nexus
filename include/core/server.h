#ifndef SERVER_H
#define SERVER_H

#include "utils/types.h"
#include <netinet/in.h>

enum { SERVER_BACKLOG = SOMAXCONN };

#define server_instance struct server*
struct server {
    struct sockaddr_in addr;
    socklen_t addr_len;
    fd_t epfd;
    port_t port;
    fd_t fd;
};

server_instance server_init(int, const char*);
void server_destroy(server_instance);
void server_handler(server_instance restrict);

#endif // SERVER_H
