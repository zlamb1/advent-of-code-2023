#ifndef ALMANAC_H
#define ALMANAC_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct seed_range {
    long start, end, map, range;
    bool matched;
} seed_range_t;

typedef struct seed_list {
    seed_range_t* ranges;
    size_t len, capacity;
} seed_list_t; 

typedef struct range {
    long src, dst, len;
} range_t;

typedef struct num_map {
    range_t* ranges; 
    size_t len, capacity;
} num_map_t;

typedef struct almanac {
    num_map_t** maps;
    size_t len, capacity;
} almanac_t;

void seed_list_init(seed_list_t* list) {
    list->ranges = malloc(sizeof(seed_range_t));
    list->len = 0;
    list->capacity = 1; 
}

void seed_list_alloc(seed_list_t* list) {
    while (list->len >= list->capacity)
        list->capacity *= 2;
    seed_range_t* ranges = malloc(sizeof(seed_range_t) * list->capacity); 
    memcpy(ranges, list->ranges, sizeof(seed_range_t) * list->len);
    free(list->ranges);
    list->ranges = ranges;
}

void seed_list_append(seed_list_t* list, seed_range_t range) {
    seed_list_alloc(list);
    *(list->ranges + list->len) = range;
    list->len++;
}

void seed_list_free(seed_list_t* list) {
    free(list->ranges); 
}

void num_map_init(num_map_t* map) {
    map->ranges = malloc(sizeof(range_t));
    map->len = 0;
    map->capacity = 1; 
}

void num_map_alloc(num_map_t* map) {
    while (map->len >= map->capacity)
        map->capacity *= 2;
    range_t* ranges = malloc(sizeof(range_t) * map->capacity); 
    memcpy(ranges, map->ranges, sizeof(range_t) * map->len);
    free(map->ranges);
    map->ranges = ranges;
}

void num_map_append(num_map_t* map, range_t range) {
    num_map_alloc(map);
    *(map->ranges + map->len) = range;
    map->len++;
}

void num_map_free(num_map_t* map) {
    free(map->ranges); 
}

void almanac_init(almanac_t* almanac) {
    almanac->maps = malloc(sizeof(num_map_t*));
    almanac->len = 0;
    almanac->capacity = 1;
}

void almanac_alloc(almanac_t* almanac) {
    while (almanac->len >= almanac->capacity)
        almanac->capacity *= 2;
    num_map_t** maps = malloc(sizeof(num_map_t*) * almanac->capacity); 
    memcpy(maps, almanac->maps, sizeof(num_map_t*) * almanac->len);
    free(almanac->maps);
    almanac->maps = maps;
}

void almanac_append(almanac_t* almanac, num_map_t* map) {
    almanac_alloc(almanac);
    *(almanac->maps + almanac->len) = map; 
    almanac->len++;
}

void almanac_free(almanac_t* almanac) {
    for (size_t i = 0; i < almanac->len; i++) {
        num_map_t* map = *(almanac->maps + i); 
        num_map_free(map);
    }
    free(almanac->maps);
}

#endif