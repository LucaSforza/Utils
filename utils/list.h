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
    size_t length;
    pthread_rwlock_t rwlock;
    int(*compare)(void*,void*);
} list_head_t;

/*
    Creazione della lista partendo dalla funzione di comparazione tra elementi
    @param compare puntatore alla funzione che compara due elementi (passati come void*) e ritorna -1 se il primo parametro
    è minire del secondo parametro, 0 se sono uguali e 1 se il secondo è più grande del primo,
    generando una relazione di equivalenza fra gli elementi utilizzati
    @return Struttura che rappresenta l'intera lista come singolo
*/
LISTDEF list_head_t list_init(int(*compare)(void*,void*));
/*
    Deinizializza tutta la memoria allocata dalla lista, anche gli elementi stessi della lista
*/
LISTDEF void list_deinit(list_head_t _this);
/*
    Controlla se un elemento si trova nella lista
    @note O(n) 
*/
LISTDEF bool list_is_member(list_head_t *_this, void *value);
/*
    Inserisci un elemento nella lista
    @note inserire un puntatore allocato nella head, poiché la list dealloca in automatico
    @note Complessità: O(n)
*/
LISTDEF bool list_insert(list_head_t *_this, void *value);
/*
    Elimina un elemento nella lista
    @note O(n)
*/
LISTDEF bool list_delete(list_head_t *_this, void *value);

/* ---------------------- BASIC COMPARE FUNCTIONS ---------------------- */

/*
    Compara due interi
    @param _this primo elemento passato come puntatore che punta nella head
    @param _that secondo elemento passato come puntatore che punta nella head 
*/
LISTDEF int list_int_compare(void *_this, void *_that) {
    return *((int*)_this) - *((int*)_that);
}

#define LIST_CMP_INT list_int_compare

/*
    Compara due double (numeri reali)
    @param _this primo elemento passato come puntatore che punta nella head
    @param _that secondo elemento passato come puntatore che punta nella head 
*/
LISTDEF int list_double_compare(void *_this, void *_that) {
    int result = 0;
    double a = *((double*)_this);
    double b = *((double*)_that);
    if(a < b) result = -1;
    else if (a > b) result = 1;
    return result;
}

#define LIST_CMP_DOUBLE list_double_compare

// TODO: aggiungere per più tipi

/* ---------------------- IMPLEMENTATION ---------------------- */

list_head_t list_init(int(*compare)(void*,void*)) {
    list_head_t _this = {0};
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
    bool result;
    pthread_rwlock_rdlock(&_this->rwlock);

    list_node_t *curr_node = _this->head;
    if(curr_node == NULL) return_defer(false);

    while(curr_node->next != NULL && _this->compare(curr_node->data_p, value) < 0) {
       curr_node = curr_node->next; 
    }
    if(curr_node->next == NULL || _this->compare(curr_node->data_p, value) > 0) {
        return_defer(false);
    }
    result = true;
defer:
    pthread_rwlock_unlock(&_this->rwlock);
    return result;
}

bool list_insert(list_head_t *_this, void *value) {
    bool result;
    pthread_rwlock_wrlock(&_this->rwlock);
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
        _this->length++;
        if(prec_p == NULL) {
            // Devo modificare la testa
            _this->head = temp_p;
        } else prec_p->next = temp_p;
        return_defer(true);
    }
    result = false;
defer:
    pthread_rwlock_unlock(&_this->rwlock);
    return result;
}

bool list_delete(list_head_t *_this, void *value_p) {
    bool result;
    pthread_rwlock_wrlock(&_this->rwlock);

    list_node_t *curr_p = _this->head;
    list_node_t *pred_p = NULL;
    int last_compare = 0;

    if(curr_p == NULL) return_defer(false);

    while(curr_p != NULL && (last_compare = _this->compare(curr_p->data_p, value_p)) < 0) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if(curr_p != NULL && last_compare == 0) {
        // Devo eliminare
        _this->length--;
        if(pred_p == NULL) {
            // rimozione primo nodo dalla lista
            _this->head = curr_p->next;
        } else {
            pred_p->next = curr_p->next;
        }
        free(curr_p->data_p);
        free(curr_p);
        return_defer(true);
    }

    result = false;

defer:
    pthread_rwlock_unlock(&_this->rwlock);
    return result;
}

/*
    ## Read-Write Locks.

    Sono dei mutex, ma offrono due funzioni per fare la lock, in lettura o in scrittura
    Ci possono essere più thread che prendono il blocco in lettura
    ma solo uno in scrittura
    lo scrittore deve aspettare che i lettori finiscono di leggere
    i lettori possono leggere contemporaneamente
*/

#endif // LIST_H_
