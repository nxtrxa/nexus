#include <stdio.h>
#include "utils/attributes.h"

typedef const void* cptr;

__unused__ void print() {
    fprintf(stderr, "Hello\n");
}
