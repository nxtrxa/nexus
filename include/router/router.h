#ifndef ROUTER_H
#define ROUTER_H

#include "core/connection.h"

#include <sys/socket.h>
#include <unistd.h>

#define __HOME__ "./static/home.nex"
#define __AUTH__ "./static/auth.nex"

#define ROUTES  \
    X(HOME)     \
    X(AUTH)

#define DISPATCHERS ROUTES

#define router_constructor(__p)             \
    (router) {                              \
        .dispatcher = router_dispatcher,    \
        .route      = (__p),                \
    }

#define X(I) I,
enum route { ROUTES };
#undef X

typedef void (*router_dispatcher_t)(connection_instance);

__STRUCT (router) {
    router_dispatcher_t dispatcher;
    route_t route;
};

void router_dispatcher(connection_instance);

#endif // ROUTER_H

