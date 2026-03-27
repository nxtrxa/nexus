#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

#include "http/request.h"
#include "utils/types.h"
#include "utils/attributes.h"

#define conn_reset(conn) do {        \
    (conn)->wlen = 0;                \
    (conn)->rlen = 0;                \
    (conn)->wsent = 0;               \
    (conn)->state = READING_HEADERS; \
    request_init(&(conn)->req);      \
} while(0)

enum {
    WRITE_BUFFER_SIZE = 4 * 1024,
    READ_BUFFER_SIZE = 4 * 1024,
};

enum __packed__ conn_state {
    READING_HEADERS,
    READING_BODY,
    WRITING,
};

__STRUCT (connection) {
    char wbuff[WRITE_BUFFER_SIZE];
    char rbuff[READ_BUFFER_SIZE];
    enum conn_state state;
    size_t wlen;
    size_t rlen;
    size_t wsent;
    fd_t fd;
    bool keep_alive;
    struct request req;
};

connection_instance conn_init(fd_t);
void conn_event_handler(connection_instance, uint32_t, fd_t);

#endif // CONNECTION_H
