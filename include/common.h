#ifndef _clox_common_h
#define _clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_PRINT_CODE
#define DEBUG_MODE
#define DEBUG_TRACE_EXCUTION
#define UINT8_COUNT UINT8_MAX + 1
#define MAX_LOCAL 1U << 15

#define PANIC(message)                                                                                     \
    do                                                                                                     \
    {                                                                                                      \
        fprintf(stderr, "[\x1b[32m%s:%d\x1b[0m] \x1b[31mpanic\x1b[0m: %s", __FILE__, __LINE__, (message)); \
        exit(1);                                                                                           \
    } while (0)

#ifdef DEBUG_MODE
#define ASSERT(condition, message) \
    do                             \
    {                              \
        if (!(condition))          \
        {                          \
            PANIC(message);        \
        }                          \
    } while (0)
#else
#define ASSERT(condition, message)
#endif

#endif