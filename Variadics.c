#include <stdio.h>
#include <stdarg.h>

void printMessage(const char* str, ...) {
    va_list args;
    va_start(args, str);

    while (str != NULL) {
        printf("%s ", str);
        str = va_arg(args, const char*);
    }

    va_end(args);
    printf("\n");
}


