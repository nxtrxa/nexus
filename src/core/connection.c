#include "core/connection.h"
#include "router/router.h"

#include <errno.h>

static void conn_close(connection_instance  conn) {
    close(conn->fd);
    free(conn);
}

static void conn_write(connection_instance  conn, fd_t epfd) {
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

static void conn_process(connection_instance conn, fd_t epfd) {
    char* end_headers = strstr(conn->rbuff, "\r\n\r\n");
    if (end_headers == NULL) {
        return;
    }

    router_dispatcher(conn);
    conn_write(conn, epfd);
}

connection_instance conn_init(fd_t fd) {
    connection_instance conn = calloc(1, sizeof(struct connection));
    if (!conn) {
        perror("calloc");
        close(fd);
        return NULL;
    }
    *conn = (struct connection) {
        .fd = fd,
        .state = READING_HEADERS,
        .keep_alive = false,
    };
    return conn;
}

void conn_event_handler(connection_instance conn, uint32_t events, fd_t epfd) {
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
            // EOF – client closed struct connection
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
