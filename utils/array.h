#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>

int* generate_array(size_t len){
    int* arr = malloc(len*sizeof(int));
    for(size_t i=0;i<len;i++){
        arr[i]=rand()%10;
    }
    return arr;
}

#endif