#ifndef PART_LIST_H
#define PART_LIST_H

#include <stdbool.h>

#include "../array.h"

MAKE_ARRAY(int)

typedef struct part {
    int num; 
    int_array_t adjacent_gears;
    bool adjacent_to_symbol;
} part_t; 

MAKE_ARRAY(part, part_t)

#endif