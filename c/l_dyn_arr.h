#ifndef L_DYN_ARR_H
#define L_DYN_ARR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * dynamic array implementation for longs
 */

typedef struct dyn_arr {
    long* data;
    size_t len, capacity; 
} dyn_arr_t;

typedef enum DYN_ARRAY_CODE {
    DYN_ARRAY_SUCCESS       =  0,
    DYN_ARRAY_OUT_OF_MEMORY = -1
} DYN_ARRAY_CODE_T;

DYN_ARRAY_CODE_T dyn_arr_init(dyn_arr_t* dyn_arr) {
    dyn_arr->data = malloc(sizeof(long)); 
    if (dyn_arr->data == NULL) 
        return DYN_ARRAY_OUT_OF_MEMORY;
    dyn_arr->len = 0;
    dyn_arr->capacity = 1; 
    return DYN_ARRAY_SUCCESS; 
}

DYN_ARRAY_CODE_T dyn_arr_alloc(dyn_arr_t* dyn_arr) {
    while (dyn_arr->len >= dyn_arr->capacity)
        dyn_arr->capacity *= 2; 
    long* data = malloc(sizeof(long) * dyn_arr->capacity); 
    if (data == NULL)
        return DYN_ARRAY_OUT_OF_MEMORY;
    memcpy(data, dyn_arr->data, sizeof(long) * dyn_arr->len);  
    free(dyn_arr->data);
    dyn_arr->data = data;
    return DYN_ARRAY_SUCCESS;
}

DYN_ARRAY_CODE_T dyn_arr_append(dyn_arr_t* dyn_arr, long l) {
    DYN_ARRAY_CODE_T result = dyn_arr_alloc(dyn_arr); 
    if (result != DYN_ARRAY_SUCCESS)
        return result;
    *(dyn_arr->data + dyn_arr->len) = l; 
    dyn_arr->len++;
    return DYN_ARRAY_SUCCESS; 
}

void dyn_arr_print(dyn_arr_t* dyn_arr) {
    for (size_t i = 0; i < dyn_arr->len; i++) {
        printf("%ld", *(dyn_arr->data + i)); 
        if (i != dyn_arr->len - 1)
            printf(", ");
    }
}

int dyn_arr_find(dyn_arr_t* dyn_arr, long s) {
    for (size_t i = 0; i < dyn_arr->len; i++)
        if (*(dyn_arr->data + i) == s) 
            return i;
    return -1; 
}

void dyn_arr_free(dyn_arr_t* dyn_arr) {
    free(dyn_arr->data); 
}

#endif