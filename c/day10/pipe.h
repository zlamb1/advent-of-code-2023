#ifndef PIPE_H
#define PIPE_H

#include <stdbool.h>
#include <stdio.h>

#include "../array.h"

MAKE_ARRAY(int)

typedef enum direction {
    NORTH,
    WEST,
    EAST,
    SOUTH
} direction; 

typedef struct pipe {
    char c; 
    struct pipe* next, *prev;
    int is_start, index, dist[2]; 
} pipe; 

#endif