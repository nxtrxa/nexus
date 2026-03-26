#ifndef STRMANIP_H
#define STRMANIP_H

#define TO_UPPER(STR) \
    do { \
        for (size_t i = 0; i < strlen(STR); i++) \
            (STR)[i] = toupper((STR)[i]); \
    } while(0)

#define TO_LOWER(STR) \
    do { \
        for (size_t i = 0; i < strlen(STR); i++) \
            (STR)[i] = tolower((STR)[i]); \
    } while(0)

#endif // STRMANIP_H

#ifdef STRMANIP_IMPLEMENTATION

__unused__ char* upper(char* str) {
    TO_UPPER(str);
    return str;
}

__unused__ static char* lower(char* str) {
    TO_LOWER(str);
    return str;
}

#endif // STRMANIP_IMPLEMENTATION
