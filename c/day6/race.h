#ifndef RACE_H
#define RACE_H

#include "../array.h"

typedef struct race {
    long time, distance;
} race_t;

MAKE_ARRAY(race, race_t)

#endif