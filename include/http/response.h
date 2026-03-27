#ifndef RESPONSE_H
#define RESPONSE_H

#include <stddef.h>

typedef struct {
    char* name;
    char* value;
} ResponseHeader;

typedef struct {
    ResponseHeader* headers;
    size_t header_cap;
    size_t header_count;
    char* body;
    int status_code;
    const char* status_text;
    size_t body_len;
} Response;

void response_init(Response* res);
void response_free(Response* res);
void response_set_status(Response* res, int code, const char* text);
void response_add_header(Response* res, const char* key, const char* value);
void response_set_body(Response* res, const char* body, size_t len);
char* response_build(const Response* res);

#endif // RESPONSE_H
