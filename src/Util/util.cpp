#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void errif(bool condtion, const char* errmsg) {
    if (condtion) {
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}