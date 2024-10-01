#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#define INIT_CAP 128

typedef const char Cstr;

// Append several items to a dynamic array
#define append_many(da, new_items, new_items_count)                                  \
    do {                                                                                    \
        if ((da)->lenght + new_items_count > (da)->capacity) {                               \
            if ((da)->capacity == 0) {                                                      \
                (da)->capacity = INIT_CAP;                                           \
            }                                                                               \
            while ((da)->lenght + new_items_count > (da)->capacity) {                        \
                (da)->capacity *= 2;                                                        \
            }                                                                               \
            (da)->data = realloc((da)->data, (da)->capacity*sizeof(*(da)->data));    \
            assert((da)->data != NULL && "Memory full, buy more RAM");                          \
        }                                                                                   \
        memcpy((da)->data + (da)->lenght, new_items, new_items_count*sizeof(*(da)->data)); \
        (da)->lenght += new_items_count;                                                     \
    } while (0)

#define append(vec,obj)                                           \
    do {                                                          \
    if ((vec)->capacity == 0) {                                        \
        (vec)->capacity = INIT_CAP;                                    \
        (vec)->data = malloc(sizeof(obj)*(vec)->capacity);              \
        if((vec)->data == NULL) assert(false && "Memory full, buy more RAM");\
    } else if((vec)->lenght == (vec)->capacity) {                         \
        (vec)->capacity = (vec)->capacity*2;                                \
        (vec)->data = realloc((vec)->data,sizeof(obj)*(vec)->capacity);  \
    }                                                             \
    (vec)->data[(vec)->lenght++] = obj;                               \
    } while(0)

#define pop(vec,ptr)                        \
        if((vec)->lenght == 0) assert(false && "Stack Underflow");        \
        *(ptr) = (vec)->data[--(vec)->lenght]   \

#define pop_at(vec, i, ptr) \
    assert(i < (vec)->lenght && "Stack Underflow"); \
    *(ptr) = (vec)->data[i]; \
    (vec)->data[i] = (vec)_>data[--(vec)->lenght]


#define return_defer(value) do { result = (value); goto defer;} while(0)

#define eprintf(...) \
    fprintf(stderr, __VA__ARGS__)

#define CHAR_TO_NUM(c) (c - '0')

#define ARRAY_LEN(arr) sizeof(arr)/sizeof(*(arr))

#endif // COMMON_H_