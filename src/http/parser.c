#include "http/parser.h"

#include <stdlib.h>
#include <string.h>

static int parse_method(const char **p, const char *end, enum http_method *method) {
    const char *start = *p;
    while (*p < end && **p != ' ' && **p != '\r' && **p != '\n') {
        (*p)++;
    }

    size_t len = (size_t)(*p - start);
    if      (len == 3 && strncmp(start, "GET", 3) == 0) *method = METHOD_GET;
    else if (len == 4 && strncmp(start, "POST", 4) == 0) *method = METHOD_POST;
    else if (len == 3 && strncmp(start, "PUT", 3) == 0) *method = METHOD_PUT;
    else if (len == 6 && strncmp(start, "DELETE", 6) == 0) *method = METHOD_DELETE;
    else if (len == 4 && strncmp(start, "HEAD", 4) == 0) *method = METHOD_HEAD;
    else if (len == 7 && strncmp(start, "OPTIONS", 7) == 0) *method = METHOD_OPTIONS;
    else if (len == 7 && strncmp(start, "CONNECT", 7) == 0) *method = METHOD_CONNECT;
    else if (len == 5 && strncmp(start, "PATCH", 5) == 0) *method = METHOD_PATCH;
    else if (len == 5 && strncmp(start, "TRACE", 5) == 0) *method = METHOD_TRACE;
    else *method = METHOD_UNKNOWN;

    return 0;
}

static char *copy_string(const char *start, size_t len) {
    char *s = malloc(len + 1);
    if (!s) {
        return NULL;
    }

    memcpy(s, start, len);
    s[len] = '\0';
    return s;
}

static int parse_path(const char **p, const char *end, char **path) {
    if (*p >= end || **p != ' ') {
        return -1;
    }

    (*p)++;
    const char *start = *p;
    while (*p < end && **p != ' ' && **p != '\r' && **p != '\n') {
        (*p)++;
    }

    size_t len = (size_t)(*p - start);
    *path = copy_string(start, len);
    return (*path == NULL) ? -1 : 0;
}

static int parse_version(const char **p, const char *end, char **version) {
    if (*p >= end || **p != ' ') {
        return -1;
    }

    (*p)++;
    const char *start = *p;
    while (*p < end && **p != '\r' && **p != '\n') {
        (*p)++;
    }

    size_t len = (size_t)(*p - start);
    *version = copy_string(start, len);
    return (*version == NULL) ? -1 : 0;
}

static int parse_header_line(const char **p, const char *end, request_instance req) {
    const char *line_start = *p;
    while (*p < end && **p != ':' && **p != '\r' && **p != '\n') {
        (*p)++;
    }

    if (*p >= end || **p != ':') {
        return -1;
    }

    const char *colon = *p;
    size_t name_len = (size_t)(colon - line_start);

    (*p)++; // skip ':'
    while (*p < end && (**p == ' ' || **p == '\t')) {
        (*p)++;
    }

    const char *value_start = *p;
    while (*p < end && **p != '\r' && **p != '\n') {
        (*p)++;
    }

    size_t value_len = (size_t)(*p - value_start);
    char *name = copy_string(line_start, name_len);
    char *value = copy_string(value_start, value_len);
    if (!name || !value) {
        free(name);
        free(value);
        return -1;
    }

    request_add_header(req, name, value);
    free(name);
    free(value);

    if (*p < end && **p == '\r') {
        (*p)++;
    }

    if (*p < end && **p == '\n') {
        (*p)++;
        return 0;
    }

    return -1;
}

int parse_request(request_instance req, const char *data, size_t len) {
    const char *p = data;
    const char *end = data + len;

    if (parse_method(&p, end, &req->method) < 0) return -1;
    if (parse_path(&p, end, &req->path) < 0) return -1;
    if (parse_version(&p, end, &req->version) < 0) return -1;

    if (p < end && *p == '\r') {
        p++;
    }
    if (p < end && *p == '\n') {
        p++;
    } else {
        return -1;
    }

    while (p < end) {
        if (*p == '\r') {
            p++;
            if (p < end && *p == '\n') {
                return 0;
            }
            return -1;
        }

        if (*p == '\n') {
            return 0;
        }

        if (parse_header_line(&p, end, req) < 0) {
            return -1;
        }
    }

    return 1;
}
