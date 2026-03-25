#include "core/connection.h"

static void conn_close(Connection* restrict conn) {
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
    if (end_headers == nullptr) {
        return;
    }


    http_res_t res =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 13\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "Hello, World!";

    size_t res_len = strlen(res);
    if (res_len > sizeof conn->wbuff) {
        res_len = sizeof conn->wbuff;
    }

    memcpy(conn->wbuff, res, res_len);
    conn->wlen = res_len;
    conn->wsent = 0;
    conn->state = WRITING;
    conn->keep_alive = true;
    conn_write(conn, epfd);
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
        .state = WRITING,
        .keep_alive = false,
    };
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
