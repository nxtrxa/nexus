#ifndef ROUTES_H
#define ROUTES_H

#include "http/request.h"
#include "http/response.h"

typedef int (*RouteHandler)(const Request* req, Response* res);

typedef struct {
    HttpMethod method;
    const char* path;
    RouteHandler handler;
} Route;

int router_dispatch(const Request* req, Response* res);

#endif // ROUTES_H
