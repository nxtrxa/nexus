#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>

#include "http/request.h"

#define conn_reset(conn) do {        \
    (conn)->wlen = 0;                \
    (conn)->rlen = 0;                \
    (conn)->wsent = 0;               \
    (conn)->state = READING_HEADERS; \
    request_init(&(conn)->req);  \
} while(0)

constexpr ssize_t WRITE_BUFFER_SIZE = 4 * 1024;
constexpr ssize_t READ_BUFFER_SIZE = 4 * 1024;

typedef enum : uint8_t {
    READING_HEADERS,
    READING_BODY,
    WRITING,
} ConnState;

typedef struct {
    char wbuff[WRITE_BUFFER_SIZE];
    char rbuff[READ_BUFFER_SIZE];
    ConnState state;
    size_t wlen;
    size_t rlen;
    size_t wsent;
    fd_t fd;
    bool keep_alive;
    Request req;
} Connection;

Connection* conn_init(fd_t);
void conn_event_handler(Connection*, uint32_t, fd_t);

#endif // CONNECTION_H

