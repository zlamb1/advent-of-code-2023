#ifndef ALMANAC_H
#define ALMANAC_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../array.h"

typedef struct seed_range {
    long start, end, map, range;
    bool matched;
} seed_range_t;

MAKE_ARRAY(seed, seed_range_t)

typedef struct range {
    long src, dst, len;
} range_t;

MAKE_ARRAY(range, range_t)

typedef struct almanac {
    range_array_t** maps;
    size_t len, capacity;
} almanac_t;

void almanac_init(almanac_t* almanac) {
    almanac->maps = malloc(sizeof(range_array_t*));
    almanac->len = 0;
    almanac->capacity = 1;
}

void almanac_alloc(almanac_t* almanac) {
    while (almanac->len >= almanac->capacity)
        almanac->capacity *= 2;
    range_array_t** maps = malloc(sizeof(range_array_t*) * almanac->capacity); 
    memcpy(maps, almanac->maps, sizeof(range_array_t*) * almanac->len);
    free(almanac->maps);
    almanac->maps = maps;
}

void almanac_append(almanac_t* almanac, range_array_t* map) {
    almanac_alloc(almanac);
    *(almanac->maps + almanac->len) = map; 
    almanac->len++;
}

void almanac_free(almanac_t* almanac) {
    for (size_t i = 0; i < almanac->len; i++) {
        range_array_t* map = *(almanac->maps + i); 
        range_array_free(map);
    }
    free(almanac->maps);
}

#endif