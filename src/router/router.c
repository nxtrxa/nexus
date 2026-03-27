#include "router/router.h"
#include "utils/attributes.h"
#include "router/handlers.h"

#include <stdlib.h>
#include <string.h>

static int send_file(connection_instance conn, FILE* f) {
    int read = 0;
    int sent = 0;
    int left = conn->wlen;

    http_res_t header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n\r\n";

    conn->state = WRITING;
    conn->keep_alive = true;
    send(conn->fd, header, strlen(header), 0);
    while ((read = fread(conn->wbuff, conn->wlen, left, f) > 0) && (read < left)) {
        sent = send(conn->fd, conn->wbuff, read, 0);
        if (sent == -1 ) break;
        sent += sent;
        left -= sent;
    }
    conn->wlen = sent;
    return (sent == -1) ? -1 : 0;
}

#define X(I)                                              \
    static void dispatch_##I(connection_instance conn) {  \
        FILE* I##_page = fopen(__##I##__, "r");           \
        if (!I##_page) {                                  \
            perror("Error opening " #I " page");          \
            return;                                       \
        }                                                 \
        if (send_file(conn, I##_page) < 0) {                  \
            perror("Error sending " #I " page");              \
            return;                                           \
        }                                                     \
        fclose(I##_page);                                     \
    }

DISPATCHERS
#undef X

#define X(I) [(I)] = dispatch_##I,
router_dispatcher_t dispatch_table[] = {
    DISPATCHERS
};
#undef X

#define DISPATCH(I) dispatch_table[(I)](conn)

#define X(M) case METHOD_##M: M##_handler(conn); return;
__inline__ void router_dispatcher(connection_instance conn) {
    switch (conn->req.method) {
        HTTP_HANDLERS
    }
}
#undef X
