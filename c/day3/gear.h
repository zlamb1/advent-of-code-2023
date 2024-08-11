#ifndef GEAR_LIST_H
#define GEAR_LIST_H

#include <stdlib.h>

#include "../array.h"

typedef struct gear {
    int offset, ratio, adjacent_parts[2]; 
    size_t len; 
} gear_t;

MAKE_ARRAY(gear, gear_t)

gear_t* gear_array_find_offset(gear_array_t* array, int offset) {
    for (size_t i = 0; i < array->len; i++) {
        gear_t* gear = array->data + i;
        if (gear->offset == offset)
            return gear;
    }
    return NULL;
}

#endif