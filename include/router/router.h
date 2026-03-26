#ifndef ROUTER_H
#define ROUTER_H

#include "core/connection.h"

#include <sys/socket.h>
#include <unistd.h>

#define home "./static/home.nex"
#define auth "./static/auth.nex"


#define ROUTES  \
    X(HOME)     \
    X(AUTH)     \

#define X(I) I,
enum { ROUTES };
#undef X

typedef const char* route_t;

#define router_instance struct router*
struct router {
    route_t (*router_dispatcher_t)(router_instance);
    route_t routes[];
};

router_instance router_init();
route_t router_dispatcher(route_t);
void dispatch_home(connection_instance conn);


#endif // ROUTER_H

