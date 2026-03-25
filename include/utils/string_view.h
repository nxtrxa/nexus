#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    size_t len;
    char* str;
} string_view;

// [[nodiscard]] extern inline string_view cstr_to_sv(char* cstr);
void sv_trim(string_view* sv);
void sv_trim_left(string_view* sv);
void sv_trim_right(string_view* sv);
void chop_by_delim(string_view* sv, char del);
void bool sv_cmp(const string_view* sv, const string_view* other );

#endif // STRING_VIEW_H

// #ifdef STRING_VIEW_IMPLEMENTATION

// [[nodiscard]] extern inline string_view cstr_to_sv(char* cstr) {
//     return (string_view) {
//         .len = strlen(cstr),
//         .str = cstr,
//     };
// }

[[nodiscard]] string_view sv_trim(string_view sv) {
    return sv_trim_left(sv_trim_right(sv));
}

[[nodiscard]] string_view sv_trim_left(string_view sv) {
    int i = 0;
    while (isspace((unsigned char)sv.str[i])) {
        i++;
    }

    return (string_view) {
        .len = sv.len - i,
        .str = sv.str + i,
    };
}

[[nodiscard]] extern string_view sv_trim_right(string_view sv) {
    int i = sv.len - 1;
    while (isspace((unsigned char)sv.str[i])) {
        i--;
    }

    return (string_view) {
        .len = i + 1,
        .str = sv.str,
    };
}

[[nodiscard]] extern string_view chop_by_delim(string_view sv, char del) {
    int i = 0;
    while (sv.str[i] != del) {
        i++;
    }

    return (string_view) {
        .len = i,
        .str = sv.str,
    };
}


[[nodiscard]] extern bool sv_cmp(string_view sv, string_view other ) {
    if (sv.len != other.len) return false;

    return memcmp(sv.str, other.str, sv.len) == 0;
}

#endif
