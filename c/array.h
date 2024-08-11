#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * dynamic array implementation for ints
 */

typedef enum DYN_ARRAY_CODE {
    DYN_ARRAY_SUCCESS       =  0,
    DYN_ARRAY_OUT_OF_MEMORY = -1
} DYN_ARRAY_CODE_T;

#define MAKE_ARRAY(name, type)                                         \
typedef struct array {                                                 \
    type* data;                                                        \
    size_t len, capacity;                                              \
} name##_array_t;                                                      \
DYN_ARRAY_CODE_T name##_array_init(name##_array_t* array) {            \
    array->data = malloc(sizeof(type));                                \
    if (array->data == NULL)                                           \
        return DYN_ARRAY_OUT_OF_MEMORY;                                \
    array->len = 0;                                                    \
    array->capacity = 1;                                               \
    return DYN_ARRAY_SUCCESS;                                          \
}                                                                      \
DYN_ARRAY_CODE_T name##_array_alloc(name##_array_t* array) {           \
    while (array->len >= array->capacity)                              \
        array->capacity *= 2;                                          \
    type* data = malloc(sizeof(type) * array->capacity);               \
    if (data == NULL)                                                  \
        return DYN_ARRAY_OUT_OF_MEMORY;                                \
    memcpy(data, array->data, sizeof(type) * array->len);              \
    free(array->data);                                                 \
    array->data = data;                                                \
    return DYN_ARRAY_SUCCESS;                                          \
}                                                                      \
DYN_ARRAY_CODE_T name##_array_append(name##_array_t* array, type el) { \
    DYN_ARRAY_CODE_T result = name##_array_alloc(array);               \
    if (result != DYN_ARRAY_SUCCESS)                                   \
        return result;                                                 \
    *(array->data + array->len) = el;                                  \
    array->len++;                                                      \
    return DYN_ARRAY_SUCCESS;                                          \
}                                                                      \
int name##_array_find(name##_array_t* array, type el) {                \
    for (size_t i = 0; i < array->len; i++)                            \
        if (*(array->data + i) == el)                                  \
            return i;                                                  \
    return -1;                                                         \
}                                                                      \
void name##_array_free(name##_array_t* array) {                        \
    free(array->data);                                                 \
}                                                                

#endif