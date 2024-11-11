#ifndef MACROS_H_
#define MACROS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>

#define INIT_CAP 128

typedef const char Cstr;

typedef int Errno;

#define MSG_ERR_FULL_MEMORY "Out of memory, buy more RAM"

// Append several items to a dynamic array
#define append_many(da, new_items, new_items_count)                                  \
    do {                                                                                    \
        if ((da)->length + new_items_count > (da)->capacity) {                               \
            if ((da)->capacity == 0) {                                                      \
                (da)->capacity = INIT_CAP;                                           \
            }                                                                               \
            while ((da)->length + new_items_count > (da)->capacity) {                        \
                (da)->capacity *= 2;                                                        \
            }                                                                               \
            (da)->data = realloc((da)->data, (da)->capacity*sizeof(*(da)->data));    \
            fatal_if((da)->data != NULL, MSG_ERR_FULL_MEMORY);                          \
        }                                                                                   \
        memcpy((da)->data + (da)->length, new_items, new_items_count*sizeof(*(da)->data)); \
        (da)->length += new_items_count;                                                     \
    } while (0)

#define append(vec,obj)                                           \
    do {                                                          \
    if ((vec)->capacity == 0) {                                        \
        (vec)->capacity = INIT_CAP;                                    \
        (vec)->data = malloc(sizeof(obj)*(vec)->capacity);              \
        fatal_if((vec)->data == NULL, MSG_ERR_FULL_MEMORY);\
    } else if((vec)->length == (vec)->capacity) {                         \
        (vec)->capacity = (vec)->capacity*2;                                \
        (vec)->data = realloc((vec)->data,sizeof(obj)*(vec)->capacity);  \
    }                                                             \
    (vec)->data[(vec)->length++] = obj;                               \
    } while(0)

#define pop(vec,ptr)                        \
        fatal_if((vec)->length != 0, "Stack Underflow");        \
        *(ptr) = (vec)->data[--(vec)->length]   \

#define pop_at(vec, i, ptr) \
    fatal_if(i < (vec)->length, "Stack Underflow"); \
    *(ptr) = (vec)->data[i]; \
    (vec)->data[i] = (vec)_>data[--(vec)->length]


#define return_defer(value) do { result = (value); goto defer;} while(0)

#define eprintf(...) \
    fprintf(stderr, __VA_ARGS__)

#define eputc(char)\
    putc((char), stderr)

#define CHAR_TO_NUM(c) (c - '0')

#define ARRAY_LEN(arr) sizeof(arr)/sizeof(*(arr))

#define IS_POW2(n) ((n != 0) && (n & (n - 1)) == 0)

#define foreach(type, item, da)\
    for(type* (item) = (da)->data; (item) < (da)->data + (da)->length; (item)++)


#include "logging.h"


#define TODO(msg) log_fatal(msg)

#include <sys/time.h> //TODO sistemare questo import

#define GET_TIME(now) \
    do {\
    struct timeval t;\
    *(now) = t.tv_sec + t.tv_usec/1000000.0;  \
} while(0)

#define _aligned_alloc(size) aligned_alloc(glob_ctx.dcache_line_size, (size))

#endif // MACROS_H_