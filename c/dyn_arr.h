#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * dynamic array implementation for ints
 */

typedef struct dyn_arr {
    int* data;
    size_t len, capacity; 
} dyn_arr_t;

typedef enum DYN_ARRAY_CODE {
    DYN_ARRAY_SUCCESS       =  0,
    DYN_ARRAY_OUT_OF_MEMORY = -1
} DYN_ARRAY_CODE_T;

DYN_ARRAY_CODE_T dyn_arr_init(dyn_arr_t* dyn_arr) {
    dyn_arr->data = malloc(sizeof(int)); 
    if (dyn_arr->data == NULL) 
        return DYN_ARRAY_OUT_OF_MEMORY;
    dyn_arr->len = 0;
    dyn_arr->capacity = 1; 
    return DYN_ARRAY_SUCCESS; 
}

DYN_ARRAY_CODE_T dyn_arr_alloc(dyn_arr_t* dyn_arr) {
    while (dyn_arr->len >= dyn_arr->capacity)
        dyn_arr->capacity *= 2; 
    int* data = malloc(sizeof(int) * dyn_arr->capacity); 
    if (data == NULL)
        return DYN_ARRAY_OUT_OF_MEMORY;
    memcpy(data, dyn_arr->data, sizeof(int) * dyn_arr->len);  
    free(dyn_arr->data);
    dyn_arr->data = data;
    return DYN_ARRAY_SUCCESS;
}

DYN_ARRAY_CODE_T dyn_arr_append(dyn_arr_t* dyn_arr, int i) {
    DYN_ARRAY_CODE_T result = dyn_arr_alloc(dyn_arr); 
    if (result != DYN_ARRAY_SUCCESS)
        return result;
    *(dyn_arr->data + dyn_arr->len) = i; 
    dyn_arr->len++;
    return DYN_ARRAY_SUCCESS; 
}

int dyn_arr_binary_search(dyn_arr_t* dyn_arr, int i) {
    int low = 0, high = dyn_arr->len - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int search_val = *(dyn_arr->data + mid);
        if (search_val < i) {
            low = mid + 1;
        } else if (search_val > i) {
            high = mid - 1;
        } else {
            return mid;
        }
    }
    return -(low + 1);
}

void dyn_arr_print(dyn_arr_t* dyn_arr) {
    for (size_t i = 0; i < dyn_arr->len; i++) {
        printf("%i", *(dyn_arr->data + i)); 
        if (i != dyn_arr->len - 1)
            printf(", ");
    }
}

int dyn_arr_find(dyn_arr_t* dyn_arr, int s) {
    for (size_t i = 0; i < dyn_arr->len; i++)
        if (*(dyn_arr->data + i) == s) 
            return i;
    return -1; 
}

void dyn_arr_free(dyn_arr_t* dyn_arr) {
    free(dyn_arr->data); 
}

#endif