#include "router/router.h"
#include "utils/attributes.h"

#include <stdlib.h>
#include <string.h>

static int send_file(FILE* f, fd_t fd, char* buff, size_t* len) {
    int sent = 0;
    int left = *len;
    int n;

    while ((sent = fread(buff, sizeof *buff, left, f) > 0) && (sent < left)) {
        n = send(fd, buff, sent, 0);
        if (n == -1 ) break;
        sent += n;
        left -= n;
    }
    *len = sent;
    return (n == -1) ? -1 : 0;
}

void dispatch_home(connection_instance conn) {
    FILE* home_page = fopen(home, "r");
    if (!home_page) {
        perror("Error opening home page");
        return;
    }

    http_res_t header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n\r\n";

    conn->state = WRITING;
    conn->keep_alive = true;
    send(conn->fd, header, strlen(header), 0);
    if (send_file(home_page, conn->fd, conn->wbuff, &conn->wlen) < 0) {
        perror("Error sending home page");
        return;
    }
    fclose(home_page);
}

// #define X(I) [(I)] = lower((#I)),
// static route_t routes_table[] = {
//     ROUTES
// };
// #undef X
//
// #define X(I) [(I)] = dispatch_##I,
// static router_dispatcher_t dispatch_table[] {
//     ROUTES
// };
// #undef X
//
// router_instance router_construct() {
//
// }
//
// route_t router_dispatch() {
//
// }
