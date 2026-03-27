#include "core/eventloop.h"
#include "core/connection.h"

#include <errno.h>

void event_loop(server_instance __s) {
    struct event_loop el = {
        .ev = {
            .events = EPOLLIN,
            .data.fd = __s->fd,
        },
    };

    if (epoll_ctl(__s->epfd, EPOLL_CTL_ADD, __s->fd, &el.ev) < 0) {
        perror("epoll_ctl");
        close(__s->fd);
        close(__s->epfd);
        return;
    }

    printf("server listening on port %d\n", __s->port);

    for (;;) {
        int nfds = epoll_wait(__s->epfd, el.events, MAX_EVENTS, -1);
        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("epoll_wait");
            break;
        }
        for (int i = 0; i < nfds; ++i) {
            if (el.events[i].data.fd == __s->fd) {
                server_handler(__s);
            } else {
                struct connection *conn = el.events[i].data.ptr;
                conn_event_handler(conn, el.events[i].events, __s->epfd);
            }
        }
    }
}
