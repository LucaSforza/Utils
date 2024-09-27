#ifndef ARENA_H_
#define ARENA_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct Region Region;

struct Region {
    size_t lenght;
    size_t capacity;
    Region *next;
    Region *previous;
    uintptr_t data[];
};

typedef struct {
    Region *start;
    Region *not_allocable;
    Region *low_memory;
} Arena;


// tutte queste costanti vanno immaginate moltiplicate per sizeof(uintptr_t) = 8
#define DEFAULT_REGION_CAPACITY (8*1024) // 8*1024*8
#define LOW_MEMORY_REGION_THRESHOLD (256) // 2KB
#define NOT_ALLOCABLE_REGION_THRESHOLD (32) // 256 byte

/*
    Alloca una quantità di byte nell'arena
    @return puntatore all'inizio della memoria allocata
*/
void *arena_alloc(Arena *a, size_t size_bytes);
/*
    Realloca su un arena una porzione di memoria
*/
void *arena_realloc(Arena *a,void *oldptr, size_t oldsz, size_t newsz);

/*
    Resetta la lunghezza delle regioni a 0, così da poter
    riusare quelle senza deallocare e riallocare tutto
*/
void arena_reset(Arena *a);
/*
    libera la memoria usata dall'arena
*/
void arena_free(Arena *a);

#ifdef STRINGS_H_
#include "strings.h"

String_Builder sb_arena_from_sv(Arena *a, String_View sv);
String_Builder sb_arena_from_cstr(Arena *a, Cstr *data);
String_Builder sb_arena_clone(Arena *a, String_Builder sb);

bool sb_arena_read_entire_file(String_Builder *sb, Arena *a, Cstr *path);
void sb_arena_append_cstr(String_Builder *sb,Arena *a, Cstr *data);

#endif // STRINGS_H_

#ifdef ARENA_IMPLEMENTATION

Region *new_region(size_t capacity) {
    Region *r = malloc(sizeof(Region) + sizeof(uintptr_t)*capacity);
    assert(r != NULL && "Memory full, buy more RAM");
    r->next = NULL;
    r->lenght = 0;
    r->capacity = capacity;
    return r;
}

void push_start(Arena *a, Region *r) {
    if(a->start == NULL) {
        a->start = r;
    } else {
        Region *x = a->start;
        a->start = r;
        r->next = x;
        x->previous = r;
        r->previous = NULL;
    }
}

void push_low_memory(Arena *a, Region *r) {
    if(a->low_memory == NULL)
        a->low_memory = r;
    else {
        Region *x = a->low_memory;
        a->low_memory = r;
        r->next = x;
        x->previous = r;
        r->previous = NULL;
    }
}

void push_not_allocable(Arena *a, Region *r) {
    if(a->not_allocable == NULL)
        a->not_allocable = r;
    else {
        Region *x = a->not_allocable;
        a->not_allocable = r;
        r->next = x;
        x->previous = r;
        r->previous = NULL;
    }
}

Region *pop_low_memory(Arena *a) {
    if(a->low_memory == NULL) return NULL;
    Region *result = a->low_memory;
    a->low_memory = result->next;
    if(a->low_memory != NULL) a->low_memory->previous = NULL;
    return result;
}

Region *pop_start(Arena *a) {
    if(a->low_memory == NULL) return NULL;
    Region *result = a->start;
    a->start = result->next;
    if(a->start != NULL) a->start->previous = NULL;
    return result;
}

void *arena_alloc(Arena *a, size_t size_bytes) {
    if(size_bytes == 0) return NULL;
    void *result = NULL;
    // aggiungo byte per assicurarmi che viene allocato un numero >= di bytes in input
    size_t size = (size_bytes + sizeof(uintptr_t) - 1)/sizeof(uintptr_t);

    if(size > DEFAULT_REGION_CAPACITY) {

        Region *r = new_region(size);
        push_not_allocable(a, r);
        result = r->data;

    } else if(a->low_memory != NULL && size <= NOT_ALLOCABLE_REGION_THRESHOLD) {

        Region *x = a->low_memory;
        result = &x->data[x->lenght];
        x->lenght += size;

        if(x->capacity - x->lenght < NOT_ALLOCABLE_REGION_THRESHOLD) {
            push_not_allocable(a, pop_low_memory(a));
        }

    } else if(a->start == NULL){

        Region *r = new_region(DEFAULT_REGION_CAPACITY);
        push_start(a, r);
        result = r->data;
        r->lenght += size;

        if(r->capacity - r->lenght < NOT_ALLOCABLE_REGION_THRESHOLD)
            push_not_allocable(a, pop_start(a));
        else if(r->capacity - r->lenght < LOW_MEMORY_REGION_THRESHOLD)
            push_low_memory(a, pop_start(a));

    } else {

        Region *x = a->start;
    
        while(x->lenght + size > x->capacity && x->next != NULL) x = x->next;
    
        if(x->lenght + size > x->capacity) {
            Region *r = new_region(DEFAULT_REGION_CAPACITY);
            push_start(a, r);
            result = r->data;
            r->lenght += size;

            if(r->capacity - r->lenght < NOT_ALLOCABLE_REGION_THRESHOLD)
                push_not_allocable(a, pop_start(a));
            else if(r->capacity - r->lenght < LOW_MEMORY_REGION_THRESHOLD)
                push_low_memory(a, pop_start(a));
        } else {
            result = &x->data[x->lenght];
            x->lenght += size;

            if(x->capacity - x->lenght < NOT_ALLOCABLE_REGION_THRESHOLD) {
                push_not_allocable(a, x);

                if(x->previous != NULL)
                    x->previous->next = x->next;
                else a->start = NULL;
            } else if(x->capacity - x->lenght < LOW_MEMORY_REGION_THRESHOLD) {
                push_low_memory(a, x);

                if(x->previous != NULL)
                    x->previous->next = x->next;
                else a->start = NULL;
            }
        }
    }
    
    return result;
}

void *arena_realloc(Arena *a,void *oldptr, size_t oldsz, size_t newsz) {
    if(newsz <= oldsz) return oldptr;
    void *newptr = arena_alloc(a, newsz);
    memcpy(newptr, oldptr, oldsz);
    return newptr;
}

void arena_reset(Arena *a) {
    for(Region *it=a->start; it != NULL; it = it->next) {
        it->lenght = 0;
    }
    for(Region *it=a->low_memory; it != NULL; it = it->next) {
        it->lenght = 0;
        push_start(a, it);
    }
    for(Region *it=a->not_allocable; it != NULL; it = it->next) {
        it->lenght = 0;
        push_start(a, it);
    }
    a->low_memory = NULL;
    a->not_allocable = NULL;
}

void free_regions(Region *r) {
    while(r) {
        Region *x = r;
        r = r->next;
        free(x);
    }
}

void arena_free(Arena *a) {
    free_regions(a->start);
    free_regions(a->not_allocable);
    free_regions(a->low_memory);
    a->start = NULL;
    a->not_allocable = NULL;
    a->low_memory = NULL;
}

#ifdef STRINGS_H_

String_Builder sb_arena_from_sv(Arena *a, String_View sv) {
    size_t lenght = INIT_CAP > sv.lenght ? INIT_CAP : sv.lenght;
    char *data = arena_alloc(a, lenght);
    return sb_from_parts(data, sv.lenght, lenght);
}
String_Builder sb_arena_from_cstr(Arena *a, Cstr *data) {
    size_t str_len = strlen(data);
    size_t capacity = INIT_CAP > str_len ? INIT_CAP : str_len;
    char *data = arena_alloc(a, capacity);
    return sb_from_parts(data, capacity, str_len);
}
String_Builder sb_arena_clone(Arena *a, String_Builder sb);


bool sb_arena_read_entire_file(String_Builder *sb, Arena *a, Cstr *path);
void sb_arena_append_cstr(String_Builder *sb,Arena *a, Cstr *data);

#endif // STRINGS_H_

#endif // ARENA_IMPLEMENTATION

#endif // ARENA_H_