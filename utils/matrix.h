#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdlib.h>
#include <stdint.h>

#include <string.h>

#include "macros.h"
#include "random.h"
#include "logging.h"

#ifndef MATRIXDEF
#define MATRIXDEF static inline
#endif // MATRIXDEF

/*
    Genera una matrice randomica casuale
    @param rows righe della matrice da generare
    @param cols colonne della matrice da generare
    @param min_val valore minimo (incluso) dei numeri casuali nelle posizioni $c_{i,j}$ della matrice
    @param max_val valore massimo (incluso) dei numeri casuali nelle posizioni $c_{i,j}$ della matrice
    @return Ritorna puntatore all'inizio della matrice (prima riga prima colonna)
    @note il puntatore va deallocato dopo l'uso della matrice
*/
MATRIXDEF double *generate_random_matrix(size_t rows, size_t cols, double min_val, double max_val);

/*
    Modifica la matrice in input (quadrata) e la rende trasposta
    @param mtx puntatore alla matrice da modificare
    @param order ordine della matrice in input
    @note La matrice in input DEVE essere quadrata
*/
MATRIXDEF void square_trasposed_matrix(double *mtx, size_t order);

/*
    Stampa la matrice in output nello stream.
    @param stream luogo dove verrà scritta la matrice in output
    @param mtx matrice da stampare
    @param rows numero di righe della matrice
    @param cols numero di colonne della matrice
*/
MATRIXDEF void fprintMatrix(FILE *stream, double *mtx, size_t rows, size_t cols);

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
double dot_product(double *vec1, double *vec2, size_t len);

/* ---------------------- IMPLEMENTATION ---------------------- */

double *generate_random_matrix(size_t rows, size_t cols, double min_val, double max_val) {
    size_t tot_length = rows*cols;

    double *result = (double*)malloc(tot_length*sizeof(*result));
    fatal_if(result == NULL ,MSG_ERR_FULL_MEMORY);

    for(size_t i = 0; i < tot_length; i++) {
        result[i] = uniform_real_distribution(min_val, max_val);
    }

    return result;
}

//TODO: rendere possibile la trasposizione di matrici rows x cols
void square_trasposed_matrix(double *mtx, size_t order) {
    double temp;
    for(size_t i = 0; i < order; i++) {
        for(size_t j = i+1; j < order; j++) {
            if(i < j) {
                size_t index = i*order + j;
                temp = mtx[index];
                mtx[index] = mtx[j*order + i];
                mtx[j*order + i] = temp;
            }
        }
    }
}

void fprintMatrix(FILE *stream, double *mtx, size_t rows, size_t cols) {
    for(size_t i=0; i < rows; i++) {
        fprintf(stream ,"    ");
        for(size_t j=0; j < cols; j++) {
            fprintf(stream, "%lf", mtx[i*cols + j]);
            if(j == cols - 1)
                putc('\n', stream);
            else fprintf(stream, " ,");
        }
    }
    
}

double dot_product(double *vec1, double *vec2, size_t len) {
    double result = 0;
    for(size_t i = 0; i < len; i++) {
        result += vec1[i]*vec2[i];
    }
    return result;
}

void fprintArrayFloat(FILE *stream, float *array,size_t lenght){

    for(size_t i=0;i<lenght;i++){
            fprintf(stream, "%f\n",array[i]);
    }
}

#endif // MATRIX_H_