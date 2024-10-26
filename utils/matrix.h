#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdlib.h>

#include <string.h>

#include "macros.h"
#include "random.h"
#include "logging.h"

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

int *generate_matrix(size_t order) {
    size_t tot_length = order*order;

    int *result = (int*)malloc(tot_length*sizeof(int));
    fatal_if(result == NULL ,MSG_ERR_FULL_MEMORY);

    for(size_t i = 0; i < tot_length; i++) {
        result[i] = rand() % 10;
    }

    return result;
}

int *generate_matrix_n_m(size_t rig,size_t col,int min_val, int max_val) {
    size_t tot_length = rig*col;

    int *result = (int*)malloc(tot_length*sizeof(int));
    fatal_if(result == NULL ,MSG_ERR_FULL_MEMORY);

    for(size_t i = 0; i < tot_length; i++) {
        result[i] = uniform_int_distribution(min_val,max_val);
    }

    return result;
}

void reverse_matrix(int *mtx, size_t order) {
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

void fprintMatrix(FILE *stream, int *mtx, size_t order) {
    for(size_t i=0; i < order; i++) {
        fprintf(stream ,"    ");
        for(size_t j=0; j < order; j++) {
            fprintf(stream, "%d", mtx[i*order + j]);
            if(j == order - 1)
                putc('\n', stream);
            else fprintf(stream, " ,");
        }
    }
}

#define eprintMatrix(mtx, order)\
    fprintMatrix(stderr, (mtx), (order))
#define printMatrix(mtx, order)\
    fprintMatrix(stdout, (mtx), (order))

void fprintMatrix_n_m(FILE *stream, int *mtx, size_t rig,size_t col) {
    for(size_t i=0; i < rig; i++) {
        fprintf(stream ,"    ");
        for(size_t j=0; j < col; j++) {
            fprintf(stream, "%d", mtx[i*rig + j]);
            if(j == col - 1)
                putc('\n', stream);
            else fprintf(stream, " ,");
        }
    }
}

#define eprintMatrix_n_m(mtx, rig,col)\
    fprintMatrix_n_m(stderr, (mtx), (rig), (col))
#define printMatrix_n_m(mtx, rig,col)\
    fprintMatrix_n_m(stdout, (mtx), (rig), (col))

int dot_product(int *vec1, int *vec2,  size_t len) {
    int result = 0;
    for(size_t i = 0; i < len; i++) {
        result += vec1[i]*vec2[i];
    }
    return result;
}

#endif // MATRIX_H_