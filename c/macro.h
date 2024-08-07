#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>

#define ASSERT_PTR(ptr) do {\
    if ((ptr) == NULL) {\
        return NULL;\
    }\
} while (0)

#define ASSERT_PTR_RET_VAL(ptr, ret_val) do {\
    if ((ptr) == NULL) {\
        return (ret_val);\
    }\
} while (0)

#endif