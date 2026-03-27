#include "routes/handlers.h"

#include <string.h>

int handle_index(const Request* req, Response* res) {
    (void)req;

    static const char body[] =
        "<html><body><h1>Nexus</h1><p>Router is online.</p></body></html>";

    response_set_status(res, 200, "OK");
    response_add_header(res, "Content-Type", "text/html; charset=utf-8");
    response_set_body(res, body, sizeof(body) - 1);
    return 0;
}

int handle_health(const Request* req, Response* res) {
    (void)req;

    static const char body[] = "ok";

    response_set_status(res, 200, "OK");
    response_add_header(res, "Content-Type", "text/plain; charset=utf-8");
    response_set_body(res, body, strlen(body));
    return 0;
}

int handle_not_found(const Request* req, Response* res) {
    (void)req;

    static const char body[] = "Not Found";

    response_set_status(res, 404, "Not Found");
    response_add_header(res, "Content-Type", "text/plain; charset=utf-8");
    response_set_body(res, body, sizeof(body) - 1);
    return 0;
}
