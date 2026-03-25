#ifndef NOTLIB_DS

/*
 * simple dynamic array implementation that shadows the array metadata in a header
 * the array is just a normal pointer. When we construct the array we allocate the header
 * and then the array elements.
*/

#include <assert.h>
#include <stdlib.h>

typedef struct {
    size_t count;
    size_t capacity;
} Header;

#ifdef NOTLIB_DA_IMPLEMENTATION

#define ARRAY_SIZE(xs) sizeof(xs) / sizeof(*(xs))
#define DA_INIT_CAP 1

#define da_new(type, name) type* name = nullptr

#define da_init(xs)                                                                 \
    do {                                                                            \
        Header* header = malloc(sizeof(Header) + sizeof(*(xs)) * DA_INIT_CAP);      \
        header->capacity = DA_INIT_CAP;                                             \
        header->count = 0;                                                          \
        xs = (void*)(header + 1);                                                   \
    } while(0)

#define da_push(xs, x)                                                                  \
    do {                                                                                \
        if (xs == nullptr) {                                                            \
            Header* header = malloc(sizeof(Header) + sizeof(*(xs)) * DA_INIT_CAP);      \
            header->capacity = DA_INIT_CAP;                                             \
            header->count = 0;                                                          \
            xs = (void*)(header + 1);                                                   \
        }                                                                               \
        Header* header = (Header*)(xs) - 1;                                             \
        if (header->count >= header->capacity) {                                        \
            header->capacity *= 2;                                                      \
            header = realloc(header, sizeof(Header) + sizeof(*(xs)) * header->capacity);\
            xs = (void*)(header + 1);                                                   \
        }                                                                               \
        (xs)[header->count++] = x;                                                      \
    } while(0)

#define da_len(xs) ((Header*)(xs) - 1)->count

#define da_free(xs) free((Header*)(xs) - 1)

#define header_of(xs) ((Header*)xs - 1)

#define da_pop(xs)                      \
    do {                                \
    assert(header_of(xs)->count > 0);   \
    (xs)[header_of(xs)->count--];       \
    } while (0)

#define da_for_each(elem, xs)                                                       \
    do {                                                                            \
    typeof(*xs) elem;                                                               \
    for (uint32_t idx = 0; idx < da_len(xs) && ((elem) = (xs)[idx], 1); ++idx) {

// Explicit declare the index
#define da_for_each_i(idx, elem, xs)                                                \
    do {                                                                            \
    typeof(*xs) elem;                                                               \
    for (uint32_t idx = 0; idx < da_len(xs) && ((elem) = (xs)[idx], 1); ++idx) {

#define for_each(elem, xs)                                                          \
    do {                                                                            \
    typeof(*xs) elem;                                                               \
    for (uint32_t idx = 0; idx < ARRAY_SIZE(xs) && ((elem) = (xs)[idx], 1); ++idx) {

#define for_each_i(idx, elem, xs)                                                   \
    do {                                                                            \
    typeof(*xs) elem;                                                               \
    for (uint32_t idx = 0; idx < ARRAY_SIZE(xs) && ((elem) = (xs)[idx], 1); ++idx) {

#define end_fe  }} while(0)

#endif // NOTLIB_DA
#endif // NOTLIB_DA_IMPLEMENTATION
