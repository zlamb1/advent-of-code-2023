#ifndef CMP_H
#define CMP_H

#include <string.h>

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

#endif
