#include "router/handlers.h"
#include "router/router.h"

#include <stdio.h>
#include <string.h>

static size_t set_response(connection_instance conn,
                           int status,
                           const char *status_text,
                           const char *content_type,
                           const char *body,
                           size_t body_len,
                           bool send_body) {
    int n = snprintf(conn->wbuff,
                     sizeof conn->wbuff,
                     "HTTP/1.1 %d %s\r\n"
                     "Content-Type: %s\r\n"
                     "Content-Length: %zu\r\n"
                     "Connection: keep-alive\r\n"
                     "\r\n",
                     status,
                     status_text,
                     content_type,
                     body_len);

    if (n < 0) {
        conn->wlen = 0;
        return 0;
    }

    size_t header_len = (size_t)n;
    if (!send_body || body_len == 0) {
        conn->wlen = header_len;
        return header_len;
    }

    size_t max_body = sizeof(conn->wbuff) - header_len;
    if (body_len > max_body) {
        conn->wlen = 0;
        return 0;
    }

    memcpy(conn->wbuff + header_len, body, body_len);
    conn->wlen = header_len + body_len;
    return conn->wlen;
}

static bool is_head_request(const connection_instance conn) {
    return conn->req.method == METHOD_HEAD;
}

static int map_route(const char *path) {
    if (!path) {
        return -1;
    }
    if (strcmp(path, "/") == 0 || strcmp(path, "/home") == 0) {
        return HOME;
    }
    if (strcmp(path, "/auth") == 0) {
        return AUTH;
    }
    return -1;
}

static bool load_route_body(int route, char *out, size_t out_cap, size_t *out_len) {
    const char *file = NULL;
    switch (route) {
        case HOME: file = __HOME__; break;
        case AUTH: file = __AUTH__; break;
        default: return false;
    }

    FILE *fp = fopen(file, "rb");
    if (!fp) {
        return false;
    }

    size_t nread = fread(out, 1, out_cap, fp);
    bool ok = !ferror(fp);
    if (ok && !feof(fp)) {
        ok = false;
    }
    fclose(fp);

    if (!ok) {
        return false;
    }

    *out_len = nread;
    return true;
}

static void finalize_response(connection_instance conn) {
    conn->state = WRITING;
    conn->keep_alive = true;
    conn->wsent = 0;
}

static void method_not_allowed(connection_instance conn) {
    static const char body[] = "Method Not Allowed\n";
    set_response(conn,
                 405,
                 "Method Not Allowed",
                 "text/plain; charset=utf-8",
                 body,
                 sizeof(body) - 1,
                 !is_head_request(conn));
    finalize_response(conn);
}

static void not_found(connection_instance conn) {
    static const char body[] = "Not Found\n";
    set_response(conn,
                 404,
                 "Not Found",
                 "text/plain; charset=utf-8",
                 body,
                 sizeof(body) - 1,
                 !is_head_request(conn));
    finalize_response(conn);
}

static void internal_error(connection_instance conn) {
    static const char body[] = "Internal Server Error\n";
    set_response(conn,
                 500,
                 "Internal Server Error",
                 "text/plain; charset=utf-8",
                 body,
                 sizeof(body) - 1,
                 !is_head_request(conn));
    finalize_response(conn);
}

static void handle_get_like(connection_instance conn) {
    int route = map_route(conn->req.path);
    if (route < 0) {
        not_found(conn);
        return;
    }

    char body[sizeof(conn->wbuff)] = {0};
    size_t body_len = 0;
    if (!load_route_body(route, body, sizeof body, &body_len)) {
        internal_error(conn);
        return;
    }

    if (set_response(conn,
                     200,
                     "OK",
                     "text/html; charset=utf-8",
                     body,
                     body_len,
                     !is_head_request(conn)) == 0) {
        internal_error(conn);
        return;
    }

    finalize_response(conn);
}

void GET_handler(connection_instance conn) {
    handle_get_like(conn);
}

void POST_handler(connection_instance conn) {
    method_not_allowed(conn);
}

void PUT_handler(connection_instance conn) {
    method_not_allowed(conn);
}

void DELETE_handler(connection_instance conn) {
    method_not_allowed(conn);
}

void HEAD_handler(connection_instance conn) {
    handle_get_like(conn);
}

void OPTIONS_handler(connection_instance conn) {
    static const char body[] = "GET, HEAD\n";
    if (set_response(conn,
                     200,
                     "OK",
                     "text/plain; charset=utf-8",
                     body,
                     sizeof(body) - 1,
                     true) == 0) {
        internal_error(conn);
        return;
    }

    finalize_response(conn);
}

void CONNECT_handler(connection_instance conn) {
    method_not_allowed(conn);
}

void PATCH_handler(connection_instance conn) {
    method_not_allowed(conn);
}

void TRACE_handler(connection_instance conn) {
    method_not_allowed(conn);
}

void UNKNOWN_handler(connection_instance conn) {
    static const char body[] = "Bad Request\n";
    set_response(conn,
                 400,
                 "Bad Request",
                 "text/plain; charset=utf-8",
                 body,
                 sizeof(body) - 1,
                 true);
    finalize_response(conn);
}
