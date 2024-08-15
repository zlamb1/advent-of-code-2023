#ifndef GALAXY_H
#define GALAXY_H

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) > (Y) ? (Y) : (X))

#include <stdlib.h>

#include "../array.h"

MAKE_ARRAY(size, size_t)

typedef struct galaxy {
    size_t id, index; 
} galaxy; 

MAKE_ARRAY(galaxy)

typedef struct galaxy_pair {
    galaxy* galaxies[2]; 
    long distance;  
} galaxy_pair; 

MAKE_ARRAY(galaxy_pair)
#endif