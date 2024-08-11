#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * dynamic array implementation for ints
 */

#ifndef ARRAY_INIT_SIZE
#define ARRAY_INIT_SIZE 16
#endif

#ifndef ARRAY_GROWTH_RATE
#define ARRAY_GROWTH_RATE 1.5
#endif

typedef enum ARRAY_OP_RESULT {
    ARRAY_OP_SUCCESS       =  0,
    ARRAY_OP_OUT_OF_MEMORY = -1
} ARRAY_OP_RESULT_T;

#define GET_MACRO(_1, _2, NAME,...) NAME

#define MAKE_ARRAY(...) GET_MACRO(__VA_ARGS__, MAKE_ARRAY2, MAKE_ARRAY1)(__VA_ARGS__)
#define MAKE_ARRAY1(TYPE) MAKE_ARRAY2(TYPE, TYPE)
#define MAKE_ARRAY2(NAME, TYPE)                                         \
typedef struct NAME##_array {                                           \
    TYPE* data;                                                         \
    size_t len, capacity;                                               \
} NAME##_array_t;                                                       \
ARRAY_OP_RESULT_T NAME##_array_init(NAME##_array_t* array) {            \
    array->data = malloc(sizeof(TYPE));                                 \
    if (array->data == NULL)                                            \
        return ARRAY_OP_OUT_OF_MEMORY;                                  \
    array->len = 0;                                                     \
    array->capacity = ARRAY_INIT_SIZE;                                  \
    return ARRAY_OP_SUCCESS;                                            \
}                                                                       \
ARRAY_OP_RESULT_T NAME##_array_alloc(NAME##_array_t* array) {           \
    while (array->len >= array->capacity)                               \
        array->capacity *= ARRAY_GROWTH_RATE;                           \
    TYPE* data = malloc(sizeof(TYPE) * array->capacity);                \
    if (data == NULL)                                                   \
        return ARRAY_OP_OUT_OF_MEMORY;                                  \
    memcpy(data, array->data, sizeof(TYPE) * array->len);               \
    free(array->data);                                                  \
    array->data = data;                                                 \
    return ARRAY_OP_SUCCESS;                                            \
}                                                                       \
ARRAY_OP_RESULT_T NAME##_array_append(NAME##_array_t* array, TYPE el) { \
    ARRAY_OP_RESULT_T result = NAME##_array_alloc(array);               \
    if (result != ARRAY_OP_SUCCESS)                                     \
        return result;                                                  \
    *(array->data + array->len) = el;                                   \
    array->len++;                                                       \
    return ARRAY_OP_SUCCESS;                                            \
}                                                                       \
int NAME##_array_find(NAME##_array_t* array, TYPE el) {                 \
    for (size_t i = 0; i < array->len; i++)                             \
        if (*(array->data + i) == el)                                   \
            return i;                                                   \
    return -1;                                                          \
}                                                                       \
void NAME##_array_free(NAME##_array_t* array) {                         \
    free(array->data);                                                  \
}                                                                

#define FORMAT_SPECIFIER(T) _Generic((T),     \
    char:          "%c\n",   char*:  "%s\n",  \
    unsigned int:  "%u\n",   int:    "%i\n",  \
    unsigned long: "%lu\n",  long:   "%ld\n", \
    float:         "%f\n",   double: "%d\n",  \
    default:       "%c\n"                     \
)

#define MAKE_ARRAY_WITH_PRINT(...) GET_MACRO(__VA_ARGS__, MAKE_ARRAY_WITH_PRINT2, MAKE_ARRAY_WITH_PRINT1)(__VA_ARGS__)
#define MAKE_ARRAY_WITH_PRINT1(TYPE) MAKE_ARRAY_WITH_PRINT2(TYPE, TYPE)
#define MAKE_ARRAY_WITH_PRINT2(NAME, TYPE)                              \
typedef struct NAME##_array {                                           \
    TYPE* data;                                                         \
    size_t len, capacity;                                               \
} NAME##_array_t;                                                       \
ARRAY_OP_RESULT_T NAME##_array_init(NAME##_array_t* array) {            \
    array->data = malloc(sizeof(TYPE));                                 \
    if (array->data == NULL)                                            \
        return ARRAY_OP_OUT_OF_MEMORY;                                  \
    array->len = 0;                                                     \
    array->capacity = ARRAY_INIT_SIZE;                                  \
    return ARRAY_OP_SUCCESS;                                            \
}                                                                       \
ARRAY_OP_RESULT_T NAME##_array_alloc(NAME##_array_t* array) {           \
    while (array->len >= array->capacity)                               \
        array->capacity *= ARRAY_GROWTH_RATE;                           \
    TYPE* data = malloc(sizeof(TYPE) * array->capacity);                \
    if (data == NULL)                                                   \
        return ARRAY_OP_OUT_OF_MEMORY;                                  \
    memcpy(data, array->data, sizeof(TYPE) * array->len);               \
    free(array->data);                                                  \
    array->data = data;                                                 \
    return ARRAY_OP_SUCCESS;                                            \
}                                                                       \
ARRAY_OP_RESULT_T NAME##_array_append(NAME##_array_t* array, TYPE el) { \
    ARRAY_OP_RESULT_T result = NAME##_array_alloc(array);               \
    if (result != ARRAY_OP_SUCCESS)                                     \
        return result;                                                  \
    *(array->data + array->len) = el;                                   \
    array->len++;                                                       \
    return ARRAY_OP_SUCCESS;                                            \
}                                                                       \
int NAME##_array_find(NAME##_array_t* array, TYPE el) {                 \
    for (size_t i = 0; i < array->len; i++)                             \
        if (*(array->data + i) == el)                                   \
            return i;                                                   \
    return -1;                                                          \
}                                                                       \
void NAME##_array_print(NAME##_array_t* array) {                        \
    for (size_t i = 0; i < array->len; i++) {                           \
        TYPE el = *(array->data + i);                                   \
        printf(FORMAT_SPECIFIER(el), el);                               \
    }                                                                   \
}                                                                       \
void NAME##_array_free(NAME##_array_t* array) {                         \
    free(array->data);                                                  \
}    

#endif