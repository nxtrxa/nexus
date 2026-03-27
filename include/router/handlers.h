#ifndef HANDLERS_H
#define HANDLERS_H

#include "http/request.h"
#include "core/connection.h"

#define HTTP_HANDLERS    \
    X(GET)               \
    X(POST)              \
    X(PUT)               \
    X(DELETE)            \
    X(HEAD)              \
    X(OPTIONS)           \
    X(CONNECT)           \
    X(PATCH)             \
    X(TRACE)             \
    X(UNKNOWN)

__STRUCT (http_handler) {
    // TO IMPLEMENT
};

#define X(M) void M##_handler(connection_instance conn);
HTTP_HANDLERS
#undef X

#endif // HANDLERS_H
