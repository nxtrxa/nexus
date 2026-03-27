#ifndef TYPES_H
#define TYPES_H

typedef const char* http_request_t;
typedef int port_t;
typedef int fd_t;
typedef const char* http_res_t;

#define __STRUCT(S) \
    typedef struct S * S##_instance; \
    struct S

#define $array       // just visual

#endif // TYPES_H
