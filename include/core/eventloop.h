#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "core/server.h"
#include <sys/epoll.h>

enum { MAX_EVENTS = 64 };

typedef struct {
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
} EventLoop;

void eventloop(Server*);

#endif // EVENTLOOP_H
