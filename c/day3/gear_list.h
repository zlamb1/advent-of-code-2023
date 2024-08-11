#ifndef GEAR_LIST_H
#define GEAR_LIST_H

#include <stdlib.h>
#include <string.h>

typedef struct gear {
    int offset, ratio, adjacent_parts[2]; 
    size_t len; 
} gear_t;

typedef struct gear_list {
    gear_t** gears;
    size_t len, capacity;
} gear_list_t; 

void gear_list_init(gear_list_t* list) {
    list->gears = malloc(sizeof(gear_t*)); 
    list->len = 0;
    list->capacity = 1; 
}

void gear_list_alloc(gear_list_t* list) {
    while (list->len >= list->capacity)
        list->capacity *= 2;
    gear_t** arr = malloc(sizeof(gear_t*) * list->capacity);
    memcpy(arr, list->gears, sizeof(gear_t*) * list->len); 
    free(list->gears);
    list->gears = arr;
}

void gear_list_append(gear_list_t* list, gear_t* gear) {
    gear_list_alloc(list); 
    *(list->gears + list->len) = gear;
    list->len++; 
}

gear_t* gear_list_find(gear_list_t* list, int offset) {
    for (size_t i = 0; i < list->len; i++) {
        gear_t* gear = *(list->gears + i);
        if (gear->offset == offset)
            return gear; 
    }
    return NULL;
}

void gear_list_free(gear_list_t* list) {
    for (size_t i = 0; i < list->len; i++) {
        gear_t* gear = *(list->gears + i);
        free(gear);
    }
    free(list->gears); 
}

#endif