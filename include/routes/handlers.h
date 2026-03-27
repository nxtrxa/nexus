#ifndef HANDLERS_H
#define HANDLERS_H

#include "http/request.h"
#include "http/response.h"

int handle_index(const Request* req, Response* res);
int handle_health(const Request* req, Response* res);
int handle_not_found(const Request* req, Response* res);

#endif // HANDLERS_H
