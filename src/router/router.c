#include "router/router.h"
#include "router/handlers.h"

#define X(M) case METHOD_##M: M##_handler(conn); return;
void router_dispatcher(connection_instance conn) {
    switch (conn->req.method) {
        HTTP_HANDLERS
    }
}
#undef X
