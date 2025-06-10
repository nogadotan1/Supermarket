#ifndef MY_MACROS_H
#define MY_MACROS_H

#include <stdio.h>
#include <stdlib.h>

// Macro to check if a pointer is NULL and return 0
#define CHECK_RETURN_0(ptr) \
    do { \
        if (!(ptr)) return 0; \
    } while (0)

// Macro to check if a pointer is NULL, print an error message, and return 0
#define CHECK_MSG_RETURN_0(ptr, msg) \
    do { \
        if (!(ptr)) { \
            printf("%s\n", msg); \
            return 0; \
        } \
    } while (0)

// Macro to free memory, close a file, and return 0
#define FREE_CLOSE_FILE_RETURN_0(ptr, file) \
    do { \
        if (ptr) free(ptr); \
        if (file) fclose(file); \
        return 0; \
    } while (0)

// Macro to close a file and return 0
#define CLOSE_RETURN_0(file) \
    do { \
        if (file) fclose(file); \
        return 0; \
    } while (0)

#endif // MY_MACROS_H
