#include "core/connection.h"
#include "http/parser.h"
#include "http/response.h"
#include "routes/routes.h"

static void conn_write(Connection* restrict conn, fd_t epfd);

static void conn_build_and_queue_response(Connection* restrict conn, fd_t epfd, Response* res) {
    char* raw = response_build(res);
    if (!raw) {
        static const char fallback[] =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Length: 21\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Internal Server Error";

        size_t fallback_len = sizeof(fallback) - 1;
        memcpy(conn->wbuff, fallback, fallback_len);
        conn->wlen = fallback_len;
        conn->keep_alive = false;
        conn->state = WRITING;
        return;
    }

    size_t res_len = strlen(raw);
    if (res_len > sizeof conn->wbuff) {
        res_len = sizeof conn->wbuff;
        conn->keep_alive = false;
    }

    memcpy(conn->wbuff, raw, res_len);
    conn->wlen = res_len;
    conn->wsent = 0;
    conn->state = WRITING;
    free(raw);

    conn_write(conn, epfd);
}

static void conn_close(Connection* restrict conn) {
    request_free(&conn->req);
    close(conn->fd);
    free(conn);
}

static void conn_write(Connection* restrict conn, fd_t epfd) {
    while (conn->wsent < conn->wlen) {
        ssize_t n = write(conn->fd, conn->wbuff + conn->wsent, conn->wlen - conn->wsent);
        if (n > 0) {
            conn->wsent += n;
        } else if (n == 0)  break;
          else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                struct epoll_event ev = {
                    .events = EPOLLIN | EPOLLOUT | EPOLLRDHUP,
                    .data.ptr = conn
                };

                epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &ev);
                return;
            } else {
                perror("write");
                conn_close(conn);
                return;
            }
        }
    }

    if (conn->wsent == conn->wlen) {
        if (conn->keep_alive) {
            conn_reset(conn);
            struct epoll_event ev = {
                .events = EPOLLIN | EPOLLRDHUP,
                .data.ptr = conn
            };
            epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &ev);
        } else {
            conn_close(conn);
        }
    }
}

static void conn_process(Connection* restrict conn, fd_t epfd) {
    char* end_headers = strstr(conn->rbuff, "\r\n\r\n");
    if (end_headers == NULL) {
        return;
    }

    request_free(&conn->req);
    request_init(&conn->req);

    int parsed = parse_request(&conn->req, conn->rbuff, conn->rlen);
    if (parsed != 0) {
        static const char bad_request[] =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 11\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Bad Request";

        memcpy(conn->wbuff, bad_request, sizeof(bad_request) - 1);
        conn->wlen = sizeof(bad_request) - 1;
        conn->wsent = 0;
        conn->state = WRITING;
        conn->keep_alive = false;
        conn_write(conn, epfd);
        return;
    }

    Response res;
    response_init(&res);

    const char* conn_header = request_get_header(&conn->req, "Connection");
    conn->keep_alive = conn_header && strcasecmp(conn_header, "keep-alive") == 0;
    response_add_header(&res, "Connection", conn->keep_alive ? "keep-alive" : "close");

    if (router_dispatch(&conn->req, &res) != 0) {
        response_free(&res);
        response_init(&res);
        response_set_status(&res, 500, "Internal Server Error");
        response_add_header(&res, "Connection", "close");
        response_add_header(&res, "Content-Type", "text/plain; charset=utf-8");
        response_set_body(&res, "Internal Server Error", strlen("Internal Server Error"));
        conn->keep_alive = false;
    }

    conn_build_and_queue_response(conn, epfd, &res);
    response_free(&res);
}

Connection* conn_init(fd_t fd) {
    Connection* conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("calloc");
        close(fd);
        return NULL;
    }
    *conn = (Connection) {
        .fd = fd,
        .state = READING_HEADERS,
        .keep_alive = false,
    };

    request_init(&conn->req);
    return conn;
}

void conn_event_handler(Connection* restrict conn, uint32_t events, fd_t epfd) {
    if (events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
        conn_close(conn);
        return;
    }

    if (events & EPOLLIN) {
        ssize_t n = read(conn->fd,
                conn->rbuff + conn->rlen,
                sizeof conn->rbuff - conn->rlen - 1);
        if (n > 0) {
            conn->rlen += n;
            conn->rbuff[conn->rlen] = '\0';
            conn_process(conn, epfd);
        } else if (n == 0) {
            // EOF – client closed Connection
            conn_close(conn);
            return;
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                conn_close(conn);
                return;
            }
        }
    }

    if (events & EPOLLOUT) {
        conn_write(conn, epfd);
    }
}
