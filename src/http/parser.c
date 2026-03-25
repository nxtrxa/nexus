#include "http/parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int parse_method(const char **p, const char *end, HttpMethod *method) {
    const char *start = *p;
    while (*p < end && **p != ' ' && **p != '\r' && **p != '\n') (*p)++;
    size_t len = *p - start;
    if      (len == 3 && strncmp(start, "GET", 3)     == 0) *method = METHOD_GET;
    else if (len == 4 && strncmp(start, "POST", 4)    == 0) *method = METHOD_POST;
    else if (len == 3 && strncmp(start, "PUT", 3)     == 0) *method = METHOD_PUT;
    else if (len == 6 && strncmp(start, "DELETE", 6)  == 0) *method = METHOD_DELETE;
    else if (len == 4 && strncmp(start, "HEAD", 4)    == 0) *method = METHOD_HEAD;
    else if (len == 7 && strncmp(start, "OPTIONS", 7) == 0) *method = METHOD_OPTIONS;
    else *method = METHOD_UNKNOWN;
    return 0;
}

static char *copy_string(const char *start, size_t len) {
    char *s = malloc(len + 1);
    if (s) {
        memcpy(s, start, len);
        s[len] = '\0';
    }
    return s;
}

static int parse_path(const char **p, const char *end, char **path) {
    if (**p != ' ') return -1;
    (*p)++;
    const char *start = *p;
    while (*p < end && **p != ' ' && **p != '\r' && **p != '\n') (*p)++;
    size_t len = *p - start;
    *path = copy_string(start, len);
    return (*path == NULL) ? -1 : 0;
}

static int parse_version(const char **p, const char *end, char **version) {
    if (**p != ' ') return -1;
    (*p)++;
    const char *start = *p;
    while (*p < end && **p != '\r' && **p != '\n') (*p)++;
    size_t len = *p - start;
    *version = copy_string(start, len);
    return (*version == NULL) ? -1 : 0;
}

static int parse_header_line(const char **p, const char *end, Request *req) {
    const char *line_start = *p;
    const char *colon = NULL;
    while (*p < end && **p != ':' && **p != '\r' && **p != '\n') (*p)++;
    if (*p >= end || **p != ':') return -1;
    colon = *p;
    size_t name_len = colon - line_start;
    (*p)++; // skip ':'
    while (*p < end && (**p == ' ' || **p == '\t')) (*p)++;
    const char *value_start = *p;
    while (*p < end && **p != '\r' && **p != '\n') (*p)++;
    size_t value_len = *p - value_start;
    char *name = copy_string(line_start, name_len);
    char *value = copy_string(value_start, value_len);
    if (!name || !value) {
        free(name); free(value);
        return -1;
    }
    // Add to request
    if (req->header_count >= req->header_cap) {
        size_t new_cap = req->header_cap ? req->header_cap * 2 : 8;
        Header *new_h = realloc(req->headers, new_cap * sizeof(Header));
        if (!new_h) {
            free(name); free(value);
            return -1;
        }
        req->headers = new_h;
        req->header_cap = new_cap;
    }
    Header *h = &req->headers[req->header_count++];
    h->name = name;
    h->value = value;

    if (*p < end && **p == '\r') (*p)++;
    if (*p < end && **p == '\n') (*p)++;
    else return -1;
    return 0;
}

int parse_request(Request *req, const char *data, size_t len) {
    const char *p = data;
    const char *end = data + len;

    // Reset request (but not freeing existing? We assume req is already initialized)
    // Actually we should free previous data if any. We'll require caller to call request_free first.
    request_init(req); // but that will leak previous if not freed. Better to have caller call request_free.
    // For now, we'll just allocate new.

    if (parse_method(&p, end, &req->method) < 0) return -1;
    if (parse_path(&p, end, &req->path) < 0) return -1;
    if (parse_version(&p, end, &req->version) < 0) return -1;
    if (p < end && *p == '\r') p++;
    if (p < end && *p == '\n') p++;
    else return -1;

    while (p < end) {
        if (*p == '\r') {
            p++;
            if (p < end && *p == '\n') { p++; break; }
            else return -1;
        }
        if (*p == '\n') { p++; break; }
        if (parse_header_line(&p, end, req) < 0) return -1;
    }
    if (p == end) return 1; // incomplete
    return 0;
}
