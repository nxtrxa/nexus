#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "core/server.h"
#include <sys/epoll.h>

enum { MAX_EVENTS = 64 };

#define eventloop_instance
struct eventloop {
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
};

void eventloop(server_instance);

#endif // EVENTLOOP_H
