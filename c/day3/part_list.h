#ifndef PART_LIST_H
#define PART_LIST_H

#include <stdbool.h>

#include "../dyn_arr.h"

typedef struct part {
    int num; 
    dyn_arr_t adjacent_gears;
    bool adjacent_to_symbol;
} part_t; 

typedef struct part_list {
    part_t** parts;
    size_t len, capacity; 
} part_list_t; 

void part_list_init(part_list_t* list) {
    list->parts = malloc(sizeof(part_t*)); 
    list->len = 0;
    list->capacity = 1; 
}

void part_list_alloc(part_list_t* list) {
    while (list->len >= list->capacity)
        list->capacity *= 2;
    part_t** arr = malloc(sizeof(part_t*) * list->capacity);
    memcpy(arr, list->parts, sizeof(part_t*) * list->len); 
    free(list->parts);
    list->parts = arr;
}

void part_list_append(part_list_t* list, part_t* part) {
    part_list_alloc(list); 
    *(list->parts + list->len) = part;
    list->len++; 
}

void part_list_free(part_list_t* list) {
    for (size_t i = 0; i < list->len; i++) {
        part_t* part = *(list->parts + i);
        dyn_arr_free(&part->adjacent_gears);
        free(part);
    }
    free(list->parts); 
}

#endif