#include "router/handlers.h"

#include <assert.h>

void GET_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "GET handler not implemented");
}

void POST_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "POST handler not implemented");
}

void PUT_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "PUT not implemented");
}

void DELETE_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "DELETE not implemented");
}

void HEAD_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "HEAD not implemented");
}


void OPTIONS_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "OPTIONS not implemented");
}

void CONNECT_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "CONNECT not implemented");
}

void PATCH_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "PATCH not implemented");
}

void TRACE_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "TRACE not implemented");
}

void UNKNOWN_handler(connection_instance conn) {
    (void)conn;
    assert(0 && "UNKNOWN not implemented");
}

