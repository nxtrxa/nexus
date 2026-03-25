#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>
#include "utils/attributes.h"
#include "utils/types.h"

#define HTTP_METHODS    \
    X(METHOD_GET)       \
    X(METHOD_POST)      \
    X(METHOD_PUT)       \
    X(METHOD_DELETE)    \
    X(METHOD_HEAD)      \
    X(METHOD_OPTIONS)   \
    X(METHOD_UNKNOWN)   \

#define X(I) I,
typedef enum : uint8_t {
    HTTP_METHODS
} HttpMethod;
#undef X

typedef struct {
    char* name;
    char* value;
} Header;

typedef struct {
    char* path;
    char* version;
    Header* headers;
    size_t header_cap;
    size_t header_count;
    HttpMethod method;
} Request;

void request_init(Request *req);
void request_free(Request *req);
void request_add_header(Request* req, const char* name, const char* value);
http_request_t request_get_header(const Request* req, http_request_t name);

#endif // REQUEST_H
