#ifndef STRMANIP_H
#define STRMANIP_H
#include <ctype.h>

#define TO_UPPER(STR) \
    do { \
        for (size_t i = 0; i < strlen(STR); i++) \
            (STR)[i] = toupper((unsigned char)(STR)[i]); \
    } while(0)

#define TO_LOWER(STR) \
    do { \
        for (size_t i = 0; i < strlen(STR); i++) \
            (STR)[i] = tolower((unsigned char)(STR)[i]); \
    } while(0)

#endif // STRMANIP_H

#ifdef STRMANIP_IMPLEMENTATION

__inline__ char* upper(char* str) {
    TO_UPPER(str);
    return str;
}

__inline__ char* lower(char* str) {
    TO_LOWER(str);
    return str;
}

#endif // STRMANIP_IMPLEMENTATION
