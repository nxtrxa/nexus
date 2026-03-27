#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "core/server.h"
#include "utils/types.h"
#include <sys/epoll.h>

enum { MAX_EVENTS = 64 };

__STRUCT (event_loop) {
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
};

void event_loop(server_instance);

#endif // EVENTLOOP_H
