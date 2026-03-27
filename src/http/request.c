#include "http/request.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void request_init(request_instance restrict req) {
    *req = (struct request) {
        .method = METHOD_UNKNOWN,
        .path = NULL,
        .version = NULL,
        .headers = NULL,
    };
}

void request_free(request_instance restrict req) {
    free(req->path);
    free(req->version);

    for (size_t i = 0; i < req->header_count; ++i) {
        free(req->headers[i].name);
        free(req->headers[i].value);
    }
    free(req->headers);
}

void request_add_header(request_instance restrict req, const char* name, const char* value) {
    if (req->header_count >= req->header_cap) {
        size_t new_cap = req->header_cap ? req->header_cap * 2 : 16;
        header_instance new_h = realloc(req->headers, new_cap * sizeof(struct request));
        if (!new_h) return;
        req->headers = new_h;
        req->header_cap = new_cap;
    }
    header_instance h = &req->headers[req->header_count++];
    h->name = strdup(name);
    h->value = strdup(value);
}

const char* request_get_header(const request_instance restrict req, const char* restrict name) {
    for (size_t i = 0; i < req->header_count; ++i) {
        if (strcasecmp(req->headers[i].name, name) == 0) return req->headers[i].value;
    }
    return NULL;
}


