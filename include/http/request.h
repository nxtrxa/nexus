#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>
#include <stdint.h>
#include "utils/types.h"

#define HTTP_METHODS    \
    X(METHOD_GET)       \
    X(METHOD_POST)      \
    X(METHOD_PUT)       \
    X(METHOD_DELETE)    \
    X(METHOD_HEAD)      \
    X(METHOD_OPTIONS)   \
    X(METHOD_UNKNOWN)   \
    X(METHOD_PATCH)     \
    X(METHOD_TRACE)     \
    X(METHOD_CONNECT)

#define X(I) I,
enum http_method {
    HTTP_METHODS
};
#undef X

__STRUCT (header) {
    char* name;
    char* value;
};

__STRUCT (request) {
    char* path;
    char* version;
    header_instance headers;
    size_t header_cap;
    size_t header_count;
    enum http_method method;
};

void request_init(request_instance req);
void request_free(request_instance req);
void request_add_header(request_instance req, const char* name, const char* value);
http_request_t request_get_header(const request_instance req, http_request_t name);

#endif // REQUEST_H
