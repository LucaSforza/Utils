#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "macros.h"
#include "strings.h"

typedef struct Node_Int Node_Int;

struct Node_Int {
    String_View key;
    int value;
    Node_Int *next;
};

typedef struct {
    Node_Int **buckets;
    size_t length;
} HashMap_Sv_Int;

size_t sv_hash(String_View sv, size_t nbucket);
HashMap_Sv_Int init_hashmap_sv_int(size_t nbucket);
int *ht_get(HashMap_Sv_Int *hasht, String_View key);
void ht_free(HashMap_Sv_Int hasht);

#ifdef HASHMAP_IMPLEMENTATION

size_t sv_hash(String_View sv, size_t nbucket) {
    size_t result = 0;
    for(char *c = sv.data; c < sv.data + sv.lenght; c++) {
        result += *c;
    }
    return result % nbucket;
}

HashMap_Sv_Int init_hashmap_sv_int(size_t nbucket) {
    Node_Int **buckets = malloc(sizeof(*buckets)*nbucket);
    assert(buckets != NULL);
    memset(buckets, 0, sizeof(*buckets)*nbucket);
    return ((HashMap_Sv_Int) {
        .buckets = buckets,
        .length = nbucket
    });
}

int *ht_get(HashMap_Sv_Int *hasht, String_View key) {
    size_t pos = sv_hash(key, hasht->length);
    for(Node_Int *it = hasht->buckets[pos]; it != NULL; it = it->next) {
        if(sv_eq(key, it->key)) {
            return &it->value;
        }
    }
    return NULL;
}

void ht_set(HashMap_Sv_Int *hasht, String_View key, int value) {
    size_t pos = sv_hash(key, hasht->length);
    for(Node_Int *it = hasht->buckets[pos]; it != NULL; it = it->next) {
        if(sv_eq(key, it->key)) {
            it->value = value;
            return;
        }
    }
    Node_Int *node_ptr = malloc(sizeof(*node_ptr));
    *node_ptr = (Node_Int) {
        .key = key,
        .value = value,
        .next = hasht->buckets[pos]
    };
    hasht->buckets[pos] = node_ptr;
}

void ht_free(HashMap_Sv_Int hasht) {
    for(Node_Int **it = hasht.buckets; it < hasht.buckets + hasht.length; it++) {
        if(*it != NULL) {
            Node_Int *p = *it;
            do {
                Node_Int *next = (p)->next;
                free(p);
                p = next;
            } while(p != NULL);
        }
    }
    free(hasht.buckets);
}

#endif // HASHMAP_IMPLEMENTATION


#endif // HASHMAP_H_