#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmp.h"
#include "format.h"

#pragma clang diagnostic ignored "-Wformat-zero-length"
#pragma GCC diagnostic ignored "-Wformat-zero-length"

/**
 * dynamic array macro implementation
 *
 * MAKE_ARRAY([NAME], TYPE): makes an array with the selected name and type; NAME is required if TYPE is a pointer
 *
 * __ARRAY_INIT_SIZE: defaults to 16; the initial capacity that will be allocated by **_array_init 
 * __ARRAY_GROWTH_RATE: defaults to 1.5; the rate at which the array grows when calling **_array_alloc
 * 
 * __MAKE_ARRAY_PRINT_FN([NAME], TYPE): makes a default print function; won't work for arrays that store non-primitive types
 */

#ifndef __ARRAY_INIT_SIZE
#define __ARRAY_INIT_SIZE 16
#endif

#ifndef __ARRAY_GROWTH_RATE
#define __ARRAY_GROWTH_RATE 1.5
#endif

typedef enum ARRAY_OP_RESULT {
    ARRAY_OP_SUCCESS       =  0,
    ARRAY_OP_OUT_OF_MEMORY = -1
} ARRAY_OP_RESULT_T;

static void* __array_malloc(size_t size, void* ctx) {
    return malloc(size);
}

static void __array_free(void* ptr, size_t size, void* ctx) {
    free(ptr); 
}

#define __PRINT_FN(X, F) _Generic((X), \
    unsigned char:  &(F),              \
    char:           &(F),              \
    signed char:    &(F),              \
    unsigned short: &(F),              \
    unsigned int:   &(F),              \
    int:            &(F),              \
    long:           &(F),              \
    unsigned long:  &(F),              \
    unsigned char*: &__print_str,      \
    char*:          &__print_str,      \
    signed char*:   &__print_str,      \
    void*:          &__print_void_ptr, \
    default:        NULL               \
)

#define GET_MACRO(_1, _2, NAME,...) NAME

#define MAKE_ARRAY(...) GET_MACRO(__VA_ARGS__, MAKE_ARRAY2, MAKE_ARRAY1)(__VA_ARGS__)
#define MAKE_ARRAY1(TYPE) MAKE_ARRAY2(TYPE, TYPE)
#define MAKE_ARRAY2(NAME, TYPE)                                                        \
typedef struct NAME##_array {                                                          \
    TYPE* data;                                                                        \
    size_t len, capacity;                                                              \
    void *alloc_ctx;                                                                   \
    void* (*alloc_fn)(size_t size, void* ctx);                                         \
    void (*free_fn)(void* ptr, size_t size, void* ctx);                                \
    int (*cmp_fn)(TYPE a, TYPE b);                                                     \
    void (*print_fn)(TYPE el);                                                         \
    void (*destructor_fn)(TYPE el);                                                    \
} NAME##_array_t;                                                                      \
void __##NAME##_print_el(TYPE el);                                                     \
void NAME##_array_zero_init(NAME##_array_t* array) {                                   \
    array->len           = 0;                                                          \
    array->capacity      = __ARRAY_INIT_SIZE;                                          \
    array->alloc_fn      = &__array_malloc;                                            \
    array->free_fn       = &__array_free;                                              \
    TYPE t;                                                                            \
    array->print_fn      = __PRINT_FN(t, __##NAME##_print_el);                         \
    array->cmp_fn        = __CMP_FN(t);                                                \
    array->destructor_fn = NULL;                                                       \
}                                                                                      \
ARRAY_OP_RESULT_T NAME##_array_alloc_init(NAME##_array_t* array) {                     \
    array->data = array->alloc_fn(sizeof(TYPE) * __ARRAY_INIT_SIZE, array->alloc_ctx); \
    if (array->data == NULL)                                                           \
        return ARRAY_OP_OUT_OF_MEMORY;                                                 \
    return ARRAY_OP_SUCCESS;                                                           \
}                                                                                      \
void NAME##_array_init(NAME##_array_t* array) {                                        \
    NAME##_array_zero_init(array);                                                     \
    NAME##_array_alloc_init(array);                                                    \
}                                                                                      \
ARRAY_OP_RESULT_T NAME##_array_alloc(NAME##_array_t* array) {                          \
    while (array->len >= array->capacity)                                              \
        array->capacity *= __ARRAY_GROWTH_RATE;                                        \
    TYPE* data = array->alloc_fn(sizeof(TYPE) * array->capacity, array->alloc_ctx);    \
    if (data == NULL)                                                                  \
        return ARRAY_OP_OUT_OF_MEMORY;                                                 \
    size_t data_size = sizeof(TYPE) * array->len;                                      \
    memcpy(data, array->data, data_size);                                              \
    array->free_fn(array->data, data_size, array->alloc_ctx);                          \
    array->data = data;                                                                \
    return ARRAY_OP_SUCCESS;                                                           \
}                                                                                      \
ARRAY_OP_RESULT_T NAME##_array_append(NAME##_array_t* array, TYPE el) {                \
    ARRAY_OP_RESULT_T result = NAME##_array_alloc(array);                              \
    if (result != ARRAY_OP_SUCCESS)                                                    \
        return result;                                                                 \
    *(array->data + array->len) = el;                                                  \
    array->len++;                                                                      \
    return ARRAY_OP_SUCCESS;                                                           \
}                                                                                      \
int NAME##_array_find(NAME##_array_t* array, TYPE el) {                                \
    if (array->cmp_fn == NULL) return -2;                                              \
    for (size_t i = 0; i < array->len; i++)                                            \
        if (array->cmp_fn(*(array->data + i), el) == 0)                                \
            return i;                                                                  \
    return -1;                                                                         \
}                                                                                      \
void __##NAME##_print_el(TYPE el) {                                                    \
    printf(FORMAT_SPECIFIER(el, ""), el);                                              \
}                                                                                      \
void NAME##_array_print(NAME##_array_t* array) {                                       \
    if (array->print_fn == NULL) {                                                     \
        fprintf(stderr, "invalid print function pointer\n");                           \
        return;                                                                        \
    }                                                                                  \
    printf("[");                                                                       \
    for (size_t i = 0; i < array->len; i++) {                                          \
        array->print_fn(*(array->data + i));                                           \
        if (i < array->len - 1) printf(", ");                                          \
    }                                                                                  \
    printf("]\n");                                                                     \
}                                                                                      \
void NAME##_array_free(NAME##_array_t* array) {                                        \
    if (array->destructor_fn != NULL) {                                                \
        for (size_t i = 0; i < array->len; i++) {                                      \
            array->destructor_fn(*(array->data + i));                                  \
        }                                                                              \
    }                                                                                  \
    array->free_fn(array->data, sizeof(TYPE) * array->capacity, array->alloc_ctx);     \
}                                                                

#endif