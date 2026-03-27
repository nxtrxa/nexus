#include "http/response.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int ensure_capacity(char** buf, size_t* cap, size_t need_len) {
    if (need_len <= *cap) {
        return 0;
    }

    size_t new_cap = *cap;
    while (new_cap < need_len) {
        new_cap *= 2;
    }

    char* new_buf = realloc(*buf, new_cap);
    if (!new_buf) {
        return -1;
    }

    *buf = new_buf;
    *cap = new_cap;
    return 0;
}

void response_init(Response* res) {
    *res = (Response) {
        .status_code = 200,
        .status_text = "OK",
        .headers = NULL,
        .body = NULL,
    };
}

void response_free(Response* res) {
    for (size_t i = 0; i < res->header_count; ++i) {
        free(res->headers[i].name);
        free(res->headers[i].value);
    }
    free(res->headers);
    free(res->body);
}

void response_set_status(Response* res, int code, const char* text) {
    res->status_code = code;
    res->status_text = text;
}

void response_add_header(Response *res, const char *key, const char *value) {
    if (res->header_count >= res->header_cap) {
        size_t new_cap = res->header_cap ? res->header_cap * 2 : 8;
        ResponseHeader *new_h = realloc(res->headers, new_cap * sizeof(ResponseHeader));
        if (!new_h) return;
        res->headers = new_h;
        res->header_cap = new_cap;
    }
    ResponseHeader *h = &res->headers[res->header_count++];
    h->name = strdup(key);
    h->value = strdup(value);
}

void response_set_body(Response *res, const char *body, size_t len) {
    free(res->body);
    res->body = malloc(len + 1);
    if (res->body) {
        memcpy(res->body, body, len);
        res->body[len] = '\0';
        res->body_len = len;
    } else {
        res->body_len = 0;
    }
}

char *response_build(const Response *res) {
    size_t size = 256;
    char *buf = malloc(size);
    if (!buf) return NULL;
    size_t offset = 0;
    int needed;

    needed = snprintf(buf + offset, size - offset,
                      "HTTP/1.1 %d %s\r\n", res->status_code, res->status_text);
    if (needed < 0) { free(buf); return NULL; }
    if (ensure_capacity(&buf, &size, offset + (size_t)needed + 1) < 0) { free(buf); return NULL; }
    needed = snprintf(buf + offset, size - offset,
                      "HTTP/1.1 %d %s\r\n", res->status_code, res->status_text);
    if (needed < 0) { free(buf); return NULL; }
    offset += (size_t)needed;

    for (size_t i = 0; i < res->header_count; i++) {
        if (ensure_capacity(&buf, &size, offset + strlen(res->headers[i].name) + strlen(res->headers[i].value) + 6) < 0) {
            free(buf);
            return NULL;
        }
        needed = snprintf(buf + offset, size - offset, "%s: %s\r\n",
                          res->headers[i].name, res->headers[i].value);
        if (needed < 0) { free(buf); return NULL; }
        offset += (size_t)needed;
    }

    if (res->body) {
        if (ensure_capacity(&buf, &size, offset + 64) < 0) { free(buf); return NULL; }
        needed = snprintf(buf + offset, size - offset, "Content-Length: %zu\r\n", res->body_len);
        if (needed < 0) { free(buf); return NULL; }
        offset += (size_t)needed;
    }

    if (ensure_capacity(&buf, &size, offset + 3) < 0) { free(buf); return NULL; }
    needed = snprintf(buf + offset, size - offset, "\r\n");
    if (needed < 0) { free(buf); return NULL; }
    offset += (size_t)needed;

    if (res->body && res->body_len > 0) {
        if (ensure_capacity(&buf, &size, offset + res->body_len + 1) < 0) { free(buf); return NULL; }
        memcpy(buf + offset, res->body, res->body_len);
        offset += res->body_len;
    }

    buf[offset] = '\0';
    return buf;
}
