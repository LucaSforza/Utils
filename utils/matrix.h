#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdlib.h>

#include <string.h>

#include "macros.h"
#include "random.h"
#include "logging.h"

#ifndef MATRIXDEF
#define MATRIXDEF static inline
#endif // MATRIXDEF

/*
    Partendo dagli argomenti della matrice leggi l'ordine
    @return ordine della matrice
*/
MATRIXDEF size_t parse_order_matrix(int argc, char **argv);

/*
    Genera una matrice randomica casuale
    @param rows righe della matrice da generare
    @param cols colonne della matrice da generare
    @param min_val valore minimo (incluso) dei numeri casuali nelle posizioni $c_{i,j}$ della matrice
    @param max_val valore massimo (incluso) dei numeri casuali nelle posizioni $c_{i,j}$ della matrice
    @return Ritorna puntatore all'inizio della matrice (prima riga prima colonna)
    @note il puntatore va deallocato dopo l'uso della matrice
*/
MATRIXDEF int *generate_random_matrix(size_t rows, size_t cols, int min_val, int max_val);

/*
    Modifica la matrice in input (quadrata) e la rende trasposta
    @param mtx puntatore alla matrice da modificare
    @param order ordine della matrice in input
    @note La matrice in input DEVE essere quadrata
*/
MATRIXDEF void square_trasposed_matrix(int *mtx, size_t order);

/*
    Stampa la matrice in output nello stream.
    @param stream luogo dove verrà scritta la matrice in output
    @param mtx matrice da stampare
    @param rows numero di righe della matrice
    @param cols numero di colonne della matrice
*/
MATRIXDEF void fprintMatrix(FILE *stream, int *mtx, size_t rows, size_t cols);

/*
    Stampa la matrice nello standard error.
    @param mtx matrice da stampare
    @param rows numero di righe della matrice
    @param cols numero di colonne della matrice
*/
#define eprintMatrix(mtx, rows, cols)\
    fprintMatrix(stderr, (mtx), (rows), (cols))

/*
    Stampa la matrice nello standard output.
    @param mtx matrice da stampare
    @param rows numero di righe della matrice
    @param cols numero di colonne della matrice
*/
#define printMatrix(mtx, rows ,cols)\
    fprintMatrix(stdout, (mtx), (rows), (cols))

/*
    Calcola il prodotto scalare tra due vettori di lunghezza len
    @param vec1 puntatore al primo vettore
    @param vec2 puntatore al secondo vettore
    @param len grandezza dei due vettori
    @return Ritorna il prodotto scalare dei due vettori
*/
int dot_product(int *vec1, int *vec2, size_t len);

/* ---------------------- IMPLEMENTATION ---------------------- */

size_t parse_order_matrix(int argc, char **argv) {
    char *program_name = argv[0];
    (void) program_name;
    fatal_if(argc != 2, "numero sbagliato di parametri");
    char *order = argv[1];
    char *endptr;
    size_t result = strtol(order, &endptr, 10);
    fatal_if(*endptr != '\0', "la stringa è sbagliata: %s", order);
    return result;
}

int *generate_random_matrix(size_t rows, size_t cols, int min_val, int max_val) {
    size_t tot_length = rows*cols;

    int *result = (int*)malloc(tot_length*sizeof(int));
    fatal_if(result == NULL ,MSG_ERR_FULL_MEMORY);

    for(size_t i = 0; i < tot_length; i++) {
        result[i] = uniform_int_distribution(min_val, max_val);
    }

    return result;
}

//TODO: rendere possibile la trasposizione di matrici rows x cols
void square_trasposed_matrix(int *mtx, size_t order) {
    int app = 0;
    for(size_t i = 0; i < order; i++) {
        for(size_t j = i+1; j < order; j++) {
            if(i < j) {
                size_t index = i*order + j;
                app = mtx[index];
                mtx[index] = mtx[j*order + i];
                mtx[j*order + i] = app;
            }
        }
    }
}

void fprintMatrix(FILE *stream, int *mtx, size_t rows, size_t cols) {
    for(size_t i=0; i < rows; i++) {
        fprintf(stream ,"    ");
        for(size_t j=0; j < cols; j++) {
            fprintf(stream, "%d", mtx[i*cols + j]);
            if(j == cols - 1)
                putc('\n', stream);
            else fprintf(stream, " ,");
        }
    }
}

int dot_product(int *vec1, int *vec2, size_t len) {
    int result = 0;
    for(size_t i = 0; i < len; i++) {
        result += vec1[i]*vec2[i];
    }
    return result;
}

#endif // MATRIX_H_