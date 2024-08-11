#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define __MAKE_ARITHMETIC_CMP_FN(NAME, TYPE) \
static int __##NAME##_cmp_fn(TYPE a, TYPE b) { return (a == b ? 0 : (a > b ? 1 : -1)); }

__MAKE_ARITHMETIC_CMP_FN(uchar, unsigned char)
__MAKE_ARITHMETIC_CMP_FN(char, char)
__MAKE_ARITHMETIC_CMP_FN(ushort, unsigned short)
__MAKE_ARITHMETIC_CMP_FN(short, short)
__MAKE_ARITHMETIC_CMP_FN(uint, unsigned int);
__MAKE_ARITHMETIC_CMP_FN(int, int)
__MAKE_ARITHMETIC_CMP_FN(ulong, unsigned long)
__MAKE_ARITHMETIC_CMP_FN(long, long)
__MAKE_ARITHMETIC_CMP_FN(float, float)
__MAKE_ARITHMETIC_CMP_FN(double, double)
__MAKE_ARITHMETIC_CMP_FN(void_ptr, void*)

static int __str_cmp_fn(char* a, char* b) { return strcmp(a, b); }

#define __CMP_FN(X) _Generic((X),        \
    unsigned char:  &__uchar_cmp_fn,     \
    char:           &__char_cmp_fn,      \
    unsigned short: &__ushort_cmp_fn,    \
    short:          &__short_cmp_fn,     \
    unsigned int:   &__uint_cmp_fn,      \
    int:            &__int_cmp_fn,       \
    unsigned long:  &__ulong_cmp_fn,     \
    long:           &__long_cmp_fn,      \
    float:          &__float_cmp_fn,     \
    double:         &__double_cmp_fn,    \
    void*:          &__void_ptr_cmp_fn,  \
    char*:          &__str_cmp_fn,       \
    default:        NULL                 \
)

static void* __array_malloc(size_t size, void* ctx) {
    return malloc(size);
}

static void __array_free(void* ptr, size_t size, void* ctx) {
    free(ptr); 
}

#define GET_MACRO(_1, _2, NAME,...) NAME

#define MAKE_ARRAY(...) GET_MACRO(__VA_ARGS__, MAKE_ARRAY2, MAKE_ARRAY1)(__VA_ARGS__)
#define MAKE_ARRAY1(TYPE) MAKE_ARRAY2(TYPE, TYPE)
#define MAKE_ARRAY2(NAME, TYPE)                                            \
typedef struct NAME##_array {                                              \
    TYPE* data;                                                            \
    size_t len, capacity;                                                  \
    void *ctx;                                                             \
    void* (*alloc)(size_t size, void* ctx);                                \
    void (*free)(void* ptr, size_t size, void* ctx);                       \
    int (*cmp_fun)(TYPE a, TYPE b);                                        \
} NAME##_array_t;                                                          \
ARRAY_OP_RESULT_T NAME##_array_init(NAME##_array_t* array) {               \
    array->data = malloc(sizeof(TYPE) * __ARRAY_INIT_SIZE);                \
    if (array->data == NULL)                                               \
        return ARRAY_OP_OUT_OF_MEMORY;                                     \
    array->len = 0;                                                        \
    array->capacity = __ARRAY_INIT_SIZE;                                   \
    array->alloc = &__array_malloc;                                        \
    array->free  = &__array_free;                                          \
    TYPE t;                                                                \
    array->cmp_fun = __CMP_FN(t);                                          \
    return ARRAY_OP_SUCCESS;                                               \
}                                                                          \
ARRAY_OP_RESULT_T NAME##_array_alloc(NAME##_array_t* array) {              \
    while (array->len >= array->capacity)                                  \
        array->capacity *= __ARRAY_GROWTH_RATE;                            \
    TYPE* data = array->alloc(sizeof(TYPE) * array->capacity, array->ctx); \
    if (data == NULL)                                                      \
        return ARRAY_OP_OUT_OF_MEMORY;                                     \
    size_t data_size = sizeof(TYPE) * array->len;                          \
    memcpy(data, array->data, data_size);                                  \
    array->free(array->data, data_size, array->ctx);                       \
    array->data = data;                                                    \
    return ARRAY_OP_SUCCESS;                                               \
}                                                                          \
ARRAY_OP_RESULT_T NAME##_array_append(NAME##_array_t* array, TYPE el) {    \
    ARRAY_OP_RESULT_T result = NAME##_array_alloc(array);                  \
    if (result != ARRAY_OP_SUCCESS)                                        \
        return result;                                                     \
    *(array->data + array->len) = el;                                      \
    array->len++;                                                          \
    return ARRAY_OP_SUCCESS;                                               \
}                                                                          \
int NAME##_array_find(NAME##_array_t* array, TYPE el) {                    \
    if (array->cmp_fun == NULL) return -2;                                 \
    for (size_t i = 0; i < array->len; i++)                                \
        if (array->cmp_fun(*(array->data + i), el) == 0)                   \
            return i;                                                      \
    return -1;                                                             \
}                                                                          \
void NAME##_array_free(NAME##_array_t* array) {                            \
    array->free(array->data, sizeof(TYPE) * array->capacity, array->ctx);  \
}                                                                

#define FORMAT_SPECIFIER(T) _Generic((T),     \
    char:          "%c\n",   char*:  "%s\n",  \
    unsigned int:  "%u\n",   int:    "%i\n",  \
    unsigned long: "%lu\n",  long:   "%ld\n", \
    float:         "%f\n",   double: "%d\n",  \
    default:       "%c\n"                     \
)

#define __MAKE_ARRAY_PRINT_FN(...) GET_MACRO(__VA_ARGS__, __MAKE_ARRAY_PRINT_FN2, __MAKE_ARRAY_PRINT_FN1)(__VA_ARGS__)
#define __MAKE_ARRAY_PRINT_FN1(TYPE) __MAKE_ARRAY_PRINT_FN2(TYPE, TYPE)
#define __MAKE_ARRAY_PRINT_FN2(NAME, TYPE)                              \
void NAME##_array_print(NAME##_array_t* array) {                        \
    for (size_t i = 0; i < array->len; i++) {                           \
        TYPE el = *(array->data + i);                                   \
        printf(FORMAT_SPECIFIER(el), el);                               \
    }                                                                   \
}                                                                       \

#endif