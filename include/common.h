#ifndef _clox_common_h
#define _clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_PRINT_CODE
// #define DEBUG_TRACE_EXCUTION

#define PANIC(message)                                                                                     \
    do                                                                                                     \
    {                                                                                                      \
        fprintf(stderr, "[\x1b[32m%s:%d\x1b[0m] \x1b[31mpanic\x1b[0m: %s", __FILE__, __LINE__, (message)); \
        exit(1);                                                                                           \
    } while (0)

#endif