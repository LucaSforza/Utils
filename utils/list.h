#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
//TODO: gestire il fatto che potrei includere la lista senza avere pthread
#include <pthread.h>

#include "macros.h"

#ifndef LISTDEF
#define LISTDEF static inline
#endif // LISTDEF 

/*
    Lista ordinata di elementi
*/
typedef struct list_node_s list_node_t;

struct list_node_s
{
    void *data_p;
    list_node_t *next; // può essere null
};

typedef struct {
    list_node_t *head;
    size_t data_size;
    int(*compare)(void*,void*);
    pthread_rwlock_t rwlock;
} list_head_t;

LISTDEF list_head_t list_init(size_t data_size, int(*compare)(void*,void*));
LISTDEF void list_deinit(list_head_t _this);
/*
    Controlla se un elemento si trova nella lista
    @note O(n) 
*/
LISTDEF bool list_is_member(list_head_t *_this, void *value);
LISTDEF bool list_insert(list_head_t *_this, void *value);
LISTDEF bool list_delete(list_node_t *head_p, void *value);

/* ---------------------- IMPLEMENTATION ---------------------- */

list_head_t list_init(size_t data_size, int(*compare)(void*,void*)) {
    list_head_t _this = {0};
    _this.data_size = data_size;
    _this.compare = compare;
    pthread_rwlock_init(&_this.rwlock, NULL);
    return _this;
}

void list_deinit(list_head_t _this) {
    pthread_rwlock_destroy(&_this.rwlock);
    list_node_t *curr_p = _this.head;
    if(curr_p == NULL) return;
    while(curr_p->next != NULL) {
        list_node_t *temp = curr_p->next;
        free(curr_p->data_p);
        free(curr_p);
        curr_p = temp;
    }
}

bool list_is_member(list_head_t *_this, void *value) {
    list_node_t *curr_node = _this->head;
    if(curr_node == NULL) return false;
    while(curr_node->next != NULL && _this->compare(curr_node->data_p, value) < 0) {
       curr_node = curr_node->next; 
    }
    if(curr_node->next == NULL || _this->compare(curr_node->data_p, value) > 0) {
        return false;
    }
    return true;
}

bool list_insert(list_head_t *_this, void *value) {
    list_node_t *curr_p = _this->head;
    list_node_t *prec_p = NULL;
    list_node_t *temp_p;

    while(curr_p->next != NULL || _this->compare(curr_p->data_p, value) < 0) {
        prec_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || _this->compare(curr_p->data_p, value) > 0) {
        temp_p = (list_node_t*)malloc(sizeof(list_node_t));
        temp_p->data_p = value;
        temp_p->next = curr_p;
        if(prec_p == NULL) {
            // Devo modificare la testa
            _this->head = temp_p;
        } else prec_p->next = temp_p;
        return true;
    }
    return false;

}

bool list_delete(list_node_t *head_p, void *value) {
    TODO("Da implementare");
}

/*
    ## Read-Write Locks.

    Sono dei mutex, ma offrono due funzioni per fare la lock, in lettura o in scrittura
    Ci possono essere più thread che prendono il blocco in lettura
    ma solo uno in scrittura
    lo scrittore deve aspettare che i lettori finiscono di leggere
    i lettori possono leggere contemporaneamente
*/

// TODO: scrivere la versioni di queste funzioni in parallelo

#endif // LIST_H_
