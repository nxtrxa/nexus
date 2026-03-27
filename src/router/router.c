#include "routes/routes.h"
#include "routes/handlers.h"

#include <string.h>

static const Route ROUTES[] = {
    { METHOD_GET, "/", handle_index },
    { METHOD_GET, "/health", handle_health },
};

static int route_path_exists(const char* path) {
    for (size_t i = 0; i < sizeof(ROUTES) / sizeof(ROUTES[0]); ++i) {
        if (strcmp(ROUTES[i].path, path) == 0) {
            return 1;
        }
    }
    return 0;
}

int router_dispatch(const Request* req, Response* res) {
    for (size_t i = 0; i < sizeof(ROUTES) / sizeof(ROUTES[0]); ++i) {
        if (ROUTES[i].method == req->method && strcmp(ROUTES[i].path, req->path) == 0) {
            return ROUTES[i].handler(req, res);
        }
    }

    if (route_path_exists(req->path)) {
        response_set_status(res, 405, "Method Not Allowed");
        response_add_header(res, "Content-Type", "text/plain; charset=utf-8");
        response_set_body(res, "Method Not Allowed", strlen("Method Not Allowed"));
        return 0;
    }

    return handle_not_found(req, res);
}
